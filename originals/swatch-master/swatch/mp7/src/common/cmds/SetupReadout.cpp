#include "swatch/mp7/cmds/SetupReadout.hpp"

// log4cplus headers
#include "log4cplus/loggingmacros.h"

// MP7 driver headers
#include "mp7/TTCNode.hpp"

// SWATCH headers
#include "swatch/xsimpletypedefs.hpp"
#include "swatch/core/rules/None.hpp"
#include "swatch/mp7/MP7Processor.hpp"


namespace swatch {
namespace mp7 {
namespace cmds {

// ----------------------------------------------------------------------------
SetupReadout::SetupReadout( const std::string& aId, swatch::action::ActionableObject& aActionable ) :
  action::Command(aId, aActionable, XUInt_t())
{
  registerParameter("internal", XBool_t(false));
  registerParameter("bxOffset", XUInt_t(1));
  registerParameter("drain", XUInt_t(), core::rules::None<XUInt_t>());
  registerParameter("bufferHWM", XUInt_t(32));
  registerParameter("bufferLWM", XUInt_t(16));
  registerParameter("fake", XUInt_t(), core::rules::None<XUInt_t>());

  addConstraint("watermarks", WatermarkConstraint("bufferLWM", "bufferHWM"));
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
action::Command::State
SetupReadout::code(const ::swatch::core::XParameterSet& aParams)
{
  const XBool_t& lInternal = aParams.get<XBool_t>("internal");
  const XUInt_t& lBxOffset = aParams.get<XUInt_t>("bxOffset");
  const XUInt_t& lBufferHWM = aParams.get<XUInt_t>("bufferHWM");
  const XUInt_t& lBufferLWM = aParams.get<XUInt_t>("bufferLWM");

  XUInt_t lDrain = aParams.get<XUInt_t>("drain");
  XUInt_t lFake = aParams.get<XUInt_t>("fake");

  MP7Processor& p = getActionable<MP7Processor>();
  ::mp7::MP7Controller& driver = p.driver();

  const ::mp7::TTCNode& ttc = driver.getTTC();
  const ::mp7::ReadoutNode& ro = driver.getReadout();
  const ::mp7::ReadoutCtrlNode& rc = ro.getNode("readout_control");

  const XUInt_t& roSize = driver.getGenerics().roChunks * 2;
  if ( lBufferLWM >= roSize or lBufferHWM >= roSize) {
    std::ostringstream msg;
    msg << "RO buffer Watermarks higher than its size : lwm = %d, hwm - %d, rosize = %d";
    setStatusMsg(msg.str());
    return State::kError;
  }

  // Enable trigger rules if in internal mode
  ttc.enableL1ATrgRules(lInternal);

  // Enable trigger throttling
  ttc.enableL1AThrottling(lInternal);

  // Set the readout counter offset
  ro.setBxOffset(lBxOffset);

  //  set TTS status by hand.
  //  1 = warningt
  //  2 = out of sync
  //  4 = busy
  //  8 = ready
  //  12 = error
  //  0 or 15 = disconnected
  ro.forceTTSState(false);

  // To drain or not to drain in internal mode?
  if ( lDrain.isNaN()) {
    LOG4CPLUS_INFO(p.getLogger(), "Autodrain : disabled");
    ro.enableAutoDrain(false);
  }
  else {
    LOG4CPLUS_INFO(p.getLogger(), "Autodrain rate : 0x" << std::hex << lDrain.value_);
    ro.enableAutoDrain(true, lDrain.value_);
  }


  // Configure big fifo's watermarks
  // Maximum: 64x
  // High water mark : 32 - 50%
  // Low water mark : 16 - 25%
  ro.setFifoWaterMarks(lBufferLWM, lBufferHWM);

  // And the derandomisers
  // Maximum: 512
  // TODO: check with Fionn
  rc.setDerandWaterMarks(64,128);


  if (lFake.isNaN()) {
    // logging.info("ReadoutControl event source selected")
    ro.selectEventSource( ::mp7::ReadoutNode::kReadoutEventSource );
  }
  else {
    // logging.info("Fake event source selected, event size : 0x%x", fakesize)
    ro.selectEventSource( ::mp7::ReadoutNode::kFakeEventSource );
    ro.configureFakeEventSize(lFake.value_);
  }

  // declare the board ready for readout
  ro.start();

  // Local mode, amc13 link disabled
  LOG4CPLUS_INFO(p.getLogger(), "AMC13 output : "  << (not lInternal ? "enabled" : "disabled"));
  ro.enableAMC13Output(not lInternal);

  return State::kDone;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
WatermarkConstraint::WatermarkConstraint(const std::string& aLowName, const std::string& aHighName):
  mLowName(aLowName),
  mHighName(aHighName)
{
  require<XUInt_t>(mLowName);
  require<XUInt_t>(mHighName);
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
void WatermarkConstraint::describe(std::ostream& aStream) const {
  aStream << mLowName << " < " << mHighName;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
core::XMatch WatermarkConstraint::verify(const core::XParameterSet& aParams) const {

  const auto& lLow = aParams.get<XUInt_t>(mLowName);
  const auto& lHigh = aParams.get<XUInt_t>(mHighName);

  return ( lLow < lHigh );
  }
// ----------------------------------------------------------------------------


} // namespace cmds
} // namespace mp7
} // namespace swatch


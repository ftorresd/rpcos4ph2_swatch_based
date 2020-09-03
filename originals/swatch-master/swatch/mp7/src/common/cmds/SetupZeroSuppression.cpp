#include "swatch/mp7/cmds/SetupZeroSuppression.hpp"

// log4cplus headers
#include "log4cplus/loggingmacros.h"

// SWATCH headers
#include "swatch/xsimpletypedefs.hpp"
#include "swatch/mp7/MP7Processor.hpp"

#include "mp7/ZeroSuppressionNode.hpp"


namespace swatch {
namespace mp7 {
namespace cmds {

// ----------------------------------------------------------------------------
SetupZeroSuppression::SetupZeroSuppression(const std::string& aId, swatch::action::ActionableObject& aActionable):
  action::Command(aId, aActionable, XUInt_t()),
  mZSInstantiated(false)
{

  // Always register the (only) parameter
  registerParameter("enableZS", XBool_t(false));
  
  ::mp7::MP7Controller& lDriver = getActionable<MP7Processor>().driver();

  // Is the firmware ZS capable?
  // Check #1: Has this fw got the ZS node?
  if ( lDriver.getReadout().getNodes("readout_zs").empty() ) {
    // The readout_zs is not present. We're in a pre-2.2.1
    // Set flag and quit
    mZSInstantiated = false;
    return;
  }

  // Check #2: if ZS has been implemented
  const ::mp7::ZeroSuppressionNode& lZSNode = lDriver.getReadout().getNode< ::mp7::ZeroSuppressionNode>("readout_zs");

  // Check if the ZS block is instantiated
  uhal::ValWord<uint32_t> lZSInstantiated = lZSNode.getNode("csr.info.zs_enabled").read();
  lZSNode.getClient().dispatch();

  mZSInstantiated = (lZSInstantiated == 1);
  
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
action::Command::State SetupZeroSuppression::code(const ::swatch::core::XParameterSet& aParams)
{


  if (!mZSInstantiated) {
    setStatusMsg("ZeroSuppression is not enabled in the current firmware");
    return State::kError;
  }
  
  const XBool_t& lEnableZS = aParams.get<XBool_t>("enableZS");

  //TODO: set enable bit
  ::mp7::MP7Controller& lDriver = getActionable<MP7Processor>().driver();
  const ::mp7::ZeroSuppressionNode& lZSNode = lDriver.getReadout().getNode< ::mp7::ZeroSuppressionNode>("readout_zs");
  lZSNode.enable(lEnableZS);
  return State::kDone;
}
// ----------------------------------------------------------------------------



} // namespace cmds
} // namespace mp7
} // namespace swatch


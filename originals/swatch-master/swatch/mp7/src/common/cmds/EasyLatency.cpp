#include "swatch/mp7/cmds/EasyLatency.hpp"


#include <log4cplus/loggingmacros.h>

#include "swatch/xsimpletypedefs.hpp"
#include "swatch/mp7/MP7AbstractProcessor.hpp"
#include "swatch/mp7/cmds/BufferTraits.hpp"
#include "swatch/core/utilities.hpp"


namespace swatch {
namespace mp7 {
namespace cmds {

// ----------------------------------------------------------------------------
template<class Selector>
EasyLatency<Selector>::EasyLatency(const std::string& aId, swatch::action::ActionableObject& aActionable) :
  swatch::action::Command(aId, aActionable, xdata::String()),
  mBufferSelector(*this)
{

  mBufferSelector.registerParameters();

  registerParameter("bankId", XUInt_t(0x0));
  registerParameter("masterLatency", XUInt_t(0x0));
  registerParameter("algoLatency", XUInt_t(0x0));
  registerParameter("internalLatency", XUInt_t(0x0));
  registerParameter("rxExtraFrames", XUInt_t(0x0));
  registerParameter("txExtraFrames", XUInt_t(0x0));
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
template<class Selector>
action::Command::State EasyLatency<Selector>::code(const ::swatch::core::XParameterSet& params)
{
  // Get the corresponding buffer kind
  const ::mp7::RxTxSelector bKind = BufferTraits<Selector>::kRxTxSelector;

  const XUInt_t& bankId = params.get<XUInt_t>("bankId");
  const XUInt_t& masterLatency = params.get<XUInt_t>("masterLatency");
  const XUInt_t& algoLatency = params.get<XUInt_t>("algoLatency");
  const XUInt_t& internalLatency = params.get<XUInt_t>("internalLatency");
  const XUInt_t& rxExtraFrames = params.get<XUInt_t>("rxExtraFrames");
  const XUInt_t& txExtraFrames = params.get<XUInt_t>("txExtraFrames");

  setProgress(0.0, "Configuring " + boost::lexical_cast<std::string>(bKind) + " buffers in latency mode");

  uint32_t depth = computeLatency(masterLatency, algoLatency, internalLatency, rxExtraFrames, txExtraFrames);

  ::mp7::ChannelManager cm = mBufferSelector.manager(params);
  ::mp7::LatencyPathConfigurator pc = ::mp7::LatencyPathConfigurator(bankId, depth);

  cm.configureBuffers(bKind, pc);

  setStatusMsg(
    boost::lexical_cast<std::string>(bKind) + " buffers configured in latency mode: "
    "depth = " + boost::lexical_cast<std::string>(depth) + ", " + core::joinAny(mBufferSelector.extractIds(cm)));

  return State::kDone;
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
template<>
uint32_t
EasyLatency<RxBufferSelector>::computeLatency(uint32_t aMaster, uint32_t aAlgo, uint32_t aInternal, uint32_t aRxExtraFrames, uint32_t aTxExtraFrames)
{
  //  return aMaster + aInternal + aRxExtraFrames;
  uint32_t lLatency = aMaster + aInternal + aRxExtraFrames;
  LOG4CPLUS_INFO(getActionable<MP7AbstractProcessor>().getLogger(), "Compute Rx Latency:"
                 << " M(" << aMaster
                 << ")+I(" << aInternal
                 << ")+xRx(" << aRxExtraFrames
                 << ") => L = " << lLatency);

  return lLatency;
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
template<>
uint32_t
EasyLatency<TxBufferSelector>::computeLatency(uint32_t aMaster, uint32_t aAlgo, uint32_t aInternal, uint32_t aRxExtraFrames, uint32_t aTxExtraFrames)
{
  uint32_t lLatency = aMaster + aInternal - aAlgo + aTxExtraFrames;

  LOG4CPLUS_INFO(getActionable<MP7AbstractProcessor>().getLogger(), "Compute Tx Latency:"
                 << " M(" << aMaster
                 << ")+I(" << aInternal
                 << ")-A(" << aAlgo
                 << ")+xTx(" << aRxExtraFrames
                 << ") => L = " << lLatency);

  return lLatency;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template class EasyLatency<RxBufferSelector>;
template class EasyLatency<TxBufferSelector>;
// ----------------------------------------------------------------------------



} // namespace cmds
} // namespace mp7
} // namespace swatch


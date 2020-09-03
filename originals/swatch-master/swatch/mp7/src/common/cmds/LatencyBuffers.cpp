#include "swatch/mp7/cmds/LatencyBuffers.hpp"

#include "swatch/xsimpletypedefs.hpp"
#include "swatch/mp7/cmds/BufferTraits.hpp"
#include "swatch/core/utilities.hpp"


namespace swatch {
namespace mp7 {
namespace cmds {



// ----------------------------------------------------------------------------
template<class Selector>
LatencyBuffers<Selector>::LatencyBuffers(const std::string& aId, swatch::action::ActionableObject& aActionable) :
  swatch::action::Command(aId, aActionable, xdata::String()),
  mBufferSelector(*this)
{

  mBufferSelector.registerParameters();

  registerParameter("bankId", XUInt_t(0x0));
  registerParameter("depth", XUInt_t(0x0));
}
// ----------------------------------------------------------------------------



// ----------------------------------------------------------------------------
template<class Selector>
action::Command::State LatencyBuffers<Selector>::code(const ::swatch::core::XParameterSet& params)
{
  // Get the corresponding buffer kind
  const ::mp7::RxTxSelector bKind = BufferTraits<Selector>::kRxTxSelector;


  const XUInt_t& bankId = params.get<XUInt_t>("bankId");
  const XUInt_t& depth = params.get<XUInt_t>("depth");

  setProgress(0.0, "Configuring buffers in latency mode");

  ::mp7::ChannelManager cm = mBufferSelector.manager(params);
  ::mp7::LatencyPathConfigurator pc = ::mp7::LatencyPathConfigurator(bankId, depth);

  cm.configureBuffers(bKind, pc);

  setStatusMsg("Finished configuring buffers in latency mode: "+core::joinAny(mBufferSelector.extractIds(cm)) );

  return State::kDone;
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
template class LatencyBuffers<RxBufferSelector>;
template class LatencyBuffers<TxBufferSelector>;
// ----------------------------------------------------------------------------




} // namespace cmds
} // namespace mp7
} // namespace swatch


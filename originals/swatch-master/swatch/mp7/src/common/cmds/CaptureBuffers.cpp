#include "swatch/mp7/cmds/CaptureBuffers.hpp"

// MP7 Core Headers
#include "mp7/MP7MiniController.hpp"    // for MP7MiniController
#include "mp7/TTCNode.hpp"

// SWATCH headers
#include "swatch/mp7/MP7AbstractProcessor.hpp"

namespace swatch {
namespace mp7 {
namespace cmds {

// ----------------------------------------------------------------------------
CaptureBuffers::CaptureBuffers(const std::string& aId, swatch::action::ActionableObject& aActionable) :
  swatch::action::Command(aId, aActionable, xdata::String())
{
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
action::Command::State
CaptureBuffers::code(const ::swatch::core::XParameterSet& params)
{

  ::mp7::MP7MiniController& driver = getActionable< MP7AbstractProcessor>().driver();

  ::mp7::TTCNode ttc = driver.getTTC();
  ::mp7::ChannelManager cm = driver.channelMgr();


  setProgress(0.5, "Clearing capture buffers ...");

  cm.clearBuffers(::mp7::kRx, ::mp7::ChanBufferNode::kCapture);
  cm.clearBuffers(::mp7::kTx, ::mp7::ChanBufferNode::kCapture);

  setProgress(0.5, "Capturing data stream ...");

  ttc.forceBTest();
  cm.waitCaptureDone();

  setStatusMsg("Capture completed");
  return State::kDone;
}
// ----------------------------------------------------------------------------

} // namespace cmds
} // namespace mp7
} // namespace swatch
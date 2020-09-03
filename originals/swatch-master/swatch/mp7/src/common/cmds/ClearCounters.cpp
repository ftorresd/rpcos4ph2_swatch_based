#include "swatch/mp7/cmds/ClearCounters.hpp"


#include "swatch/mp7/MP7Processor.hpp"
#include "mp7/TTCNode.hpp"

namespace swatch {
namespace mp7 {
namespace cmds {

// ----------------------------------------------------------------------------
ClearCounters::ClearCounters(const std::string& aId, action::ActionableObject& aActionable) :
  Command(aId, aActionable, xdata::String())
{
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
action::Command::State ClearCounters::code(const core::XParameterSet& aParams)
{
  // Reset TTC counters
  MP7Processor& lProcessor = getActionable<MP7Processor>();
  ::mp7::MP7Controller& lDriver = lProcessor.driver();
  setProgress(0.,"Clearing TTC counters");

  lDriver.getTTC().clear();

  setResult(xdata::String("Counters cleared"));
  return State::kDone;
}
// ----------------------------------------------------------------------------

} // namespace cmds
} // namespace mp7
} // namespace swatch
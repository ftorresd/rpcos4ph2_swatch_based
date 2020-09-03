#include "swatch/mp7/cmds/HardReset.hpp"

// log4cplus headers
#include "log4cplus/loggingmacros.h"

// SWATCH headers
#include "swatch/mp7/MP7Processor.hpp"

#include "mp7/MmcController.hpp"


namespace swatch {
namespace mp7 {
namespace cmds {

// ----------------------------------------------------------------------------
HardReset::HardReset(const std::string& aId, swatch::action::ActionableObject& aActionable) :
  Command(aId, aActionable, xdata::Integer())
{
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
::swatch::action::Command::State
HardReset::code(const ::swatch::core::XParameterSet& aParams)
{
  MP7AbstractProcessor& p = getActionable<MP7AbstractProcessor>();
  ::mp7::MmcController mmcController(p.driver().hw());

  setProgress(0., "Performing Hard Reset of the board ...");

  mmcController.hardReset();

  setStatusMsg("Hard Reset Completed");

  return State::kDone;
}
// ----------------------------------------------------------------------------


} // namespace cmds
} // namespace mp7
} // namespace swatch


#include "swatch/mp7/cmds/DeleteFirmware.hpp"

// log4cplus headers
#include "log4cplus/loggingmacros.h"

// SWATCH headers
#include "swatch/mp7/MP7Processor.hpp"

#include "mp7/MmcController.hpp"


namespace swatch {
namespace mp7 {
namespace cmds {

// ----------------------------------------------------------------------------
DeleteFirmware::DeleteFirmware(const std::string& aId, swatch::action::ActionableObject& aActionable) :
  Command(aId, aActionable, xdata::Integer())
{
  registerParameter("sdfile", xdata::String(""));
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
::swatch::action::Command::State
DeleteFirmware::code(const ::swatch::core::XParameterSet& aParams)
{
  MP7AbstractProcessor& p = getActionable<MP7AbstractProcessor>();

  std::string sdfile = aParams.get<xdata::String>("sdfile");

  if (sdfile.empty()) {
    setStatusMsg("Please provide a filename to delete");
    return State::kError;
  }

  if (!sdfile.compare("GoldenImage.bin")) {
    setStatusMsg("One must *not* delete the Golden Image!");
    return State::kError;
  }

  setProgress(0., "Deleting firmware image from uSD card ...");

  ::mp7::MmcController mmcController(p.driver().hw());
  mmcController.deleteFileFromSD(sdfile);

  //need to check if file has actually been deleted

  setStatusMsg("Delete Firmware Completed");

  return State::kDone;
}
// ----------------------------------------------------------------------------



} // namespace cmds
} // namespace mp7
} // namespace swatch


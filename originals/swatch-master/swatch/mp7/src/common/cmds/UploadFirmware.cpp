#include "swatch/mp7/cmds/UploadFirmware.hpp"

#include <fstream>

// log4cplus headers
#include "log4cplus/loggingmacros.h"

// SWATCH headers
#include "swatch/mp7/MP7Processor.hpp"

#include "mp7/MmcController.hpp"


namespace swatch {
namespace mp7 {
namespace cmds {

// ----------------------------------------------------------------------------
UploadFirmware::UploadFirmware(const std::string& aId, swatch::action::ActionableObject& aActionable) :
  Command(aId, aActionable, xdata::Integer())
{
  registerParameter("localfile", xdata::String(""));
  registerParameter("sdfile", xdata::String(""));
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
::swatch::action::Command::State
UploadFirmware::code(const ::swatch::core::XParameterSet& aParams)
{
  MP7AbstractProcessor& p = getActionable<MP7AbstractProcessor>();

  std::string localfile = aParams.get<xdata::String>("localfile");
  std::string sdfile = aParams.get<xdata::String>("sdfile");
  std::ifstream firmware(localfile.c_str());

  if (localfile.empty() || sdfile.empty() || !firmware.good()) {
    setStatusMsg("No filepath or filename given, or file does not exist");
    return State::kError;
  }

  setProgress(0., "Uploading firmware image to uSD card ...");

  ::mp7::MmcController mmcController(p.driver().hw());
  mmcController.copyFileToSD(localfile, sdfile);

  setStatusMsg("Upload completed");
  return State::kDone;
}
// ----------------------------------------------------------------------------


} // namespace cmds
} // namespace mp7
} // namespace swatch


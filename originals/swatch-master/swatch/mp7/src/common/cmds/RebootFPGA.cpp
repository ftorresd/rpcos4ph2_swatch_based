#include "swatch/mp7/cmds/RebootFPGA.hpp"

// log4cplus headers
#include "log4cplus/loggingmacros.h"

// SWATCH headers
#include "swatch/mp7/MP7Processor.hpp"

#include "mp7/MmcController.hpp"


namespace swatch {
namespace mp7 {
namespace cmds {

// ----------------------------------------------------------------------------
RebootFPGA::RebootFPGA(const std::string& aId, swatch::action::ActionableObject& aActionable) :
  Command(aId, aActionable, xdata::Integer())
{
  registerParameter("sdfile", xdata::String(""));
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
::swatch::action::Command::State
RebootFPGA::code(const ::swatch::core::XParameterSet& aParams)
{
  MP7AbstractProcessor& p = getActionable<MP7AbstractProcessor>();
  std::string sdfile = aParams.get<xdata::String>("sdfile").value_;

  if (sdfile.empty()) {
    setStatusMsg("Please provide a firmware filename to reboot FPGA");
    return State::kError;
  }

  setProgress(0., "Rebooting FPGA ...");

  ::mp7::MmcController mmcController(p.driver().hw());
  mmcController.rebootFPGA(sdfile);

  ::mp7::MmcPipeInterface mmcNode(p.driver().hw().getNode< ::mp7::MmcPipeInterface>("uc"));
  std::string rebootfile = mmcNode.GetTextSpace();

  if (rebootfile.compare(sdfile)) {
    setStatusMsg("FPGA did not reboot with the firmware image provided. Please check filename!");
    return State::kError;
  }

  setStatusMsg("Reboot FPGA Completed");

  return State::kDone;
}
// ----------------------------------------------------------------------------



} // namespace cmds
} // namespace mp7
} // namespace swatch


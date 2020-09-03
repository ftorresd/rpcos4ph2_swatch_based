#include "swatch/amc13/cmds/Reboot.hpp"

#include "swatch/xsimpletypedefs.hpp"
#include "swatch/amc13/AMC13Manager.hpp"

// AMC13 Headers
#include "amc13/AMC13.hh"
#include "amc13/AMC13Simple.hh"         // for AMC13Simple, etc
#include "amc13/Flash.hh"               // for Flash

namespace swatch {
namespace amc13 {
namespace cmds {

// --------------------------------------------------------
Reboot::Reboot(const std::string& aId, swatch::action::ActionableObject& aActionable) :
  Command(aId, aActionable, XBool_t(false))
{
  // Wakeup timeout
  registerParameter("timeout", XUInt_t(30));
}
// --------------------------------------------------------


// --------------------------------------------------------
action::Command::State Reboot::code(const core::XParameterSet& aParams)
{
  
  uint32_t lTimeout = aParams.get<XUInt_t>("timeout");

  AMC13Manager& amc13 = getActionable<AMC13Manager>();

  setStatusMsg("Loading FW from flash storage");

  ::amc13::AMC13& driver = amc13.driver();

  driver.getFlash()->loadFlash();

  setProgress(0.5, "Waiting for T1 & 2 to wake up again");

  // Sleep for 2 seconds first ...
  boost::this_thread::sleep_for( boost::chrono::seconds(2) );

  for (size_t i=0; i<lTimeout; i++) {
    try {
      uint32_t vT1 = driver.read(::amc13::AMC13::T1, "STATUS.FIRMWARE_VERS");
      uint32_t vT2 = driver.read(::amc13::AMC13::T2, "STATUS.FIRMWARE_VERS");

      std::ostringstream oss;
      oss << "Firmware versions - T1: 0x" << std::hex << vT1 << ", T2: 0x" << vT2;
      setResult(XBool_t(true));
      setStatusMsg("AMC13 is alive! " + oss.str());
      return State::kDone;
    }
    catch (const uhal::exception::exception& e) {
    }
    boost::this_thread::sleep_for( boost::chrono::seconds(1));
  }

  setStatusMsg("AMC13 did not wake up after reboot");
  return State::kError;
}
// --------------------------------------------------------

} // namespace cmds
} // namespace amc13
} // namespace swatch

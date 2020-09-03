#include "swatch/mp7/cmds/CheckFirmwareRevision.hpp"

#include "swatch/xsimpletypedefs.hpp"
#include "swatch/mp7/MP7Processor.hpp"


namespace swatch {
namespace mp7 {
namespace cmds {
  

// ----------------------------------------------------------------------------
CheckFirmwareRevision::CheckFirmwareRevision(const std::string& aId, action::ActionableObject& aActionable) :
  Command(aId, aActionable, xdata::String())
{
  registerParameter("infraRev", XUInt_t(0));
  registerParameter("algoRev", XUInt_t(0));

}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
action::Command::State
CheckFirmwareRevision::code(const core::XParameterSet& aParams)
{
  MP7Processor& lProcessor = getActionable<MP7Processor>();
  ::mp7::MP7Controller& lDriver = lProcessor.driver();


  const XUInt_t& lInfraRev = aParams.get<XUInt_t>("infraRev");
  const XUInt_t& lAlgoRev = aParams.get<XUInt_t>("algoRev");

  uint32_t lFwInfraRev = lDriver.getCtrl().readFwRevision();
  uint32_t lFwAlgoRev = lDriver.getCtrl().readAlgoRevision();

  std::ostringstream lErrMsg;

  // Check for infra mismatch
  if ( lInfraRev != lFwInfraRev) {
    lErrMsg << "Infra Firmware mismatch - Expected: 0x" << std::hex << uint32_t(lInfraRev) << " Found: 0x" << std::hex << lFwInfraRev << ". ";
  }

  // Check for algo mismatch
  if ( lAlgoRev != lFwAlgoRev) {
    lErrMsg << "Algo Firmware mismatch - Expected: 0x" << std::hex << uint32_t(lAlgoRev) << " Found: 0x" << std::hex << lFwAlgoRev << ". ";
  }

  if ( lErrMsg.str().empty() ) return State::kDone;

  setStatusMsg(lErrMsg.str());
  return State::kError;
}
// ----------------------------------------------------------------------------

} // namespace cmds
} // namespace mp7
} // namespace swatch

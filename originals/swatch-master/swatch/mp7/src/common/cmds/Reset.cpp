#include "swatch/mp7/cmds/Reset.hpp"

#include "swatch/mp7/MP7Processor.hpp"
#include "swatch/core/rules/IsAmong.hpp"


namespace swatch {
namespace mp7 {
namespace cmds {

// ----------------------------------------------------------------------------
Reset::Reset(const std::string& aId, swatch::action::ActionableObject& aActionable):
  Command(aId, aActionable, xdata::String())
{
  registerParameter("clockSource", xdata::String("external"), core::rules::IsAmong({"external", "internal"}));
  registerParameter("clockConfig", xdata::String("external"));
  registerParameter("ttcConfig", xdata::String("external"));
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
//--
action::Command::State
Reset::code(const core::XParameterSet& aParams)
{

  // setProgress(0., "Resetting MP7");

  MP7Processor& lProcessor = getActionable<MP7Processor>();
  ::mp7::MP7Controller& lDriver = lProcessor.driver();
  setProgress(0.,"Resetting clocks");

  const xdata::String& clkSrc = aParams.get<xdata::String>("clockSource");
  const xdata::String& clkCfg = aParams.get<xdata::String>("clockConfig");
  const xdata::String& ttcCfg = aParams.get<xdata::String>("ttcConfig");

  if ( clkSrc == "internal" && clkSrc == "external" ) {
    setStatusMsg("Clock can be set only to 'internal' or 'external'");
    return State::kError;
  }

  try {
    // TODO: acquire lock
    // std::string clkSrc = "external";
    lDriver.reset(clkSrc, clkCfg, ttcCfg);
  }
  catch ( ::mp7::exception& e ) {

    setStatusMsg("Reset failed: "+e.description());
    return State::kError;
  }

  // Reset MGTs
  setProgress(0.5,"Resetting MGTs");
  lDriver.channelMgr().resetMGTs();

  // Reset AMC13 block
  setProgress(0.68,"Resetting AMC-AMC13 Link");
  lDriver.getReadout().resetAMC13Block();


  setProgress(0.83,"Resetting AMC-AMC13 Link");
  lDriver.resetPayload();

  setResult(xdata::String("Reset completed"));

  return State::kDone;
}
// ----------------------------------------------------------------------------



} // namespace cmds
} // namespace mp7
} // namespace swatch


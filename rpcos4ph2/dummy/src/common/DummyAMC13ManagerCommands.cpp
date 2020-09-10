
#include "rpcos4ph2/dummy/DummyAMC13ManagerCommands.hpp"


// XDAQ headers
#include "xdata/Vector.h"

// SWATCH headers
#include "swatch/dtm/AMCPortCollection.hpp"
#include "swatch/dtm/AMCPort.hpp"
#include "rpcos4ph2/dummy/DummyAMC13Manager.hpp"
#include "rpcos4ph2/dummy/DummyAMC13Driver.hpp"



namespace rpcos4ph2 {
namespace dummy {


///////////////////////////////
/*  DummyAMC13RebootCommand  */

DummyAMC13RebootCommand::DummyAMC13RebootCommand(const std::string& aId, swatch::action::ActionableObject& aActionable) :
  AbstractConfigureCommand(aId, aActionable)
{
}

DummyAMC13RebootCommand::~DummyAMC13RebootCommand()
{
}

void DummyAMC13RebootCommand::runAction(bool aGoIntoError)
{
  DummyAMC13Driver& lDriver = getActionable<DummyAMC13Manager>().getDriver();
  lDriver.reboot();
}


//////////////////////////////
/*  DummyAMC13ResetCommand  */

DummyAMC13ResetCommand::DummyAMC13ResetCommand(const std::string& aId, swatch::action::ActionableObject& aActionable) :
  AbstractConfigureCommand(aId, aActionable)
{
}

DummyAMC13ResetCommand::~DummyAMC13ResetCommand()
{
}

void DummyAMC13ResetCommand::runAction(bool aGoIntoError)
{
  DummyAMC13Driver& lDriver = getActionable<DummyAMC13Manager>().getDriver();
  if (!aGoIntoError)
    lDriver.reset();
  else
    lDriver.forceClkTtcState(dummy::kError);
}


////////////////////////////////
/*  DummyConfigureEvbCommand  */

DummyAMC13ConfigureEvbCommand::DummyAMC13ConfigureEvbCommand(const std::string& aId, swatch::action::ActionableObject& aActionable) :
  AbstractConfigureCommand(aId, aActionable)
{
}

DummyAMC13ConfigureEvbCommand::~DummyAMC13ConfigureEvbCommand()
{
}

void DummyAMC13ConfigureEvbCommand::runAction(bool aGoIntoError)
{
  DummyAMC13Manager& lMgr = getActionable<DummyAMC13Manager>();
  DummyAMC13Driver& lDriver = lMgr.getDriver();
  if (!aGoIntoError)
    lDriver.configureEvb(lMgr.getStub().fedId);
  else
    lDriver.forceEvbState(dummy::kError);
}


//////////////////////////////////
/*  DummyConfigureSLinkCommand  */

DummyAMC13ConfigureSLinkCommand::DummyAMC13ConfigureSLinkCommand(const std::string& aId, swatch::action::ActionableObject& aActionable) :
  AbstractConfigureCommand(aId, aActionable)
{
}

DummyAMC13ConfigureSLinkCommand::~DummyAMC13ConfigureSLinkCommand()
{
}

void DummyAMC13ConfigureSLinkCommand::runAction(bool aGoIntoError)
{
  DummyAMC13Manager& lMgr = getActionable<DummyAMC13Manager>();
  DummyAMC13Driver& lDriver = lMgr.getDriver();
  if (!aGoIntoError)
    lDriver.configureSLink(lMgr.getStub().fedId);
  else
    lDriver.forceSLinkState(dummy::kError);
}


/////////////////////////////////////
/*  DummyConfigureAMCPortsCommand  */

DummyAMC13ConfigureAMCPortsCommand::DummyAMC13ConfigureAMCPortsCommand(const std::string& aId, swatch::action::ActionableObject& aActionable) :
  AbstractConfigureCommand(aId, aActionable)
{
}

DummyAMC13ConfigureAMCPortsCommand::~DummyAMC13ConfigureAMCPortsCommand()
{
}

void DummyAMC13ConfigureAMCPortsCommand::runAction(bool aGoIntoError)
{
  DummyAMC13Manager& lMgr = getActionable<DummyAMC13Manager>();

  xdata::Vector<xdata::String> lPorts;
  for (auto lIt=lMgr.getAMCPorts().getPorts().begin(); lIt != lMgr.getAMCPorts().getPorts().end(); lIt++) {
    if (!(*lIt)->isMasked())
      lPorts.push_back((*lIt)->getId());
  }
  addExecutionDetails("ports", lPorts);

  DummyAMC13Driver& lDriver = lMgr.getDriver();
  if (!aGoIntoError)
    lDriver.configureAMCPorts();
  else
    lDriver.forceAMCPortState(dummy::kError);
}


////////////////////////////
/*  DummyStartDaqCommand  */

DummyAMC13StartDaqCommand::DummyAMC13StartDaqCommand(const std::string& aId, swatch::action::ActionableObject& aActionable) :
  AbstractConfigureCommand(aId, aActionable)
{
}

DummyAMC13StartDaqCommand::~DummyAMC13StartDaqCommand()
{
}

void DummyAMC13StartDaqCommand::runAction(bool aGoIntoError)
{
  DummyAMC13Driver& lDriver = getActionable<DummyAMC13Manager>().getDriver();
  if (!aGoIntoError)
    lDriver.startDaq();
}


////////////////////////////
/*  DummyStartDaqCommand  */

DummyAMC13StopDaqCommand::DummyAMC13StopDaqCommand(const std::string& aId, swatch::action::ActionableObject& aActionable) :
  AbstractConfigureCommand(aId, aActionable)
{
}

DummyAMC13StopDaqCommand::~DummyAMC13StopDaqCommand()
{
}

void DummyAMC13StopDaqCommand::runAction(bool aGoIntoError)
{
  DummyAMC13Driver& lDriver = getActionable<DummyAMC13Manager>().getDriver();
  if (!aGoIntoError)
    lDriver.stopDaq();
}



/////////////////////////////////////////
/*  DummyAMC13ForceClkTtcStateCommand  */

DummyAMC13ForceClkTtcStateCommand::DummyAMC13ForceClkTtcStateCommand(const std::string& aId, swatch::action::ActionableObject& aActionable) :
  AbstractForceStateCommand(aId, aActionable)
{
}

DummyAMC13ForceClkTtcStateCommand::~DummyAMC13ForceClkTtcStateCommand()
{
}

swatch::action::Command::State DummyAMC13ForceClkTtcStateCommand::code(const swatch::core::XParameterSet& aParamSet)
{
  DummyAMC13Driver& lDriver = getActionable<DummyAMC13Manager>().getDriver();
  lDriver.forceClkTtcState(parseState(aParamSet));
  return kDone;
}


//////////////////////////////////////
/*  DummyAMC13ForceEvbStateCommand  */

DummyAMC13ForceEvbStateCommand::DummyAMC13ForceEvbStateCommand(const std::string& aId, swatch::action::ActionableObject& aActionable) :
  AbstractForceStateCommand(aId, aActionable)
{
}

DummyAMC13ForceEvbStateCommand::~DummyAMC13ForceEvbStateCommand()
{
}

swatch::action::Command::State DummyAMC13ForceEvbStateCommand::code(const swatch::core::XParameterSet& aParamSet)
{
  DummyAMC13Driver& lDriver = getActionable<DummyAMC13Manager>().getDriver();
  lDriver.forceEvbState(parseState(aParamSet));
  return kDone;
}


////////////////////////////////////////
/*  DummyAMC13ForceSLinkStateCommand  */

DummyAMC13ForceSLinkStateCommand::DummyAMC13ForceSLinkStateCommand(const std::string& aId, swatch::action::ActionableObject& aActionable) :
  AbstractForceStateCommand(aId, aActionable)
{
}

DummyAMC13ForceSLinkStateCommand::~DummyAMC13ForceSLinkStateCommand()
{
}

swatch::action::Command::State DummyAMC13ForceSLinkStateCommand::code(const swatch::core::XParameterSet& aParamSet)
{
  DummyAMC13Driver& lDriver = getActionable<DummyAMC13Manager>().getDriver();
  lDriver.forceSLinkState(parseState(aParamSet));
  return kDone;
}


//////////////////////////////////////////
/*  DummyAMC13ForceAMCPortStateCommand  */

DummyAMC13ForceAMCPortStateCommand::DummyAMC13ForceAMCPortStateCommand(const std::string& aId, swatch::action::ActionableObject& aActionable) :
  AbstractForceStateCommand(aId, aActionable)
{
}

DummyAMC13ForceAMCPortStateCommand::~DummyAMC13ForceAMCPortStateCommand()
{
}

swatch::action::Command::State DummyAMC13ForceAMCPortStateCommand::code(const swatch::core::XParameterSet& aParamSet)
{
  DummyAMC13Driver& lDriver = getActionable<DummyAMC13Manager>().getDriver();
  lDriver.forceAMCPortState(parseState(aParamSet));
  return kDone;
}


} // namespace dummy
} // namespace rpcos4ph2

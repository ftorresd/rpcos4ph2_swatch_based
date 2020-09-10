
#include "rpcos4ph2/dummy/DummyProcessorCommands.hpp"


// XDAQ headers
#include "xdata/Vector.h"

// SWATCH headers
#include "rpcos4ph2/dummy/DummyProcessor.hpp"
#include "rpcos4ph2/dummy/DummyProcDriver.hpp"
#include "swatch/processor/Port.hpp"
#include "swatch/processor/PortCollection.hpp"


namespace rpcos4ph2 {
namespace dummy {


//////////////////////////
/*  DummyRebootCommand  */

DummyRebootCommand::DummyRebootCommand(const std::string& aId, swatch::action::ActionableObject& aActionable) :
  AbstractConfigureCommand(aId, aActionable)
{
}

DummyRebootCommand::~DummyRebootCommand()
{
}

void DummyRebootCommand::runAction(bool aGoIntoError)
{
  DummyProcDriver& lDriver = getActionable<DummyProcessor>().getDriver();
  lDriver.reboot();
}


/////////////////////////
/*  DummyResetCommand  */

DummyResetCommand::DummyResetCommand(const std::string& aId, swatch::action::ActionableObject& aActionable) :
  AbstractConfigureCommand(aId, aActionable)
{
}

DummyResetCommand::~DummyResetCommand()
{
}

void DummyResetCommand::runAction(bool aGoIntoError)
{
  DummyProcDriver& lDriver = getActionable<DummyProcessor>().getDriver();
  if (!aGoIntoError)
    lDriver.reset();
}


///////////////////////////////
/*  DummyConfigureTxCommand  */

DummyConfigureTxCommand::DummyConfigureTxCommand(const std::string& aId, swatch::action::ActionableObject& aActionable) :
  AbstractConfigureCommand(aId, aActionable)
{
}

DummyConfigureTxCommand::~DummyConfigureTxCommand()
{
}

void DummyConfigureTxCommand::runAction(bool aGoIntoError)
{
  DummyProcessor& lProc = getActionable<DummyProcessor>();

  xdata::Vector<xdata::String> lPorts;
  for (auto lIt=lProc.getOutputPorts().getPorts().begin(); lIt != lProc.getOutputPorts().getPorts().end(); lIt++)
    lPorts.push_back((*lIt)->getId());
  addExecutionDetails("ports", lPorts);

  DummyProcDriver& lDriver = lProc.getDriver();
  if (!aGoIntoError)
    lDriver.configureTxPorts();
}


///////////////////////////////
/*  DummyConfigureRxCommand  */

DummyConfigureRxCommand::DummyConfigureRxCommand(const std::string& aId, swatch::action::ActionableObject& aActionable) :
  AbstractConfigureCommand(aId, aActionable)
{
}

DummyConfigureRxCommand::~DummyConfigureRxCommand()
{
}

void DummyConfigureRxCommand::runAction(bool aGoIntoError)
{
  DummyProcessor& lProc = getActionable<DummyProcessor>();

  xdata::Vector<xdata::String> lPorts;
  for (auto lIt=lProc.getInputPorts().getPorts().begin(); lIt != lProc.getInputPorts().getPorts().end(); lIt++) {
    if (!(*lIt)->isMasked())
      lPorts.push_back((*lIt)->getId());
  }
  addExecutionDetails("ports", lPorts);

  DummyProcDriver& lDriver = lProc.getDriver();
  if (!aGoIntoError)
    lDriver.configureRxPorts();
}


////////////////////////////////
/*  DummyConfigureDaqCommand  */

DummyConfigureDaqCommand::DummyConfigureDaqCommand(const std::string& aId, swatch::action::ActionableObject& aActionable) :
  AbstractConfigureCommand(aId, aActionable)
{
}

DummyConfigureDaqCommand::~DummyConfigureDaqCommand()
{
}

void DummyConfigureDaqCommand::runAction(bool aGoIntoError)
{
  DummyProcDriver& lDriver = getActionable<DummyProcessor>().getDriver();
  if (!aGoIntoError)
    lDriver.configureReadout();
}


/////////////////////////////////
/*  DummyConfigureAlgoCommand  */

DummyConfigureAlgoCommand::DummyConfigureAlgoCommand(const std::string& aId, swatch::action::ActionableObject& aActionable) :
  AbstractConfigureCommand(aId, aActionable)
{
}

DummyConfigureAlgoCommand::~DummyConfigureAlgoCommand()
{
}

void DummyConfigureAlgoCommand::runAction(bool aGoIntoError)
{
  DummyProcDriver& lDriver = getActionable<DummyProcessor>().getDriver();
  if (!aGoIntoError)
    lDriver.configureAlgo();
}


/////////////////////////////////////////////
/*  DummyProcessorForceClkTtcStateCommand  */

DummyProcessorForceClkTtcStateCommand::DummyProcessorForceClkTtcStateCommand(const std::string& aId, swatch::action::ActionableObject& aActionable) :
  AbstractForceStateCommand(aId, aActionable)
{
}

DummyProcessorForceClkTtcStateCommand::~DummyProcessorForceClkTtcStateCommand()
{
}

swatch::action::Command::State DummyProcessorForceClkTtcStateCommand::code(const swatch::core::XParameterSet& aParamSet)
{
  DummyProcDriver& lDriver = getActionable<DummyProcessor>().getDriver();
  lDriver.forceClkTtcState(parseState(aParamSet));
  return kDone;
}


//////////////////////////////////////////////
/*  DummyProcessorForceRxPortsStateCommand  */

DummyProcessorForceRxPortsStateCommand::DummyProcessorForceRxPortsStateCommand(const std::string& aId, swatch::action::ActionableObject& aActionable) :
  AbstractForceStateCommand(aId, aActionable)
{
}

DummyProcessorForceRxPortsStateCommand::~DummyProcessorForceRxPortsStateCommand()
{
}

swatch::action::Command::State DummyProcessorForceRxPortsStateCommand::code(const swatch::core::XParameterSet& aParamSet)
{
  DummyProcDriver& lDriver = getActionable<DummyProcessor>().getDriver();
  lDriver.forceRxPortsState(parseState(aParamSet));
  return kDone;
}


//////////////////////////////////////////////
/*  DummyProcessorForceTxPortsStateCommand  */

DummyProcessorForceTxPortsStateCommand::DummyProcessorForceTxPortsStateCommand(const std::string& aId, swatch::action::ActionableObject& aActionable) :
  AbstractForceStateCommand(aId, aActionable)
{
}

DummyProcessorForceTxPortsStateCommand::~DummyProcessorForceTxPortsStateCommand()
{
}

swatch::action::Command::State DummyProcessorForceTxPortsStateCommand::code(const swatch::core::XParameterSet& aParamSet)
{
  DummyProcDriver& lDriver = getActionable<DummyProcessor>().getDriver();
  lDriver.forceTxPortsState(parseState(aParamSet));
  return kDone;
}


//////////////////////////////////////////////
/*  DummyProcessorForceReadoutStateCommand  */

DummyProcessorForceReadoutStateCommand::DummyProcessorForceReadoutStateCommand(const std::string& aId, swatch::action::ActionableObject& aActionable) :
  AbstractForceStateCommand(aId, aActionable)
{
}

DummyProcessorForceReadoutStateCommand::~DummyProcessorForceReadoutStateCommand()
{
}

swatch::action::Command::State DummyProcessorForceReadoutStateCommand::code(const swatch::core::XParameterSet& aParamSet)
{
  DummyProcDriver& lDriver = getActionable<DummyProcessor>().getDriver();
  lDriver.forceReadoutState(parseState(aParamSet));
  return kDone;
}


///////////////////////////////////////////
/*  DummyProcessorForceAlgoStateCommand  */

DummyProcessorForceAlgoStateCommand::DummyProcessorForceAlgoStateCommand(const std::string& aId, swatch::action::ActionableObject& aActionable) :
  AbstractForceStateCommand(aId, aActionable)
{
}

DummyProcessorForceAlgoStateCommand::~DummyProcessorForceAlgoStateCommand()
{
}

swatch::action::Command::State DummyProcessorForceAlgoStateCommand::code(const swatch::core::XParameterSet& aParamSet)
{
  DummyProcDriver& lDriver = getActionable<DummyProcessor>().getDriver();
  lDriver.forceAlgoState(parseState(aParamSet));
  return kDone;
}



} // namespace dummy
} // namespace rpcos4ph2

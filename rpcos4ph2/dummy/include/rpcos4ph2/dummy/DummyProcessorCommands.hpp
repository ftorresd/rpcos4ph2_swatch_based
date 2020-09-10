/**
 * @file    DummyProcessorCommands.hpp
 * @author  Alessandro Thea
 * @brief   ActionableObject class
 * @date    February 2015
 *
 */

#ifndef _RPCOS4PH2_DUMMY_DUMMYPROCESSORCOMMANDS_HPP__
#define _RPCOS4PH2_DUMMY_DUMMYPROCESSORCOMMANDS_HPP__


#include "rpcos4ph2/dummy/AbstractConfigureCommand.hpp"
#include "rpcos4ph2/dummy/AbstractForceStateCommand.hpp"


namespace rpcos4ph2 {
namespace dummy {


class DummyRebootCommand : public AbstractConfigureCommand {
public:
  DummyRebootCommand(const std::string& aId, swatch::action::ActionableObject& aActionable);
  ~DummyRebootCommand();

private:
  void runAction(bool aErrorOccurs);
};

class DummyResetCommand : public AbstractConfigureCommand {
public:
  DummyResetCommand(const std::string& aId, swatch::action::ActionableObject& aActionable);
  ~DummyResetCommand();

private:
  void runAction(bool aErrorOccurs);
};

class DummyConfigureTxCommand : public AbstractConfigureCommand {
public:
  DummyConfigureTxCommand(const std::string& aId, swatch::action::ActionableObject& aActionable);
  ~DummyConfigureTxCommand();

private:
  void runAction(bool aErrorOccurs);
};

class DummyConfigureRxCommand : public AbstractConfigureCommand {
public:
  DummyConfigureRxCommand(const std::string& aId, swatch::action::ActionableObject& aActionable);
  ~DummyConfigureRxCommand();

private:
  void runAction(bool aErrorOccurs);
};


class DummyConfigureDaqCommand : public AbstractConfigureCommand {
public:
  DummyConfigureDaqCommand(const std::string& aId, swatch::action::ActionableObject& aActionable);
  ~DummyConfigureDaqCommand();

private:
  void runAction(bool aErrorOccurs);
};


class DummyConfigureAlgoCommand : public AbstractConfigureCommand {
public:
  DummyConfigureAlgoCommand(const std::string& aId, swatch::action::ActionableObject& aActionable);
  ~DummyConfigureAlgoCommand();

private:
  void runAction(bool aErrorOccurs);
};


class DummyProcessorForceClkTtcStateCommand : public AbstractForceStateCommand {
public:
  DummyProcessorForceClkTtcStateCommand(const std::string& aId, swatch::action::ActionableObject& aActionable);
  ~DummyProcessorForceClkTtcStateCommand();

private:
  State code(const swatch::core::XParameterSet& aParamSet);
};


class DummyProcessorForceRxPortsStateCommand : public AbstractForceStateCommand {
public:
  DummyProcessorForceRxPortsStateCommand(const std::string& aId, swatch::action::ActionableObject& aActionable);
  ~DummyProcessorForceRxPortsStateCommand();

private:
  State code(const swatch::core::XParameterSet& aParamSet);
};


class DummyProcessorForceTxPortsStateCommand : public AbstractForceStateCommand {
public:
  DummyProcessorForceTxPortsStateCommand(const std::string& aId, swatch::action::ActionableObject& aActionable);
  ~DummyProcessorForceTxPortsStateCommand();

private:
  State code(const swatch::core::XParameterSet& aParamSet);
};


class DummyProcessorForceReadoutStateCommand : public AbstractForceStateCommand {
public:
  DummyProcessorForceReadoutStateCommand(const std::string& aId, swatch::action::ActionableObject& aActionable);
  ~DummyProcessorForceReadoutStateCommand();

private:
  State code(const swatch::core::XParameterSet& aParamSet);
};


class DummyProcessorForceAlgoStateCommand : public AbstractForceStateCommand {
public:
  DummyProcessorForceAlgoStateCommand(const std::string& aId, swatch::action::ActionableObject& aActionable);
  ~DummyProcessorForceAlgoStateCommand();

private:
  State code(const swatch::core::XParameterSet& aParamSet);
};



} // namespace dummy
} // namespace rpcos4ph2

#endif /* _RPCOS4PH2_DUMMY_DUMMYPROCESSORCOMMAND_HPP__ */

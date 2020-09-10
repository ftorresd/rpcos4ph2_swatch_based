/*
 * File:   AbstractConfigureCommand.hpp
 * Author: tom
 * Date:   February 2016
 */

#ifndef _RPCOS4PH2_DUMMY_ABSTRACTCONFIGURECOMMAND_HPP__
#define _RPCOS4PH2_DUMMY_ABSTRACTCONFIGURECOMMAND_HPP__


#include "swatch/action/Command.hpp"


namespace rpcos4ph2 {
namespace dummy {


class AbstractConfigureCommand : public swatch::action::Command {
public:
  AbstractConfigureCommand(const std::string& aId, swatch::action::ActionableObject& aActionable);
  virtual ~AbstractConfigureCommand();

protected:
  State code(const swatch::core::XParameterSet& aParams);

  virtual void runAction(bool aErrorOccurs) = 0;
};


} // end ns: dummy
} // end ns: swatch

#endif  /* _SWATCH_DUMMY_ABSTRACTCONFIGURECOMMAND_HPP_ */


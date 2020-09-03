/*
 * File:   AbstractConfigureCommand.hpp
 * Author: tom
 * Date:   February 2016
 */

#ifndef __SWATCH_DUMMY_ABSTRACTCONFIGURECOMMAND_HPP__
#define __SWATCH_DUMMY_ABSTRACTCONFIGURECOMMAND_HPP__


#include "swatch/action/Command.hpp"


namespace swatch {
namespace dummy {


class AbstractConfigureCommand : public action::Command {
public:
  AbstractConfigureCommand(const std::string& aId, action::ActionableObject& aActionable);
  virtual ~AbstractConfigureCommand();

protected:
  State code(const core::XParameterSet& aParams);

  virtual void runAction(bool aErrorOccurs) = 0;
};


} // end ns: dummy
} // end ns: swatch

#endif  /* _SWATCH_DUMMY_ABSTRACTCONFIGURECOMMAND_HPP_ */


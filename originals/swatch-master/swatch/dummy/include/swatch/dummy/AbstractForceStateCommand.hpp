/*
 * File:   AbstractForceStateCommand.hpp
 * Author: tom
 * Date:   February 2016
 */

#ifndef __SWATCH_DUMMY_ABSTRACTFORCESTATECOMMAND_HPP__
#define __SWATCH_DUMMY_ABSTRACTFORCESTATECOMMAND_HPP__


#include "swatch/action/Command.hpp"
#include "swatch/dummy/ComponentState.hpp"


namespace swatch {
namespace dummy {


class AbstractForceStateCommand : public action::Command {
public:
  AbstractForceStateCommand(const std::string& aId, action::ActionableObject& aActionable);
  virtual ~AbstractForceStateCommand();

protected:
  static ComponentState parseState(const core::XParameterSet& aParameSet);
};


} // end ns: dummy
} // end ns: swatch

#endif  /* _SWATCH_DUMMY_ABSTRACTFORCESTATECOMMAND_HPP_ */


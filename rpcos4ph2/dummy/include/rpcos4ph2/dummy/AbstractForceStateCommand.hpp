/*
 * File:   AbstractForceStateCommand.hpp
 * Author: tom
 * Date:   February 2016
 */

#ifndef _RPCOS4PH2_DUMMY_ABSTRACTFORCESTATECOMMAND_HPP__
#define _RPCOS4PH2_DUMMY_ABSTRACTFORCESTATECOMMAND_HPP__


#include "swatch/action/Command.hpp"
#include "rpcos4ph2/dummy/ComponentState.hpp"


namespace rpcos4ph2 {
namespace dummy {


class AbstractForceStateCommand : public swatch::action::Command {
public:
  AbstractForceStateCommand(const std::string& aId, swatch::action::ActionableObject& aActionable);
  virtual ~AbstractForceStateCommand();

protected:
  static ComponentState parseState(const swatch::core::XParameterSet& aParameSet);
};


} // end ns: dummy
} // end ns: swatch

#endif  /* _SWATCH_DUMMY_ABSTRACTFORCESTATECOMMAND_HPP_ */


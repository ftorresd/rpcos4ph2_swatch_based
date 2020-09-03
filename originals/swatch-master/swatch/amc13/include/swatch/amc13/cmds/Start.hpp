#ifndef __SWATCH_AMC13_CMDS_START_HPP__
#define __SWATCH_AMC13_CMDS_START_HPP__

#include <string>                       // for string

#include "swatch/action/Command.hpp"      // for swatch::action::Command


namespace swatch {
namespace amc13 {
namespace cmds {

class Start : public swatch::action::Command {
public:
  
  /**
   * @brief      Constructor.
   *
   * @details    Defines input parameters
   *             - bx (uint)
   *             - cycle (uint)
   *
   * @param[in]  aId          Command identifier
   * @param      aActionable  Target actionable object
   */
  Start(const std::string& aId, swatch::action::ActionableObject& aActionable);

  /**
   * @brief      Destroys the object.
   */
  virtual ~Start() {}

  /**
   * @brief      Aligns MP7 MGTs to a selected bunch crossing/cycle.
   *
   * @param[in]  aParams  Input parameters
   *
   * @return     Command execution status
   */
  virtual State code(const core::XParameterSet& aParams);

};

} // namespace cmds
} // namespace amc13
} // namespace swatch


#endif /* __SWATCH_AMC13_CMDS_START_HPP__ */
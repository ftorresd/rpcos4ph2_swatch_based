#ifndef __SWATCH_MP7_CMDS_CLEARCOUNTERSCOMMAND_HPP__
#define __SWATCH_MP7_CMDS_CLEARCOUNTERSCOMMAND_HPP__

#include "swatch/action/Command.hpp"
  
namespace swatch {
namespace mp7 {
namespace cmds {

/**
 * @brief      Class for clear counters command.
 */
class ClearCounters : public action::Command {
public:
  /**
   * @brief      Constructor.
   *
   * @details    No input parameters
   *
   * @param[in]  aId          Command identifier
   * @param      aActionable  Target actionable object
   */
  ClearCounters(const std::string& aId, action::ActionableObject& aActionable);
  
  /**
   * @brief      Destroys the object.
   */
  virtual ~ClearCounters() {}
  
  /**
   * @brief      Clears board counters.
   *
   * @param[in]  aParams  Input parameters
   *
   * @return     Command execution status
   */
  virtual State code(const core::XParameterSet& aParams);
};

} // namespace cmds
} // namespace mp7
} // namespace swatch

#endif /* __SWATCH_MP7_CMDS_CLEARCOUNTERSCOMMAND_HPP__ */
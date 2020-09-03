#ifndef __SWATCH_MP7_CMDS_RESETCOMMAND_HPP__
#define __SWATCH_MP7_CMDS_RESETCOMMAND_HPP__

#include "swatch/action/Command.hpp"


namespace swatch {
namespace mp7 {
namespace cmds {

/**
 * @brief      Class for reset command.
 */
class Reset : public action::Command {
public:

  /**
   * @brief      Constructor.
   *
   * @details    Defines input parameters
   *             - clockSource (str)
   *             - clockConfig (str)
   *             - ttcConfig (str)
   *
   * @param[in]  aId          Command identifier
   * @param      aActionable  Target actionable object
   */
  Reset(const std::string& aId, action::ActionableObject& aActionable);
  
  /**
   * @brief      Destroys the object.
   */
  virtual ~Reset() {}
  
  /**
   * @brief      Issues a global reset to the MP7 and configures clocking and
   *             ttc infrastructure
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



#endif /* __SWATCH_MP7_CMDS_RESETCOMMAND_HPP__ */
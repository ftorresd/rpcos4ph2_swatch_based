#ifndef __SWATCH_MP7_CMDS_XXX_HPP__
#define __SWATCH_MP7_CMDS_XXX_HPP__

#include "swatch/action/Command.hpp"


namespace swatch {
namespace mp7 {
namespace cmds {

class SetID : public action::Command {
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
  SetID(const std::string& aId, action::ActionableObject& aActionable);

  /**
   * @brief      Destroys the object.
   */
  ~SetID() {}

  /**
   * @brief      Configured the board id register.
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



#endif /* __SWATCH_MP7_CMDS_XXX_HPP__ */
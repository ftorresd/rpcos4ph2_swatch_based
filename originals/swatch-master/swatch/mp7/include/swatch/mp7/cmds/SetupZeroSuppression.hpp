#ifndef __SWATCH_MP7_CMDS_SetupZeroSuppression_HPP__
#define __SWATCH_MP7_CMDS_SetupZeroSuppression_HPP__

#include <string>                       // for string

#include "swatch/action/Command.hpp"


namespace swatch {
namespace mp7 {
namespace cmds {

/**
 * @brief      Class for setup zero suppression.
 */
class SetupZeroSuppression : public action::Command {
public:

  /**
   * @brief      Constructor.
   *
   * @details    Defines input parameters
   *             - enableZS (bool)
   *
   * @param[in]  aId          Command identifier
   * @param      aActionable  Target actionable object
   */
  SetupZeroSuppression( const std::string& aId, swatch::action::ActionableObject& aActionable );

  /**
   * @brief      Destroys the object.
   */
  virtual ~SetupZeroSuppression() {}

  /**
   * @brief      Configures MP7 zero suppression block.
   *
   * @param[in]  aParams  Input parameters
   *
   * @return     Command execution status
   */
  virtual action::Command::State code(const ::swatch::core::XParameterSet& aParams);

private:
  bool mZSInstantiated;
};

} // namespace cmds
} // namespace mp7
} // namespace swatch



#endif /* __SWATCH_MP7_CMDS_SetupZeroSuppression_HPP__ */
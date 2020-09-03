#ifndef __SWATCH_AMC13_CMDS_RESET_HPP__
#define __SWATCH_AMC13_CMDS_RESET_HPP__

#include <string>                       // for string

#include "swatch/action/Command.hpp"      // for swatch::action::Command

  
namespace swatch {
namespace amc13 {
namespace cmds {

class Reset : public swatch::action::Command {
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
  Reset(const std::string& aId, swatch::action::ActionableObject& aActionable);

  /**
   * @brief      Destroys the object.
   */
  virtual ~Reset() {}

  /**
   * @brief      Performs a soft reset of "known" AMC13 registers.
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


#endif /* __SWATCH_AMC13_CMDS_RESET_HPP__ */
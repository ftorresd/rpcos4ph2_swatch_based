#ifndef __SWATCH_AMC13_CMDS_REBOOT_HPP__
#define __SWATCH_AMC13_CMDS_REBOOT_HPP__

#include <string>                       // for string

#include "swatch/action/Command.hpp"      // for swatch::action::Command

  
namespace swatch {
namespace amc13 {
namespace cmds {

class Reboot : public swatch::action::Command {
public:
  /**
   * @brief      Constructor.
   *
   * @details    Defines input parameters
   *             - timeout (uint)
   *             
   * @param[in]  aId          Command identifier
   * @param      aActionable  Target actionable object
   */
  Reboot(const std::string& aId, swatch::action::ActionableObject& aActionable);

  /**
   * @brief      Destroys the object.
   */
  virtual ~Reboot() {}

  /**
   * @brief      Reload the firmware of the T1 and T2 AMC13 FPGAs.
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


#endif /* __SWATCH_AMC13_CMDS_REBOOT_HPP__ */
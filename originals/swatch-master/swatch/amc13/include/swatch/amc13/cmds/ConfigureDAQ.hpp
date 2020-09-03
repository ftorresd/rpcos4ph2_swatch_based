#ifndef __SWATCH_AMC13_CMDS_CONFIGUREDAQ_HPP__
#define __SWATCH_AMC13_CMDS_CONFIGUREDAQ_HPP__

#include <string>                       // for string

#include "swatch/action/Command.hpp"      // for swatch::action::Command


namespace swatch {
namespace amc13 {
namespace cmds {

class ConfigureDAQ : public swatch::action::Command {
public:

  /**
   * @brief      Constructor.
   *
   * @details    Defines input parameters
   *             - slinkMask (uint)
   *             - bcnOffset (uint)
   *
   * @param[in]  aId          Command identifier
   * @param      aActionable  Target actionable object
   */
  ConfigureDAQ(const std::string& aId, swatch::action::ActionableObject& aActionable);

  /**
   * @brief      Destroys the object.
   */
  virtual ~ConfigureDAQ() {}

  /**
   * @brief      Configures AMC13's DAQ firmware
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


#endif /* __SWATCH_AMC13_CMDS_CONFIGUREDAQ_HPP__ */
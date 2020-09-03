#ifndef __SWATCH_MP7_CMDS_CONFIGURERXMGTSCOMMAND_HPP__
#define __SWATCH_MP7_CMDS_CONFIGURERXMGTSCOMMAND_HPP__

#include "swatch/action/Command.hpp"

#include "swatch/mp7/IOChannelSelector.hpp"


namespace swatch {
namespace mp7 {
namespace cmds {

class ConfigureRxMGTs : public swatch::action::Command {
public:
  /**
   * @brief      Constructor.
   *
   * @details    Defines input parameters
   *             - orbitTag (bool)
   *             - polarity (bool)
   *
   * @param[in]  aId          Command identifier
   * @param      aActionable  Target actionable object
   */
    ConfigureRxMGTs(const std::string& aId, swatch::action::ActionableObject& aActionable);

  /**
   * @brief      Destroys the object.
   */
  virtual ~ConfigureRxMGTs() {}

  /**
   * @brief      Configures 10Gb Rx ports 
   *
   * @param[in]  aParams  Input parameters
   *
   * @return     Command execution status
   */

  virtual action::Command::State code(const core::XParameterSet& aParams);

private:

  RxMGTSelector mRxMGTSelector;
};

} // namespace cmds
} // namespace mp7
} // namespace swatch

#endif /* __SWATCH_MP7_CMDS_CONFIGURERXMGTSCOMMAND_HPP__ */
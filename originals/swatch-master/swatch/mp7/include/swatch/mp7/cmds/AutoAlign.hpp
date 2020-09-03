#ifndef __SWATCH_MP7_CMDS_AUTOALIGNCOMMAND_HPP__
#define __SWATCH_MP7_CMDS_AUTOALIGNCOMMAND_HPP__

#include <string>                       // for string

#include "swatch/action/Command.hpp"

#include "swatch/mp7/IOChannelSelector.hpp"


namespace swatch {
namespace mp7 {
namespace cmds {

class AutoAlign : public swatch::action::Command {
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
  AutoAlign(const std::string& aId, swatch::action::ActionableObject& aActionable);

  /**
   * @brief      Destroys the object.
   */
  virtual ~AutoAlign() {}

  /**
   * @brief      Aligns MP7 RX mgts to the minimum possible value.
   *
   * @param[in]  aParams  Input parameters
   *
   * @return     Command execution status
   */
  virtual action::Command::State code (const core::XParameterSet& aParams);

private:
  
  RxMGTSelector mRxMGTSelector;

};

} // namespace cmds
} // namespace mp7
} // namespace swatch


#endif /* __SWATCH_MP7_CMDS_AUTOALIGNCOMMAND_HPP__ */
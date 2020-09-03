#ifndef __SWATCH_MP7_CMDS_LOADZEROSUPPRESSIONMENU_HPP__
#define __SWATCH_MP7_CMDS_LOADZEROSUPPRESSIONMENU_HPP__

#include <string>                       // for string

#include "swatch/action/Command.hpp"


namespace swatch {
namespace mp7 {
namespace cmds {

class LoadZeroSuppressionMenu : public action::Command {
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
  LoadZeroSuppressionMenu( const std::string& aId, swatch::action::ActionableObject& aActionable );

  /**
   * @brief      Destroys the object.
   */
  virtual ~LoadZeroSuppressionMenu() {}

  /** 
   * @brief      { function_description }
   *
   * @param[in]  aParams  Input parameters
   *
   * @return     Command execution status
   */
  virtual action::Command::State code(const ::swatch::core::XParameterSet& aParams);

private:
  bool mZSInstantiated;
  uint32_t mNumCaptureIds;
};

} // namespace cmds
} // namespace mp7
} // namespace swatch



#endif /* __SWATCH_MP7_CMDS_LOADZEROSUPPRESSIONMENU_HPP__ */
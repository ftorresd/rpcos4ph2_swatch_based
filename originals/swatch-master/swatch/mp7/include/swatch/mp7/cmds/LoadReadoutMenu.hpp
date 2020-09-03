#ifndef __SWATCH_MP7_CMDS_LOADREADOUTMENU_HPP__
#define __SWATCH_MP7_CMDS_LOADREADOUTMENU_HPP__

#include <string>                       // for string

#include "swatch/action/Command.hpp"


namespace swatch {
namespace mp7 {
namespace cmds {

class LoadReadoutMenu : public action::Command {
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
  LoadReadoutMenu( const std::string& aId, swatch::action::ActionableObject& aActionable );
  
  /**
   * @brief      Destroys the object.
   */
  virtual ~LoadReadoutMenu();
  
  /**
   * @brief      { function_description }
   *
   * @param[in]  aParams  A parameters
   *
   * @return     { description_of_the_return_value }
   */
  virtual action::Command::State code(const ::swatch::core::XParameterSet& aParams);

private:
  uint32_t mBanks;
  uint32_t mModes;
  uint32_t mCaptures;
};

} // namespace cmds
} // namespace mp7
} // namespace swatch



#endif /* __SWATCH_MP7_CMDS_LOADREADOUTMENU_HPP__ */
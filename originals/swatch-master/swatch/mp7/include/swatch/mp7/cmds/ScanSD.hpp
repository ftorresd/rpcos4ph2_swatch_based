#ifndef __SWATCH_MP7_CMDS_SCANSDCOMMAND_HPP__
#define __SWATCH_MP7_CMDS_SCANSDCOMMAND_HPP__

#include <string>                       // for string

#include "swatch/action/Command.hpp"

#include "boost/algorithm/string/join.hpp"


namespace swatch {
namespace mp7 {
namespace cmds {

class ScanSD : public ::swatch::action::Command {
public:
  /**
   * @brief      Constructor.
   *
   * @details    No input parameters
   *
   * @param[in]  aId          Command identifier
   * @param      aActionable  Target actionable object
   */  
  ScanSD(const std::string& aId, swatch::action::ActionableObject& aActionable);
  
  /**
   * @brief      Destroys the object.
   */
  virtual ~ScanSD() {}

  /**
   * @brief      Lists the content of the MP7 SD card.
   *
   * @param[in]  aParams  Input parameters
   *
   * @return     Command execution status
   */
  virtual State code(const ::swatch::core::XParameterSet& aParams);
};

} // namespace cmds
} // namespace mp7
} // namespace swatch



#endif /* __SWATCH_MP7_CMDS_SCANSDCOMMAND_HPP__ */
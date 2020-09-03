#ifndef __SWATCH_MP7_CMDS_CHECKFIRMWAREREVISIONCOMMAND_HPP__
#define __SWATCH_MP7_CMDS_CHECKFIRMWAREREVISIONCOMMAND_HPP__

#include "swatch/action/Command.hpp"

namespace swatch {
namespace mp7 {
namespace cmds {

/**
 * @brief      Class for check firmware revision command.
 */
class CheckFirmwareRevision : public action::Command {
public:
  /**
   * @brief      Constructor
   *
   * @details    Defines input parameters
   *             - infraRev (uint)
   *             - algoRev (uint)
   *             
   * @param[in]  aId          Command identifier
   * @param      aActionable  Target actionable object
   */
  CheckFirmwareRevision( const std::string& aId, action::ActionableObject& aActionable);

  /**
   * @brief      Destroys the object.
   */
  virtual ~CheckFirmwareRevision() {}
  
  /**
   * @brief      Compares firmware revision for 
   *
   * @param[in]  aParams  A parameters
   *
   * @return     Command execution status
   */
  virtual State code(const core::XParameterSet& aParams);
};

} // namespace cmds
} // namespace mp7
} // namespace swatch

#endif /* __SWATCH_MP7_CMDS_CHECKFIRMWAREREVISIONCOMMAND_HPP__ */
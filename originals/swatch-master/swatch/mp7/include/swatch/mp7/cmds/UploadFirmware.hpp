#ifndef __SWATCH_MP7_CMDS_UPLOADFIRMWARECOMMAND_HPP__
#define __SWATCH_MP7_CMDS_UPLOADFIRMWARECOMMAND_HPP__

#include <string>                       // for string

#include "swatch/action/Command.hpp"


namespace swatch {
namespace mp7 {
namespace cmds {

/**
 * @class      UploadFirmwareCommand
 *
 * Upload firmare image on the on-board SC cards
 */
class UploadFirmware : public ::swatch::action::Command {
public:

 /**
   * @brief      Constructor.
   *
   * @details    Defines input parameters
   *             - localfile (str)
   *             - sdfile (str)
   *
   * @param[in]  aId          Command identifier
   * @param      aActionable  Target actionable object
   */
  UploadFirmware(const std::string& aId, swatch::action::ActionableObject& aActionable);

  /**
   * @brief      Destroys the object.
   */
  virtual ~UploadFirmware() {}

  /**
   * @brief      Uploads a firmware image to the MP7 SD card.
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



#endif /* __SWATCH_MP7_CMDS_UPLOADFIRMWARECOMMAND_HPP__ */
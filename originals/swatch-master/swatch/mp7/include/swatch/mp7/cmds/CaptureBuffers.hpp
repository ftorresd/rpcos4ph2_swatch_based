#ifndef __SWATCH_MP7_CAPTUREBUFFERCOMMAND_HPP__
#define __SWATCH_MP7_CAPTUREBUFFERCOMMAND_HPP__

// MP7 headers
#include "mp7/definitions.hpp"
#include "mp7/PathConfigurator.hpp"

// SWATCH headers
#include "swatch/action/Command.hpp"      // for Command

// SWATCH MP7 headers
#include "swatch/mp7/IOChannelSelector.hpp"

namespace swatch {
namespace mp7 {
namespace cmds {


/**
 * @class      CaptureBuffersCommand
 *
 * @brief      Class for capture buffers command.
 */
class CaptureBuffers : public swatch::action::Command {
public:
  
  /**
   * @brief      Constructor.
   *
   * @details    No input parameters.
   *
   * @param[in]  aId          Command identifier
   * @param      aActionable  Target actionable object
   */
  CaptureBuffers(const std::string& aId, swatch::action::ActionableObject& aActionable);

  /**
   * @brief      Destroys the object.
   */
  virtual ~CaptureBuffers() {}

  /**
   * @brief      Issues a capture signal to all link buffers.
   *
   * @param[in]  aParams  Input parameters
   *
   * @return     Command execution status
   */
  virtual swatch::action::Command::State code(const ::swatch::core::XParameterSet& aParams);
};

} // namespace cmds
} // namespace mp7
} // namespace swatch
#endif /* __SWATCH_MP7_CAPTUREBUFFERCOMMAND_HPP__ */
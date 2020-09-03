#ifndef __SWATCH_MP7_CMDS_ZEROINPUTS_HPP__
#define __SWATCH_MP7_CMDS_ZEROINPUTS_HPP__

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
 * @class      ZeroInputs
 *
 * @brief      Class for zero inputs command.
 */
class ZeroInputs : public swatch::action::Command {
public:

  /**
   * @brief      Construtor.
   *
   * @param[in]  aId          Command identifier
   * @param      aActionable  Target actionable object
   */
  ZeroInputs(const std::string& aId, swatch::action::ActionableObject& aActionable);

  /**
   * @brief      Destroys the object.
   */
  virtual ~ZeroInputs() {}

  /**
   * @brief      Configure all known MP7 Rx buffers to send zeroes to the algorithm block.
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

#endif /* __SWATCH_MP7_CMDS_ZEROINPUTS_HPP__ */

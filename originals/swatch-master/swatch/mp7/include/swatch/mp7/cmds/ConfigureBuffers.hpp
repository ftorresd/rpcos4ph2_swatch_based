#ifndef __SWATCH_MP7_CMDS_CONFIGUREBUFFERS_HPP__
#define __SWATCH_MP7_CMDS_CONFIGUREBUFFERS_HPP__

#include <stdint.h>                     // for uint32_t
#include <map>                          // for map
#include <string>                       // for string

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
 * @class      ConfigureBuffersCommand
 *
 * @brief      Command to configure the MP7 buffers
 *
 * @tparam     Selector  Channel Selector type
 */
template<class Selector>
class ConfigureBuffers : public swatch::action::Command {
public:

  /**
   * @brief      Constructor.
   *
   * @details    Defines input parameters
   *             - startBx (uint) : 0-3563
   *             - startCycle (uint) : 0-5
   *             - stopBx (uint) : 0-3563
   *             - stopCycle (uint) : 0-5
   *             - payload (str) : Pattern filename for 
   *             - mode (str) : Latency, Capture, PlayOnce, PlayLoop, Pattern,
   *               Zeroes, CaptureStrobe, Pattern3G, PlayOnceStrobe, PlayOnce3G
   *
   * @param[in]  aId          Command identifier
   * @param      aActionable  Target actionable object
   */
  ConfigureBuffers(const std::string& aId, swatch::action::ActionableObject& aActionable);

  /**
   * @brief      Destroys the object.
   */
  virtual ~ConfigureBuffers() {}

  /**
   * @brief      Configures the MP7 buffers.
   *
   * @param[in]  aParams  Input parameters
   *
   * @return     Command execution status
   */
  virtual swatch::action::Command::State code(const ::swatch::core::XParameterSet& aParams);

protected:
  
  static const std::map< std::string, ::mp7::TestPathConfigurator::Mode > kBufferModeMap;

  Selector mBufferSelector;
};

typedef ConfigureBuffers<RxBufferSelector> ConfigureRxBuffers;
typedef ConfigureBuffers<TxBufferSelector> ConfigureTxBuffers;

} // namespace cmds
} // namespace mp7
} // namespace swatch
  
#endif /* __SWATCH_MP7_CMDS_CONFIGUREBUFFERS_HPP__ */

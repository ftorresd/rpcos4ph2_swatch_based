#ifndef __SWATCH_MP7_CMDS_LATENCYBUFFERSCOMMAND_HPP__
#define __SWATCH_MP7_CMDS_LATENCYBUFFERSCOMMAND_HPP__

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
 * Command template to configure buffers in latency mode.
 *
 * @tparam     Selector  { description }
 */
template<class Selector>
class LatencyBuffers : public swatch::action::Command {
public:
  /**
   */
  LatencyBuffers(const std::string& aId, swatch::action::ActionableObject& aActionable);

  /**
   * Destructor
   */
  virtual ~LatencyBuffers() {}

  /**
   *
   * @param aParams
   * @return
   */
  virtual State code(const ::swatch::core::XParameterSet& aParams);

private:
  //! Buffers helper class
  Selector mBufferSelector;
};

typedef LatencyBuffers<RxBufferSelector> LatencyRxBuffers;
typedef LatencyBuffers<TxBufferSelector> LatencyTxBuffers;

} // namespace cmds
} // namespace mp7
} // namespace swatch



#endif /* __SWATCH_MP7_CMDS_LATENCYBUFFERSCOMMAND_HPP__ */
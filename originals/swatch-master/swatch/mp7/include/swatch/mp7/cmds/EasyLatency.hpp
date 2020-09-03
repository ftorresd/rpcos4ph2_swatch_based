#ifndef __SWATCH_MP7_CMDS_EASYLATENCYCOMMAND_HPP__
#define __SWATCH_MP7_CMDS_EASYLATENCYCOMMAND_HPP__

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
 *
 */
template<class Selector>
class EasyLatency : public swatch::action::Command {
public:

  /**
   * @brief      Constructor.
   *
   * @details    Defines input parameters
   *             - bankId (uint):
   *             - masterLatency (uint):
   *             - algoLatency (uint):
   *             - internalLatency (uint):
   *             - rxExtraFrames (uint):
   *             - txExtraFrames (uint):
   *
   * @param[in]  aId          Command identifier
   * @param      aActionable  Target actionable object
   */ 
  EasyLatency(const std::string& aId, swatch::action::ActionableObject& aActionable);
  virtual ~EasyLatency() {}
  virtual State code(const ::swatch::core::XParameterSet& aParams);

protected:

  uint32_t computeLatency( uint32_t aMaster, uint32_t aAlgo, uint32_t aInternal, uint32_t aRxExtraFramess, uint32_t aTxExtraFrames);

  Selector mBufferSelector;
};

typedef EasyLatency<RxBufferSelector> EasyRxLatency;
typedef EasyLatency<TxBufferSelector> EasyTxLatency;

} // namespace cmds
} // namespace mp7
} // namespace swatch



#endif /* __SWATCH_MP7_CMDS_EASYLATENCYCOMMAND_HPP__ */
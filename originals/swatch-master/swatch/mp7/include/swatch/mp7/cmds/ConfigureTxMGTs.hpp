#ifndef __SWATCH_MP7_CMDS_CONFIGURETXMGTSCOMMAND_HPP__
#define __SWATCH_MP7_CMDS_CONFIGURETXMGTSCOMMAND_HPP__

#include "swatch/action/Command.hpp"

#include "swatch/mp7/IOChannelSelector.hpp"


namespace swatch {
namespace mp7 {
namespace cmds {

class ConfigureTxMGTs : public swatch::action::Command {
public:

  /**
   * @brief      Constructor.
   *
   * @details    Defines input parameters
   *             - orbitTag (bool)
   *             - loopback (bool)
   *             - polarity (bool)
   *
   * @param[in]  aId          Command identifier
   * @param      aActionable  Target actionable object
   */
  ConfigureTxMGTs(const std::string& aId, swatch::action::ActionableObject& aActionable);
  
  /**
   * @brief      Destroys the object.
   */
  virtual ~ConfigureTxMGTs() {}
  
  /**
   * @brief      Configured MP7 Tx ports
   *
   * @param[in]  aParams  Input parameters
   *
   * @return     Command execution status
   */
  virtual action::Command::State code(const core::XParameterSet& aParams);
private:
  TxMGTSelector mTxMGTSelector;
};

} // namespace cmds
} // namespace mp7
} // namespace swatch



#endif /* __SWATCH_MP7_CMDS_CONFIGURETXMGTSCOMMAND_HPP__ */
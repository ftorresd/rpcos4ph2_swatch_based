#ifndef __SWATCH_MP7_CMDS_DATAVALIDFORMATTERCOMMAND_HPP__
#define __SWATCH_MP7_CMDS_DATAVALIDFORMATTERCOMMAND_HPP__

#include <string>

#include "swatch/action/Command.hpp"
#include "swatch/mp7/IOChannelSelector.hpp"


namespace swatch {
namespace mp7 {
namespace cmds {


/**
 * @brief      Class for datavalid formatter command.
 */
class DatavalidFormatter : public swatch::action::Command {
public:
  /**
   * @brief      Constructor.
   *
   * @details    Defines input paramters:
   *             - startBx (uint)
   *             - startCycle (uint)
   *             - stopBx (uint)
   *             - stopCycle (uint)
   *
   * @param[in]  aId          Command identifier
   * @param      aActionable  Target actionable object.
   */
  DatavalidFormatter(const std::string& aId, swatch::action::ActionableObject& aActionable);

  /**
   * @brief      Destroys the object.
   */
  virtual ~DatavalidFormatter() {}

  /**
   * @brief      Configures the MP7 header formatters in firmware.
   *
   * @param[in]  aParams  Input parameters.
   *
   * @return     Command execution status.
   */
  virtual State code(const ::swatch::core::XParameterSet& aParams);

private:
  //! Command core
  TxChannelSelector mFmtSelector;
};

} // namespace cmds
} // namespace mp7
} // namespace swatch



#endif /* __SWATCH_MP7_CMDS_DATAVALIDFORMATTERCOMMAND_HPP__ */
#ifndef __SWATCH_MP7_CMDS_ALIGNRXSTOCOMMAND_HPP__
#define __SWATCH_MP7_CMDS_ALIGNRXSTOCOMMAND_HPP__

#include <string>                       // for string

#include "swatch/action/Command.hpp"

#include "swatch/mp7/IOChannelSelector.hpp"


namespace swatch {
namespace mp7 {
namespace cmds {

class AlignRxsTo : public swatch::action::Command {
public:
  
  /**
   * @brief      Constructor.
   *
   * @details    Defines input parameters
   *             - bx (uint)
   *             - cycle (uint)
   *
   * @param[in]  aId          Command identifier
   * @param      aActionable  Target actionable object
   */
  AlignRxsTo(const std::string& aId, swatch::action::ActionableObject& aActionable);

  /**
   * @brief      Destroys the object.
   */
  virtual ~AlignRxsTo() {}
  
  /**
   * @brief      Aligns MP7 MGTs to a selected bunch crossing/cycle.
   *
   * @param[in]  aParams  Input parameters
   *
   * @return     Command execution status
   */
  virtual action::Command::State code(const core::XParameterSet& aParams);
  
private:
  RxMGTSelector mRxMGTSelector;
};

} // namespace cmds
} // namespace mp7
} // namespace swatch


#endif /* __SWATCH_MP7_CMDS_ALIGNRXSTOCOMMAND_HPP__ */
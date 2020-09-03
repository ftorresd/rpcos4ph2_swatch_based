#include "swatch/mp7/cmds/SetID.hpp"

#include "swatch/xsimpletypedefs.hpp"
#include "swatch/mp7/MP7Processor.hpp"


namespace swatch {
namespace mp7 {
namespace cmds {

// ----------------------------------------------------------------------------
SetID::SetID(const std::string& aId, swatch::action::ActionableObject& aActionable):
  Command(aId, aActionable, XUInt_t())
{
  registerParameter("boardId", XUInt_t(0x0));
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
action::Command::State
SetID::code(const core::XParameterSet& aParams)
{
  MP7Processor& p = getActionable<MP7Processor>();
  ::mp7::MP7Controller& driver = p.driver();

  const XUInt_t& boardId = aParams.get<XUInt_t>("boardId");

  const ::mp7::CtrlNode& lCtrl = driver.getCtrl();

  // FIXME: replace with CtrlNode command when implemented
  lCtrl.getNode("board_id").write(boardId);
  lCtrl.getClient().dispatch();

  setResult(XUInt_t(boardId));
  return State::kDone;
}
// ----------------------------------------------------------------------------


} // namespace cmds
} // namespace mp7
} // namespace swatch


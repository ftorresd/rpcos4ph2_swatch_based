#include "swatch/amc13/cmds/ResetCounters.hpp"


#include "swatch/xsimpletypedefs.hpp"
#include "swatch/amc13/AMC13Manager.hpp"

// AMC13 Headers
#include "amc13/AMC13.hh"
#include "amc13/AMC13Simple.hh"         // for AMC13Simple, etc


namespace swatch {
namespace amc13 {
namespace cmds {

// --------------------------------------------------------
ResetCounters::ResetCounters(const std::string& aId, swatch::action::ActionableObject& aActionable) :
  Command(aId, aActionable, XInt_t())
{
}
// --------------------------------------------------------


// --------------------------------------------------------
action::Command::State ResetCounters::code(const core::XParameterSet& aParams)
{

  AMC13Manager& amc13mgr = getActionable<AMC13Manager>();

  ::amc13::AMC13& board = amc13mgr.driver();

  board.resetCounters();
  return State::kDone;
}
// --------------------------------------------------------


} // namespace cmds
} // namespace amc13
} // namespace swatch
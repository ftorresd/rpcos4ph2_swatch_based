#include "swatch/amc13/cmds/Start.hpp"

#include "swatch/xsimpletypedefs.hpp"
#include "swatch/amc13/AMC13Manager.hpp"

// AMC13 Headers
#include "amc13/AMC13.hh"
#include "amc13/AMC13Simple.hh"         // for AMC13Simple, etc

namespace swatch {
namespace amc13 {
namespace cmds {

// ----------------------------------------------------------------------------
Start::Start(const std::string& aId, swatch::action::ActionableObject& aActionable) :
  action::Command(aId, aActionable, XInt_t())
{
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
action::Command::State
Start::code(const core::XParameterSet& aParams)
{
  getActionable<AMC13Manager>().driver().startRun();

  return State::kDone;
}
// ----------------------------------------------------------------------------


} // namespace cmds
} // namespace amc13
} // namespace swatch

#include "rpcos4ph2/dummy/AbstractForceStateCommand.hpp"


#include <algorithm>


#include "xdata/String.h"

#include "rpcos4ph2/dummy/ComponentState.hpp"


namespace rpcos4ph2 {
namespace dummy {


AbstractForceStateCommand::AbstractForceStateCommand(const std::string& aId, swatch::action::ActionableObject& aActionable) :
  Command(aId, aActionable, xdata::String("Dummy 'force state' command's default result!"))
{
  registerParameter("state", xdata::String("error"));
}


AbstractForceStateCommand::~AbstractForceStateCommand()
{
}


ComponentState AbstractForceStateCommand::parseState(const swatch::core::XParameterSet& aParams)
{
  std::string lStateParam = aParams.get<xdata::String>("state").value_;

  // Transform to lowercase
  std::transform(lStateParam.begin(), lStateParam.end(), lStateParam.begin(), ::tolower);

  if (lStateParam == "unreachable" || lStateParam == "not-reachable")
    return ComponentState::kNotReachable;
  else if (lStateParam == "error")
    return ComponentState::kError;
  else if (lStateParam == "warning")
    return ComponentState::kWarning;
  else if (lStateParam == "good")
    return ComponentState::kGood;
  else
    XCEPT_RAISE(swatch::core::RuntimeError,"Invalid state string '" + aParams.get<xdata::String>("state").value_ + "'");
}

} // end ns: dummy
} // end ns: swatch

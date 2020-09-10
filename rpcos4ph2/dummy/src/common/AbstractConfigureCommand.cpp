
#include "rpcos4ph2/dummy/AbstractConfigureCommand.hpp"


#include "xdata/Boolean.h"
#include "xdata/String.h"
#include "xdata/UnsignedInteger.h"


namespace rpcos4ph2 {
namespace dummy {


AbstractConfigureCommand::AbstractConfigureCommand(const std::string& aId, swatch::action::ActionableObject& aActionable) :
  Command(aId, aActionable, xdata::String("Dummy command's default result!"))
{
  registerParameter("cmdDuration", xdata::UnsignedInteger(5));
  registerParameter("returnWarning", xdata::Boolean(false));
  registerParameter("returnError", xdata::Boolean(false));
  registerParameter("throw", xdata::Boolean(false));
}


AbstractConfigureCommand::~AbstractConfigureCommand()
{
}


swatch::action::Command::State AbstractConfigureCommand::code(const swatch::core::XParameterSet& aParams)
{
  const size_t lNrSeconds = aParams.get<xdata::UnsignedInteger>("cmdDuration").value_;

  State lState = kDone;
  if (aParams.get<xdata::Boolean>("throw").value_)
    lState = kError;
  else if (aParams.get<xdata::Boolean>("returnError").value_)
    lState = kError;
  else if (aParams.get<xdata::Boolean>("returnWarning").value_)
    lState = kWarning;

  for (size_t i=0; i<(lNrSeconds*4); i++) {
    std::ostringstream lMsg;
    lMsg << "Done " << i << " of " << (lNrSeconds*4) << " things";
    setProgress(float(i)/float(lNrSeconds*4), lMsg.str());
    boost::this_thread::sleep_for(boost::chrono::milliseconds(250));
  }

  this->runAction(lState == kError);

  if (aParams.get<xdata::Boolean>("throw").value_)
    XCEPT_RAISE(swatch::core::RuntimeError,"An exceptional error occurred!");
  return lState;
}

} // end ns: dummy
} // end ns: swatch

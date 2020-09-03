
#include "swatch/action/test/DummySleepCommand.hpp"


#include "boost/chrono.hpp"

#include "xdata/UnsignedInteger.h"

#include "swatch/action/test/DummyActionableObject.hpp"


using namespace swatch;
namespace swatch {
namespace action {
namespace test {

DummySleepCommand::DummySleepCommand(const std::string& aId, ActionableObject& aActionable) :
  Command(aId, aActionable, xdata::Integer(-33))
{
  registerParameter("n", xdata::UnsignedInteger(50));
  registerParameter("millisecPerSleep", xdata::UnsignedInteger(100));
}


DummySleepCommand::~DummySleepCommand()
{
}


Command::State DummySleepCommand::code(const core::XParameterSet& aParams)
{
  DummyActionableObject& res = getActionable<DummyActionableObject>();

  setStatusMsg("Dummy sleep command just started");

  unsigned int n(aParams.get<xdata::UnsignedInteger>("n").value_);
  unsigned int millisecPerSleep(aParams.get<xdata::UnsignedInteger>("millisecPerSleep").value_);

  for (unsigned int i = 0; i < n; i++) {
    boost::this_thread::sleep_for(boost::chrono::milliseconds(millisecPerSleep));

    std::ostringstream oss;
    oss << "Dummy sleep command for object '" << res.getPath() << "' progressed. " << i << " of " << n << " sleeps done; " << (n - i) * millisecPerSleep << " milli sec remain";
    setProgress( float(i) / n, oss.str());
  }

  setStatusMsg("Dummy sleep command for object '" + res.getPath() + "' completed");
  return State::kDone;
}

} /* namespace test */
} /* namespace action */
} /* namespace swatch */

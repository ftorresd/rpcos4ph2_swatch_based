
#include "rpcos4ph2/dummy/DummyReadout.hpp"


#include "rpcos4ph2/dummy/DummyProcDriver.hpp"


namespace rpcos4ph2 {
namespace dummy {


DummyReadoutInterface::DummyReadoutInterface(DummyProcDriver& aDriver) :
  ReadoutInterface(),
  mDriver(aDriver)
{
}


DummyReadoutInterface::~DummyReadoutInterface()
{
}


void DummyReadoutInterface::retrieveMetricValues()
{
  DummyProcDriver::ReadoutStatus lStatus = mDriver.getReadoutStatus();
  setMetricValue<>(mMetricAMCCoreReady, lStatus.amcCoreReady);
  setMetricValue<>(mMetricTTS, lStatus.ttsState);
  setMetricValue<>(mMetricEventCounter, lStatus.eventCounter);
}


} // namespace dummy
} // namespace rpcos4ph2

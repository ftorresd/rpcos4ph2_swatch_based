
#include "swatch/dummy/DummyReadout.hpp"


#include "swatch/dummy/DummyProcDriver.hpp"


namespace swatch {
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
} // namespace swatch


#include "rpcos4ph2/dummy/DummyRxPort.hpp"


#include "swatch/core/MetricConditions.hpp"
#include "rpcos4ph2/dummy/DummyProcDriver.hpp"


namespace rpcos4ph2 {
namespace dummy {


DummyRxPort::DummyRxPort(const std::string& aId, uint32_t aNumber, DummyProcDriver& aDriver) :
  InputPort(aId),
  mChannelId(aNumber),
  mDriver(aDriver),
  mWarningSign(registerMetric<bool>("warningSign"))
{
  setWarningCondition<>(mWarningSign, swatch::core::EqualCondition<bool>(true));
}


DummyRxPort::~DummyRxPort()
{
}


void DummyRxPort::retrieveMetricValues()
{
  DummyProcDriver::RxPortStatus lStatus = mDriver.getRxPortStatus(mChannelId);

  setMetricValue<>(mMetricIsLocked, lStatus.isLocked);
  setMetricValue<>(mMetricIsAligned, lStatus.isAligned);
  setMetricValue<>(mMetricCRCErrors, lStatus.crcErrCount);
  setMetricValue<>(mWarningSign, lStatus.warningSign);
}


} // namespace dummy
} // namespace rpcos4ph2

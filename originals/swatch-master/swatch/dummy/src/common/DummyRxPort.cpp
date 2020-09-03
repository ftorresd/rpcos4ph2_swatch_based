
#include "swatch/dummy/DummyRxPort.hpp"


#include "swatch/core/MetricConditions.hpp"
#include "swatch/dummy/DummyProcDriver.hpp"


namespace swatch {
namespace dummy {


DummyRxPort::DummyRxPort(const std::string& aId, uint32_t aNumber, DummyProcDriver& aDriver) :
  InputPort(aId),
  mChannelId(aNumber),
  mDriver(aDriver),
  mWarningSign(registerMetric<bool>("warningSign"))
{
  setWarningCondition<>(mWarningSign, core::EqualCondition<bool>(true));
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
} // namespace swatch

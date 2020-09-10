
#include "rpcos4ph2/dummy/DummyTxPort.hpp"


#include "swatch/core/MetricConditions.hpp"
#include "rpcos4ph2/dummy/DummyProcDriver.hpp"


namespace rpcos4ph2 {
namespace dummy {


DummyTxPort::DummyTxPort(const std::string& aId, uint32_t aNumber, DummyProcDriver& aDriver) :
  OutputPort(aId),
  mChannelId(aNumber),
  mDriver(aDriver),
  mWarningSign(registerMetric<bool>("warningSign"))
{
  setWarningCondition<>(mWarningSign, swatch::core::EqualCondition<bool>(true));
}

DummyTxPort::~DummyTxPort()
{
}


void DummyTxPort::retrieveMetricValues()
{
  DummyProcDriver::TxPortStatus lStatus = mDriver.getTxPortStatus(mChannelId);

  setMetricValue<>(mMetricIsOperating, lStatus.isOperating);
  setMetricValue<>(mWarningSign, lStatus.warningSign);
}


} // namespace dummy
} // namespace rpcos4ph2

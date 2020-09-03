
#include "swatch/dummy/DummyTTC.hpp"
#include "swatch/dummy/DummyProcDriver.hpp"
#include "swatch/core/MetricConditions.hpp"


namespace swatch {
namespace dummy {


DummyTTC::DummyTTC(DummyProcDriver& aDriver) :
  TTCInterface(),
  mDriver(aDriver),
  mWarningSign(registerMetric<bool>("warningSign"))
{
  setWarningCondition<>(mWarningSign, core::EqualCondition<bool>(true));
}


DummyTTC::~DummyTTC()
{
}


void DummyTTC::retrieveMetricValues()
{
  DummyProcDriver::TTCStatus lStatus = mDriver.getTTCStatus();

  setMetricValue<>(mMetricL1ACounter, lStatus.eventCounter);
  setMetricValue<>(mMetricBunchCounter, lStatus.bunchCounter);
  setMetricValue<>(mMetricOrbitCounter, lStatus.orbitCounter);

  setMetricValue<>(mMetricIsClock40Locked, lStatus.clk40Locked);
  setMetricValue<>(mMetricHasClock40Stopped, lStatus.clk40Stopped);
  setMetricValue<>(mMetricIsBC0Locked, lStatus.bc0Locked);

  setMetricValue<>(mMetricSingleBitErrors, lStatus.errSingleBit);
  setMetricValue<>(mMetricDoubleBitErrors, lStatus.errDoubleBit);

  setMetricValue<>(mWarningSign, lStatus.warningSign);
}


} // namespace dummy
} // namespace swatch

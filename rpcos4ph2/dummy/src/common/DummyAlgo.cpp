
#include "rpcos4ph2/dummy/DummyAlgo.hpp"
#include "rpcos4ph2/dummy/DummyProcDriver.hpp"
#include "swatch/core/MetricConditions.hpp"


namespace rpcos4ph2 {
namespace dummy {


DummyAlgo::DummyAlgo(DummyProcDriver& aDriver) :
  AlgoInterface(),
  mDriver(aDriver),
  mRateCounterA(registerMetric<float>("rateCounterA", swatch::core::GreaterThanCondition<float>(80e3), swatch::core::GreaterThanCondition<float>(40e3))),
  mRateCounterB(registerMetric<float>("rateCounterB", swatch::core::GreaterThanCondition<float>(80e3), swatch::core::GreaterThanCondition<float>(40e3)))
{
  for (size_t i=0; i<500; i++)
    registerMetric<float>("rate_counter_" + boost::lexical_cast<std::string>(i));
}


DummyAlgo::~DummyAlgo()
{
}


void DummyAlgo::retrieveMetricValues()
{
  DummyProcDriver::AlgoStatus lStatus = mDriver.getAlgoStatus();

  setMetricValue(mRateCounterA, lStatus.rateCounterA);
  setMetricValue(mRateCounterB, lStatus.rateCounterB);
}

} // namespace dummy
} // namespace rpcos4ph2

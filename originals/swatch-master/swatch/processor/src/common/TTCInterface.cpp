/*
 * File:   TTCInterface.cpp
 * Author: Tom Williams
 * Created May 2015
 */

#include "swatch/processor/TTCInterface.hpp"

// SWATCH Headers
#include "swatch/core/MetricConditions.hpp"


namespace swatch {
namespace processor {


TTCInterface::TTCInterface() :
  core::MonitorableObject("ttc"),
  mMetricL1ACounter(registerMetric<uint32_t>("l1aCounter")),
  mMetricBunchCounter(registerMetric<uint32_t>("bunchCounter")),
  mMetricOrbitCounter(registerMetric<uint32_t>("orbitCounter")),
  mMetricIsClock40Locked(registerMetric<bool>("isClock40Locked", core::EqualCondition<bool>(false))),
  mMetricHasClock40Stopped(registerMetric<bool>("hasClock40Stopped", core::EqualCondition<bool>(true))),
  mMetricIsBC0Locked(registerMetric<bool>("isBC0Locked", core::EqualCondition<bool>(false))),
  mMetricSingleBitErrors(registerMetric<uint32_t>("singleBitErrors", core::GreaterThanCondition<uint32_t>(0))),
  mMetricDoubleBitErrors(registerMetric<uint32_t>("doubleBitErrors", core::GreaterThanCondition<uint32_t>(0)))
{
}


const std::vector<std::string>
TTCInterface::kDefaultMetrics = {
  "l1aCounter",
  "bunchCounter",
  "orbitCounter",

  "isClock40Locked",
  "hasClock40Stopped",
  "isBC0Locked",
  "singleBitErrors",
  "doubleBitErrors",
};


} // end ns core
} // end ns swatch

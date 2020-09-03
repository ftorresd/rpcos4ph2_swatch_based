/*
 * File:   Port.cpp
 * Author: ale
 */

#include "swatch/processor/Port.hpp"


#include "swatch/core/MetricConditions.hpp"


namespace swatch {
namespace processor {


InputPort::InputPort( const std::string& aId, const std::string& aAlias ) :
  MaskableObject(aId, aAlias),
  mMetricIsLocked( registerMetric<bool>(kMetricIdIsLocked, core::EqualCondition<bool>(false)) ),
  mMetricIsAligned( registerMetric<bool>(kMetricIdIsAligned, core::EqualCondition<bool>(false)) ),
  mMetricCRCErrors( registerMetric<uint32_t>(kMetricIdCRCErrors, core::GreaterThanCondition<uint32_t>(0)) )
{
}


const std::string InputPort::kMetricIdIsLocked = "isLocked";
const std::string InputPort::kMetricIdIsAligned = "isAligned";
const std::string InputPort::kMetricIdCRCErrors = "crcErrors";


const std::vector<std::string> InputPort::kDefaultMetrics = { kMetricIdIsLocked, kMetricIdIsAligned, kMetricIdCRCErrors };



OutputPort::OutputPort( const std::string& aId, const std::string& aAlias ) :
  MonitorableObject(aId, aAlias),
  mMetricIsOperating( registerMetric<bool>(kMetricIdIsOperating, core::EqualCondition<bool>(false)) )
{
}


const std::string OutputPort::kMetricIdIsOperating = "isOperating";

const std::vector<std::string> OutputPort::kDefaultMetrics = { kMetricIdIsOperating };


}
}

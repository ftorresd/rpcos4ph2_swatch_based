/**
 * @file    DummyMonitorableObjects.cpp
 * @author  Luke Kreczko, Tom Williams
 * @date    December 2015
 *
 */

#include "swatch/core/test/DummyMonitorableObjects.hpp"


#include "swatch/core/MetricConditions.hpp"
#include "swatch/logger/Logger.hpp"


namespace swatch {
namespace core {
namespace test {


DummyMonitorableObject::DummyMonitorableObject() :
  MonitorableObject("DummyMonitorableObject"),
  mThrowAfterRetrievingMetricValues(false)
{
}

DummyMonitorableObject::DummyMonitorableObject(const std::string& aId) :
  MonitorableObject(aId),
  mThrowAfterRetrievingMetricValues(false)
{
}


DummyMonitorableObject::DummyMonitorableObject(const std::string& aId, const std::string& aAlias) :
  MonitorableObject(aId, aAlias),
  mThrowAfterRetrievingMetricValues(false)
{
}


DummyMonitorableObject::~DummyMonitorableObject()
{
}


MonitorableObject& DummyMonitorableObject::addMonitorable(MonitorableObject* aObj)
{
  MonitorableObject::addMonitorable(aObj);
  return *aObj;
}


void DummyMonitorableObject::setNextMetricValues(const std::map<std::string, IntegerMetricDataType_t>& aMetricValues)
{
  mNextMetricValues = aMetricValues;
}

void DummyMonitorableObject::throwAfterRetrievingMetricValues(bool aThrow)
{
  mThrowAfterRetrievingMetricValues = aThrow;
}


void DummyMonitorableObject::retrieveMetricValues()
{
  for (auto lIt=mNextMetricValues.cbegin(); lIt != mNextMetricValues.cend(); lIt++) {
    if (SimpleMetric<IntegerMetricDataType_t>* lMetric = this->getObjPtr<SimpleMetric<IntegerMetricDataType_t> >(lIt->first))
      setMetricValue<>(*lMetric, lIt->second);
  }

  if (mThrowAfterRetrievingMetricValues)
    XCEPT_RAISE(RuntimeError,"Error occurred when retrieving metric values");
}



log4cplus::Logger DummyMasterMonitorableObject::sLogger = swatch::logger::Logger::getInstance("swatch.core.DummyMasterMonitorableObject");

DummyMasterMonitorableObject::DummyMasterMonitorableObject() :
  DummyMonitorableObject()
{
  setMonitorableStatus(mStatus, sLogger);
}


DummyMasterMonitorableObject::DummyMasterMonitorableObject(const std::string& aId) :
  DummyMonitorableObject(aId)
{
  setMonitorableStatus(mStatus, sLogger);
}


DummyMasterMonitorableObject::~DummyMasterMonitorableObject()
{
}


} /* namespace test */
} /* namespace core */
} /* namespace swatch */

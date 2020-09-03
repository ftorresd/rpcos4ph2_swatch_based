/**
 * @file    DummyMonitorableObjects.hpp
 * @author  Luke Kreczko, Tom Williams
 * @date    December 2015
 *
 */

#ifndef __SWATCH_CORE_TEST_DUMMYMONITORABLEOBJECTS_HXX__
#define __SWATCH_CORE_TEST_DUMMYMONITORABLEOBJECTS_HXX__


namespace swatch {
namespace core {
namespace test {


template <typename DataType>
SimpleMetric<DataType>& DummyMonitorableObject::registerMetric(const std::string& aId)
{
  return MonitorableObject::registerMetric<DataType>(aId);
}


template <typename DataType, class ErrorCondition>
SimpleMetric<DataType>& DummyMonitorableObject::registerMetric(const std::string& aId, const ErrorCondition& aErrorCondition)
{
  return MonitorableObject::registerMetric<DataType>(aId, aErrorCondition);
}


template <typename DataType, class ErrorCondition, class WarnCondition>
SimpleMetric<DataType>& DummyMonitorableObject::registerMetric(const std::string& aId, const ErrorCondition& aErrorCondition, const WarnCondition& aWarnCondition)
{
  return MonitorableObject::registerMetric<DataType>(aId, aErrorCondition, aWarnCondition);
}


template <typename DataType, class MetricIt>
ComplexMetric<DataType>& DummyMonitorableObject::registerComplexMetric(const std::string& aId, const MetricIt& aMetricBegin, const MetricIt& aMetricEnd, const typename ComplexMetric<DataType>::CalculateFunction_t& aFunction)
{
  return MonitorableObject::registerComplexMetric<DataType>(aId, aMetricBegin, aMetricEnd, aFunction);
}


template <typename DataType, class MonObjIt>
ComplexMetric<DataType>& DummyMonitorableObject::registerComplexMetric(const std::string& aId, const MonObjIt& aMonObjBegin, const MonObjIt& aMonObjEnd, const typename ComplexMetric<DataType>::CalculateFunction2_t& aFunction)
{
  return MonitorableObject::registerComplexMetric<DataType>(aId, aMonObjBegin, aMonObjEnd, aFunction);
}


template <typename DataType, class ConditionType>
void DummyMonitorableObject::setErrorCondition(Metric<DataType>& aMetric, const ConditionType& aErrorCondition)
{
  MonitorableObject::setErrorCondition(aMetric, aErrorCondition);
}


template <typename DataType, class ConditionType>
void DummyMonitorableObject::setWarningCondition(Metric<DataType>& aMetric, const ConditionType& aWarningCondition)
{
  MonitorableObject::setWarningCondition(aMetric, aWarningCondition);
}


template <typename DataType, class ErrorConditionType, class WarnConditionType>
void DummyMonitorableObject::setConditions(Metric<DataType>& aMetric, const ErrorConditionType& aErrorCondition, const WarnConditionType& aWarnCondition)
{
  MonitorableObject::setConditions(aMetric, aErrorCondition, aWarnCondition);
}


} /* namespace test */
} /* namespace core */
} /* namespace swatch */

#endif /* __SWATCH_CORE_TEST_DUMMYMONITORABLEOBJECTS_HXX__ */

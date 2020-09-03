/**
 * @file    DummyMonitorableObjects.hpp
 * @author  Luke Kreczko, Tom Williams
 * @date    December 2015
 *
 */

#ifndef __SWATCH_CORE_TEST_DUMMYMONITORABLEOBJECTS_HPP__
#define __SWATCH_CORE_TEST_DUMMYMONITORABLEOBJECTS_HPP__


#include "swatch/core/AbstractMonitorableStatus.hpp"
#include "swatch/core/MonitorableObject.hpp"

#include "swatch/core/test/DummyMonitorableStatus.hpp"

namespace swatch {
namespace core {
namespace test {


class DummyMonitorableObject : public MonitorableObject {
public:
  DummyMonitorableObject();

  DummyMonitorableObject(const std::string& aId);

  DummyMonitorableObject(const std::string& aId, const std::string& aAlias);

  virtual ~DummyMonitorableObject();

  typedef int IntegerMetricDataType_t ;


  /* Expose several of MonitorableObject's protected methods so that can be called easily in unit tests */

  template <typename DataType>
  SimpleMetric<DataType>& registerMetric(const std::string& aId);

  template <typename DataType, class ErrorCondition>
  SimpleMetric<DataType>& registerMetric(const std::string& aId, const ErrorCondition& aErrorCondition);

  template <typename DataType, class ErrorCondition, class WarnCondition>
  SimpleMetric<DataType>& registerMetric(const std::string& aId, const ErrorCondition& aErrorCondition, const WarnCondition& aWarnCondition);

  template <typename DataType, class MetricIt>
  ComplexMetric<DataType>& registerComplexMetric(const std::string& aId, const MetricIt& aMetricBegin, const MetricIt& aMetricEnd, const typename ComplexMetric<DataType>::CalculateFunction_t& aFunction);

  template <typename DataType, class MetricIt>
  ComplexMetric<DataType>& registerComplexMetric(const std::string& aId, const MetricIt& aMetricBegin, const MetricIt& aMetricEnd, const typename ComplexMetric<DataType>::CalculateFunction2_t& aFunction);

  template <typename DataType, class ConditionType>
  void setErrorCondition(Metric<DataType>& aMetric, const ConditionType& aErrorCondition);

  template <typename DataType, class ConditionType>
  void setWarningCondition(Metric<DataType>& aMetric, const ConditionType& aWarningCondition);

  template <typename DataType, class ErrorConditionType, class WarnConditionType>
  void setConditions(Metric<DataType>& aMetric, const ErrorConditionType& aErrorCondition, const WarnConditionType& aWarnCondition);

  MonitorableObject& addMonitorable(MonitorableObject* aObj);

  /* END OF: Expose several of MonitorableObject's protected methods so that can be called easily in unit tests */

  //! Specify what value each metric will get, the next time that the retrieveMetricValues method is called
  void setNextMetricValues(const std::map<std::string, IntegerMetricDataType_t>& aMetricValues);

  void throwAfterRetrievingMetricValues(bool aThrow);

private:
  virtual void retrieveMetricValues();

  std::map<std::string, IntegerMetricDataType_t> mNextMetricValues;
  bool mThrowAfterRetrievingMetricValues;
};


class DummyMasterMonitorableObject : public DummyMonitorableObject {
public:
  DummyMasterMonitorableObject();

  DummyMasterMonitorableObject(const std::string& aId);

  ~DummyMasterMonitorableObject();

private:
  DummyMonitorableStatus mStatus;

  static log4cplus::Logger sLogger;
};


} /* namespace test */
} /* namespace core */
} /* namespace swatch */


#include "swatch/core/test/DummyMonitorableObjects.hxx"


#endif /* SWATCH_CORE_TEST_DUMMYMONITORABLEOBJECTS_HPP */

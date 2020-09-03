/**
 * @file    MonitorableObject.hpp
 * @author  Andy Rose
 * @date    May 2015
 */

#ifndef __SWATCH_CORE_MONITORABLEOBJECT_HPP__
#define __SWATCH_CORE_MONITORABLEOBJECT_HPP__

// Standard headers
#include <string>
#include <vector>

// boost headers
#include "boost/function.hpp"
#include "boost/noncopyable.hpp"        // for noncopyable
#include "boost/unordered/unordered_map.hpp"

// log4cplus headers
#include <log4cplus/logger.h>

// SWATCH headers
#include "swatch/core/exception.hpp"
#include "swatch/core/monitoring/Status.hpp"
#include "swatch/core/Object.hpp"
#include "swatch/core/StatusFlag.hpp"


namespace swatch {
namespace core {


class AbstractMetric;
class AbstractMonitorableStatus;
class MetricReadGuard;
class MetricUpdateGuard;
class MetricView;
class MonitorableObjectSnapshot;
template <typename DataType> class Metric;
template <typename DataType> class SimpleMetric;
template <typename DataType> class ComplexMetric;


//! An object that contains metrics and/or other monitorable objects; i.e. an object representing a resource whose status (Good/Warning/Error) can be monitored by reading various quantities
class MonitorableObject : public Object {
public:
  explicit MonitorableObject( const std::string& aId );

  explicit MonitorableObject( const std::string& aId, const std::string& aAlias );

  virtual ~MonitorableObject();

  /**
    * Names of registered metrics.
    * @return metric names
    */
  std::vector< std::string > getMetrics() const;

  //! Retrieve metric with given ID
  const AbstractMetric& getMetric( const std::string& aId ) const;

  //! Retrieve metric with given ID
  AbstractMetric& getMetric( const std::string& aId );

  //! Retrieve metric with given ID
  template <typename DataType>
  const SimpleMetric<DataType>& getMetric( const std::string& aId ) const;

  //! Retrieve metric with given ID
  template <typename DataType>
  SimpleMetric<DataType>& getMetric( const std::string& aId );

  //! Retrieve metric with given ID
  template <typename DataType>
  const ComplexMetric<DataType>& getComplexMetric( const std::string& aId ) const;

  //! Retrieve metric with given ID
  template <typename DataType>
  ComplexMetric<DataType>& getComplexMetric( const std::string& aId );

  //! Get overall object status based on status flags of child Metrics and child MonitorableObjects; returns kNoLimit in case there are no metrics.
  StatusFlag getStatusFlag() const;

  MonitorableObjectSnapshot getStatus() const;

  //! Update values of this object's metrics
  void updateMetrics();

  //! Update values of this object's metrics
  void updateMetrics(const MetricUpdateGuard& aGuard);

  void setMonitoringStatus(const swatch::core::monitoring::Status aMonStatus);
  swatch::core::monitoring::Status getMonitoringStatus() const;

protected:

  /*!
   * register a metric of type swatch::core::Metric<DataType> , without any error or warning limits
   * @param aId ID string of the metric
   * @returns A reference to the registered metric
   */
  template <typename DataType>
  SimpleMetric<DataType>& registerMetric(const std::string& aId);

  template <typename DataType>
  SimpleMetric<DataType>& registerMetric(const std::string& aId, const char* aAlias);

  template <typename DataType>
  SimpleMetric<DataType>& registerMetric(const std::string& aId, const std::string& aAlias);

  /*!
   * register a metric of type swatch::core::Metric<DataType>
   * @param aId ID string of the metric
   * @param aErrorCondition Functor that is used to determine whether metric's value indicates an error; must inherit from swatch::core::MetricCondition<DataType>
   * @returns A reference to the registered metric
   */
  template <typename DataType, class ErrorCondition>
  SimpleMetric<DataType>& registerMetric(const std::string& aId, const ErrorCondition& aErrorCondition);

  template <typename DataType, class ErrorCondition>
  SimpleMetric<DataType>& registerMetric(const std::string& aId, const char* aAlias, const ErrorCondition& aErrorCondition);

  template <typename DataType, class ErrorCondition>
  SimpleMetric<DataType>& registerMetric(const std::string& aId, const std::string& aAlias, const ErrorCondition& aErrorCondition);

  /*!
   * register a metric of type swatch::core::Metric<DataType>
   * @param aId ID string of the metric
   * @param aErrorCondition Functor that is used to determine whether metric's value indicates an error; must inherit from swatch::core::MetricCondition<DataType>
   * @param aWarnCondition Functor that is used to determine whether metric's value constitutes a warning; must inherit from swatch::core::MetricCondition<DataType>
   * @returns A reference to the registered metric
   */
  template <typename DataType, class ErrorCondition, class WarnCondition>
  SimpleMetric<DataType>& registerMetric(const std::string& aId, const ErrorCondition& aErrorCondition, const WarnCondition& aWarnCondition);

  template <typename DataType, class ErrorCondition, class WarnCondition>
  SimpleMetric<DataType>& registerMetric(const std::string& aId, const std::string& aAlias, const ErrorCondition& aErrorCondition, const WarnCondition& aWarnCondition);

  /*!
   * register a metric of type swatch::core::ComplexMetric<DataType> , without any error or warning limits
   * @param aId ID string of the metric
   * @returns A reference to the registered metric
   */
  template <typename DataType, class MetricIt>
  ComplexMetric<DataType>& registerComplexMetric(const std::string& aId, const MetricIt& aMetricBegin, const MetricIt& aMetricEnd,
      const typename ComplexMetric<DataType>::CalculateFunction_t& aCalculateFunction,
      const typename ComplexMetric<DataType>::FilterFunction_t& aFilterFunction = NULL);

  template <typename DataType, class MetricIt>
  ComplexMetric<DataType>& registerComplexMetric(const std::string& aId, const std::string& aAlias,
      const MetricIt& aMetricBegin, const MetricIt& aMetricEnd,
      const typename ComplexMetric<DataType>::CalculateFunction_t& aCalculateFunction,
      const typename ComplexMetric<DataType>::FilterFunction_t& aFilterFunction = NULL);

  /*!
   * register a metric of type swatch::core::ComplexMetric<DataType> , without any error or warning limits
   * @param aId ID string of the metric
   * @returns A reference to the registered metric
   */
  template <typename DataType>
  ComplexMetric<DataType>& registerComplexMetric(const std::string& aId, const MetricView& aMetricView,
      const typename ComplexMetric<DataType>::CalculateFunction_t& aCalculateFunction,
      const typename ComplexMetric<DataType>::FilterFunction_t& aFilterFunction = NULL);

  template <typename DataType>
  ComplexMetric<DataType>& registerComplexMetric(const std::string& aId, const std::string& aAlias, const MetricView& aMetricView,
      const typename ComplexMetric<DataType>::CalculateFunction_t& aCalculateFunction,
      const typename ComplexMetric<DataType>::FilterFunction_t& aFilterFunction = NULL);

  /*!
   * register a metric of type swatch::core::ComplexMetric<DataType> , without any error or warning limits
   * @param aId ID string of the metric
   * @returns A reference to the registered metric
   */
  template <typename DataType, class MonObjIt>
  ComplexMetric<DataType>& registerComplexMetric(const std::string& aId, const MonObjIt& aMonObjBegin, const MonObjIt& aMonObjEnd,
      const typename ComplexMetric<DataType>::CalculateFunction2_t& aCalculateFunction,
      const typename ComplexMetric<DataType>::FilterFunction_t& aFilterFunction = NULL);

  template <typename DataType, class MonObjIt>
  ComplexMetric<DataType>& registerComplexMetric(const std::string& aId, const std::string& aAlias,
      const MonObjIt& aMonObjBegin, const MonObjIt& aMonObjEnd,
      const typename ComplexMetric<DataType>::CalculateFunction2_t& aCalculateFunction,
      const typename ComplexMetric<DataType>::FilterFunction_t& aFilterFunction = NULL);


  //! Set value of metric
  template <typename DataType>
  void setMetricValue(SimpleMetric<DataType>& metric, const DataType& value);

  //! Set error condition of metric
  template <typename DataType, class ConditionType>
  void setErrorCondition(Metric<DataType>& aMetric, const ConditionType& aErrorCondition);

  //! Set warning condition of metric
  template <typename DataType, class ConditionType>
  void setWarningCondition(Metric<DataType>& aMetric, const ConditionType& aWarningCondition);

  //! Set error & warning conditions of metric
  template <typename DataType, class ErrorConditionType, class WarnConditionType>
  void setConditions(Metric<DataType>& aMetric, const ErrorConditionType& aErrorCondition, const WarnConditionType& aWarnCondition);

  //! User-defined function that retrieves values of all the object's metrics from the hardware
  virtual void retrieveMetricValues() = 0;

  void addMonitorable(MonitorableObject* aMonObj);
  template <class DeleterType>
  void addMonitorable(MonitorableObject* aMonObj, DeleterType aDeleter);

  //! Sets this object's monitorable status pointer (used by MetricWriteGuard, in updateMetrics method)
  void setMonitorableStatus(AbstractMonitorableStatus& aStatus, log4cplus::Logger& aLogger);

private:
  // Common implementation of addMonitorable methods
  void finishAddingMonitorable(MonitorableObject* aMonObj);

  typedef boost::unordered_map< std::string , AbstractMetric* > MetricMap_t;
  typedef boost::unordered_map< std::string , MonitorableObject* > MonObjMap_t;

  MetricMap_t mSimpleMetrics;
  MetricMap_t mMetrics;
  MonObjMap_t mMonObjChildren;

  //! Stores details in case error occurs when updating the metric values
  std::string mUpdateErrorMsg;
  swatch::core::monitoring::Status mMonitoringStatus;
  AbstractMonitorableStatus* mStatus;
  log4cplus::Logger mLogger;

  std::vector< std::pair<const AbstractMetric*, boost::function<void ()> > > mDependantMetrics;

  friend class MetricUpdateGuard;
  friend class MetricReadGuard;
};


SWATCH_DEFINE_EXCEPTION(MetricAlreadyExistsInMonitorableObject)
SWATCH_DEFINE_EXCEPTION(MetricRegistrationError)
SWATCH_DEFINE_EXCEPTION(MetricNotFoundInMonitorableObject)


class MetricUpdateGuard : boost::noncopyable {
public:
  MetricUpdateGuard(MonitorableObject& aMonObj);
  ~MetricUpdateGuard();

  bool isCorrectGuard(const MonitorableObject& aMonObj) const;

private:
  AbstractMonitorableStatus& mObjStatus;
};


class MetricReadGuard : boost::noncopyable {
public:
  MetricReadGuard(const MonitorableObject& aMonObj);
  ~MetricReadGuard();

  bool isCorrectGuard(const MonitorableObject& aMonObj) const;

private:
  AbstractMonitorableStatus& mObjStatus;
};


class MonitorableObjectSnapshot {
public:
  MonitorableObjectSnapshot(const std::string& aPath, 
                           swatch::core::StatusFlag ,
                           swatch::core::monitoring::Status );

  ~MonitorableObjectSnapshot();

  //! Returns monitorable object's ID path
  const std::string& getPath() const;

  //! Returns monitorable object's ID
  std::string getId() const;

  swatch::core::StatusFlag getStatusFlag() const;

  swatch::core::monitoring::Status getMonitoringStatus() const;
  
private:
  std::string mPath;
  swatch::core::StatusFlag mFlag;
  swatch::core::monitoring::Status mMonitoringStatus;  
};


} // namespace core
} // namespace swatch

#include "swatch/core/MonitorableObject.hxx"

#endif  /* __SWATCH_CORE_MONITORABLEOBJECT_HPP__ */


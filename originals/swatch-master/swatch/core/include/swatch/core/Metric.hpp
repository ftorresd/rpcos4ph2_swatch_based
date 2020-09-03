/**
 * @file    BaseMetric.hpp
 * @author  Tom Williams
 * @date    July 2015
 */

#ifndef __SWATCH_CORE_BASEMETRIC_HPP__
#define __SWATCH_CORE_BASEMETRIC_HPP__


#include <stddef.h>                     // for NULL
#include <string>                       // for string
#include <sys/time.h>                   // for timeval

#include "boost/smart_ptr/shared_ptr.hpp"  // for shared_ptr
#include "boost/thread/pthread/mutex.hpp"  // for mutex

#include "swatch/core/AbstractMetric.hpp"
#include "swatch/core/monitoring/Status.hpp"  // for monitoring::Status


namespace swatch {
namespace core {

class MonitorableObject;

template<typename DataType>
class MetricCondition;

/**
 * @brief      Metric encapsulating monitoring data of given type.
 *
 * @tparam     DataType  type of the monitoring data.
 */
template<typename DataType>
class Metric : public AbstractMetric {

public:

  /*!
   * @brief      Construct a metric with no error/warning limits
   *
   * @param[in]  aId   Id of the new metric
   * @param[in]  aId   Alias for the new metric (no restrictions on characters)
   */
  Metric(const std::string& aId, const std::string& aAlias);

  /*!
   * @brief      Construct the Metric
   *
   * @param[in]  aId              A identifier
   * @param[in]  aId              Alias for the new metric (no restrictions on characters)
   * @param      aErrorCondition  Functor used to determine if the metric's
   *                              value indicates an error; the Metric takes
   *                              ownership of this condition object. NULL value
   *                              represents no error condition.
   * @param      aWarnCondition   Functor used to determine if the metric's
   *                              value indicates a warning; the Metric takes
   *                              ownership of this condition object. NULL value
   *                              represents no warning condition.
   */
  Metric(const std::string& aId, const std::string& aAlias, MetricCondition<DataType>* aErrorCondition, MetricCondition<DataType>* aWarnCondition = NULL);

  virtual ~Metric();

  /**
   * @brief      Returns metric value, status flag, and error/warning
   *             conditions, within a swatch::core::MetricSnapshot object.
   *             (THREAD SAFE)
   *
   * @return     The snapshot.
   */
  MetricSnapshot getSnapshot() const;

  /**
   * @brief      Returns the status flag and monitoring mask/setting
   *
   * @return     The status.
   */
  std::pair<StatusFlag, monitoring::Status> getStatus() const;

  /**
   * @brief      Returns time at which metric's value was last updated. (THREAD
   *             SAFE)
   *
   * @return     The update time.
   */
  SteadyTimePoint_t getUpdateTime() const;

  /**
   * @brief      Sets the monitoring status of this metric.
   *
   * @param[in]  aStatus  A monitoring status object.
   */
  void setMonitoringStatus(monitoring::Status aStatus);

private:
  std::pair<StatusFlag, monitoring::Status> getStatus(const boost::lock_guard<boost::mutex>& aGuard) const;    

  //! Set the value of the metric to being unknown
  void setValueUnknown();

  template <class ConditionType>
  void setErrorCondition(const ConditionType& aErrorCondition);

  template <class ConditionType>
  void setWarningCondition(const ConditionType& aWarningCondition);

  void addDependantMetric(const AbstractMetric& aMetric, const boost::function<void ()>& aUpdateFunction)
  {
    mDependantMetrics.push_back( std::make_pair(&aMetric, aUpdateFunction) );
  }

protected:
  const std::vector<std::pair<const AbstractMetric*, boost::function<void ()> > >& getDependantMetrics() const
  {
    return mDependantMetrics;
  }

  // TODO: Maybe eventually update to read-write mutex if needed ???
  //! Mutex used to stop corruption of value_
  mutable boost::mutex mMutex;

  //! Latest retrieved value of metric; set to NULL if metric value is unknown.
  boost::shared_ptr<const DataType> mValue;

  TimePoint mUpdateTime;

private:
  boost::shared_ptr<MetricCondition<DataType> > mErrorCondition;
  boost::shared_ptr<MetricCondition<DataType> > mWarnCondition;

  swatch::core::monitoring::Status mMonitoringStatus;

  std::vector< std::pair<const AbstractMetric*, boost::function<void ()> > > mDependantMetrics;
  friend class MonitorableObject;
};



/**
 * Metric condition for monitoring data of given type
 *
 * @tparam DataType type of the monitoring data
 */
template<typename DataType>
class MetricCondition : public AbstractMetricCondition {
public:
  MetricCondition();

  virtual ~MetricCondition();

  virtual bool operator()(const DataType&) const = 0;
};


} // namespace core
} // namespace swatch



#include "swatch/core/Metric.hxx"

#endif  /* __SWATCH_CORE_BASEMETRIC_HPP__ */


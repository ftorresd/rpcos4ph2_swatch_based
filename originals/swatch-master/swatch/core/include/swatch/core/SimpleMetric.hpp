/**
 * @file    Metric.hpp
 * @author  Tom Williams
 * @date    May 2015
 */

#ifndef __SWATCH_CORE_METRIC_HPP__
#define __SWATCH_CORE_METRIC_HPP__


#include <stddef.h>                     // for NULL
#include <string>                       // for string
#include <sys/time.h>                   // for timeval

#include "boost/smart_ptr/shared_ptr.hpp"  // for shared_ptr
#include "boost/thread/pthread/mutex.hpp"  // for mutex

#include "swatch/core/AbstractMetric.hpp"
#include "swatch/core/Metric.hpp"
#include "swatch/core/monitoring/Status.hpp"  // for monitoring::Status


namespace swatch {
namespace core {

class MonitorableObject;


template <typename DataType>
class SimpleMetric : public Metric<DataType> {
public:
  //! Construct a metric with no error/warning limits
  SimpleMetric(const std::string& aId, const std::string& aAlias);

  /*!
   * Construct the Metric
   * @param aId Alias for the new metric (no restrictions on characters)
   * @param aErrorCondition Functor used to determine if the metric's value indicates an error; the Metric takes ownership of this condition object. NULL value represents no error condition.
   * @param aWarningCondition Functor used to determine if the metric's value indicates a warning; the Metric takes ownership of this condition object. NULL value represents no warning condition.
   */
  SimpleMetric(const std::string& aId, const std::string& aAlias, MetricCondition<DataType>* aErrorCondition, MetricCondition<DataType>* aWarnCondition = NULL);

  ~SimpleMetric();

protected:
  //! Set the value of the metric
  void setValue(const DataType& aValue);

  friend class MonitorableObject;
};


} // namespace core
} // namespace swatch


#endif  /* __SWATCH_CORE_METRIC_HPP__ */


/**
 * @file    AbstractMetric.hpp
 * @author  Tom Williams
 * @date    May 2015
 */

#ifndef __SWATCH_CORE_ABSTRACTMETRIC_HPP__
#define __SWATCH_CORE_ABSTRACTMETRIC_HPP__


#include <iosfwd>
#include <string>
#include <sys/time.h>
#include <typeinfo>

#include "boost/any.hpp"
#include "boost/function.hpp"
#include "boost/smart_ptr/shared_ptr.hpp"

#include "swatch/core/exception.hpp"
#include "swatch/core/LeafObject.hpp"
#include "swatch/core/monitoring/Status.hpp"
#include "swatch/core/StatusFlag.hpp"
#include "swatch/core/TimePoint.hpp"
#include "swatch/core/utilities.hpp"


namespace swatch {
namespace core {

class AbstractMetricCondition;
class MetricSnapshot;
class MonitorableObject;

/*!
 * @brief Represents some set of monitoring data that is retrieved from hardware.
 *
 * This class is intended to contain the value of some monitoring data, provide
 * an interface to update the stored data value, and an interface to access that
 * stored data value, along with a corresponding status flag that expresses whether
 * or not the value lies within some internally-stored limits
 */
class AbstractMetric : public LeafObject {
public:
  AbstractMetric(const std::string& aId, const std::string& aAlias);

  virtual ~AbstractMetric();

  //! Returns metric value, status flag, and error/warning conditions, within a swatch::core::MetricSnapshot object. (THREAD SAFE)
  virtual MetricSnapshot getSnapshot() const = 0;

  //! Returns the status flag and monitoring mask/setting
  virtual std::pair<StatusFlag, monitoring::Status> getStatus() const = 0;

  //! Returns time at which metric's value was last updated. (THREAD SAFE)
  virtual SteadyTimePoint_t getUpdateTime() const = 0;

  //! Set this metric's monitoring mask / setting (i.e. enabled/non-critical/disabled)
  virtual void setMonitoringStatus(monitoring::Status aMonStatus) = 0;

protected:

  //! Set the value of the metric to being unknown
  virtual void setValueUnknown() = 0;

private:
  virtual void addDependantMetric(const AbstractMetric& aMetric, const boost::function<void ()>& aUpdateFunction)  = 0;

  friend class MonitorableObject;
};


std::ostream& operator<<(std::ostream& aOut, const AbstractMetricCondition& aCondition);


//! Base class for error/warning conditions stored in metric classes (that inherit from AbstractMetric).
class AbstractMetricCondition {
public:
  AbstractMetricCondition();
  virtual ~AbstractMetricCondition();

protected:
  virtual void print(std::ostream& aStream) const = 0;

  friend std::ostream& operator<<(std::ostream& aOut, const AbstractMetricCondition& aCondition);
};


} // namespace core
} // namespace swatch



#endif  /* __SWATCH_CORE_ABSTRACTMETRIC_HPP__ */

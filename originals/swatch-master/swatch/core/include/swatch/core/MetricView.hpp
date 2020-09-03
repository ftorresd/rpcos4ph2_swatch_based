/**
 * @file    MetricView.hpp
 * @author  Tom Williams
 * @date    May 2015
 */

#ifndef __SWATCH_CORE_METRICVIEW_HPP__
#define __SWATCH_CORE_METRICVIEW_HPP__


#include <map>
#include <set>
#include <stddef.h>                     // for size_t
#include <string>


// forward declarations
namespace log4cplus {
class Logger;
}

namespace swatch {
namespace core {

class AbstractMetric;
class MonitorableObject;

//! Represents map of Metrics whose ID paths match a specified regex
class MetricView {
public:

  MetricView(swatch::core::MonitorableObject& aMonObject, const std::string& aRegex);

  ~MetricView();

  //! Update values of all metrics in this view; calls updateMetrics() method of all associated monitorable objects
  void update();

  //! Number of metrics in the view
  size_t size() const;

  typedef std::map<std::string, swatch::core::AbstractMetric*>::iterator iterator;

  typedef std::map<std::string, swatch::core::AbstractMetric*>::const_iterator const_iterator;

  iterator begin();

  iterator end();

  const_iterator begin() const;

  const_iterator end() const;


private:

  static log4cplus::Logger sLogger;

  typedef std::map<std::string, swatch::core::AbstractMetric*> MetricMap_t;

  std::map<std::string, swatch::core::AbstractMetric*> mMetricsMap;

  std::set<swatch::core::MonitorableObject*> mMonitorableObjects;
};

} // namespace core
} // namespace swatch


#endif  /* __SWATCH_CORE_METRICVIEW_HPP__ */


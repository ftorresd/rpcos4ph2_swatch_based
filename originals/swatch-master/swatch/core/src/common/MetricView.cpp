/*
 * File:   MetricView.cpp
 * Author: Tom Williams
 * Date: May 2015
 */

#include "swatch/core/MetricView.hpp"


#include <ostream>                      // for operator<<, basic_ostream
#include <vector>                       // for vector

// boost headers
#include "boost/foreach.hpp"
#include "boost/regex.hpp"

// log4cplus headers
#include "log4cplus/loggingmacros.h"

// SWATCH headers
#include "swatch/logger/Logger.hpp"
#include "swatch/core/LeafObject.hpp"
#include "swatch/core/MonitorableObject.hpp"
#include "swatch/core/Object.hpp"



namespace swatch {
namespace core {

log4cplus::Logger MetricView::sLogger = swatch::logger::Logger::getInstance("swatch.core.MetricView");

MetricView::MetricView(swatch::core::MonitorableObject& aMonObject, const std::string& aRegex)
{
  LOG4CPLUS_DEBUG(sLogger, "Creating MetricView from object '" << aMonObject.getPath() << "' with regex " << aRegex);

  swatch::core::Object::iterator lObjIt;
  for (lObjIt = aMonObject.begin(); lObjIt != aMonObject.end(); lObjIt++) {
    
    if (swatch::core::AbstractMetric* lMetric = dynamic_cast<swatch::core::AbstractMetric*>(&*lObjIt)) {
      const std::string lMetricPath = lMetric->getPath();

      // Check against regex
      boost::cmatch match;
      if ( boost::regex_match (lMetricPath.c_str(), match, boost::regex(aRegex))) {
        LOG4CPLUS_DEBUG(sLogger, "MetricView : metric with path \"" << lMetricPath << "\" matches regex");
        mMetricsMap[lMetricPath] = lMetric;
        
        std::string lMonObjRelPath = lMetricPath.substr(aMonObject.getPath().size()+1);
        if (lMonObjRelPath.size() == lMetric->getId().size())
          mMonitorableObjects.insert(&aMonObject);
        else {
         lMonObjRelPath.resize(lMonObjRelPath.size() - lMetric->getId().size() - 1);
         mMonitorableObjects.insert(&aMonObject.getObj<MonitorableObject>(lMonObjRelPath));
        }
      }
    }

  }
}


MetricView::~MetricView()
{
}


void MetricView::update()
{
  BOOST_FOREACH(std::set<swatch::core::MonitorableObject*>::value_type monObj, mMonitorableObjects) {
    monObj->updateMetrics();
  }
}


size_t MetricView::size() const
{
  return mMetricsMap.size();
}


MetricView::iterator MetricView::begin()
{
  return mMetricsMap.begin();
}


MetricView::iterator MetricView::end()
{
  return mMetricsMap.end();
}


MetricView::const_iterator MetricView::begin() const
{
  return mMetricsMap.begin();
}


MetricView::const_iterator MetricView::end() const
{
  return mMetricsMap.end();
}


}
}

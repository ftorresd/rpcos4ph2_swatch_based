/*
 * MetricSettings.hpp
 *
 *  Created on: 26 Oct 2015
 *      Author: kreczko
 */

#ifndef __SWATCH_ACTION_MONITORINGSETTING_HPP__
#define __SWATCH_ACTION_MONITORINGSETTING_HPP__


#include <vector>
#include <map>

#include "boost/shared_ptr.hpp"

#include "swatch/core/monitoring/Status.hpp"


namespace swatch {
namespace action {

class MonitoringSetting {
public:
  MonitoringSetting(const std::string aId, const swatch::core::monitoring::Status aMonStatus);

//      boost::shared_ptr<AbstractMetricCondition>,
//      boost::shared_ptr<AbstractMetricCondition>,
//      );
  swatch::core::monitoring::Status getStatus() const;
  const std::string& getId() const;


  //! Returns metric's warning condition; NULL returned if metric doesn't have any warning condition
//    const AbstractMetricCondition* getWarningCondition() const;
//
//    //! Returns metric's error condition; NULL returned if metric doesn't have any error condition
//    const AbstractMetricCondition* getErrorCondition() const;


private:
  std::string mId;
  swatch::core::monitoring::Status mMonitoringStatus;
//    const boost::shared_ptr<AbstractMetricCondition> mErrorCondition_;
//    const boost::shared_ptr<AbstractMetricCondition> mWarnCondition;
};

typedef std::vector<MonitoringSetting> MonitoringSettings_t;
typedef boost::shared_ptr<const MonitoringSetting> MonitoringSettingPtr_t;

} // namespace action
} // namespace swatch


#endif /* __SWATCH_ACTION_MONITORINGSETTING_HPP__ */

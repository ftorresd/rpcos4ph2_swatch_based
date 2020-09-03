
#include "swatch/action/MonitoringSetting.hpp"



namespace swatch {
namespace action {

MonitoringSetting::MonitoringSetting(const std::string aId, const swatch::core::monitoring::Status aMonStatus) :
  mId(aId),
  mMonitoringStatus(aMonStatus)
{

}

swatch::core::monitoring::Status MonitoringSetting::getStatus() const
{
  return mMonitoringStatus;
}

const std::string& MonitoringSetting::getId() const
{
  return mId;
}

} // namespace action
} // namespace swatch

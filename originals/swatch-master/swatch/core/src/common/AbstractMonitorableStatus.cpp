
#include "swatch/core/AbstractMonitorableStatus.hpp"


// boost headers
#include "boost/thread/lock_options.hpp"  // for adopt_lock_t


namespace swatch {
namespace core {


//------------------------------------------------------------------------------------
AbstractMonitorableStatus::AbstractMonitorableStatus()
{
}

//------------------------------------------------------------------------------------
AbstractMonitorableStatus::~AbstractMonitorableStatus()
{
}

//------------------------------------------------------------------------------------
boost::unique_lock<boost::mutex>& AbstractMonitorableStatus::getUniqueLock(MonitorableStatusGuard& aGuard) const
{
  return aGuard.mLockGuard;
}


//------------------------------------------------------------------------------------
MonitorableStatusGuard::MonitorableStatusGuard(const AbstractMonitorableStatus& aStatus) :
  mStatus(aStatus),
  mLockGuard(aStatus.mMutex)
{
}

//------------------------------------------------------------------------------------
MonitorableStatusGuard::MonitorableStatusGuard(const AbstractMonitorableStatus& aStatus, boost::adopt_lock_t) :
  mStatus(aStatus),
  mLockGuard(aStatus.mMutex, boost::adopt_lock_t())
{
}

//------------------------------------------------------------------------------------
MonitorableStatusGuard::~MonitorableStatusGuard()
{
}

//------------------------------------------------------------------------------------
bool MonitorableStatusGuard::isCorrectGuard(const AbstractMonitorableStatus& aStatus) const
{
  return (&mStatus == &aStatus);
}


}
}

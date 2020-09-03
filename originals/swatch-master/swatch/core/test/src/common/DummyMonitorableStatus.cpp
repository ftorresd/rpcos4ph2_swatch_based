#include "swatch/core/test/DummyMonitorableStatus.hpp"

#include "swatch/core/exception.hpp"

#include "boost/thread/thread.hpp"


namespace swatch {
namespace core {
namespace test {


DummyMonitorableStatus::DummyMonitorableStatus() :
  mIsUpdatingMetrics(false),
  mNumberOfMetricReaders(0)
{
}

DummyMonitorableStatus::~DummyMonitorableStatus()
{
}

void DummyMonitorableStatus::throwIfWrongGuard(const MonitorableStatusGuard& aGuard) const
{
  if ( ! aGuard.isCorrectGuard(*this) )
    XCEPT_RAISE(core::InvalidArgument,"Incorrect MonitorableStatusGuard given to DummyMonitorableStatus method");
}

bool DummyMonitorableStatus::isUpdatingMetrics(const MonitorableStatusGuard& aGuard) const
{
  throwIfWrongGuard(aGuard);
  return mIsUpdatingMetrics;
}

void DummyMonitorableStatus::waitUntilReadyToUpdateMetrics(MonitorableStatusGuard& aGuard)
{
  throwIfWrongGuard(aGuard);
  while ( mIsUpdatingMetrics || (mNumberOfMetricReaders > 0) ) {
    getUniqueLock(aGuard).unlock();
    boost::this_thread::sleep_for(boost::chrono::microseconds(2));
    getUniqueLock(aGuard).lock();
  };
  mIsUpdatingMetrics = true;
}

void DummyMonitorableStatus::finishedUpdatingMetrics(const MonitorableStatusGuard& aGuard)
{
  throwIfWrongGuard(aGuard);
  mIsUpdatingMetrics = false;
}

void DummyMonitorableStatus::waitUntilReadyToReadMetrics(MonitorableStatusGuard& aGuard)
{
  throwIfWrongGuard(aGuard);
  while ( mIsUpdatingMetrics ) {
    getUniqueLock(aGuard).unlock();
    boost::this_thread::sleep_for(boost::chrono::microseconds(2));
    getUniqueLock(aGuard).lock();
  };
  mNumberOfMetricReaders++;
}

void DummyMonitorableStatus::finishedReadingMetrics(const MonitorableStatusGuard& aGuard)
{
  throwIfWrongGuard(aGuard);
  mNumberOfMetricReaders--;
}


} /* namespace test */
} /* namespace core */
} /* namespace swatch */

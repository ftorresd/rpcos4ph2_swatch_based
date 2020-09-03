/**
 * @file    AbstractMonitorableStatus.hpp
 * @author  Tom Williams
 * @date    November 2015
 */

#ifndef __SWATCH_CORE_ABSTRACTMONITORABLESTATUS_HPP__
#define __SWATCH_CORE_ABSTRACTMONITORABLESTATUS_HPP__

// BOOST headers
#include "boost/noncopyable.hpp"
#include "boost/thread/lock_types.hpp"
#include "boost/thread/pthread/mutex.hpp"


// forward declarations
namespace boost {
struct adopt_lock_t;
}


namespace swatch {
namespace core {


class MonitorableStatusGuard;

class AbstractMonitorableStatus : public boost::noncopyable {
protected:
  AbstractMonitorableStatus();

public:
  virtual ~AbstractMonitorableStatus();

  virtual bool isUpdatingMetrics(const MonitorableStatusGuard& aGuard) const = 0;

  virtual void waitUntilReadyToUpdateMetrics(MonitorableStatusGuard& aGuard) = 0;

  virtual void finishedUpdatingMetrics(const MonitorableStatusGuard& aGuard) = 0;

  virtual void waitUntilReadyToReadMetrics(MonitorableStatusGuard& aGuard) = 0;

  virtual void finishedReadingMetrics(const MonitorableStatusGuard& aGuard) = 0;

protected:
  boost::unique_lock<boost::mutex>& getUniqueLock(MonitorableStatusGuard& aGuard) const;

protected:
  mutable boost::mutex mMutex;

  friend class MonitorableStatusGuard;
};


class MonitorableStatusGuard {
public:
  MonitorableStatusGuard(const AbstractMonitorableStatus& aStatus);
  MonitorableStatusGuard(const AbstractMonitorableStatus& aStatus, boost::adopt_lock_t);
  virtual ~MonitorableStatusGuard();

  bool isCorrectGuard(const AbstractMonitorableStatus& aStatus) const;

private:
  MonitorableStatusGuard(); // no default CTOR
  MonitorableStatusGuard(const MonitorableStatusGuard&); // non-copyable
  MonitorableStatusGuard& operator=(const MonitorableStatusGuard& aGuard);// non-assignable

  const AbstractMonitorableStatus& mStatus;
  boost::unique_lock<boost::mutex> mLockGuard;

  friend class AbstractMonitorableStatus;
};


} // namespace core
} // namespace swatch

#endif  /* __SWATCH_CORE_ABSTRACTMONITORABLESTATUS_HPP__ */


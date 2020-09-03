
#ifndef __SWATCH_CORE_TEST_DUMMYMONITORABLESTATUS_HPP__
#define __SWATCH_CORE_TEST_DUMMYMONITORABLESTATUS_HPP__


#include "swatch/core/AbstractMonitorableStatus.hpp"


namespace swatch {
namespace core {
namespace test {


class DummyMonitorableStatus : public AbstractMonitorableStatus {
public:
  DummyMonitorableStatus();
  ~DummyMonitorableStatus();

  void throwIfWrongGuard(const MonitorableStatusGuard& aGuard) const;

  bool isUpdatingMetrics(const MonitorableStatusGuard& aGuard) const;
  void waitUntilReadyToUpdateMetrics(MonitorableStatusGuard& aGuard);
  void finishedUpdatingMetrics(const MonitorableStatusGuard& aGuard);

  void waitUntilReadyToReadMetrics(MonitorableStatusGuard& aGuard);
  void finishedReadingMetrics(const MonitorableStatusGuard& aGuard);

private:
  bool mIsUpdatingMetrics;
  size_t mNumberOfMetricReaders;
};


} /* namespace test */
} /* namespace core */
} /* namespace swatch */

#endif /* SWATCH_CORE_TEST_DUMMYMONITORABLEOBJECTS_HPP */

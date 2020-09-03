/**
 * @file    Status.hpp
 * @author  Tom Williams
 * @date    May 2017
 */


#ifndef __SWATCH_CORE_MONITORING_STATUS_HPP__
#define __SWATCH_CORE_MONITORING_STATUS_HPP__

#include <iosfwd>
#include <map>
#include <string>

namespace swatch {
namespace core {
namespace monitoring {

/**
 * Flag for deciding if failures of MonitorableObject will affect the parent.
 * If set to kENABLED they will (default behaviour) and if set to kNON_CRITICAL
 * they will not.
 */
enum Status {
  kEnabled,
  kNonCritical,
  kDisabled
};

extern const std::map<std::string, Status> kStringToStatus;

std::ostream& operator<<(std::ostream& aStream, const monitoring::Status aMonStatus);

} // namespace monitoring
} // namespace core
} // namespace swatch


#endif /* __SWATCH_CORE_MONITORING_STATUS_HPP__ */

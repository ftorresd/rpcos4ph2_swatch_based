
#include "swatch/core/monitoring/Status.hpp"


#include <boost/assign.hpp>


namespace swatch {
namespace core {

namespace monitoring {

extern const std::map<std::string, Status> kStringToStatus = boost::assign::map_list_of("enabled", kEnabled)(
      "non-critical", kNonCritical)("disabled", kDisabled);

std::ostream& operator<<(std::ostream& aStream, const monitoring::Status aMonStatus)
{
  switch (aMonStatus) {
    case kEnabled:
      aStream << "enabled";
      break;
    case kNonCritical:
      aStream << "non-critical";
      break;
    case kDisabled:
      aStream << "disabled";
      break;
    default:
      aStream << "enabled";
      break;
  }
  return aStream;
}

} // namespace monitoring
} // namespace core
} // namespace swatch

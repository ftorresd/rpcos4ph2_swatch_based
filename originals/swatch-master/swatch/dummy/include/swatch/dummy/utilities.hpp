
#ifndef __SWATCH_DUMMY_UTILITIES_HPP__
#define __SWATCH_DUMMY_UTILITIES_HPP__


#include <stdint.h>
#include <vector>


namespace swatch {

// forward declarations
namespace core {
class MonitorableObject;
class MetricSnapshot;
class MonitorableObjectSnapshot;
}

namespace dummy {

bool filterOutMaskedPorts(const core::MonitorableObject& aObj);

bool filterOutDisabledActionables(const core::MonitorableObject& aObj);

const uint32_t* sumUpCRCErrors(const std::vector<core::MetricSnapshot>& aSnapshots);

const uint32_t* countObjectsInError(const std::vector<core::MonitorableObjectSnapshot>& aSnapshots);

}
}


#endif /* __SWATCH_DUMMY_UTILITIES_HPP__ */


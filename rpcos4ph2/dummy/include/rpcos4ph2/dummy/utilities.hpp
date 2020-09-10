
#ifndef _RPCOS4PH2_DUMMY_UTILITIES_HPP__
#define _RPCOS4PH2_DUMMY_UTILITIES_HPP__


#include <stdint.h>
#include <vector>

#include "swatch/action/ActionableObject.hpp"
#include "swatch/core/MonitorableObject.hpp"
#include "swatch/core/MetricSnapshot.hpp"


namespace rpcos4ph2 {

// forward declarations
namespace core {
class MonitorableObject;
class MetricSnapshot;
class MonitorableObjectSnapshot;
}

namespace dummy {

bool filterOutMaskedPorts(const swatch::core::MonitorableObject& aObj);

bool filterOutDisabledActionables(const swatch::core::MonitorableObject& aObj);

const uint32_t* sumUpCRCErrors(const std::vector<swatch::core::MetricSnapshot>& aSnapshots);

const uint32_t* countObjectsInError(const std::vector<swatch::core::MonitorableObjectSnapshot>& aSnapshots);

}
}


#endif /* _RPCOS4PH2_DUMMY_UTILITIES_HPP__ */



#include "rpcos4ph2/dummy/utilities.hpp"


// Standard headers
#include <cstdlib>

// SWATCH headers
#include "swatch/action/ActionableObject.hpp"
#include "swatch/core/MonitorableObject.hpp"
#include "swatch/core/MetricSnapshot.hpp"
#include "swatch/processor/Port.hpp"


namespace rpcos4ph2 {
namespace dummy {


bool filterOutMaskedPorts(const swatch::core::MonitorableObject& aObj)
{
  return !dynamic_cast<const swatch::processor::InputPort&>(aObj).isMasked();
}


bool filterOutDisabledActionables(const swatch::core::MonitorableObject& aObj)
{
  return (aObj.getMonitoringStatus() == swatch::core::monitoring::kEnabled);
}


const uint32_t* sumUpCRCErrors(const std::vector<swatch::core::MetricSnapshot>& aSnapshots)
{
  uint32_t lResult = 0;
//  std::cout << "> sumUpCRCErrors: " << aSnapshots.size() << " snapshots received"<< std::endl;
  for (auto lIt=aSnapshots.begin(); lIt != aSnapshots.end(); lIt++) {
    if (lIt->isValueKnown())
      (lResult) += lIt->getValue<uint32_t>();
    else
      return NULL;
  }
//  std::cout << "  result: " << lResult << std::endl;
  return new uint32_t(lResult);
}


const uint32_t* countObjectsInError(const std::vector<swatch::core::MonitorableObjectSnapshot>& aSnapshots)
{
  uint32_t lResult = 0;
//  std::cout << "> countObjectsInError: " << aSnapshots.size() << " snapshots received" << std::endl;

  for (auto lIt=aSnapshots.begin(); lIt != aSnapshots.end(); lIt++) {
    if (lIt->getStatusFlag() == swatch::core::kUnknown)
      return NULL;
    else if (lIt->getStatusFlag() == swatch::core::kError)
      lResult++;
  }

//  std::cout << "  result = " << lResult << std::endl;
  return new uint32_t(lResult);
}


} // ns: dummy
} // ns: swatch


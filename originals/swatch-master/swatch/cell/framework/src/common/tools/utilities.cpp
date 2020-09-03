

#include "swatchcell/framework/tools/utilities.h"


#include "swatch/action/ActionableObject.hpp"
#include "swatch/dtm/DaqTTCManager.hpp"
#include "swatch/processor/Processor.hpp"
#include "swatch/system/System.hpp"


namespace swatchcellframework {
namespace tools {


void updateMetricsOfEnabledObjects(swatch::system::System& aSystem)
{
  typedef std::deque <swatch::processor::Processor*>::const_iterator ProcessorIt_t;
  typedef std::deque <swatch::dtm::DaqTTCManager*>::const_iterator DaqTTCMgrIt_t;

  // Loop over processors; only update status of enabled ones
  for (ProcessorIt_t lIt = aSystem.getProcessors().begin(); lIt != aSystem.getProcessors().end(); lIt++) {
    if ((*lIt)->getStatus().isEnabled())
      updateMetrics(**lIt);
  }

  // Loop over AMC13s; only update status of enabled ones
  for (DaqTTCMgrIt_t lIt = aSystem.getDaqTTCs().begin(); lIt != aSystem.getDaqTTCs().end(); lIt++) {
    if ((*lIt)->getStatus().isEnabled())
      updateMetrics(**lIt);
  }
}


void updateMetrics(swatch::action::ActionableObject& aObj)
{
//  std::cout << "updateMetrics(ActObj '" << aObj.getPath() << "')" << std::endl;
  // Create one metric update guard to be used for all descendant monitorable objects, to avoid the actionable 
  // ... object's status continually flip-flopping between "updating metrics" & "no uptdating metrics"
  swatch::core::MetricUpdateGuard lUpdateGuard(aObj);

  // Crawl through all non-disabled monitorable objects in hierarchy, and call update method of their child metrics
  std::string lDisabledObjPath;
  for (swatch::core::Object::iterator it = aObj.begin(); it != aObj.end(); it++) {
    if (swatch::core::MonitorableObject * lMonSubObj = dynamic_cast<swatch::core::MonitorableObject*> (&*it)) {
//      std::cout << "MonObj '" << lMonSubObj->getPath() << "'" << std::endl;
      // Skip this sub-object if under a disabled object
      if ( (!lDisabledObjPath.empty()) && (lMonSubObj->getPath().find(lDisabledObjPath)==size_t(0)) )
        continue;
      else if (lMonSubObj->getMonitoringStatus() == swatch::core::monitoring::kDisabled) {
        lDisabledObjPath = lMonSubObj->getPath();
        continue;
      } else
        lMonSubObj->updateMetrics(lUpdateGuard);
//      std::cout << "   metrics were updated!" << std::endl;
    }
  }
}


} // end ns: tools
} // end ns: swatchcellframework



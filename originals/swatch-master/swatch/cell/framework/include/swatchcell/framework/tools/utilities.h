
#ifndef __SWATCHCELL_FRAMEWORK_TOOLS_UTILITIES_H__
#define __SWATCHCELL_FRAMEWORK_TOOLS_UTILITIES_H__


// Forward declarations 
namespace swatch {
namespace action {
class ActionableObject;
}
namespace system {
class System;
}
}


namespace swatchcellframework {
namespace tools {

  //! Calls updateMetrics function on all processors and AMC13s that are enabled 
void updateMetricsOfEnabledObjects(swatch::system::System& aSystem);

//! Calls update method of all metrics within an actionable object ((e.g. processor, system, ...), and all of its descendants that are non-disabled MonitorableObject  
void updateMetrics(swatch::action::ActionableObject& aObj);

} // end ns: tools
} // end ns: swatchcellframework

#endif /* _swatchcell_framework_tools_utilities_h_ */


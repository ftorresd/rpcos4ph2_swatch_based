/* 
 * File:   MonitoringThread.hpp
 * Author: tom
 * Date:   February 2016
 */

#ifndef __SWATCHCELL_FRAMEWORK_MONITORINGTHREAD_H__
#define __SWATCHCELL_FRAMEWORK_MONITORINGTHREAD_H__


// Standard headers
#include <stddef.h>

// boost headers
#include "boost/chrono.hpp"
#include "boost/noncopyable.hpp"
#include "boost/regex_fwd.hpp"
#include "boost/thread/condition_variable.hpp"
#include "boost/thread/mutex.hpp"
#include "boost/thread/thread.hpp"
#include <boost/foreach.hpp>
#include "boost/shared_ptr.hpp"


// xdata types
#include "xdata/xdata.h"
#include "xdata/TableIterator.h"
#include "xdata/Table.h"


///! GC THE SLAYER
#include "xdata/Boolean.h"
#include "xdata/Float.h"
#include "xdata/Double.h"
#include "xdata/Integer.h"
#include "xdata/TimeVal.h"
#include "xdata/String.h"
#include "xdata/UnsignedLong.h"
#include "xdata/UnsignedInteger32.h"
#include "xdata/UnsignedInteger.h"
#include "xdata/UnsignedShort.h"

#include "swatchcell/framework/InfoSpaceBuilder.h"
#include "swatchcell/framework/PeriodicWorkerThread.hpp"
#include "swatch/system/System.hpp"


// Forward declarations
namespace swatch {
namespace action {
class ActionableObject;
class MaskableObject;
}
}

namespace swatchcellframework {

// Forward declarations
class CellAbstract;
class CellContext;


class MonitoringThread : public PeriodicWorkerThread {
public:

  MonitoringThread(const std::string& aParentLoggerName, CellContext& aContext, size_t aPeriodInSeconds);

  ~MonitoringThread();

  Stats getStats() const;
  
  //!build xml flashlist definition out of the Infospace
  inline void getFlashlist( std::ostream & flash ) { mInfospace.getFlashlist( flash ); };

private:
  //! Iterates over the enabled processors & AMC13s in the system, and calls the SWATCH API's "update metrics" methods 
  void doWork(); 
  
  CellContext& mContext;

  
  //! ! GC THE SLAYER
  InfoSpaceBuilder mInfospace;
  std::map< std::string, xdata::Serializable * > mMoniMap;

  static boost::regex kColNameRegex;

  void fillInfospace( swatch::system::System& aSystem );

  void resetInfospaceTables();

  void createTables(swatch::system::System& aSystem);

  xdata::Table* createActionableTable(const std::vector<swatch::action::ActionableObject*>& lActionables);

  xdata::Table* createPortTable(const std::vector<swatch::core::MonitorableObject*>& lPorts);

  void fillProcessorTables(swatch::system::System& aSystem);

  void fillTables(swatch::system::System& aSystem);
  
  void fillActionableTable(const std::string& aTableName, swatch::action::ActionableObject& aActionable);

  void fillPortTable(const std::string& aTableName, const std::string& aActionableId, swatch::core::MonitorableObject& aPort);

  static std::string getInfospaceTableName(const std::string& aPrefix, const swatch::core::Object& aObject);

  static bool checkColumnName(const std::string& aColName);

  static std::string getLowercaseCopy(const std::string& aString);
};
  
  
} // end ns: swatchcellframework


#endif	/* _SWATCHCELL_FRAMEWORK_MONITORINGTHREAD_HPP_ */


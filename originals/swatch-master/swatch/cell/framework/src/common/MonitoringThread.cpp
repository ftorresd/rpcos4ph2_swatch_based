/*
 * File:   MonitoringThread.cpp
 * Author: tom
 * Date:   February 2016
 */

 // SWATCH cell headers
#include "swatchcell/framework/CellAbstract.h"
#include "swatchcell/framework/tools/panelFunctions.h"
#include "swatchcell/framework/tools/utilities.h"
#include "swatchcell/framework/MonitoringThread.h"

#include "swatchcell/framework/InfoSpaceBuilder.h"

// boost headers
#include "boost/algorithm/string/case_conv.hpp"
#include "boost/chrono/chrono_io.hpp"
#include "boost/chrono/io/ios_base_state.hpp"
#include "boost/regex.hpp"
#include "boost/thread/lock_guard.hpp"
#include "boost/unordered/unordered_set.hpp"

// SWATCH headers
#include "swatch/dtm/DaqTTCManager.hpp"
#include "swatch/processor/Processor.hpp"
#include "swatch/system/System.hpp"

#include "swatch/processor/Port.hpp"
#include "swatch/processor/PortCollection.hpp"
#include "swatch/dtm/AMCPort.hpp"
#include "swatch/dtm/AMCPortCollection.hpp"


namespace swatchcellframework {


MonitoringThread::MonitoringThread(const std::string& aParentLoggerName, CellContext& aContext, size_t aPeriodInSeconds) :
  PeriodicWorkerThread(aParentLoggerName + ".MonitoringThread", aPeriodInSeconds),
  mContext(aContext),
  mInfospace( getLowercaseCopy(mContext.getSubsystemId() + "_cell"), aContext, getLogger()) 
{
  // 'static' db tables
  mMoniMap["RunNumber"] = new xdata::UnsignedLong;
}


MonitoringThread::~MonitoringThread()
{
  // TODO: Need to ensure that the thread is no longer active by this point 
  //  - will likely require adding new method to base class
 
  BOOST_FOREACH( auto & moni, mMoniMap )
    delete moni.second;
}


void MonitoringThread::doWork()
{
  CellContext::SharedGuard_t lGuard(mContext);
  swatch::system::System& lSys = mContext.getSystem(lGuard);

  swatchcellframework::tools::updateMetricsOfEnabledObjects(lSys);

  try {
    fillInfospace( lSys );
  }
  catch (const std::exception& aException) {
    LOG4CPLUS_ERROR(getLogger(), "Exception of type '" << swatch::core::demangleName(typeid(aException).name())
            << "' thrown within monitoring thread when filling infospace. Message: " << aException.what());
  }
  catch ( ... ) { 
    LOG4CPLUS_ERROR(getLogger(), "Object of unknown type thrown within monitoring thread when filling infospace");
  }
}



boost::regex MonitoringThread::kColNameRegex("[a-zA-Z0-9_$#]+");


void MonitoringThread::fillInfospace( swatch::system::System& aSystem )
{
  LOG4CPLUS_DEBUG(getLogger(), "Monitoring thread: Filling infospace");

  typedef boost::chrono::steady_clock SteadyClock_t;
  SteadyClock_t::time_point lStartTime = SteadyClock_t::now();

  resetInfospaceTables();
  boost::chrono::milliseconds lResetTime = boost::chrono::duration_cast<boost::chrono::milliseconds>(SteadyClock_t::now() - lStartTime);
  lStartTime = SteadyClock_t::now();
  
  // 1) Run number: Set to 0 if out of run, actual value otherwise
  if (mContext.getRunControlOperation().getFSM().getState() == RunControl::kStateRunning)
    mMoniMap["RunNumber"]->setValue(mContext.getRunControlOperation().getRunNumber());
  else
    mMoniMap["RunNumber"]->setValue(xdata::UnsignedLong(0));

  createTables(aSystem);
  boost::chrono::milliseconds lCreateTime = boost::chrono::duration_cast<boost::chrono::milliseconds>(SteadyClock_t::now() - lStartTime);
  lStartTime = SteadyClock_t::now();
  
  fillTables(aSystem);
  boost::chrono::milliseconds lFillTime = boost::chrono::duration_cast<boost::chrono::milliseconds>(SteadyClock_t::now() - lStartTime);
  lStartTime = SteadyClock_t::now();

  LOG4CPLUS_DEBUG(getLogger(), "Monitoring thread: pushing updated data to infospace");
  mInfospace.push( mMoniMap );
  boost::chrono::milliseconds lPushTime = boost::chrono::duration_cast<boost::chrono::milliseconds>(SteadyClock_t::now() - lStartTime);

  std::ostringstream lMsg;
  lMsg << boost::chrono::symbol_format;
  lMsg << "Monitoring thread: Infospace tables filled, and data pushed (";
  lMsg << lResetTime << " reset + " << lCreateTime << " create + " << lFillTime << " fill + " << lPushTime << " push)";
  
  LOG4CPLUS_INFO(getLogger(), lMsg.str());
}


void MonitoringThread::resetInfospaceTables()
{
  for(auto lIt = mMoniMap.begin(); lIt != mMoniMap.end(); lIt++) {
    if (xdata::Table* lTable = dynamic_cast<xdata::Table*>(lIt->second)) {
      // 1) Save the column definitions
      std::vector< std::pair<std::string, std::string> > lColumnDefinitions;
      BOOST_FOREACH(const std::string& lColName, lTable->getColumns())
      {
        lColumnDefinitions.push_back( std::make_pair(lColName, lTable->getColumnType(lColName)));
      }

      // 2) xdata::Table::clear() - clears both rows and columns from table
      lTable->clear();

      // 3) Add back the column definitions
      for (auto lColumnIt = lColumnDefinitions.begin(); lColumnIt != lColumnDefinitions.end(); lColumnIt++)
        lTable->addColumn(lColumnIt->first, lColumnIt->second);
    }
  }  
}


void MonitoringThread::createTables(swatch::system::System& aSystem)
{
  std::map<std::string, std::vector<swatch::action::ActionableObject*> > lActionablesByTableName;
  std::map<std::string, std::vector<swatch::core::MonitorableObject*> > lPortsByTableName;

  // 1) Loop over the processors, and DAQ-TTC managers, determining which of them is of each type
  for (auto lIt = aSystem.getProcessors().begin(); lIt != aSystem.getProcessors().end(); lIt++) {
    swatch::processor::Processor& lProc = **lIt;
    
    // Determine this processor's table name
    const std::string lTableName = getInfospaceTableName("processor", lProc);
    
    // If this processor's table doesn't already exists, then add to the map of processors by type
    if (mMoniMap.count(lTableName) == 0) {
      if (lActionablesByTableName.count(lTableName) == 0)
        lActionablesByTableName.insert( std::make_pair(lTableName, std::vector<swatch::action::ActionableObject*>()));
      lActionablesByTableName.at(lTableName).push_back(&lProc);
    }
    
    auto lRxPorts = lProc.getInputPorts().getPorts();
    for (auto lPortIt = lRxPorts.begin(); lPortIt != lRxPorts.end(); lPortIt++) {
      const std::string lPortTableName = getInfospaceTableName("rxPort", **lPortIt);

      if (mMoniMap.count(lPortTableName) == 0) {
        
        if (lPortsByTableName.count(lPortTableName) == 0)
          lPortsByTableName.insert( std::make_pair(lPortTableName, std::vector<swatch::core::MonitorableObject*>()) );
        lPortsByTableName.at(lPortTableName).push_back(*lPortIt);
      }
    }

    auto lTxPorts = lProc.getOutputPorts().getPorts();
    for (auto lPortIt = lTxPorts.begin(); lPortIt != lTxPorts.end(); lPortIt++) {
      const std::string lPortTableName = getInfospaceTableName("txPort", **lPortIt);

      if (mMoniMap.count(lPortTableName) == 0) {
        
        if (lPortsByTableName.count(lPortTableName) == 0)
          lPortsByTableName.insert( std::make_pair(lPortTableName, std::vector<swatch::core::MonitorableObject*>()) );
        lPortsByTableName.at(lPortTableName).push_back(*lPortIt);
      }
    }
  }

  for (auto lIt = aSystem.getDaqTTCs().begin(); lIt != aSystem.getDaqTTCs().end(); lIt++) {
    swatch::dtm::DaqTTCManager& lDTM = **lIt;

    // Determine this processor's table name
    const std::string lTableName = getInfospaceTableName("amc13", lDTM);
    
    // If this processor's table doesn't already exists, then add to the map of processors by type
    if (mMoniMap.count(lTableName) == 0) {
      if (lActionablesByTableName.count(lTableName) == 0)
        lActionablesByTableName.insert( std::make_pair(lTableName, std::vector<swatch::action::ActionableObject*>()));
      lActionablesByTableName.at(lTableName).push_back(&lDTM);
    }

    auto lPorts = lDTM.getAMCPorts().getPorts();
    for (auto lPortIt = lPorts.begin(); lPortIt != lPorts.end(); lPortIt++) {
      const std::string lPortTableName = getInfospaceTableName("amc13", **lPortIt);

      if (mMoniMap.count(lPortTableName) == 0) {
        
        if (lPortsByTableName.count(lPortTableName) == 0)
          lPortsByTableName.insert( std::make_pair(lPortTableName, std::vector<swatch::core::MonitorableObject*>()) );
        lPortsByTableName.at(lPortTableName).push_back(*lPortIt);
      }
    }

  }


  // 2) Loop over "new" actionable tables, determining column names for each table
  for (auto lIt = lActionablesByTableName.begin(); lIt != lActionablesByTableName.end(); lIt++) {
    if (!checkColumnName(lIt->first)) {
      LOG4CPLUS_WARN(getLogger(), "Actionable table name '" + lIt->first + "' is not a valid column name; cannot add this table.");
      continue;
    }

    std::ostringstream lLogMsg;
    lLogMsg << "Monitoring thread: creating table '" << lIt->first << "' for " << lIt->second.size() << " boards:";
    for(auto lActionableIt = lIt->second.begin(); lActionableIt != lIt->second.end(); lActionableIt++)
      lLogMsg << "  '" << (*lActionableIt)->getId() << "'";
    LOG4CPLUS_INFO(getLogger(), lLogMsg.str());

    mMoniMap[lIt->first] = createActionableTable(lIt->second);
  }
  
  // 3) Loop over "new" port tables, determining column names for each table
  for (auto lIt = lPortsByTableName.begin(); lIt != lPortsByTableName.end(); lIt++) {
    if (!checkColumnName(lIt->first)) {
      LOG4CPLUS_WARN(getLogger(), "Port table name '" + lIt->first + "' is not a valid column name; cannot add this table.");
      continue;
    }

    std::ostringstream lLogMsg;
    lLogMsg << "Monitoring thread: creating table '" << lIt->first << "' for " << lIt->second.size() << " ports:";
    for(auto lActionableIt = lIt->second.begin(); lActionableIt != lIt->second.end(); lActionableIt++)
      lLogMsg << "  '" << (*lActionableIt)->getId() << "'";
    LOG4CPLUS_INFO(getLogger(), lLogMsg.str());

    mMoniMap[lIt->first] = createPortTable(lIt->second);
  }
}

  
xdata::Table* MonitoringThread::createActionableTable(const std::vector<swatch::action::ActionableObject*>& lActionables)
{
  swatch::action::ActionableObject& lObj0 = *lActionables.at(0);

  xdata::Table* lTable = new xdata::Table();
  lTable->addColumn("id", xdata::String().type());

  for(swatch::core::Object::iterator lIt = lObj0.begin(); lIt != lObj0.end(); lIt++) {
    if (const swatch::core::AbstractMetric* lMetric = dynamic_cast<const swatch::core::AbstractMetric*>(&*lIt)) {
      // Skip metrics within ports
      if ( const swatch::processor::Processor* lProc = dynamic_cast<swatch::processor::Processor*>(&lObj0) ) {
        if (lProc->getInputPorts().isAncestorOf(*lMetric) || lProc->getOutputPorts().isAncestorOf(*lMetric)) {
          if (lObj0.getNumberOfGenerationsTo(*lMetric) > 2)
            continue;
        }
      }
      else {
        if (lObj0.getNumberOfGenerationsTo(*lMetric) > 2)
          continue;
      }

      const std::string lMetricRelPath = lMetric->getPath().substr(lObj0.getPath().size() + 1);
      bool lMetricExistsInAllProcessors = true;

      for (auto lAcIt = lActionables.begin(); lAcIt != lActionables.end(); lAcIt++) {
        try {
          (*lAcIt)->getObj<swatch::core::AbstractMetric>(lMetricRelPath);
        } catch (const std::exception& aException) {
          lMetricExistsInAllProcessors = false;
        }

      }

      if ( lMetricExistsInAllProcessors ) {
//        // Check that the metric doesn't contain any "_", since "." will be replaced by "_" for column names
//        if (lMetricRelPath.find('_') != std::string::npos) {
//          LOG4CPLUS_WARN(No , "Cannot create infospace table column for actionable metric '" << lMetricRelPath << "' since it contains one or more underscores, '_'");
//          continue;
//        }

        std::string lColName = lMetricRelPath;
        std::replace(lColName.begin(), lColName.end(), '.', '_');
        
        if (!checkColumnName(lColName)) {
          LOG4CPLUS_WARN(getLogger(), "Cannot create infospace table column '" << lColName << "' - invalid column name.");
          continue;
        }
        
        lTable->addColumn(lColName, lMetric->getSnapshot().getAsSerializable().type());
      }
    }
  }

  return lTable;
}


xdata::Table* MonitoringThread::createPortTable(const std::vector<swatch::core::MonitorableObject*>& lPorts)
{
  swatch::core::MonitorableObject& lPort0 = *lPorts.at(0);

  xdata::Table* lTable = new xdata::Table();
  lTable->addColumn("boardId", xdata::String().type());
  lTable->addColumn("portId", xdata::String().type());
  if ( dynamic_cast<swatch::action::MaskableObject*>(&lPort0) )
    lTable->addColumn("isMasked", xdata::Boolean().type());

  for(swatch::core::Object::iterator lIt = lPort0.begin(); lIt != lPort0.end(); lIt++) {
    if (const swatch::core::AbstractMetric* lMetric = dynamic_cast<const swatch::core::AbstractMetric*>(&*lIt)) {
      // Skip metrics within ports -- currently achieved by excluding all metrics more than two generations below
      if ( lPort0.getNumberOfGenerationsTo(*lMetric) > 2)
        continue;

      const std::string lMetricRelPath = lMetric->getPath().substr(lPort0.getPath().size() + 1);
      bool lMetricExistsInAllProcessors = true;

      for (auto lPortIt = lPorts.begin(); lPortIt != lPorts.end(); lPortIt++) {
        try {
          (*lPortIt)->getObj<swatch::core::AbstractMetric>(lMetricRelPath);
        } catch (const std::exception& aException) {
          lMetricExistsInAllProcessors = false;
        }

      }

      if ( lMetricExistsInAllProcessors ) {
//        // Check that the metric doesn't contain any "_", since "." will be replaced by "_" for column names
//        if (lMetricRelPath.find('_') != std::string::npos) {
//          LOG4CPLUS_WARN(mLogger, "Cannot create infospace table column for actionable metric '" << lMetricRelPath << "' since it contains one or more underscores, '_'");
//          continue;
//        }

        std::string lColName = lMetricRelPath;
        std::replace(lColName.begin(), lColName.end(), '.', '_');

        if (!checkColumnName(lColName)) {
          LOG4CPLUS_WARN(getLogger(), "Cannot create infospace table column '" << lColName << "' - invalid column name.");
          continue;
        }
        
        lTable->addColumn(lColName, lMetric->getSnapshot().getAsSerializable().type());
      }
    }
  }

  return lTable;
}


void MonitoringThread::fillTables(swatch::system::System& aSystem)
{
  LOG4CPLUS_DEBUG(getLogger(), "Monitoring thread: Filling tables");

  for (auto lProcIt = aSystem.getProcessors().begin(); lProcIt != aSystem.getProcessors().end(); lProcIt++) {
    swatch::processor::Processor& lProc = **lProcIt;

    if (lProc.getStatus().isEnabled() && lProc.getMonitoringStatus() != swatch::core::monitoring::kDisabled) {
      // Determine this processor's table name, and fill that table
      fillActionableTable(getInfospaceTableName("processor", lProc), lProc);

      swatch::processor::InputPortCollection& lRxPorts = lProc.getInputPorts();
      if (lRxPorts.getMonitoringStatus() != swatch::core::monitoring::kDisabled) {
        for (auto lPortIt = lRxPorts.getPorts().begin(); lPortIt != lRxPorts.getPorts().end(); lPortIt++) {
          if ((*lPortIt)->getMonitoringStatus() != swatch::core::monitoring::kDisabled)
            fillPortTable(getInfospaceTableName("rxPort", **lPortIt), lProc.getId(), **lPortIt);
        }
      } // for: rx ports

      swatch::processor::OutputPortCollection& lTxPorts = lProc.getOutputPorts();
      if (lTxPorts.getMonitoringStatus() != swatch::core::monitoring::kDisabled) {
        for (auto lPortIt = lTxPorts.getPorts().begin(); lPortIt != lTxPorts.getPorts().end(); lPortIt++) {
          if ((*lPortIt)->getMonitoringStatus() != swatch::core::monitoring::kDisabled)
            fillPortTable(getInfospaceTableName("txPort", **lPortIt), lProc.getId(), **lPortIt);
        }
      } // for: tx ports
    }
  } // for: processors


  for (auto lAMC13It = aSystem.getDaqTTCs().begin(); lAMC13It != aSystem.getDaqTTCs().end(); lAMC13It++) {
    swatch::dtm::DaqTTCManager& lDTM = **lAMC13It;
 
    if (lDTM.getStatus().isEnabled() && lDTM.getMonitoringStatus() != swatch::core::monitoring::kDisabled) {
      // Determine this AMC13's table name, and fill that table
      fillActionableTable(getInfospaceTableName("amc13", lDTM), lDTM);

      // Iterate over backplane ports
      if (lDTM.getAMCPorts().getMonitoringStatus() != swatch::core::monitoring::kDisabled) {
        for (auto lPortIt = lDTM.getAMCPorts().getPorts().begin(); lPortIt != lDTM.getAMCPorts().getPorts().end(); lPortIt++) {
          swatch::dtm::AMCPort& lPort = **lPortIt;

          if (lPort.getMonitoringStatus() != swatch::core::monitoring::kDisabled) 
            fillPortTable(getInfospaceTableName("amc13", lPort), lDTM.getId(), lPort);
        } // for: AMC ports
      }
    }
  } // for: AMC13s
}


void MonitoringThread::fillActionableTable(const std::string& aTableName, swatch::action::ActionableObject& aActionable)
{
  auto lTableIt = mMoniMap.find(aTableName);
  if (lTableIt == mMoniMap.end()) {
    LOG4CPLUS_WARN(getLogger(), "Could not find any infospace table of name '" + aTableName + "' for actionable '" + aActionable.getPath() + "'");
  } else {
    LOG4CPLUS_DEBUG(getLogger(), "Filling infospace table '" + aTableName + "' with info from actionable '" + aActionable.getPath() + "'");
    xdata::Table& lTable = dynamic_cast<xdata::Table&>(*lTableIt->second);
    size_t lRowIdx = lTable.getRowCount();

    // Set the id column
    xdata::String lId(aActionable.getId());
    lTable.setValueAt(lRowIdx, "id", lId);

    // Set the other columns to values of metrics
    std::vector<std::string> lColumnNamesVec = lTable.getColumns();
    boost::unordered_map<std::string, const swatch::core::AbstractMetric*> lMetricColumns;
    for (auto lIt = lColumnNamesVec.begin(); lIt != lColumnNamesVec.end(); lIt++) {
      if (*lIt != "id")
        lMetricColumns.insert(std::make_pair(*lIt, (const swatch::core::AbstractMetric*) NULL));
    }
    for(swatch::core::Object::iterator lIt = aActionable.begin(); lIt != aActionable.end(); lIt++) {
      if (const swatch::core::AbstractMetric* lMetric = dynamic_cast<const swatch::core::AbstractMetric*>(&*lIt)) {

        // TODO: Replace with metric path -> col name conversion function
        std::string lColName = lMetric->getPath().substr(aActionable.getPath().size() + 1);
        std::replace(lColName.begin(), lColName.end(), '.', '_');

        if (lMetricColumns.find(lColName) != lMetricColumns.end()) {
          if (lMetricColumns[lColName] != NULL) {
            LOG4CPLUS_ERROR(getLogger(), "Infospace column '" << lColName << "' in current row of table '" 
                    << aTableName << "', has already been filled by another metric, '" 
                    << lMetricColumns[lColName]->getPath() << "'; cannot set this cell to value of metric '" 
                    << lMetric->getPath() << "'");            
          }
          try {
            lTable.setValueAt(lRowIdx, lColName, const_cast<xdata::Serializable&>(lMetric->getSnapshot().getAsSerializable()));
            lMetricColumns[lColName] = lMetric;
          } catch (const std::exception& aException) {
            LOG4CPLUS_ERROR(getLogger(), "Exception of type '" << swatch::core::demangleName(typeid(aException).name()) 
                    << "' thrown when filling infospace table '" << aTableName << "' column '" << lColName
                    << "', from object '" << aActionable.getPath() << "'");
          }
        }
      }
    }

    for (auto lIt = lMetricColumns.begin(); lIt != lMetricColumns.end(); lIt++) {
      if (lIt->second == NULL) {
            LOG4CPLUS_WARN(getLogger(), "Infospace column '" << lIt->first << "' in table '" << aTableName 
                    << "' was not filled in row " << lRowIdx << " for actionable '" << aActionable.getPath() 
                    << "' because no corresponding metric was found.");
      }
    }

  }
}


void MonitoringThread::fillPortTable(const std::string& aTableName, const std::string& aActionableId, swatch::core::MonitorableObject& aPort)
{
  auto lTableIt = mMoniMap.find(aTableName);
  if (lTableIt == mMoniMap.end()) {
    LOG4CPLUS_WARN(getLogger(), "Could not find any infospace table of name '" + aTableName + "' for port '" + aPort.getPath() + "'");
  } else {
    
    LOG4CPLUS_DEBUG(getLogger(), "Filling infospace table '" + aTableName + "' with info from port '" + aPort.getPath() + "'");
    xdata::Table& lTable = dynamic_cast<xdata::Table&>(*lTableIt->second);
    size_t lRowIdx = lTable.getRowCount();

    // Set the fixed columns: boardId, id & isMasked column
    xdata::String lBoardId(aActionableId);
    lTable.setValueAt(lRowIdx, "boardId", lBoardId);
    xdata::String lPortId(aPort.getId());
    lTable.setValueAt(lRowIdx, "portId", lPortId);
    if (swatch::action::MaskableObject* lMaskable = dynamic_cast<swatch::action::MaskableObject*>(&aPort)) {
      xdata::Boolean lIsMasked(lMaskable->isMasked());
      lTable.setValueAt(lRowIdx, "isMasked", lIsMasked);
    }

    // Set the other columns to values of metrics
    std::vector<std::string> lColumnNamesVec = lTable.getColumns();
    boost::unordered_map<std::string, const swatch::core::AbstractMetric*> lMetricColumns;
    for (auto lIt = lColumnNamesVec.begin(); lIt != lColumnNamesVec.end(); lIt++) {
      if ((*lIt != "portId") && (*lIt != "boardId") && (*lIt != "isMasked"))
        lMetricColumns.insert(std::make_pair(*lIt, (const swatch::core::AbstractMetric*) NULL));
    }
    for(swatch::core::Object::iterator lIt = aPort.begin(); lIt != aPort.end(); lIt++) {
      if (const swatch::core::AbstractMetric* lMetric = dynamic_cast<const swatch::core::AbstractMetric*>(&*lIt)) {

        // TODO: Replace with metric path -> col name conversion function
        std::string lColName = lMetric->getPath().substr(aPort.getPath().size() + 1);
        std::replace(lColName.begin(), lColName.end(), '.', '_');

        if (lMetricColumns.find(lColName) != lMetricColumns.end()) {
          if (lMetricColumns[lColName] != NULL) {
            LOG4CPLUS_ERROR(getLogger(), "Infospace column '" << lColName << "' in current row of table '" 
                    << aTableName << "', has already been filled by another metric, '" 
                    << lMetricColumns[lColName]->getPath() << "'; cannot set this cell to value of metric '" 
                    << lMetric->getPath() << "'");            
          }
          try {
            lTable.setValueAt(lRowIdx, lColName, const_cast<xdata::Serializable&>(lMetric->getSnapshot().getAsSerializable()));
            lMetricColumns[lColName] = lMetric;
          } catch (const std::exception& aException) {
            LOG4CPLUS_ERROR(getLogger(), "Exception of type '" << swatch::core::demangleName(typeid(aException).name()) 
                    << "' thrown when filling infospace table '" << aTableName << "' column '" << lColName
                    << "', from object '" << aPort.getPath() << "'");
          }
        }
      }
    }

    for (auto lIt = lMetricColumns.begin(); lIt != lMetricColumns.end(); lIt++) {
      if (lIt->second == NULL) {
            LOG4CPLUS_WARN(getLogger(), "Infospace column '" << lIt->first << "' in table '" << aTableName 
                    << "' was not filled in row " << lRowIdx << " for port '" << aPort.getPath() 
                    << "' because no corresponding metric was found.");
      }
    }  

  }
}


std::string MonitoringThread::getInfospaceTableName(const std::string& aPrefix, const swatch::core::Object& aObject)
{
  std::string lClassName = swatch::core::demangleName(typeid(aObject).name());
  std::string lTableName = aPrefix + "_" + lClassName.substr(lClassName.rfind(':')+1);
  std::replace(lTableName.begin(), lTableName.end(), ':', '#');
  return lTableName;
}


bool MonitoringThread::checkColumnName(const std::string& aColName)
{
  boost::cmatch lMatch;
  if ( ! boost::regex_match (aColName.c_str(), lMatch, kColNameRegex))
    return false;
  else if ( aColName.size() > 30 )
    return false;
  else
    return true;
}


std::string MonitoringThread::getLowercaseCopy(const std::string& aString)
{
  std::string lInputString(aString);
  boost::algorithm::to_lower(lInputString);
  return lInputString;
}


} // ns: swatchcellframework




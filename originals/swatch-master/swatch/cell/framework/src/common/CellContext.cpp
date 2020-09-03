#include "swatchcell/framework/CellContext.h"

#include "swatch/action/ThreadPool.hpp"
#include "swatch/core/Factory.hpp"
#include "swatch/dtm/AMCPort.hpp"
#include "swatch/dtm/DaqTTCManager.hpp"
#include "swatch/processor/Port.hpp"
#include "swatch/processor/Processor.hpp"
#include "swatch/system/System.hpp"
#include "swatch/xml/XmlGateKeeper.hpp"
#include "swatch/xml/XmlSystem.hpp"

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"

#include "ts/framework/CellOperationFactory.h"
#include "ts/framework/Level1Source.h"
#include "ts/runcontrol/FEDMap.h"

// SWATCH cell Headers
#include "swatchcell/framework/CellAbstract.h"
#include "swatchcell/framework/DbManager.h"
#include "swatchcell/framework/LogMessageQueue.h"
#include "swatchcell/framework/RunControl.h"

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/range/algorithm/copy.hpp>
#include "boost/foreach.hpp"


namespace swatchcellframework {


ContextSharedGuard::ContextSharedGuard(const CellContext& aContext) : 
  mLock(aContext.mMutex)
{
}

ContextSharedGuard::~ContextSharedGuard()
{
}


ContextWriteGuard::ContextWriteGuard(const CellContext& aContext) :
  mLock(aContext.mMutex)
{
}

ContextWriteGuard::~ContextWriteGuard()
{
}



// Static Members
const size_t CellContext::kLogSize = 500;

// --------------------------------------------------------
CellContext::GuiContext::GuiContext() :
  mDefaultSystemDescriptionFilePath(getEnvVar("SWATCH_DEFAULT_INIT_FILE","")),
  mLatestSystemDescriptionFilePath(mDefaultSystemDescriptionFilePath),
  mDefaultGateKeeperXmlFilePath(getEnvVar("SWATCH_DEFAULT_GATEKEEPER_XML","")),
  mLatestGateKeeperXmlFilePath(mDefaultGateKeeperXmlFilePath),
  mDefaultGateKeeperKey(getEnvVar("SWATCH_DEFAULT_GATEKEEPER_KEY","")),
  mLatestGateKeeperKey(mDefaultGateKeeperKey)
{
}

// --------------------------------------------------------
CellContext::GuiContext::~GuiContext()
{
}


// --------------------------------------------------------
const std::string& CellContext::GuiContext::getDefaultSystemDescriptionFilePath() const
{
  return mDefaultSystemDescriptionFilePath;
}


// --------------------------------------------------------
const std::string& CellContext::GuiContext::getDefaultGateKeeperXmlFilePath() const
{
  return mDefaultGateKeeperXmlFilePath;
}


// --------------------------------------------------------
const std::string& CellContext::GuiContext::getDefaultGateKeeperKey() const
{
  return mDefaultGateKeeperKey;
}


// --------------------------------------------------------
std::string CellContext::GuiContext::getLatestSystemDescriptionFilePath() const
{
  boost::lock_guard<boost::mutex> lGuard(mMutex);
  return mLatestSystemDescriptionFilePath;
}


// --------------------------------------------------------
std::string CellContext::GuiContext::getLatestGateKeeperXmlFilePath() const
{
  boost::lock_guard<boost::mutex> lGuard(mMutex);
  return mLatestGateKeeperXmlFilePath;
}


// --------------------------------------------------------
std::string CellContext::GuiContext::getLatestGateKeeperKey() const
{
  boost::lock_guard<boost::mutex> lGuard(mMutex);
  return mLatestGateKeeperKey;
}


// --------------------------------------------------------
void CellContext::GuiContext::setSystemDescriptionFilePath(const std::string& aPath)
{
  boost::lock_guard<boost::mutex> lGuard(mMutex);
  mLatestSystemDescriptionFilePath = aPath;
}

// --------------------------------------------------------
void CellContext::GuiContext::setGateKeeperXmlFilePath(const std::string& aPath)
{
  boost::lock_guard<boost::mutex> lGuard(mMutex);
  mLatestGateKeeperXmlFilePath = aPath;
}


// --------------------------------------------------------
void CellContext::GuiContext::setGateKeeperKey(const std::string& aKey)
{
  boost::lock_guard<boost::mutex> lGuard(mMutex);
  mLatestGateKeeperKey = aKey;
}



// --------------------------------------------------------
std::string CellContext::GuiContext::getEnvVar(const std::string& aVarName, const std::string& aDefaultValue)
{
  const char* lValue = getenv(aVarName.c_str());
  if (lValue)
    return lValue;
  else
    return aDefaultValue;
}



// --------------------------------------------------------
const std::string CellContext::kRunControlOperationName = "Run Control";


// --------------------------------------------------------
CellContext::CellContext(log4cplus::Logger& log, tsframework::CellAbstract* cell) :
  tsframework::CellAbstractContext(log, cell),
  mSys(new swatch::system::System( swatch::system::SystemStub("empty", "swatch::system::System") ), SystemDeleter() ),
  mSystemCreationTime(TimePoint_t::clock::now()),
  mSystemSource("Empty system"),
  mDbCustomClobCallback("", DbManager::ClobCallback_t(NULL)),
  mDefaultLogAppenderIDs{log.getName(), "swatch"},
  mMonitoringThread(log.getName(), *this, 20)
{
  // Initialize here or later?
  initializeLogAppenders();

  // Set Level1Source's subsystem field
  const char* lSubsystemId = getenv("SUBSYSTEM_ID");
  if (lSubsystemId != NULL)
    getLevel1Source().setSubsystem(lSubsystemId);
  else
    LOG4CPLUS_ERROR(getLogger(), "Cannot setup Level1Source correctly, since 'SUBSYSTEM_ID' environment variable is not defined.");
  getLevel1Source().setPeriodicAction(this, &swatchcellframework::CellContext::l1SourceAction);
}

// --------------------------------------------------------
CellContext::~CellContext()
{
}

// --------------------------------------------------------
void CellContext::initializeLogAppenders() {
  BOOST_FOREACH( const std::string& id, mDefaultLogAppenderIDs ) {
    createLogAppender(id);
  }
}


std::string CellContext::getSubsystemId() const
{
  const char* lSubsysId = getenv("SUBSYSTEM_ID");
  if (lSubsysId == NULL)
    XCEPT_RAISE(swatch::core::RuntimeError,"Could not determine subsystem ID, since 'SUBSYSTEM_ID' env var is not set!");
  else
    return std::string(lSubsysId);
}


// --------------------------------------------------------
swatch::system::System& CellContext::getSystem(const SharedGuard_t& aGuard)
{
  return *mSys;
}

// --------------------------------------------------------
swatch::system::System& CellContext::getSystem(const WriteGuard_t& aGuard)
{
  return *mSys;
}


// --------------------------------------------------------
const CellContext::TimePoint_t& CellContext::getSystemCreationTime(const SharedGuard_t& aGuard) const
{
  return mSystemCreationTime;
}


// --------------------------------------------------------
const std::string& CellContext::getSystemSource(const SharedGuard_t& aGuard) const
{
  return mSystemSource;
}


// --------------------------------------------------------
swatch::system::System& CellContext::replaceSystemFromFile(const std::string& aFilePath, const WriteGuard_t& aGuard)
{
  // TODO: Throw if any operation is engaged (i.e. not in "ground" state / in transitions from "ground" state)

  swatch::system::SystemStub lSysStub = swatch::xml::system::xmlFileToSystemStub(aFilePath);

  // Create the new system only then
  swatch::system::System& lNewSys = replaceSystem(lSysStub, aGuard);
  mSystemSource = "File '" + aFilePath + "'";
  return lNewSys;
}

// --------------------------------------------------------
swatch::system::System& CellContext::replaceSystemFromDb(const std::string& aSubsystemId, const std::string& aConfigKey, const WriteGuard_t& aGuard)
{
  // 1) Extract system stub from DB (parsing CLOB as XML)
  DbManager lDbMgr(*this, aSubsystemId);
  swatch::system::SystemStub lSysStub = lDbMgr.getHwStub(aConfigKey);

  // 2) Create the system from the stub
  swatch::system::System& lNewSys = replaceSystem(lSysStub, aGuard);
  mSystemSource = "DB, config key '" + aConfigKey + "'";
  return lNewSys;
}


// --------------------------------------------------------
const swatch::action::GateKeeper* CellContext::getGateKeeper(const SharedGuard_t& aGuard) const
{
  return mGateKeeper.get();
}


// --------------------------------------------------------
const swatch::action::GateKeeper* CellContext::getGateKeeper(const WriteGuard_t& aGuard) const
{
  return mGateKeeper.get();
}


// --------------------------------------------------------
const CellContext::TimePoint_t& CellContext::getGateKeeperCreationTime(const SharedGuard_t& aGuard) const 
{
  return mGateKeeperCreationTime;
}


// --------------------------------------------------------
const std::string& CellContext::getGateKeeperSource(const SharedGuard_t& aGuard) const
{
  return mGateKeeperSource;
}


// --------------------------------------------------------
const swatch::action::GateKeeper& CellContext::setGateKeeperFromFile(const std::string& aFilePath, const std::string& aKey, const WriteGuard_t& aGuard)
{
  mGateKeeper.reset( new swatch::xml::XmlGateKeeper(aFilePath, aKey) );
  mGateKeeperCreationTime = TimePoint_t::clock::now();
  mGateKeeperSource = "File '" + aFilePath + "', key '" + aKey + "'";

  // If GK construction was successful (i.e. no exception thrown), then
  getGuiContext().setGateKeeperXmlFilePath(aFilePath);
  getGuiContext().setGateKeeperKey(aKey);

  return *mGateKeeper;
}


// --------------------------------------------------------
const swatch::action::GateKeeper& CellContext::setGateKeeperFromDb(const std::string& aSubsystemId, const std::string& aConfigKey, const std::string& aRunSettingsKey, const WriteGuard_t& aGuard)
{
  //DbGateKeeperBuilder gkb(*this, logger_, aSubsystemId);
  //mGateKeeper.reset( new swatch::xml::XmlGateKeeper(gkb.buildFrom(aConfigKey, aRunSettingsKey)) );
  DbManager lDbMgr(*this, aSubsystemId);
  mGateKeeper.reset(new swatch::xml::XmlGateKeeper(lDbMgr.buildGateKeeper(aConfigKey, aRunSettingsKey, mDbCustomClobCallback.first, mDbCustomClobCallback.second)));
  mGateKeeperCreationTime = TimePoint_t::clock::now();
  mGateKeeperSource = "DB, config key '" + aConfigKey + "', RS key '" + aRunSettingsKey + "'";

  return *mGateKeeper;
}

// --------------------------------------------------------
void CellContext::registerCustomConfigDbClob(const std::string& aColName, const DbManager::ClobCallback_t& aCallback)
{
  boost::lock_guard<boost::shared_mutex> lLock(mMutex);
  mDbCustomClobCallback = std::make_pair(aColName, aCallback);
}

// --------------------------------------------------------
CellContext::GuiContext& CellContext::getGuiContext()
{
  return mGuiContext;
}

// --------------------------------------------------------
void CellContext::createLogAppender(const std::string& aId)
{
  boost::lock_guard<boost::mutex> lLock(mLogMutex);

  if ( mLogAppenders.find(aId) != mLogAppenders.end() ) {
    // TODO: Custom custom exception
    XCEPT_RAISE(swatch::core::RuntimeError,"Log collector '"+aId+"' already exists");
  }

  // Create a container reference
  log4cplus::SharedAppenderPtr lLogPtr= log4cplus::SharedAppenderPtr( new LogMessageQueue(aId, 500) );

  // TODO: Not sure it can work
  log4cplus::Logger lLogger = log4cplus::Logger::getInstance(aId);
  lLogger.addAppender(lLogPtr);

  mLogAppenders[aId] = lLogPtr;

  LOG4CPLUS_INFO(lLogger, aId + " TS appender... appended");

}


// --------------------------------------------------------
void CellContext::retireLogAppender(const std::string& aId)
{
  boost::lock_guard<boost::mutex> lLock(mLogMutex);

  // if collector doesn't exist, nothing happens.
  // Should the collector throw?

  LogAppenderMap_t::iterator it;
  if ( (it = mLogAppenders.find(aId) ) != mLogAppenders.end() ) {
    log4cplus::Logger lLogger = log4cplus::Logger::getInstance(aId);
    lLogger.removeAppender(it->second);

    mLogAppenders.erase(it->first);
  }

}

// --------------------------------------------------------
log4cplus::SharedAppenderPtr CellContext::getLogAppender(const std::string& aId) const
{
  boost::lock_guard<boost::mutex> lLock(mLogMutex);

  // TODO: Rethrow custom exception
  return mLogAppenders.at( aId );
}


// --------------------------------------------------------
std::set<std::string> CellContext::getLogAppenderIDs() const
{
  boost::lock_guard<boost::mutex> lLock(mLogMutex);
  std::set<std::string> ids;
  std::transform(mLogAppenders.begin(), mLogAppenders.end(), std::inserter(ids, ids.end()), boost::bind(&LogAppenderMap_t::value_type::first, _1));
  return ids;
}


// --------------------------------------------------------
const std::vector<std::string>& CellContext::getDefaultLogAppenderIDs() const {
  return mDefaultLogAppenderIDs;
}


// --------------------------------------------------------
void CellContext::clearLogAppenders() {

  boost::lock_guard<boost::mutex> lLock(mLogMutex);
  // Get the full list of ids
  std::set<std::string> lAppendersToDelete;
  std::transform(mLogAppenders.begin(), mLogAppenders.end(), std::inserter(lAppendersToDelete, lAppendersToDelete.end()), boost::bind(&LogAppenderMap_t::value_type::first, _1));

  LogAppenderMap_t newQueues;

  // Copy default log queues over
  BOOST_FOREACH( const std::string& id, mDefaultLogAppenderIDs ) {
    newQueues[id] = mLogAppenders.at(id);
    // Don't delete me
    lAppendersToDelete.erase(id);
  }

  BOOST_FOREACH( const std::string& id, lAppendersToDelete ) {
    log4cplus::Logger lLogger = log4cplus::Logger::getInstance(id);
    lLogger.removeAppender(mLogAppenders[id]);
  }

  mLogAppenders.swap(newQueues);

}


// --------------------------------------------------------
RunControl& CellContext::getRunControlOperation()
{
  RunControl* lRunControlOp = dynamic_cast<RunControl*>(&getOperationFactory()->getOperation(kRunControlOperationName));
  if ( lRunControlOp != NULL )
    return *lRunControlOp;
  else {
    XCEPT_RAISE(tsexception::CellException, "Run control operation ('" + kRunControlOperationName + "', type '" + swatch::core::demangleName(typeid(lRunControlOp).name()) + "') cannot be cast to swatchcellframework::RunControl");
  }
}


// --------------------------------------------------------
MonitoringThread& CellContext::getMonitoringThread()
{
  return mMonitoringThread;
}

// --------------------------------------------------------
std::map<uint32_t, uint32_t> CellContext::parseFEDEnableMap(const std::string& aString)
{
  tsruncontrol::FEDMap lMapParser;
  lMapParser.init(aString);
  std::map<uint32_t, uint32_t> lFedMap;
  BOOST_FOREACH(uint32_t lFed, lMapParser.getFeds() ) {
    lFedMap[lFed] = lMapParser.activeSLink(lFed) || lMapParser.activeTTS(lFed);
  }

  return lFedMap;
}


void CellContext::SystemDeleter::operator()(swatch::system::System* aSys)
{
  mDeleter(aSys);
}


swatch::system::System& CellContext::replaceSystem(const swatch::system::SystemStub& aStub, const WriteGuard_t& aGuard)
{
  // 1) Delete pre-existing system instance
  mSys.reset((swatch::system::System*) NULL);

  // 2) Construct new system instance
  try {
    swatch::system::System* lNewSys = swatch::core::Factory::get()->make<swatch::system::System>(aStub.creator, aStub);
    mSys.reset(lNewSys, SystemDeleter());
    mSystemCreationTime = TimePoint_t::clock::now();
  }
  catch(const std::exception& aExc) {
    mSys.reset(new swatch::system::System( swatch::system::SystemStub("empty", "swatch::system::System") ), SystemDeleter());
    mSystemCreationTime = TimePoint_t::clock::now();
    throw;
  }

  // 3) Check that relative ID paths of metrics to closest port / processor / DAQ-TTC manager ancestor are no longer than 30 chars
  boost::unordered_set<std::string> lOverlyLongMetricIdPaths;
  const swatch::core::Object* lAncestorActionable = NULL;
  const swatch::core::Object* lAncestorPort = NULL;

  for (const auto& lObj : *mSys) {
    if (auto lActObj = dynamic_cast<const swatch::action::ActionableObject* const>(&lObj))
      lAncestorActionable = lActObj;
    else if (not lAncestorActionable->isAncestorOf(lObj))
      lAncestorActionable = NULL;

    if (auto lPort = dynamic_cast<const swatch::processor::InputPort* const>(&lObj))
      lAncestorPort = lPort;
    else if (auto lPort = dynamic_cast<const swatch::processor::OutputPort* const>(&lObj))
      lAncestorPort = lPort;
    else if (auto lPort = dynamic_cast<const swatch::dtm::AMCPort* const>(&lObj))
      lAncestorPort = lPort;
    else if (not lAncestorPort->isAncestorOf(lObj))
      lAncestorPort = NULL;

    if (auto lMetric = dynamic_cast<const swatch::core::AbstractMetric* const>(&lObj) ) {
      const std::string lMetricIdPath(lMetric->getPath());
      std::string lAncestorIdPath;
      if (lAncestorPort != NULL)
        lAncestorIdPath = lAncestorPort->getPath();
      else if (lAncestorActionable != NULL)
        lAncestorIdPath = lAncestorActionable->getPath();
      const size_t lRelIdPathLength = lMetricIdPath.size() - lAncestorIdPath.size() - 1;

      if (lRelIdPathLength > 30)
        lOverlyLongMetricIdPaths.insert(lMetricIdPath.substr(lAncestorIdPath.size() + 1));
    }
  }

  if ( not lOverlyLongMetricIdPaths.empty() ) {
    std::ostringstream lMsgStream;
    lMsgStream << "The ID paths for the following " << lOverlyLongMetricIdPaths.size();
    lMsgStream << " metrics, relative to ancestor port / processor / DTM, are longer than 30 characters: ";
    for (const auto& lRelIdPath : lOverlyLongMetricIdPaths)
      lMsgStream << '"' << lRelIdPath << "\", ";
    const std::string lErrorMsg(lMsgStream.str());

    mSys.reset(new swatch::system::System( swatch::system::SystemStub("empty", "swatch::system::System") ), SystemDeleter());


    XCEPT_RAISE(MetricRelativeIdPathsTooLong, lErrorMsg.substr(0, lErrorMsg.size() - 2));
  }


  // 3) Reset threadpool, and resize it to the number of Actionable objects in the system
  const size_t lHwConcurrency = boost::thread::hardware_concurrency();
  const size_t lNrActionables = mSys->getProcessors().size() + mSys->getDaqTTCs().size();
  const size_t lDesiredNumThreads = ((lHwConcurrency == 0) ? lNrActionables : std::min(lHwConcurrency, lNrActionables));
  const size_t lThreadPoolSize = swatch::action::ThreadPool::getInstance().size();

  if (lDesiredNumThreads == lThreadPoolSize) {
    LOG4CPLUS_INFO(getLogger(), "Leaving thread pool unchanged since already contains " << lThreadPoolSize << " threads (" << lNrActionables << " actionables, HW concurrency = " << lHwConcurrency << ")");
  }
  else {
    LOG4CPLUS_INFO(getLogger(), "Resetting thread pool to contain " << lDesiredNumThreads << " threads (current size " << lThreadPoolSize << ", " << lNrActionables << " actionables, HW concurrency = " << lHwConcurrency << ")");

    swatch::action::ThreadPool::reset();
    swatch::action::ThreadPool::getInstance(lDesiredNumThreads);
  }

  // Update log appenders
  clearLogAppenders();
  createLogAppender(mSys->getId());

  return *mSys;
}


class SystemAnalyzer : public boost::noncopyable {
public:
  SystemAnalyzer(swatch::system::System& aSystem, log4cplus::Logger& aLogger) :
    mSystem(aSystem),
    mLogger(aLogger)
  {}

  virtual ~SystemAnalyzer() {}
  
  tsframework::Level1Source::SeverityLevel severity() {
    
    tsframework::Level1Source::SeverityLevel lSeverityLevel = tsframework::Level1Source::INFO;
    
    swatch::core::StatusFlag lFlag = mSystem.getStatusFlag();
    switch(lFlag) {
      case swatch::core::kUnknown:
      case swatch::core::kWarning:
        lSeverityLevel = tsframework::Level1Source::WARNING;
        break;
      case swatch::core::kError:
        lSeverityLevel = tsframework::Level1Source::ERROR;
        break;
      default:
        lSeverityLevel = tsframework::Level1Source::INFO;
    }
    return lSeverityLevel;
  }
  
  std::string report() {
  
    swatch::core::StatusFlag lFlag = mSystem.getStatusFlag();
    if ( lFlag == swatch::core::kGood)
      return boost::lexical_cast<std::string>(lFlag);
    
    std::set<std::string> lNaughtyChildren;
    BOOST_FOREACH( swatch::processor::Processor* lProcessor, mSystem.getProcessors() ) {
      if (lProcessor->getMonitoringStatus() == swatch::core::monitoring::kEnabled) {
        boost::copy (findChildMonObjsWithStatus(*lProcessor, lFlag, mLogger), std::inserter(lNaughtyChildren, lNaughtyChildren.end()));
        boost::copy (findChildMetricsWithStatus(*lProcessor, lFlag), std::inserter(lNaughtyChildren, lNaughtyChildren.end()));
      }
    }
    
    BOOST_FOREACH( swatch::dtm::DaqTTCManager* lDTM, mSystem.getDaqTTCs() ) { 
      if (lDTM->getMonitoringStatus() == swatch::core::monitoring::kEnabled) {
        boost::copy (findChildMonObjsWithStatus(*lDTM, lFlag, mLogger), std::inserter(lNaughtyChildren, lNaughtyChildren.end()));
        boost::copy (findChildMetricsWithStatus(*lDTM, lFlag), std::inserter(lNaughtyChildren, lNaughtyChildren.end()));
      }
    }
    
    boost::copy (findChildMetricsWithStatus(mSystem, lFlag), std::inserter(lNaughtyChildren, lNaughtyChildren.end()));

    if ( lNaughtyChildren.empty() )
      return boost::lexical_cast<std::string>(lFlag);
    
    std::ostringstream lReport;
    lReport << lFlag << " components : ";

    std::set<std::string>::const_iterator lLast = std::prev(lNaughtyChildren.end());
    std::copy(lNaughtyChildren.begin(), lLast, std::ostream_iterator<std::string>(lReport, ", "));
    lReport << *lLast;

//    std::cout << lMsg.str() << std::endl;
    return lReport.str();
  }
  
private:
  
  static std::set<std::string> findChildMonObjsWithStatus( const swatch::core::MonitorableObject& aMonitorable, swatch::core::StatusFlag aStatusFlag, log4cplus::Logger& aLogger ) {
    
    std::set<std::string> lNaughty;
    
    if ( aMonitorable.getMonitoringStatus() != swatch::core::monitoring::kEnabled &&
      aMonitorable.getStatusFlag() == swatch::core::kGood ) return lNaughty;
      
    // Loop over submonitorables
    BOOST_FOREACH( const std::string lId, aMonitorable.getChildren()) {

      const swatch::core::MonitorableObject* lMonObj = 0x0;
      if( ( lMonObj = aMonitorable.getObjPtr<swatch::core::MonitorableObject>(lId) ) == 0x0 )
        continue;
      // only enabled children contribute to the status, but not in good state
      if (lMonObj->getMonitoringStatus() != swatch::core::monitoring::kEnabled || lMonObj->getStatusFlag() != aStatusFlag )
        continue;
//      std::cout << "Analyzer: " << lMonObj->getPath() << " " << lMonObj->getStatusFlag() << std::endl;

      const swatch::core::LeafObject& lNaughtyObject = findCommonCauseOfMonitoringStatus(*lMonObj, aLogger);
      lNaughty.insert( lNaughtyObject.getPath().substr(aMonitorable.getPath().size()+1) );
    }
    
    return lNaughty;
  }

  static const swatch::core::LeafObject& findCommonCauseOfMonitoringStatus(const swatch::core::MonitorableObject& aMonitorable, log4cplus::Logger& aLogger)
  {
    typedef swatch::core::MonitorableObject MonObj_t;
    typedef swatch::core::AbstractMetric Metric_t;
    const swatch::core::StatusFlag lStatus = aMonitorable.getStatusFlag();

    const MonObj_t* lMonObj = &aMonitorable;

    while (true) {
      const MonObj_t* lCausalMonObjChild = NULL;
      const Metric_t* lCausalMetricChild = NULL;

      for (const auto& lId : lMonObj->getChildren() ) {

        if (const MonObj_t* lMonChild = lMonObj->getObjPtr<MonObj_t>(lId)) {
          if ( (lMonChild->getMonitoringStatus() == swatch::core::monitoring::kEnabled) and (lMonChild->getStatusFlag() == lStatus) ) {

            if (lCausalMonObjChild != NULL)
              return *lMonObj;
            else
              lCausalMonObjChild = lMonChild;
          }
        } // if MonObj_t

        if (const Metric_t* lMetric = lMonObj->getObjPtr<Metric_t>(lId)) {
          if ( lMetric->getStatus() == std::make_pair(lStatus, swatch::core::monitoring::kEnabled) ) {
            if (lCausalMetricChild != NULL)
              return *lMonObj;
            else
              lCausalMetricChild = lMetric;
          }
        } // if Metric_t
      } // for child IDs

      if ((lCausalMonObjChild != NULL) and (lCausalMetricChild != NULL))
        return *lMonObj;
      else if (lCausalMonObjChild != NULL)
        lMonObj = lCausalMonObjChild;
      else if (lCausalMetricChild != NULL)
        return *lCausalMetricChild;
      else {
        // If this function has been written correctly, should not enter this clause; but, just in case, print error message, and return function argument
        LOG4CPLUS_ERROR(aLogger, "UNEXPECTED LOGICAL ERROR. swatchcellframework::SystemAnalyzer::findCommonCauseOfMonitoringStatus "
          << "could not identify any children causing status '" << lStatus << "' for monitorable object '" << lMonObj->getPath()
          << "' (function argument is '" << aMonitorable.getPath() << "')");
        return *lMonObj;
      }

    }
  }


  static std::set<std::string> findChildMetricsWithStatus( const swatch::core::MonitorableObject& aMonitorable, swatch::core::StatusFlag aStatusFlag ) {
    
    std::set<std::string> lNaughty;
    
    if (aMonitorable.getMonitoringStatus() != swatch::core::monitoring::kEnabled &&
        aMonitorable.getStatusFlag() == swatch::core::kGood )
      return lNaughty;
      
    // Loop over submonitorables
    BOOST_FOREACH( const std::string lId, aMonitorable.getChildren()) {

      const swatch::core::AbstractMetric* lMetric = 0x0;
      if( ( lMetric = aMonitorable.getObjPtr<swatch::core::AbstractMetric>(lId) ) == 0x0 )
        continue;
      // only enabled children contribute to the status, but not in good state
      std::pair<swatch::core::StatusFlag, swatch::core::monitoring::Status> lStatus = lMetric->getStatus();
      if (lStatus.second != swatch::core::monitoring::kEnabled || lStatus.first != aStatusFlag )
        continue;
//      std::cout << "Analyzer: " << lMonObj->getPath() << " " << lMonObj->getStatusFlag() << std::endl;

      lNaughty.insert(lId);
    }
    
    return lNaughty;
  }


  swatch::system::System& mSystem;
  log4cplus::Logger& mLogger;
};




void CellContext::l1SourceAction()
{
  LOG4CPLUS_INFO(getLogger(), "Executing swatchcellframework::CellContext::l1SourceAction");

//  if (getRunControlOperation().getFSM().getStateName(getRunControlOperation().getFSM().getCurrentState()) == RunControlBase::kTrConfigure) {

  const std::string lSystemStatusAlarmName = "SWATCH_system_monitoring_state";
  CellContext::SharedGuard_t lContextGuard(*this);
  swatch::core::StatusFlag lMonStatus = getSystem(lContextGuard).getStatusFlag();

//  tsframework::Level1Source::SeverityLevel lSeverityLevel = tsframework::Level1Source::INFO;
//  if ((lMonStatus == swatch::core::kUnknown) || (lMonStatus == swatch::core::kWarning))
//    lSeverityLevel = tsframework::Level1Source::WARNING;
//  else if (lMonStatus == swatch::core::kError)
//    lSeverityLevel = tsframework::Level1Source::ERROR;
//  getLevel1Source().setAlarm(lSystemStatusAlarmName, lSeverityLevel, "SWATCH system monitoring status is " + boost::lexical_cast<std::string>(lMonStatus));
  
  SystemAnalyzer lAnalyzer(getSystem(lContextGuard), getLogger());
  getLevel1Source().setAlarm(lSystemStatusAlarmName, lAnalyzer.severity(), "SWATCH monitoring "+lAnalyzer.report());
  
}


} // end ns swatchframework

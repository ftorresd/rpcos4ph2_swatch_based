
#ifndef __SWATCHCELL_FRAMEWORK_CELLCONTEXT_H__
#define __SWATCHCELL_FRAMEWORK_CELLCONTEXT_H__


#include <string>

// boost headers
#include <boost/shared_ptr.hpp>

// TS headers
#include "ts/framework/CellAbstractContext.h"

// SWATCH headers
#include "swatch/action/ActionableSystem.hpp"

// SWATCH cell headers
#include "swatchcell/framework/DbManager.h"
#include "swatchcell/framework/MonitoringThread.h"
//#include "swatchcell/framework/LogMessageQueue.hpp"


namespace swatch {
namespace action {
class GateKeeper;
}

namespace system {
class System;
class SystemStub;
}
}

namespace log4cplus {
class Logger;
}


namespace swatchcellframework {

class LogMessageQueue;
class RunControl;

SWATCH_DEFINE_EXCEPTION(MetricRelativeIdPathsTooLong)


class ContextSharedGuard : public boost::noncopyable {
public:
  ContextSharedGuard(const CellContext& aContext);
  ~ContextSharedGuard();

private:
  boost::shared_lock<boost::shared_mutex> mLock;
};

class ContextWriteGuard : public boost::noncopyable {
public:
  ContextWriteGuard(const CellContext& aContext);
  ~ContextWriteGuard();
  
private:
  boost::unique_lock<boost::shared_mutex> mLock;
};


//! A TS CellContext class containing a SWATCH system
class CellContext : public tsframework::CellAbstractContext {
public:

  static const std::string kRunControlOperationName;
  
  typedef ContextSharedGuard SharedGuard_t;
  typedef ContextWriteGuard WriteGuard_t;
  
  class GuiContext {
  public:
    GuiContext();
    virtual ~GuiContext();

    const std::string& getDefaultSystemDescriptionFilePath() const;

    const std::string& getDefaultGateKeeperXmlFilePath() const;

    const std::string& getDefaultGateKeeperKey() const;

    std::string getLatestSystemDescriptionFilePath() const;

    std::string getLatestGateKeeperXmlFilePath() const;

    std::string getLatestGateKeeperKey() const;

    void setSystemDescriptionFilePath(const std::string& aPath);

    void setGateKeeperXmlFilePath(const std::string& aPath);

    void setGateKeeperKey(const std::string& aPath);

  private:

    //! Returns value of an environment variable if it exists, or aDefaultValue otherwise
    static std::string getEnvVar(const std::string& aVarName, const std::string& aDefaultValue);

    // Default initial JSON file path (taken from env var)
    std::string mDefaultSystemDescriptionFilePath;

    //! Latest JSON file path
    std::string mLatestSystemDescriptionFilePath;

    // Default initial XML file path (taken from env var)
    std::string mDefaultGateKeeperXmlFilePath;

    // Latest XML file path (taken from env var)
    std::string mLatestGateKeeperXmlFilePath;

    //! Default initial XML key (taken from env var)
    std::string mDefaultGateKeeperKey;

    //! Latest XML key (taken from env var)
    std::string mLatestGateKeeperKey;

    mutable boost::mutex mMutex;
  };

  CellContext(log4cplus::Logger& log, tsframework::CellAbstract* cell);

  virtual ~CellContext();

  std::string getSubsystemId() const;

  typedef boost::chrono::system_clock::time_point TimePoint_t;

  swatch::system::System& getSystem(const SharedGuard_t& aGuard);
  swatch::system::System& getSystem(const WriteGuard_t& aGuard);
  const TimePoint_t& getSystemCreationTime(const SharedGuard_t& aGuard) const;
  const std::string& getSystemSource(const SharedGuard_t& aGuard) const;
  
  swatch::system::System& replaceSystemFromFile(const std::string& aFilePath, const WriteGuard_t& aGuard);
  swatch::system::System& replaceSystemFromDb(const std::string& aSubsystemId, const std::string& aConfigKey, const WriteGuard_t& aGuard);
  
  /**
   * GateKeeper getter
   * @return Copy of Gatekeeper shared pointer
   */
  const swatch::action::GateKeeper* getGateKeeper(const SharedGuard_t& aGuard) const;
  const swatch::action::GateKeeper* getGateKeeper(const WriteGuard_t& aGuard) const;
  const TimePoint_t& getGateKeeperCreationTime(const SharedGuard_t& aGuard) const;
  const std::string& getGateKeeperSource(const SharedGuard_t& aGuard) const;

  /**
   * Guicontext getter
   * @return reference to GuiContext object
   */
  GuiContext& getGuiContext();

  const swatch::action::GateKeeper& setGateKeeperFromFile(const std::string& aFilePath, const std::string& aKey, const WriteGuard_t& aGuard);

  const swatch::action::GateKeeper& setGateKeeperFromDb(const std::string& aSubsystemId, const std::string& aConfigKey, const std::string& aRunSettingsKey, const WriteGuard_t& aGuard);

  void registerCustomConfigDbClob(const std::string& aColName, const DbManager::ClobCallback_t& aCallback);

  void createLogAppender( const std::string& aId );

  void retireLogAppender( const std::string& aId );

  log4cplus::SharedAppenderPtr getLogAppender( const std::string& aId ) const;

  std::set<std::string> getLogAppenderIDs() const;

  const std::vector<std::string>& getDefaultLogAppenderIDs() const;

  void clearLogAppenders();

  RunControl& getRunControlOperation();

  MonitoringThread& getMonitoringThread();

  // TO BE REMOVED: Helper function to parse FED Enable Map
  static std::map<uint32_t, uint32_t> parseFEDEnableMap( const std::string& aString );

private:
  // Convenience typedef
  typedef std::map<std::string, log4cplus::SharedAppenderPtr > LogAppenderMap_t;

  class SystemDeleter {
  public:
    SystemDeleter() {}
    ~SystemDeleter() {}

    void operator()(swatch::system::System* aSys);

  private:
    swatch::action::ActionableSystem::Deleter mDeleter;
  };

  void initializeLogAppenders();

  /*! 
   * @brief Constructs system from supplied stub, setting this as the context's new system; then updates size of swatch::action::ThreadPool, and updates the log appenders
   * 
   * @return The new system, constructed from the stub
   */
  swatch::system::System& replaceSystem(const swatch::system::SystemStub& aStub, const WriteGuard_t& aGuard);
  
  void l1SourceAction();


  boost::shared_ptr<swatch::system::System> mSys; // Store system in shared_ptr so that can specify deleter
  TimePoint_t mSystemCreationTime;
  std::string mSystemSource;

  boost::shared_ptr<const swatch::action::GateKeeper> mGateKeeper;
  TimePoint_t mGateKeeperCreationTime;
  std::string mGateKeeperSource;

  std::pair<std::string, DbManager::ClobCallback_t> mDbCustomClobCallback;

  mutable boost::shared_mutex mMutex;

  GuiContext mGuiContext;

  static const size_t kLogSize;

  mutable boost::mutex mLogMutex;

  LogAppenderMap_t mLogAppenders;

  //! Default log appenders. They are not cleared by clear
  std::vector<std::string> mDefaultLogAppenderIDs;

  MonitoringThread mMonitoringThread;

  friend class ContextSharedGuard;
  friend class ContextWriteGuard;
};



} // namespace swatchcellframework


#endif  /* __SWATCHCELL_FRAMEWORK_CELLCONTEXT_H__ */


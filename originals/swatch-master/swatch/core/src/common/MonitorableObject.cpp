
#include "swatch/core/MonitorableObject.hpp"


#include <stddef.h>                     // for NULL
#include <sys/time.h>                   // for gettimeofday, timeval, etc
#include <exception>                    // for exception
#include <stdexcept>                    // for runtime_error, out_of_range
#include <typeinfo>                     // for type_info

// boost headers
#include "boost/foreach.hpp"

// log4cplus headers
#include <log4cplus/loggingmacros.h>
#include "log4cplus/logger.h"           // for Logger

// SWATCH headers
#include "swatch/core/AbstractMonitorableStatus.hpp"
#include "swatch/core/LeafObject.hpp"
#include "swatch/core/utilities.hpp"
#include "swatch/logger/Logger.hpp"


using namespace std;

namespace swatch {
namespace core {


// TODO (when migrated to full C++11) : Call other MonitorableObject constructor to minimise duplication
MonitorableObject::MonitorableObject( const std::string& aId ) :
  Object( aId ),
  mSimpleMetrics(),
  mMetrics(),
  mUpdateErrorMsg(""),
  mMonitoringStatus(monitoring::kEnabled),
  mStatus(NULL),
  mLogger(swatch::logger::Logger::getInstance("swatch.core.MonitorableObject"))
{
}


MonitorableObject::MonitorableObject( const std::string& aId, const std::string& aAlias ) :
  Object( aId, aAlias ),
  mSimpleMetrics(),
  mMetrics(),
  mUpdateErrorMsg(""),
  mMonitoringStatus(monitoring::kEnabled),
  mStatus(NULL),
  mLogger(swatch::logger::Logger::getInstance("swatch.core.MonitorableObject"))
{
}


MonitorableObject::~MonitorableObject()
{
  mSimpleMetrics.clear();
  mMetrics.clear();
}


std::vector<std::string> MonitorableObject::getMetrics() const
{
  std::vector<std::string> lNames;
  BOOST_FOREACH( MetricMap_t::value_type p, mMetrics) {
    lNames.push_back( p.first );
  }
  return lNames;
}


const AbstractMetric& MonitorableObject::getMetric( const std::string& aId ) const
{
  try {
    return *mMetrics.at( aId );
  }
  catch ( const std::out_of_range& e ) {
    XCEPT_RAISE(MetricNotFoundInMonitorableObject,"MonitorableObject \"" + getPath() + "\" does not contain metric of ID \"" + aId + "\"");
  }
}



AbstractMetric& MonitorableObject::getMetric( const std::string& aId )
{
  try {
    return *mMetrics.at( aId );
  }
  catch ( const std::out_of_range& e ) {
    XCEPT_RAISE(MetricNotFoundInMonitorableObject,"MonitorableObject \"" + getPath() + "\" does not contain metric of ID \"" + aId + "\"");
  }
}


StatusFlag MonitorableObject::getStatusFlag() const
{
  StatusFlag result = kNoLimit;

  // If this object is disabled, then return kNoLimit as status
  if (mMonitoringStatus == swatch::core::monitoring::kDisabled)
    return kNoLimit;

  for (auto lIt = mMonObjChildren.begin(); lIt != mMonObjChildren.end(); lIt++) {
    const MonitorableObject& lMonChild = *(lIt->second);
    // only enabled children contribute to the status
    if (lMonChild.getMonitoringStatus() == monitoring::kEnabled)
      result = result & lMonChild.getStatusFlag();
  }

  BOOST_FOREACH( MetricMap_t::value_type p, mMetrics) {
    std::pair<StatusFlag, monitoring::Status> lMetricStatus = p.second->getStatus();
    // only enabled metrics contribute to the status
    if (lMetricStatus.second == monitoring::kEnabled)
      result = result & lMetricStatus.first;
  }

  return result;
}


MonitorableObjectSnapshot MonitorableObject::getStatus() const
{
  return MonitorableObjectSnapshot(getPath(), getStatusFlag(), mMonitoringStatus);
}


void MonitorableObject::updateMetrics()
{
  MetricUpdateGuard lGuard(*this);

  updateMetrics(lGuard);
}


void MonitorableObject::updateMetrics(const MetricUpdateGuard& aGuard)
{
  if (!aGuard.isCorrectGuard(*this))
    XCEPT_RAISE(RuntimeError,"Metric write guard for incorrect object given to monitorable object '" + getId() + "'");

  SteadyTimePoint_t startTime = SteadyTimePoint_t::clock::now();

  try {
    this->retrieveMetricValues();

    // TODO: should lock a mutex ??
    mUpdateErrorMsg.clear();
  }
  catch (const std::exception& e) {
    // TODO: should lock a mutex ??
    mUpdateErrorMsg = e.what();

    LOG4CPLUS_WARN(mLogger, "Exception of type '" << demangleName(typeid(e).name()) << "' was thrown by retrieveMetricValues() method of monitorable object '" << this->getPath() << "'. Exception message: " << e.what());
  }

  BOOST_FOREACH(MetricMap_t::value_type p, mSimpleMetrics) {
    // last update before start time equals failure
    bool failedUpdate = (p.second->getUpdateTime() < startTime);
    bool isEnabled = p.second->getStatus().second
                     != monitoring::kDisabled;
    // only set the value to unknown for enabled metrics
    if (failedUpdate && isEnabled)
      p.second->setValueUnknown();
  }

  for (auto lIt=mDependantMetrics.begin(); lIt != mDependantMetrics.end(); lIt++)
    lIt->second();
}

void MonitorableObject::setMonitoringStatus(const swatch::core::monitoring::Status aMonStatus)
{
  mMonitoringStatus = aMonStatus;
}

swatch::core::monitoring::Status
MonitorableObject::getMonitoringStatus() const
{
  return mMonitoringStatus;
}

void MonitorableObject::addMonitorable(MonitorableObject* aMonObj)
{
  addObj(aMonObj);
  finishAddingMonitorable(aMonObj);
}

void MonitorableObject::setMonitorableStatus(AbstractMonitorableStatus& aStatus, log4cplus::Logger& aLogger)
{
  if ((mStatus == NULL) || (mStatus == &aStatus)) {
    mStatus = &aStatus;
    mLogger = aLogger;
  }
  else
    XCEPT_RAISE(RuntimeError,"Status of monitorable object '" + getId() + "' has already been set");
}

void MonitorableObject::finishAddingMonitorable(MonitorableObject* aMonObj)
{
  mMonObjChildren.insert( MonObjMap_t::value_type(aMonObj->getId(), aMonObj) );

  // Set status of new child, and all its monitorable descendants
  // (use setStatus method to check that descendants aren't already using custom status instances defined by end user)
  if (mStatus != NULL) {
    aMonObj->setMonitorableStatus(*mStatus, mLogger);

    for (Object::iterator lIt = aMonObj->begin(); lIt != aMonObj->end(); lIt++) {
      if ( MonitorableObject* lChildMonObj = dynamic_cast<MonitorableObject*>(&*lIt) )
        lChildMonObj->setMonitorableStatus(*mStatus, mLogger);
    }
  }
}



MetricUpdateGuard::MetricUpdateGuard(MonitorableObject& aMonObj) :
  mObjStatus(*aMonObj.mStatus)
{
  if (aMonObj.mStatus == NULL)
    XCEPT_RAISE(RuntimeError,"Status not defined for monitorable object " + aMonObj.getId());

  MonitorableStatusGuard lLockGuard(mObjStatus);
  mObjStatus.waitUntilReadyToUpdateMetrics(lLockGuard);
}


MetricUpdateGuard::~MetricUpdateGuard()
{
  MonitorableStatusGuard lLockGuard(mObjStatus);
  mObjStatus.finishedUpdatingMetrics(lLockGuard);
}


bool MetricUpdateGuard::isCorrectGuard(const MonitorableObject& aMonObj) const
{
  return (aMonObj.mStatus == &mObjStatus);
}



MetricReadGuard::MetricReadGuard(const MonitorableObject& aMonObj) :
  mObjStatus(*aMonObj.mStatus)
{
  if (aMonObj.mStatus == NULL)
    XCEPT_RAISE(RuntimeError,"Status not defined for monitorable object " + aMonObj.getId());

  MonitorableStatusGuard lLockGuard(mObjStatus);
  mObjStatus.waitUntilReadyToReadMetrics(lLockGuard);
}


MetricReadGuard::~MetricReadGuard()
{
  MonitorableStatusGuard lLockGuard(mObjStatus);
  mObjStatus.finishedReadingMetrics(lLockGuard);
}


bool MetricReadGuard::isCorrectGuard(const MonitorableObject& aMonObj) const
{
  return (aMonObj.mStatus == &mObjStatus);
}



MonitorableObjectSnapshot::MonitorableObjectSnapshot(const std::string& aPath, 
                                                     swatch::core::StatusFlag aFlag,
                                                     swatch::core::monitoring::Status aMonStatus) :
  mPath(aPath),
  mFlag(aFlag),
  mMonitoringStatus(aMonStatus)
{
}


MonitorableObjectSnapshot::~MonitorableObjectSnapshot()
{
}


const std::string& MonitorableObjectSnapshot::getPath() const
{
  return mPath;
}


std::string MonitorableObjectSnapshot::getId() const
{
  std::size_t lIdxLastDot = mPath.rfind('.');
  if (lIdxLastDot == std::string::npos)
    return mPath;
  else
    return mPath.substr(lIdxLastDot+1);
}


swatch::core::StatusFlag MonitorableObjectSnapshot::getStatusFlag() const
{
  return mFlag;
}


swatch::core::monitoring::Status MonitorableObjectSnapshot::getMonitoringStatus() const
{
  return mMonitoringStatus;
}


}
}

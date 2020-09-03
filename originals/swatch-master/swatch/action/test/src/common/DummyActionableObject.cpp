/**
 * @file    DummyActionableObject.cpp
 * @author  Alessandro Thea
 * @date    February 2015
 *
 */
#include "swatch/action/test/DummyActionableObject.hpp"


#include "xdata/UnsignedInteger.h"

#include "swatch/core/exception.hpp"


namespace swatch {
namespace action {
namespace test {


DummyActionableObject::MonChild::MonChild(const std::string& aId, const DummyActionableObject& aObj) :
  MonitorableObject(aId),
  mObj(aObj),
  mDummyMetric(registerMetric<uint32_t>("dummyMetric"))
{
}


DummyActionableObject::MonChild::~MonChild()
{
}


void DummyActionableObject::MonChild::retrieveMetricValues()
{
  setMetricValue(mDummyMetric, mObj.getNumber());
}


DummyActionableObject::WaitCommand::WaitCommand(const std::string& aId, ActionableObject& aActionable) :
  Command::Command(aId, aActionable, xdata::UnsignedInteger(42))
{
}


DummyActionableObject::WaitCommand::~WaitCommand()
{
}


Functionoid::State DummyActionableObject::WaitCommand::code(const core::XParameterSet& aParams)
{
  DummyActionableObject& aObj = getActionable<DummyActionableObject>();

  setProgress(0.2, "I'm about to wait ...");

  aObj.wait();

  setProgress(0.8, "I've finished waiting!");

  return kDone;
}



DummyActionableObject::DummyActionableObject(const std::string& aId, const std::string& aAlias) :
  ActionableObject(aId, aAlias, "swatch.action.test." + aId),
  mNumber(0),
  mDummyMetric(registerMetric<uint32_t>("dummyMetric")),
  mWait(false),
  mContinue(false),
  mWaitingThread(),
  mWaitBeforeSettingMetricValue(false)
{
}


DummyActionableObject::~DummyActionableObject()
{
}


const std::vector<std::string>& DummyActionableObject::getGateKeeperContexts() const
{
  // Can't set the table names in constructor, since don't know parent at that time ...
  // ... instead, have to set tables names first time this method is called
  if ( mGateKeeperTables.empty() ) {
    mGateKeeperTables.push_back(getPath());

    std::string basePath = getPath();
    basePath.resize(basePath.size() - getId().size());
    mGateKeeperTables.push_back(basePath + "common");
  }
  return mGateKeeperTables;
}


uint32_t DummyActionableObject::getNumber() const
{
  boost::lock_guard<boost::mutex> lGuard(mMutex);
  return mNumber;
}


void DummyActionableObject::setNumber(uint32_t aNumber)
{
  boost::lock_guard<boost::mutex> lGuard(mMutex);
  mNumber = aNumber;
}


const core::SimpleMetric<uint32_t>& DummyActionableObject::getDummyMetric() const
{
  return mDummyMetric;
}


CommandSequence& DummyActionableObject::registerSequence(const std::string& aId, const std::string& aFirstCmdId)
{
  return ActionableObject::registerSequence(aId, aFirstCmdId);
}

CommandSequence& DummyActionableObject::registerSequence(const std::string& aId, Command& aFirstCmd)
{
  return ActionableObject::registerSequence(aId, aFirstCmd);
}


StateMachine& DummyActionableObject::registerStateMachine(const std::string& aId, const std::string& aInitialState, const std::string& aErrorState)
{
  return ActionableObject::registerStateMachine(aId, aInitialState, aErrorState);
}


void DummyActionableObject::pleaseWait()
{
  boost::lock_guard<boost::mutex> lGuard(mMutex);
  mWait = true;
}

void DummyActionableObject::pleaseWaitBeforeSettingMetricValue()
{
  boost::lock_guard<boost::mutex> lGuard(mMutex);
  mWait = true;
  mWaitBeforeSettingMetricValue = true;
}


boost::thread::id DummyActionableObject::getWaitingThread()
{
  boost::lock_guard<boost::mutex> lGuard(mMutex);
  return mWaitingThread;
}


void DummyActionableObject::pleaseContinue()
{
  boost::lock_guard<boost::mutex> lGuard(mMutex);
  mContinue = true;
  mConditionVar.notify_all();
}


void DummyActionableObject::wait() const
{
  boost::unique_lock<boost::mutex> lGuard(mMutex);

  if (mWait) {
//    std::cout << "DummyActionableObject::wait ... STARTING" << std::endl;
    // Wait until no other threads are waiting ...
    while (mWaitingThread != boost::thread::id()) {
      mConditionVar.wait(lGuard);
    }

    mWaitingThread = boost::this_thread::get_id();

    // Now wait until we're told to continue ...
    while (! mContinue) {
//      std::cout << "DummyActionableObject::wait ... WAITING until told to continue." << std::endl;
      mConditionVar.wait(lGuard);
    }

//    std::cout << "DummyActionableObject::wait ... CONTINUING" << std::endl;
    // Finally, reset mWaitingThread, and let other threads have their turn to wait ...
    mWaitingThread = boost::thread::id();
    mContinue = false;
    mConditionVar.notify_all();
  }
}


void DummyActionableObject::retrieveMetricValues()
{
  if (mWaitBeforeSettingMetricValue)
    wait();

  setMetricValue(mDummyMetric, getNumber());

  if (!mWaitBeforeSettingMetricValue)
    wait();
}



} /* namespace test */
} /* namespace action */
} /* namespace swatch */

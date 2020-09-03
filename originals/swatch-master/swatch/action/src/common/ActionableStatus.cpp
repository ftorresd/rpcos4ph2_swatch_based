
#include "swatch/action/ActionableStatus.hpp"


// Standard C++ headers
#include <assert.h>
#include <stdexcept>

// boost headers
#include "boost/iterator/indirect_iterator.hpp"
#include "boost/iterator/iterator_facade.hpp"  // for operator==, etc
#include "boost/lexical_cast.hpp"
#include "boost/thread/lock_algorithms.hpp"  // for lock
#include "boost/thread/lock_options.hpp"  // for adopt_lock_t
#include "boost/thread/lock_types.hpp"  // for unique_lock
#include "boost/thread/pthread/condition_variable.hpp"
#include "boost/thread/pthread/mutex.hpp"  // for mutex



namespace swatch {
namespace action {


// STATIC MEMBERS INITIALIZATION
const std::string ActionableSnapshot::kNullStateMachineId = "";
const std::string ActionableSnapshot::kNullStateId = "";


//------------------------------------------------------------------------------------
ActionableSnapshot::ActionableSnapshot() :
  mAlive(true),
  mStateMachineId(kNullStateMachineId),
  mState(kNullStateId),
  mUpdatingMetrics(false),
  mNumberOfWaitingMetricUpdates(0),
  mNumberOfMetricReaders(0),
  mWaitingToRunAction(false),
  mEnabled(true)
{
}


//------------------------------------------------------------------------------------
bool ActionableSnapshot::isAlive() const
{
  return mAlive;
}

//------------------------------------------------------------------------------------
bool ActionableSnapshot::isEngaged() const
{
  return mStateMachineId != kNullStateMachineId;
}


//------------------------------------------------------------------------------------
bool ActionableSnapshot::isRunning() const
{
  return !mRunningActions.empty();
}

//------------------------------------------------------------------------------------
bool ActionableSnapshot::isUpdatingMetrics() const
{
  return mUpdatingMetrics;
}

//------------------------------------------------------------------------------------
const std::string& ActionableSnapshot::getState() const
{
  return mState;
}

//------------------------------------------------------------------------------------
const Functionoid* ActionableSnapshot::getLastRunningAction() const
{
  if (mRunningActions.empty())
    return NULL;
  else
    return mRunningActions.back();
}


//------------------------------------------------------------------------------------
const std::vector<const Functionoid*>& ActionableSnapshot::getRunningActions() const
{
  return mRunningActions;
}


//------------------------------------------------------------------------------------
const std::string& ActionableSnapshot::getStateMachineId() const
{
  return mStateMachineId;
}


//------------------------------------------------------------------------------------
bool ActionableSnapshot::isEnabled() const
{
  return mEnabled;
}


bool ActionableSnapshot::isActionWaitingToRun() const
{
  return mWaitingToRunAction;
}




//------------------------------------------------------------------------------------
ActionableStatusGuardVec_t lockMutexes(const std::vector<const ActionableStatus*>& aStatusVec)
{
  // Lock the mutexes ...
  std::vector<boost::mutex*> lMutexes;
  for (auto lIt=aStatusVec.begin(); lIt != aStatusVec.end(); lIt++)
    lMutexes.push_back(& (*lIt)->mMutex);

  boost::indirect_iterator<std::vector<boost::mutex*>::iterator> lBegin(lMutexes.begin()), lEnd(lMutexes.end());
  boost::lock(lBegin, lEnd);

  // ... then put them into status guards
  ActionableStatusGuardVec_t lGuardVec;
  for (auto lIt=aStatusVec.begin(); lIt != aStatusVec.end(); lIt++) {
    boost::shared_ptr<ActionableStatusGuard> lGuard(new ActionableStatusGuard(**lIt, boost::adopt_lock_t()));
    lGuardVec.push_back(lGuard);
  }
  return lGuardVec;
}


//------------------------------------------------------------------------------------
ActionableStatus::ActionableStatus() :
  mStatus()
{
}

//------------------------------------------------------------------------------------
ActionableStatus::~ActionableStatus()
{
}

//------------------------------------------------------------------------------------
ActionableSnapshot ActionableStatus::getSnapshot(const ActionableStatusGuard& aGuard) const
{
  throwIfWrongGuard(aGuard);
  return mStatus;
}

//------------------------------------------------------------------------------------
bool ActionableStatus::isAlive(const ActionableStatusGuard& aGuard) const
{
  throwIfWrongGuard(aGuard);
  return mStatus.isAlive();
}

//------------------------------------------------------------------------------------
bool ActionableStatus::isEngaged(const ActionableStatusGuard& aGuard) const
{
  throwIfWrongGuard(aGuard);
  return mStatus.isEngaged();
}

//------------------------------------------------------------------------------------
const std::string& ActionableStatus::getStateMachineId(const ActionableStatusGuard& aGuard) const
{
  throwIfWrongGuard(aGuard);
  return mStatus.getStateMachineId();
}

//------------------------------------------------------------------------------------
const std::string& ActionableStatus::getState(const ActionableStatusGuard& aGuard) const
{
  throwIfWrongGuard(aGuard);
  return mStatus.getState();
}

//------------------------------------------------------------------------------------
bool ActionableStatus::isBusy(const ActionableStatusGuard& aGuard) const
{
  throwIfWrongGuard(aGuard);
  return mStatus.isRunning();
}

//------------------------------------------------------------------------------------
bool ActionableStatus::isUpdatingMetrics(const core::MonitorableStatusGuard& aGuard) const
{
  throwIfWrongGuard(aGuard);
  return mStatus.isUpdatingMetrics();
}

//------------------------------------------------------------------------------------
const std::vector<const Functionoid*>& ActionableStatus::getRunningActions(const ActionableStatusGuard& aGuard) const
{
  throwIfWrongGuard(aGuard);
  return mStatus.getRunningActions();
}

//------------------------------------------------------------------------------------
const Functionoid* ActionableStatus::getLastRunningAction(const ActionableStatusGuard& aGuard) const
{
  throwIfWrongGuard(aGuard);
  return mStatus.getLastRunningAction();
}


//------------------------------------------------------------------------------------
bool ActionableStatus::isEnabled(const ActionableStatusGuard& aGuard) const
{
  throwIfWrongGuard(aGuard);
  return mStatus.isEnabled();
}


//------------------------------------------------------------------------------------
void ActionableStatus::enable(const ActionableStatusGuard& aGuard)
{
  throwIfWrongGuard(aGuard);
  mStatus.mEnabled = true;
}


//------------------------------------------------------------------------------------
void ActionableStatus::disable(const ActionableStatusGuard& aGuard)
{
  throwIfWrongGuard(aGuard);
  mStatus.mEnabled = false;
}


//------------------------------------------------------------------------------------
void ActionableStatus::setStateMachine(const std::string& aStateMachine, const std::string& aState, const ActionableStatusGuard& aGuard)
{
  throwIfWrongGuard(aGuard);
  mStatus.mStateMachineId = aStateMachine;
  mStatus.mState = aState;
}

//------------------------------------------------------------------------------------
void ActionableStatus::setNoStateMachine(const ActionableStatusGuard& aGuard)
{
  throwIfWrongGuard(aGuard);
  mStatus.mStateMachineId = ActionableSnapshot::kNullStateMachineId;
  mStatus.mState = ActionableSnapshot::kNullStateId;
}

//------------------------------------------------------------------------------------
void ActionableStatus::setState(const std::string& aState, const ActionableStatusGuard& aGuard)
{
  throwIfWrongGuard(aGuard);
  mStatus.mState = aState;
}

//------------------------------------------------------------------------------------
void ActionableStatus::addAction(const Functionoid& aAction, const ActionableStatusGuard& aGuard)
{
  throwIfWrongGuard(aGuard);
  mStatus.mRunningActions.push_back(&aAction);
}

//------------------------------------------------------------------------------------
void ActionableStatus::popAction(const ActionableStatusGuard& aGuard)
{
  throwIfWrongGuard(aGuard);
  mStatus.mRunningActions.pop_back();
  if ( mStatus.mRunningActions.empty() )
    mConditionVar.notify_all();
}

//------------------------------------------------------------------------------------
void ActionableStatus::kill(const ActionableStatusGuard& aGuard)
{
  throwIfWrongGuard(aGuard);
  mStatus.mAlive = false;
}


//------------------------------------------------------------------------------------
void ActionableStatus::waitUntilReadyToUpdateMetrics(core::MonitorableStatusGuard& aGuard)
{
  throwIfWrongGuard(aGuard);

  mStatus.mNumberOfWaitingMetricUpdates++;
  while ((mStatus.mRunningActions.size() > 0) || mStatus.mWaitingToRunAction || isUpdatingMetrics(aGuard) || (mStatus.mNumberOfMetricReaders > 0)) {
    mConditionVar.wait(getUniqueLock(aGuard));
  }

  mStatus.mNumberOfWaitingMetricUpdates--;
  mStatus.mUpdatingMetrics = true;
}


//------------------------------------------------------------------------------------
void ActionableStatus::finishedUpdatingMetrics(const core::MonitorableStatusGuard& aGuard)
{
  throwIfWrongGuard(aGuard);
  assert ( mStatus.mUpdatingMetrics );

  mStatus.mUpdatingMetrics = false;
  mConditionVar.notify_all();
}


//------------------------------------------------------------------------------------
void ActionableStatus::waitUntilReadyToReadMetrics(core::MonitorableStatusGuard& aGuard)
{
  throwIfWrongGuard(aGuard);

  while (isUpdatingMetrics(aGuard) || (mStatus.mNumberOfWaitingMetricUpdates > 0)) {
    mConditionVar.wait(getUniqueLock(aGuard));
  }

  mStatus.mNumberOfMetricReaders++;
}


//------------------------------------------------------------------------------------
void ActionableStatus::finishedReadingMetrics(const core::MonitorableStatusGuard& aGuard)
{
  throwIfWrongGuard(aGuard);

  mStatus.mNumberOfMetricReaders--;
  mConditionVar.notify_all();
}


//------------------------------------------------------------------------------------
void ActionableStatus::waitUntilReadyToRunAction(const Functionoid& aAction, ActionableStatusGuard& aGuard)
{
  throwIfWrongGuard(aGuard);

  if ( getRunningActions(aGuard).size() )
    XCEPT_RAISE(core::RuntimeError,"waitUntilReadyToRunAction: actions already running");
  else if ( mStatus.mWaitingToRunAction )
    XCEPT_RAISE(core::RuntimeError,"waitUntilReadyToRunAction: another thread already waiting");

  mStatus.mWaitingToRunAction = true;

  while ( isUpdatingMetrics(aGuard) ) {
    mConditionVar.wait(getUniqueLock(aGuard));
  }

  mStatus.mRunningActions.push_back(&aAction);
  mStatus.mWaitingToRunAction = false;
}


void ActionableStatus::waitUntilReadyToRunAction(const std::vector<std::pair<ActionableStatus*, ActionableStatusGuard*> >& aVec, const Functionoid& aAction)
{
  // 1) Check that:
  //     - there aren't any NULL pointers; and ...
  //     - the guards are all for the correct status instance;
  typedef std::vector<std::pair<ActionableStatus*, ActionableStatusGuard*> > Vec_t;
  for (Vec_t::const_iterator lIt=aVec.begin(); lIt!=aVec.end(); lIt++) {
    if (lIt->first == NULL)
      XCEPT_RAISE(core::RuntimeError,"waitUntilReadyToRunAction: status pointer in element " + boost::lexical_cast<std::string>(lIt-aVec.begin()) + " is NULL");
    else if (lIt->second == NULL)
      XCEPT_RAISE(core::RuntimeError,"waitUntilReadyToRunAction: guard pointer in element " + boost::lexical_cast<std::string>(lIt-aVec.begin()) + " is NULL");

    lIt->first->throwIfWrongGuard(*lIt->second);
  }

  // 2) Check that:
  //     - no actions are already running; and ...
  //     - no other threads are already waiting
  for (Vec_t::const_iterator lIt=aVec.begin(); lIt!=aVec.end(); lIt++) {
    if ( lIt->first->getRunningActions(*lIt->second).size() )
      XCEPT_RAISE(core::RuntimeError,"waitUntilReadyToRunAction: actions already running");
    else if ( lIt->first->mStatus.mWaitingToRunAction )
      XCEPT_RAISE(core::RuntimeError,"waitUntilReadyToRunAction: another thread already waiting");
  }

  // 3) Declare action waiting to run on each status instance, and unlock the mutexes (to avoid deadlocks whilst waiting for each status to change)
  for (Vec_t::const_iterator lIt=aVec.begin(); lIt!=aVec.end(); lIt++) {
    lIt->first->mStatus.mWaitingToRunAction = true;
    lIt->first->getUniqueLock(*lIt->second).unlock();
  }

  // 4) Re-lock the mutex for each status instance individually, and wait until isUpdatingMetrics() returns false
  for (Vec_t::const_iterator lIt=aVec.begin(); lIt!=aVec.end(); lIt++) {
    ActionableStatusGuard lGuard(*lIt->first);
    while ( lIt->first->isUpdatingMetrics(lGuard) ) {
      lIt->first->mConditionVar.wait(lIt->first->getUniqueLock(lGuard));
    }
    lIt->first->mStatus.mRunningActions.push_back(&aAction);
    lIt->first->mStatus.mWaitingToRunAction = false;
  }

  // 5) Re-lock all the original lock guard objects
  std::vector<boost::unique_lock<boost::mutex>*> lLockGuardVec;
  for (Vec_t::const_iterator lIt=aVec.begin(); lIt!=aVec.end(); lIt++)
    lLockGuardVec.push_back( &lIt->first->getUniqueLock(*lIt->second) );

  boost::indirect_iterator<std::vector<boost::unique_lock<boost::mutex>*>::iterator> lBegin(lLockGuardVec.begin()), lEnd(lLockGuardVec.end());
  boost::lock(lBegin, lEnd);
}


//------------------------------------------------------------------------------------
void ActionableStatus::throwIfWrongGuard(const core::MonitorableStatusGuard& aGuard) const
{
  if ( ! aGuard.isCorrectGuard(*this) )
    XCEPT_RAISE(IncorrectActionableGuard,"");
}


} // end ns action
} // end ns swatch


#include "swatch/action/SystemStateMachine.hpp"


// Standard headers
#include <math.h>                       // for pow
#include <algorithm>                    // for min_element
#include <exception>                    // for exception
#include <iostream>                     // for operator<<, basic_ostream, cout, etc
#include <sstream>                      // for basic_stringbuf<>::int_type, etc
#include <utility>                      // for pair

// boost headers
#include "boost/bind.hpp"
#include "boost/chrono/duration.hpp"    // for operator<, microseconds, etc
#include "boost/date_time/posix_time/posix_time_config.hpp"
#include "boost/date_time/posix_time/posix_time_types.hpp"
#include "boost/date_time/time.hpp"     // for base_time
#include "boost/foreach.hpp"
#include "boost/lexical_cast.hpp"       // for lexical_cast
#include "boost/thread/lock_types.hpp"  // for unique_lock
#include "boost/thread/v2/thread.hpp"   // for sleep_for

// log4cplus headers
#include <log4cplus/loggingmacros.h>

// SWATCH headers
#include "swatch/action/ActionableObject.hpp"
#include "swatch/action/ActionableSystem.hpp"
#include "swatch/action/GateKeeper.hpp"
#include "swatch/action/MonitoringSetting.hpp"  // for Status::kEnabled, etc
#include "swatch/core/ReadOnlyXParameterSet.hpp"
#include "swatch/action/SystemBusyGuard.hpp"
#include "swatch/action/ThreadPool.hpp"


namespace swatch {
namespace action {


//------------------------------------------------------------------------------------
SystemTransition::Step::Step(const std::vector<Transition*>& aTransitions) :
  mTransitions(aTransitions)
{
}

//------------------------------------------------------------------------------------
const std::vector<Transition*>& SystemTransition::Step::get()
{
  return mTransitions;
}


//------------------------------------------------------------------------------------
const std::vector<Transition*>& SystemTransition::Step::cget() const
{
  return mTransitions;
}


//------------------------------------------------------------------------------------
SystemTransition::SystemTransition(const std::string& aId, const std::string& aAlias, SystemStateMachine& aOp, ActionableSystem::StatusContainer& aStatusMap, const std::string& aStartState, const std::string& aEndState) :
  SystemFunctionoid(aId, aAlias, aOp.getActionable()),
  mFSM(aOp),
  mStatusMap(aStatusMap),
  mStartState(aStartState),
  mEndState(aEndState),
  mGateKeeper(NULL),
  mState(kInitial),
  mStepIt(mSteps.end())
{
}


//------------------------------------------------------------------------------------
SystemTransition::~SystemTransition()
{
}


//------------------------------------------------------------------------------------
SystemTransition::iterator SystemTransition::begin()
{
  return mSteps.begin();
}


//------------------------------------------------------------------------------------
SystemTransition::iterator SystemTransition::end()
{
  return mSteps.end();
}


SystemTransition::const_iterator SystemTransition::begin() const
{
  return mSteps.begin();
}


SystemTransition::const_iterator SystemTransition::end() const
{
  return mSteps.end();
}


size_t SystemTransition::size() const
{
  return mSteps.size();
}


SystemTransitionSnapshot SystemTransition::getStatus() const
{
  boost::unique_lock<boost::mutex> lLock(mMutex);
  return getStatus(lLock);
}


const std::string& SystemTransition::getStartState() const
{
  return mStartState;
}


const std::string& SystemTransition::getEndState() const
{
  return mEndState;
}


const SystemStateMachine& SystemTransition::getStateMachine() const
{
  return mFSM;
}


SystemStateMachine& SystemTransition::getStateMachine()
{
  return mFSM;
}


SystemTransition& SystemTransition::add(const std::vector<Transition*>& aTransitions)
{
  // 1) Consistency checks
  if (aTransitions.empty())
    XCEPT_RAISE(InvalidSystemTransition,"Cannot add empty step to system transition");

  for (std::vector<Transition*>::const_iterator lIt=aTransitions.begin(); lIt!=aTransitions.end(); lIt++) {
    if (*lIt == NULL)
      XCEPT_RAISE(InvalidSystemTransition,"Element "+boost::lexical_cast<std::string>(lIt-aTransitions.begin())+" of child transition vector is NULL");

    const ActionableObject& lObj = (*lIt)->getStateMachine().getActionable();

    //Throw if any transitions in vector are not from children of this system
    bool isChild = false;
    std::vector<std::string> children = mFSM.getActionable().getChildren();
    for (std::vector<std::string>::const_iterator lIt2=children.begin(); lIt2!=children.end(); lIt2++) {
      ActionableObject* lChild = mFSM.getActionable().getObjPtr<ActionableObject>(*lIt2);
      if ( &lObj == lChild )
        isChild = true;
    }
    if (isChild == false)
      XCEPT_RAISE(InvalidSystemTransition,"Cannot add transition on non-child object '"+lObj.getPath()+"' to system state machine '"+mFSM.getPath());


    // Throw if any two transitions in vector are from same child
    for (std::vector<Transition*>::const_iterator lIt2=aTransitions.begin(); lIt2!=aTransitions.end(); lIt2++) {
      if ( (lIt != lIt2) && (&lObj == &(*lIt2)->getStateMachine().getActionable()) )
        XCEPT_RAISE(InvalidSystemTransition,"Multiple transitions requested for same child object  '"+lObj.getPath()+"' in system-level transition");
    }

    // Throw if (for any child) transition is from different state machine than has been registered before
    bool newParticipant = true;
    for (std::set<const StateMachine*>::const_iterator lIt2 = mFSM.getParticipants().begin(); lIt2 != mFSM.getParticipants().end(); lIt2++) {
      if ( (&(*lIt2)->getActionable() == &lObj) ) {
        if (*lIt2 == &(*lIt)->getStateMachine())
          newParticipant = false;
        else
          XCEPT_RAISE(InvalidSystemTransition,"Object '"+lObj.getPath()+"', transition '"+(*lIt)->getId()+"' is inconsistent with transitions already in system-level state machine '"+mFSM.getPath()+"'");
      }
    }

    // Throw if any child transition is inconsistent with previous steps from this transition (i.e. if it's start state != end state from transition in previous step)
    const Transition* lLastTransition = NULL;
    for (std::vector<Step>::const_iterator lStepIt=mSteps.begin(); lStepIt!=mSteps.end(); lStepIt++) {
      for (std::vector<Transition*>::const_iterator lIt2=lStepIt->cget().begin(); lIt2!=lStepIt->cget().end(); lIt2++) {
        if (& (*lIt2)->getActionable() == & (*lIt)->getActionable())
          lLastTransition = *lIt2;
      }
    }
    if ( (lLastTransition != NULL) && (lLastTransition->getEndState() != (*lIt)->getStartState()))
      XCEPT_RAISE(InvalidSystemTransition,"Object '"+lObj.getPath()+ "', transition '"+(*lIt)->getId()+"' (start state: '"+(*lIt)->getStartState()+"') is incompatible with last transition in same step, '"+lLastTransition->getId()+"' (end state: '"+lLastTransition->getEndState()+"')");
  }

  for (std::vector<Transition*>::const_iterator lIt=aTransitions.begin(); lIt!=aTransitions.end(); lIt++) {
    addParticipant((*lIt)->getActionable());
    mFSM.mChildFSMs.insert( &(*lIt)->getStateMachine() );
    mFSM.mNonConstChildFSMs.insert( &(*lIt)->getStateMachine() );
  }
  mSteps.push_back( Step(aTransitions) );

  return *this;
}


void SystemTransition::checkForMissingParameters(const GateKeeper& aGateKeeper, std::map<const Transition*, std::vector<CommandVec::MissingParam> >& aMissingParams) const
{
  checkForMissingParameters(aGateKeeper, aMissingParams, mStatusMap.lockMutexes());
}


void SystemTransition::checkForMissingParameters(const GateKeeper& aGateKeeper, std::map<const Transition*, std::vector<CommandVec::MissingParam> >& aMissingParams, const ActionableSystem::GuardMap_t& aGuardMap) const
{
  aMissingParams.clear();

  for (std::vector<Step>::const_iterator lIt=mSteps.begin(); lIt!=mSteps.end(); lIt++) {
    for (std::vector<Transition*>::const_iterator lIt2=lIt->cget().begin(); lIt2!=lIt->cget().end(); lIt2++) {
      const ActionableObject& lChild = (*lIt2)->getActionable();
      // Ignore this transition in case the child has been disabled
      if (!mStatusMap.getStatus(lChild).isEnabled(*aGuardMap.at(&lChild)))
        continue;

      std::vector<core::ReadOnlyXParameterSet> lParamSets;
      std::vector<CommandVec::MissingParam> lMissingParams;
      (*lIt2)->checkForMissingParameters(aGateKeeper, lParamSets, lMissingParams);
      if ( ! lMissingParams.empty() )
        aMissingParams[*lIt2] = lMissingParams;
    }
  }
}


void SystemTransition::checkForInvalidParameters(const GateKeeper& aGateKeeper, std::map< const Transition*, std::vector<CommandVec::ParamRuleViolationList> >& aRuleViolations) const
{
  checkForInvalidParameters(aGateKeeper, aRuleViolations, mStatusMap.lockMutexes());
}


void SystemTransition::checkForInvalidParameters(const GateKeeper& aGateKeeper, std::map<const Transition*,std::vector<CommandVec::ParamRuleViolationList> >& aRuleViolations, const ActionableSystem::GuardMap_t& aGuardMap) const
{
  aRuleViolations.clear();

  for (const Step& lStep : mSteps) {
    for (const Transition* lObjTransition : lStep.cget()) {
      const ActionableObject& lChild = lObjTransition->getActionable();
      // Ignore this transition in case the child has been disabled
      if (!mStatusMap.getStatus(lChild).isEnabled(*aGuardMap.at(&lChild)))
        continue;

      std::vector<core::ReadOnlyXParameterSet> lParamSets;
      std::vector<CommandVec::MissingParam> lMissingParams;
      lObjTransition->checkForMissingParameters(aGateKeeper, lParamSets, lMissingParams);
      std::vector<CommandVec::ParamRuleViolationList> lRuleViolations;
      lObjTransition->checkForInvalidParameters(lParamSets, lRuleViolations);
      if ( ! lRuleViolations.empty() )
        aRuleViolations[lObjTransition] = lRuleViolations;
    }
  }
}


void SystemTransition::registerErrorAnalyser(const SnapshotAnalyser_t& aFunction)
{
  mErrorAnalyser = aFunction;
}


void SystemTransition::registerWarningAnalyser(const SnapshotAnalyser_t& aFunction)
{
  mWarningAnalyser = aFunction;
}


void SystemTransition::exec(const GateKeeper& aGateKeeper, const bool& aUseThreadPool)
{
  boost::shared_ptr<SystemBusyGuard> lBusyGuard;
  std::set<const ActionableObject*> lEnabledChildren;
  {
    // Put the actionable status mutexes into lock guards before doing anything
    // (even checkForMissingParameters relies on status member data - enabled/disabled flag)
    ActionableSystem::GuardMap_t lStatusGuardMap = mStatusMap.lockMutexes();

    // 0) Throw if any parameters are missing from gatekeeper, or any parameter values violate rules/constraints
    std::map<const Transition*, std::vector<Transition::MissingParam> > lMissingParams;
    checkForMissingParameters(aGateKeeper, lMissingParams, lStatusGuardMap);
    if ( ! lMissingParams.empty() )
      XCEPT_RAISE(ParameterNotFound, "Could not find value of parameters for " + boost::lexical_cast<std::string>(lMissingParams.size()) + " transitions");

    std::map<const Transition*, std::vector<Transition::ParamRuleViolationList> > lRuleViolations;
    checkForInvalidParameters(aGateKeeper, lRuleViolations, lStatusGuardMap);
    if ( ! lRuleViolations.empty() )
      XCEPT_RAISE(CommandParameterCheckFailed, "Parameter values fail rules and/or constraints in " + boost::lexical_cast<std::string>(lRuleViolations.size()) + " transitions");      
    
    // 1) Check current state; if in correct state, then
    //    request control of the resource (incl. children), and continue
    const ActionableSystem& lSystem = getStateMachine().getActionable();
    ActionableStatus& lSysStatus = mStatusMap.getSystemStatus();
    ActionableStatusGuard& lSysGuard = *lStatusGuardMap.at(&lSystem);

    // 1a) Check that system is engaged in correct state machine, and is in the correct state
    if ( lSysStatus.getStateMachineId(lSysGuard) !=  getStateMachine().getId())
      XCEPT_RAISE(ResourceInWrongStateMachine,"System '"+lSystem.getPath()+"' is not yet engaged in state machine '"+getStateMachine().getId()+"'");

    else if ( lSysStatus.getState(lSysGuard) != getStartState() )
      XCEPT_RAISE(ResourceInWrongState,"System '"+lSystem.getPath()+"' is in state '"+lSysStatus.getState(lSysGuard)+"'; transition '"+getId()+"' cannot be run");

    // 1b) Check that enabled children are engaged in correct state machine, and are in the correct state
    typedef std::map<ActionableObject*, const Transition* > ObjTransitionMap_t;
    ObjTransitionMap_t childTransitionMap;
    for (SystemTransition::const_iterator lIt=(end()-1); lIt != (begin()-1); lIt--) {
      for (std::vector<Transition*>::const_iterator lIt2=lIt->cget().begin(); lIt2!=lIt->cget().end(); lIt2++)
        childTransitionMap[ &(*lIt2)->getStateMachine().getActionable() ] = *lIt2;
    }

    BOOST_FOREACH( const ObjTransitionMap_t::value_type e, childTransitionMap ) {
      const ActionableStatusGuard& lChildGuard = *lStatusGuardMap.at(e.first);
      const ActionableStatus& lChildStatus = mStatusMap.getStatus(*e.first);

      // Ignore children that aren't enabled
      if (!lChildStatus.isEnabled(lChildGuard))
        continue;

      lEnabledChildren.insert(e.first);
      if ( lChildStatus.getStateMachineId(lChildGuard) != e.second->getStateMachine().getId() )
        XCEPT_RAISE(ResourceInWrongStateMachine,"Resource '"+e.first->getPath()+"' is not yet engaged in state machine '"+e.second->getStateMachine().getId()+"'");
      else if ( lChildStatus.getState(lChildGuard) != e.second->getStartState() )
        XCEPT_RAISE(ResourceInWrongState,"Resource '"+e.first->getPath()+"' is in state "+lChildStatus.getState(lChildGuard)+", transition '"+e.second->getId()+"' cannot be run");
    }

    SystemBusyGuard::Callback_t lCallback = boost::bind(&SystemTransition::changeState, this, _1, _2);
    lBusyGuard.reset(new SystemBusyGuard(*this, mStatusMap, lStatusGuardMap, lCallback));
  }

  // 2) Reset the status of this transition's state variables
  {
    boost::unique_lock<boost::mutex> lock( mMutex );

    mGateKeeper = & aGateKeeper;
    mState = kInitial;
    mStepIt = mSteps.end();
    mStatusOfCompletedSteps.clear();
    mStatusOfCompletedSteps.reserve(mSteps.size());
    mEnabledChildren = lEnabledChildren;
    mMessage = "";
    mCachedMonitoringSettings.clear();
    mFSM.extractMonitoringSettings(aGateKeeper, getEndState(), mCachedMonitoringSettings);
  }

  // 3) Execute the command
  if ( aUseThreadPool) {
    boost::unique_lock<boost::mutex> lock(mMutex);
    mState = kScheduled;

    ThreadPool& pool = ThreadPool::getInstance();
    pool.addTask<SystemTransition, SystemBusyGuard>(this , &SystemTransition::runSteps, lBusyGuard);
  }
  else {
    // otherwise execute in same thread
    this->runSteps(lBusyGuard);
  }
}


SystemTransitionSnapshot SystemTransition::getStatus(const boost::unique_lock<boost::mutex>& aLockGuard) const
{
  float runningTime = 0.0;
  switch (mState) {
    case kInitial :
    case kScheduled :
      break;
    default :
      boost::posix_time::ptime endTime;
      if (mState == kRunning)
        endTime = boost::posix_time::microsec_clock::universal_time();
      else
        endTime = mExecEndTime;

      boost::posix_time::time_duration duration = endTime - mExecStartTime;
      runningTime = duration.total_seconds();
      runningTime += double(duration.fractional_seconds()) / pow(10.0, duration.num_fractional_digits());

      break;
  }

  const Step* currentStep =  ( ((mStepIt == mSteps.end()) || (mState == State::kError)) ? NULL : &*mStepIt);

  std::set<std::string> lEnabledChildren;
  for (auto lIt=mEnabledChildren.begin(); lIt != mEnabledChildren.end(); lIt++)
    lEnabledChildren.insert((*lIt)->getPath());

  return SystemTransitionSnapshot(IdAliasPair(*this), IdAliasPair(getActionable()), mState, runningTime, currentStep, mStatusOfCompletedSteps, mSteps.size(), lEnabledChildren, mMessage);
}


void SystemTransition::runSteps(boost::shared_ptr<SystemBusyGuard> aGuard)
{
  // 1) Declare that I'm running
  {
    boost::unique_lock<boost::mutex> lock( mMutex );
    mExecStartTime = boost::posix_time::microsec_clock::universal_time();
    // Finish straight away if there aren't any steps to run
    if ( mSteps.empty() ) {
      mState = kDone;
      mStepIt = mSteps.end();
      mExecEndTime = mExecStartTime;
      return;
    }
    else {
      mState = kRunning;
      mStepIt = mSteps.begin();
    }
  }

  // 2) Run the steps
  try {
    bool lWarningOccurred = false;

    while ( true ) {
      // 2.i) Start running the child transitions in this step (for enabled children)
      for (auto lIt=mStepIt->get().begin(); lIt!=mStepIt->get().end(); lIt++) {
        Transition& lChildTransition = *(*lIt);
        if (mEnabledChildren.count(&lChildTransition.getActionable()) == 0)
          continue;
        ActionableObject& lChild = (*lIt)->getActionable();
        lChildTransition.exec(&aGuard->getChildGuard(lChild), *mGateKeeper);
      }

      // 2.ii) Wait for them all to complete
      bool lErrorOccurred = false;
      for (auto lIt=mStepIt->get().begin(); lIt!=mStepIt->get().end(); lIt++) {
        if (mEnabledChildren.count(&(*lIt)->getActionable()) == 0)
          continue;

        while ( (*lIt)->getActionable().getStatus().getRunningActions().back() != this ) {
          boost::this_thread::sleep_for(boost::chrono::microseconds(100));
        }
        if ( (*lIt)->getState() == kError )
          lErrorOccurred = true;
        else if ( (*lIt)->getState() == kWarning )
          lWarningOccurred = true;
      }

      // 2.iii) Harvest the status objects (before locking mutex, to avoid deadlock scenarios)
      //        (For disabled objects, there is no status object, so use NULL pointer)
      std::vector<boost::shared_ptr<const TransitionSnapshot> > lStatusVec;
      for (auto lIt=mStepIt->get().begin(); lIt!=mStepIt->get().end(); lIt++) {
        boost::shared_ptr<const TransitionSnapshot> lStatus;
        if (mEnabledChildren.count(&(*lIt)->getActionable()))
          lStatus.reset(new TransitionSnapshot((*lIt)->getStatus()));
        lStatusVec.push_back(lStatus);
      }

      // 2.iv) Lock the mutex, and update member data
      boost::unique_lock<boost::mutex> lLock(mMutex);
      mStatusOfCompletedSteps.push_back( lStatusVec );

      // Don't execute any more steps if there was an error
      if ( lErrorOccurred ) {
        mState = kError;
        mExecEndTime = boost::posix_time::microsec_clock::universal_time();
        if (mErrorAnalyser) {
          try {
            mMessage = mErrorAnalyser(getStatus(lLock));
          }
          catch (const std::exception& aExc) {
            mMessage = "Exception of type '" + core::demangleName(typeid(aExc).name()) + "' thrown by error analyser function. Details: " + aExc.what();
          }
          catch (...) {
            mMessage = "Exception thrown by error analyser function.";
          }
        }
        return;
      }

      // Increment the "current step" iterator
      ++mStepIt;

      // Exit the loop if no more steps remain
      if ( mStepIt == mSteps.end() ) {
        mState = ( lWarningOccurred ? kWarning : kDone );
        mExecEndTime = boost::posix_time::microsec_clock::universal_time();
        if (lWarningOccurred && mWarningAnalyser) {
          try {
            mMessage = mWarningAnalyser(getStatus(lLock));
          }
          catch (const std::exception& aExc) {
            mMessage = "Exception of type '" + core::demangleName(typeid(aExc).name()) + "' thrown by warning analyser function. Details: " + aExc.what();
          }
          catch (...) {
            mMessage = "Exception thrown by warning analyser function.";
          }
        }
        return;
      }
    }

  }
  catch (const std::exception& e) {
    std::cout << "An exception occurred in SystemTransition::runSteps method: " << e.what() << std::endl;

    boost::unique_lock<boost::mutex> lock( mMutex );
    mState = kError;
    mExecEndTime = boost::posix_time::microsec_clock::universal_time();
  }

  // 3) Apply monitoring settings
  mFSM.applyMonitoringSettings(mCachedMonitoringSettings);

  // 4) The resource is released by destruction of BusyGuard
}


//------------------------------------------------------------------------------------
void SystemTransition::changeState(const ActionableStatusGuard& aGuard, std::ostream& aLogMessageSuffix)
{
  State lActionState = getStatus().getState();
  std::string lNewState = getEndState();
  if ((lActionState != kDone) && (lActionState != kWarning))
    lNewState = getStateMachine().getErrorState();

  mStatusMap.getSystemStatus().setState(lNewState, aGuard);
  aLogMessageSuffix << "Entering state '" << lNewState << "'";
}


//------------------------------------------------------------------------------------
SystemStateMachine::SystemStateMachine(const std::string& aId, ActionableSystem& aResource, ActionableSystem::StatusContainer& aStatusMap, const std::string& aInitialState, const std::string& aErrorState) :
  Object(aId),
  mResource(aResource),
  mStatusMap(aStatusMap),
  mInitialState(aInitialState),
  mErrorState(aErrorState)
{
  addState(mInitialState);
  addState(mErrorState);
}


//------------------------------------------------------------------------------------
SystemStateMachine::~SystemStateMachine()
{
}


//------------------------------------------------------------------------------------
const ActionableSystem& SystemStateMachine::getActionable() const
{
  return mResource;
}


//------------------------------------------------------------------------------------
ActionableSystem& SystemStateMachine::getActionable()
{
  return mResource;
}


//------------------------------------------------------------------------------------
const std::string& SystemStateMachine::getInitialState() const
{
  return mInitialState;
}


//------------------------------------------------------------------------------------
const std::string& SystemStateMachine::getErrorState() const
{
  return mErrorState;
}


//------------------------------------------------------------------------------------
const std::set<const StateMachine*>& SystemStateMachine::getParticipants() const
{
  return mChildFSMs;
}


//------------------------------------------------------------------------------------
const std::vector<std::string>& SystemStateMachine::getStates() const
{
  return mStates;
}


//------------------------------------------------------------------------------------
const std::map<std::string, SystemTransition*>& SystemStateMachine::getTransitions(const std::string& aStateId) const
{
  return getState(aStateId).transitionMap;
}


//------------------------------------------------------------------------------------
void SystemStateMachine::addState(const std::string& aStateId)
{
  if ( mStateMap.count(aStateId) )
    XCEPT_RAISE(StateAlreadyDefined,"State '"+aStateId+"' has already been defined");
  else {
    State* lState = new State(aStateId);
    addObj(lState);
    mStateMap[aStateId] = lState;
    mStates.push_back(aStateId);
  }
}


//------------------------------------------------------------------------------------
SystemTransition& SystemStateMachine::addTransition(const std::string& aTransitionId, const std::string& aFromState, const std::string& aToState)
{
  return addTransition(aTransitionId, "", aFromState, aToState);
}


//------------------------------------------------------------------------------------
SystemTransition& SystemStateMachine::addTransition(const std::string& aTransitionId, const std::string& aAlias, const std::string& aFromState, const std::string& aToState)
{
  State& lFromState = getState(aFromState);

  if (mStateMap.count(aToState) == 0 )
    XCEPT_RAISE(StateNotDefined,"State '"+aToState+"' does not exist in state machine '"+getPath()+"'");
  else if (lFromState.transitionMap.count(aTransitionId))
    XCEPT_RAISE(TransitionAlreadyDefined,"Transition '"+aTransitionId+"' from state '"+aFromState+"' already defined in state machine '"+getPath()+"'");

  SystemTransition* t = new SystemTransition(aTransitionId, aAlias, *this, mStatusMap, aFromState, aToState);
  lFromState.addTransition(t);

  return *t;
}


//------------------------------------------------------------------------------------
void SystemStateMachine::reset(const GateKeeper& aGateKeeper)
{
  ActionableSystem::GuardMap_t lGuardMap = mStatusMap.lockMutexes();
  const ActionableStatusGuard& lGuard = *lGuardMap.at(&getActionable());

  // Throw if system/children are not in this state machine or running transition
  checkEngagedAndNotInTransition(lGuard, "reset");
  for (auto smIt = mNonConstChildFSMs.begin(); smIt!=mNonConstChildFSMs.end(); smIt++) {
    const ActionableObject& lChild = (*smIt)->getActionable();
    const ActionableStatusGuard& lChildGuard = *lGuardMap.at(&(*smIt)->getActionable());
    if (mStatusMap.getStatus(lChild).isEnabled(lChildGuard) && aGateKeeper.isEnabled((*smIt)->getActionable().getPath()))
      checkChildEngagedAndNotInTransition(**smIt, lChildGuard, "reset");
  }

  // If not thrown already, then all is good: enable/disable children ...
  // ... reset states of system + enabled children; and reset their maskables of enabled children
  LOG4CPLUS_INFO(mResource.getLogger(), "Resetting system state machine '" << getId() << "'; entering state '" << getInitialState() << "'");
  mStatusMap.getSystemStatus().setState(getInitialState(), lGuard);

  disableChildren(aGateKeeper, lGuardMap);

  for (std::set<StateMachine*>::const_iterator smIt=mNonConstChildFSMs.begin(); smIt != mNonConstChildFSMs.end(); smIt++) {
    ActionableObject& lChild = (*smIt)->getActionable();
    const ActionableStatusGuard& lChildGuard = *lGuardMap.at(&lChild);
    ActionableStatus& lChildStatus = mStatusMap.getStatus((*smIt)->getActionable());

    if (lChildStatus.isEnabled(lChildGuard)) {
      (*smIt)->reset(aGateKeeper, lChildGuard);
      lChild.setMonitoringStatus(core::monitoring::kEnabled);
    }
    else
      lChild.setMonitoringStatus(core::monitoring::kNonCritical);
  }
  
  // Reset monitoring settings on children, and apply settings from gatekeeper
  resetMonitoringSettings();
  MonitoringSettings_t lMonSettings;
  extractMonitoringSettings(aGateKeeper, getInitialState(), lMonSettings);
  applyMonitoringSettings(lMonSettings);
}


//------------------------------------------------------------------------------------
void SystemStateMachine::engage(const GateKeeper& aGateKeeper)
{
  ActionableSystem::GuardMap_t lGuardMap = mStatusMap.lockMutexes();
  const ActionableStatusGuard& lSysGuard = *lGuardMap.at(&getActionable());
  ActionableStatus& lSysStatus = mStatusMap.getSystemStatus();

  // Throw if system or any of the non-disabled participating children are already in a state machine
  if (lSysStatus.isEngaged(lSysGuard))
    XCEPT_RAISE(ResourceInWrongStateMachine,"Cannot engage other state machine; system '"+getPath()+"' currently in state machine"+lSysStatus.getStateMachineId(lSysGuard)+"'");

  for (auto lIt=this->getParticipants().begin(); lIt!=this->getParticipants().end(); lIt++) {
    const ActionableObject& lChild = (*lIt)->getActionable();
    const ActionableStatusGuard& lChildGuard = *lGuardMap.at(&lChild);
    const ActionableStatus& lChildStatus = mStatusMap.getStatus(lChild);
    if (lChildStatus.isEnabled(lChildGuard) && aGateKeeper.isEnabled(lChild.getPath()) && lChildStatus.isEngaged(lChildGuard))
      XCEPT_RAISE(ResourceInWrongStateMachine,"Cannot engage other state machine; resource '"+lChild.getPath()+"' currently in state machine '"+lChildStatus.getStateMachineId(lChildGuard)+"'");
  }

  // If not thrown already, then all is good: enable/disable children, engage system in this state machine
  // ... put enabled children in appropriate state machine, and reset their maskables
  LOG4CPLUS_INFO(mResource.getLogger(), "Engaging state machine '" << getId() << "'; entering state '" << getInitialState() << "'");
  lSysStatus.setStateMachine(getId(), getInitialState(), lSysGuard);

  disableChildren(aGateKeeper, lGuardMap);

  for (auto lIt=mNonConstChildFSMs.begin(); lIt != mNonConstChildFSMs.end(); lIt++) {
    ActionableObject& lChild = (*lIt)->getActionable();
    const ActionableStatusGuard& lChildGuard = *lGuardMap.at(&lChild);
    ActionableStatus& lChildStatus = mStatusMap.getStatus(lChild);

    if (lChildStatus.isEnabled(lChildGuard)) {
      (*lIt)->engage(aGateKeeper, lChildGuard);
      lChild.setMonitoringStatus(core::monitoring::kEnabled);
    }
    else
      lChild.setMonitoringStatus(core::monitoring::kNonCritical);
  }

  // Reset monitoring settings on children, and apply settings from gatekeeper
  resetMonitoringSettings();
  MonitoringSettings_t lMonSettings;
  extractMonitoringSettings(aGateKeeper, getInitialState(), lMonSettings);
  applyMonitoringSettings(lMonSettings);
}


//------------------------------------------------------------------------------------
void SystemStateMachine::disengage()
{
  ActionableSystem::GuardMap_t lGuardMap = mStatusMap.lockMutexes();
  const ActionableStatusGuard& lSysGuard = *lGuardMap.at(&getActionable());

  // Throw if system/children are not in this state machine or running transition
  checkEngagedAndNotInTransition(lSysGuard, "disengage");
  for (auto smIt = mNonConstChildFSMs.begin(); smIt!=mNonConstChildFSMs.end(); smIt++) {
    const ActionableStatusGuard& lChildGuard = *lGuardMap.at(&(*smIt)->getActionable());
    if (mStatusMap.getStatus((*smIt)->getActionable()).isEnabled(lChildGuard))
      checkChildEngagedAndNotInTransition(**smIt, lChildGuard, "disengage");
  }

  // If haven't thrown so far, then disengage the state machines
  LOG4CPLUS_INFO(mResource.getLogger(), "Disengaging from state machine '" << getId() << "'");
  mStatusMap.getSystemStatus().setNoStateMachine(lSysGuard);

  for (auto smIt = mNonConstChildFSMs.begin(); smIt!=mNonConstChildFSMs.end(); smIt++) {
    ActionableObject& lChild = (*smIt)->getActionable();
    const ActionableStatusGuard& lChildGuard = *lGuardMap.at(&lChild);
    ActionableStatus& lChildStatus = mStatusMap.getStatus((*smIt)->getActionable());

    if ( lChildStatus.getStateMachineId(lChildGuard) == (*smIt)->getId() ) {
      LOG4CPLUS_INFO(lChild.getLogger(), "Disengaging from state machine '" << (*smIt)->getId() << "'");
      lChildStatus.setNoStateMachine(lChildGuard);
    }
  }
}


//------------------------------------------------------------------------------------
void SystemStateMachine::checkEngagedAndNotInTransition(const ActionableStatusGuard& aGuard, const std::string& aAction) const
{
  const ActionableStatus& lStatus = mStatusMap.getSystemStatus();

  // Throw if system is not in this state machine
  if ( lStatus.getStateMachineId(aGuard) != getId() ) {
    std::ostringstream oss;
    oss << "Cannot " << aAction << " state machine '" << getId() << "' of '" << getActionable().getPath() << "'; ";
    if ( ! lStatus.isEngaged(aGuard) )
      oss << "NOT in any state machine.";
    else
      oss << "currently in state machine '" << lStatus.getStateMachineId(aGuard) << "'";
    XCEPT_RAISE(ResourceInWrongStateMachine,oss.str());
  }

  // Throw if system is currently running a transition
  if (const SystemTransition* t = lStatus.getFirstRunningActionOfType<SystemTransition>(aGuard))
    XCEPT_RAISE(ActionableSystemIsBusy,"Cannot "+aAction+" state machine '"+getId()+"'; resource '"+getActionable().getPath()+"' is busy in transition '"+t->getId()+"'");
}


//------------------------------------------------------------------------------------
void SystemStateMachine::checkChildEngagedAndNotInTransition(const StateMachine& aStateMachine, const ActionableStatusGuard& aGuard, const std::string& aAction) const
{
  const ActionableObject& lChild = aStateMachine.getActionable();
  ActionableStatus& lStatus = mStatusMap.getStatus(lChild);

  // Throw if child is in wrong state machine
  if (lStatus.getStateMachineId(aGuard) != aStateMachine.getId()) {
    std::ostringstream oss;
    oss << "Cannot " << aAction << " state machine '" << aStateMachine.getId() << "' of '" << lChild.getPath() << "'; ";
    if ( ! lStatus.isEngaged(aGuard) )
      oss << "NOT in any state machine.";
    else
      oss << "currently in state machine '" << lStatus.getStateMachineId(aGuard) << "'";
    XCEPT_RAISE(ResourceInWrongStateMachine,oss.str());
  }
  // Throw if child currently in a transition
  else if (const Transition* t = lStatus.getFirstRunningActionOfType<Transition>(aGuard))
    XCEPT_RAISE(ActionableObjectIsBusy,"Cannot "+aAction+" state machine '"+getId()+"'; child resource '"+lChild.getPath()+"' is busy in transition '"+t->getId()+"'");
}


//------------------------------------------------------------------------------------
void SystemStateMachine::disableChildren(const GateKeeper& aGateKeeper, const ActionableSystem::GuardMap_t& aGuardMap)
{
  for (auto lIt=mResource.getActionableChildren().begin(); lIt != mResource.getActionableChildren().end(); lIt++) {
    ActionableObject& lChild = *(lIt->second);
    const ActionableStatusGuard& lGuard = *aGuardMap.at(&lChild);
    ActionableStatus& lStatus = mStatusMap.getStatus(lChild);

    if ( ! aGateKeeper.isEnabled(lChild.getPath())) {
      LOG4CPLUS_INFO(lChild.getLogger(), "Disabling");
      lStatus.disable(lGuard);
    }
  }
}


SystemStateMachine::State::State(const std::string& aId) :
  Object(aId)
{
}


void SystemStateMachine::State::addTransition(SystemTransition* aTransition)
{
  addObj(aTransition);
  transitionMap[ aTransition->getId() ] = aTransition;
}


const SystemStateMachine::State& SystemStateMachine::getState(const std::string& aStateId) const
{
  std::map<std::string, State*>::const_iterator lIt = mStateMap.find(aStateId);
  if (lIt != mStateMap.end())
    return *lIt->second;
  else
    XCEPT_RAISE(StateNotDefined,"State '" + aStateId + "' does not exist in system FSM '"+getPath()+"''");
}


SystemStateMachine::State& SystemStateMachine::getState(const std::string& aStateId)
{
  std::map<std::string, State*>::const_iterator lIt = mStateMap.find(aStateId);
  if (lIt != mStateMap.end())
    return *lIt->second;
  else
    XCEPT_RAISE(StateNotDefined,"State '" + aStateId + "' does not exist in system FSM '"+getPath()+"''");
}


void SystemStateMachine::resetMonitoringSettings()
{
  std::vector<std::string> lChildIds = getActionable().getChildren();
  
  for(auto lChildIt = lChildIds.begin(); lChildIt != lChildIds.end(); lChildIt++) {
    if (getActionable().getActionableChildren().count(*lChildIt) != 0)
      continue;

    Object* lChild = getActionable().getObjPtr<Object>(*lChildIt);
    if (core::AbstractMetric* lMetric = dynamic_cast<core::AbstractMetric*>(lChild))
      lMetric->setMonitoringStatus(core::monitoring::kEnabled);
    else if (dynamic_cast<core::MonitorableObject*>(lChild) != NULL) {
      for (auto lObjIt = lChild->begin(); lObjIt != lChild->end(); lObjIt++) {
        if (core::MonitorableObject* lMonObj = dynamic_cast<core::MonitorableObject*>(&*lObjIt))
          lMonObj->setMonitoringStatus(core::monitoring::kEnabled);
        else if (core::AbstractMetric* lMetric = dynamic_cast<core::AbstractMetric*>(&*lObjIt))
          lMetric->setMonitoringStatus(core::monitoring::kEnabled);
      }
    }
  }
}


void SystemStateMachine::extractMonitoringSettings(const GateKeeper& aGateKeeper, const std::string& aState, MonitoringSettings_t& aMonSettings) const
{
  aMonSettings.clear();
  const std::vector<std::string> lContextsToLookIn = {getActionable().getId()};

  std::vector<std::string> lChildIds = getActionable().getChildren();
  
  for(auto lChildIt = lChildIds.begin(); lChildIt != lChildIds.end(); lChildIt++) {
    if (const_cast<ActionableSystem&>(getActionable()).getActionableChildren().count(*lChildIt) != 0)
      continue;

    if (core::AbstractMetric* lMetric = const_cast<ActionableSystem&>(getActionable()).getObjPtr<core::AbstractMetric>(*lChildIt)) {
      const GateKeeper::MonitoringSetting_t lMonSetting = aGateKeeper.getMonitoringSetting(aState, lMetric->getId(),
         lContextsToLookIn);
      if (lMonSetting)
        aMonSettings.push_back(*lMonSetting);
    }
      
    if (core::MonitorableObject* lMonObj = const_cast<ActionableSystem&>(getActionable()).getObjPtr<core::MonitorableObject>(*lChildIt)) {
      for (auto lObjIt = lMonObj->begin(); lObjIt != lMonObj->end(); lObjIt++) {
        if ((dynamic_cast<core::MonitorableObject*>(&*lObjIt) != NULL) || (dynamic_cast<core::AbstractMetric*>(&*lObjIt))) {
          std::string lRelPath = lObjIt->getPath().substr(getActionable().getPath().size());
          const GateKeeper::MonitoringSetting_t lMonSetting = aGateKeeper.getMonitoringSetting(aState, lRelPath,
             lContextsToLookIn);
          if (lMonSetting)
            aMonSettings.push_back(*lMonSetting);
        }
      }
    }
  }
}


void SystemStateMachine::applyMonitoringSettings(const MonitoringSettings_t& aSettings)
{
  for (auto lIt=aSettings.begin(); lIt != aSettings.end(); lIt++) {
    if (core::MonitorableObject* lMonObj = getActionable().getObjPtr<core::MonitorableObject>(lIt->getId()))
      lMonObj->setMonitoringStatus(lIt->getStatus());
    else if (core::AbstractMetric* lMetric = getActionable().getObjPtr<core::AbstractMetric>(lIt->getId()))
      lMetric->setMonitoringStatus(lIt->getStatus());
  }
}


//------------------------------------------------------------------------------------


SystemTransitionSnapshot::SystemTransitionSnapshot(const IdAliasPair& aAction, const IdAliasPair& aActionable, State aState, float aRunningTime, const SystemTransition::Step* aCurrentStep, const StepVec_t& aFinishedStepStatuses, size_t aTotalNumSteps, const std::set<std::string>& aEnabledChildren, const std::string& aMessage) :
  ActionSnapshot(aAction, aActionable, aState, aRunningTime),
  mTotalNumSteps( aTotalNumSteps ),
  mNumCompletedSteps( aFinishedStepStatuses.size() ),
  mEnabledChildren(aEnabledChildren),
  mStepStatuses( aFinishedStepStatuses ),
  mMessage(aMessage)
{
  if (aCurrentStep != NULL) {
    mStepStatuses.push_back( std::vector< boost::shared_ptr<const TransitionSnapshot> > ());
    for (std::vector<Transition*>::const_iterator lIt=aCurrentStep->cget().begin(); lIt!=aCurrentStep->cget().end(); lIt++) {
      boost::shared_ptr<const TransitionSnapshot> childStatus;
      // Only set child snapshot pointer to non-NULL value if that child is enabled
      if ( mEnabledChildren.count((*lIt)->getActionable().getPath()) > 0)
        childStatus.reset(new TransitionSnapshot((*lIt)->getStatus()));
      mStepStatuses.back().push_back( childStatus );
    }
  }
}


float SystemTransitionSnapshot::getProgress() const
{
  if (mTotalNumSteps == 0)
    return (getState() == State::kDone) ? 1.0: 0.0;
  else if (mStepStatuses.empty())
    return 0.0;
  else if ((getState() == State::kError) || (getState() == State::kRunning)) {
    float baseProgress = float(mStepStatuses.size() - 1)/float(mTotalNumSteps);

    // Determine progress of slowest parallel transition in last step
    typedef std::vector<boost::shared_ptr<const TransitionSnapshot> > StepStatus_t;
    const StepStatus_t& lStatusLastStep = mStepStatuses.back();
    std::vector<float> lLastStepProgressVec;
    for (StepStatus_t::const_iterator lIt=lStatusLastStep.begin(); lIt!=lStatusLastStep.end(); lIt++) {
      // Child snapshot pointer will be NULL if that child wasn't enabled
      if (*lIt != NULL)
        lLastStepProgressVec.push_back((*lIt)->getProgress());
    }
    float lastStepProgress = (*std::min_element(lLastStepProgressVec.begin(), lLastStepProgressVec.end()))/float(mTotalNumSteps);
    return baseProgress + lastStepProgress;
  }
  else
    return 1.0;
}


size_t SystemTransitionSnapshot::getNumberOfCompletedSteps() const
{
  return mNumCompletedSteps;
}


size_t SystemTransitionSnapshot::getTotalNumberOfSteps() const
{
  return mTotalNumSteps;
}


const std::set<std::string>& SystemTransitionSnapshot::getEnabledChildren() const
{
  return mEnabledChildren;
}

SystemTransitionSnapshot::const_iterator SystemTransitionSnapshot::begin() const
{
  return mStepStatuses.begin();
}

SystemTransitionSnapshot::const_iterator SystemTransitionSnapshot::end() const
{
  return mStepStatuses.end();
}


const SystemTransitionSnapshot::Step_t& SystemTransitionSnapshot::at(size_t aIndex) const
{
  return mStepStatuses.at(aIndex);
}


size_t SystemTransitionSnapshot::size() const
{
  return mStepStatuses.size();
}


const std::string& SystemTransitionSnapshot::getMessage() const
{
  return mMessage;
}


}
}


#include "swatch/action/StateMachine.hpp"


// Standard headers
#include <stddef.h>                     // for NULL
#include <ostream>                      // for operator<<, basic_ostream, etc
#include <utility>                      // for pair

// boost headers
#include "boost/bind.hpp"

// log4cplus headers
#include <log4cplus/loggingmacros.h>

// SWATCH headers
#include "swatch/core/AbstractMetric.hpp"
#include "swatch/action/ActionableStatus.hpp"
#include "swatch/action/ActionableObject.hpp"
#include "swatch/action/BusyGuard.hpp"
#include "swatch/action/CommandSequence.hpp"
#include "swatch/action/Functionoid.hpp"
#include "swatch/action/GateKeeper.hpp"
#include "swatch/core/LeafObject.hpp"
#include "swatch/core/ReadOnlyXParameterSet.hpp"
#include "swatch/core/MonitorableObject.hpp"




namespace swatch {
namespace action {

//------------------------------------------------------------------------------------
StateMachine::StateMachine(const std::string& aId, const std::string& aAlias, ActionableObject& aResource, ActionableStatus& aStatus, const std::string& aInitialState, const std::string& aErrorState) :
  Object(aId, aAlias),
  mResource(aResource),
  mStatus(aStatus),
  mInitialState(aInitialState),
  mErrorState(aErrorState)
{
  addState(mInitialState);
  addState(mErrorState);
}


//------------------------------------------------------------------------------------
StateMachine::~StateMachine()
{
}


//------------------------------------------------------------------------------------
const ActionableObject& StateMachine::getActionable() const
{
  return mResource;
}


//------------------------------------------------------------------------------------
ActionableObject& StateMachine::getActionable()
{
  return mResource;
}


//------------------------------------------------------------------------------------
const std::string& StateMachine::getInitialState() const
{
  return mInitialState;
}


//------------------------------------------------------------------------------------
const std::string& StateMachine::getErrorState() const
{
  return mErrorState;
}


//------------------------------------------------------------------------------------
const std::vector<std::string>& StateMachine::getStates() const
{
  return mStates;
}


//------------------------------------------------------------------------------------
const std::map<std::string, Transition*>& StateMachine::getTransitions(const std::string& aStateId) const
{
  return getState(aStateId).transitionMap;
}


//------------------------------------------------------------------------------------
Transition& StateMachine::getTransition(const std::string& aStateId, const std::string& aTransition)
{
  const std::map<std::string, Transition*>& lTransitions = getTransitions(aStateId);

  if ( lTransitions.count(aTransition) == 0 )
    XCEPT_RAISE(TransitionNotDefined,"Transition '"+aTransition+"' from state '"+aStateId+"' does not exist");
  else {
    return *(lTransitions.find(aTransition)->second);
  }
}


//------------------------------------------------------------------------------------
void StateMachine::addState(const std::string& aStateId)
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
Transition& StateMachine::addTransition(const std::string& aTransitionId, const std::string& aFromState, const std::string& aToState)
{
  return addTransition(aTransitionId, "", aFromState, aToState);
}


//------------------------------------------------------------------------------------
Transition& StateMachine::addTransition(const std::string& aTransitionId, const std::string& aAlias, const std::string& aFromState, const std::string& aToState)
{
  State& lFromState = getState(aFromState);

  if (mStateMap.count(aToState) == 0 )
    XCEPT_RAISE(StateNotDefined,"State '"+aToState+"' does not exist in state machine '"+getPath()+"'");
  else if (lFromState.transitionMap.count(aTransitionId))
    XCEPT_RAISE(TransitionAlreadyDefined,"Transition '"+aTransitionId+"' from state '"+aFromState+"' already defined in state machine '"+getPath()+"'");

  Transition* t = new Transition(aTransitionId, aAlias, *this, mStatus, aFromState, aToState);
  lFromState.addTransition(t);
  return *t;
}


//------------------------------------------------------------------------------------
void StateMachine::engage(const GateKeeper& aGateKeeper)
{
  ActionableStatusGuard lGuard(mStatus);
  engage(aGateKeeper, lGuard);
}


//------------------------------------------------------------------------------------
void StateMachine::engage(const GateKeeper& aGateKeeper, const ActionableStatusGuard& aGuard)
{
  // Throw if currently in other state machine
  if (mStatus.getStateMachineId(aGuard) != ActionableSnapshot::kNullStateMachineId )
    XCEPT_RAISE(ResourceInWrongStateMachine,"Cannot engage other state machine; resource '"+getPath()+"' currently in state machine '"+mStatus.getStateMachineId(aGuard)+"'");

  LOG4CPLUS_INFO(mResource.getLogger(), "Engaging state machine '" << getId() << "'; entering state '" << getInitialState() << "'");
  mStatus.setStateMachine(getId(), getInitialState(), aGuard);

  // Reset maskable objects (unmasked unless specified otherwise in gatekeeper)
  mResource.resetAndApplyMasks(aGateKeeper, aGuard);

  // Reset monitoring settings on children, and apply settings from gatekeeper
  resetMonitoringSettings();
  MonitoringSettings_t lMonSettings;
  extractMonitoringSettings(aGateKeeper, getInitialState(), lMonSettings);
  applyMonitoringSettings(lMonSettings);
}


//------------------------------------------------------------------------------------
void StateMachine::disengage()
{
  ActionableStatusGuard lGuard(mStatus);

  // Throw if currently in other state machine
  if (mStatus.getStateMachineId(lGuard) != this->getId()) {
    std::ostringstream oss;
    oss << "Cannot reset resource '" << mResource.getPath() << "' state machine '" << getId() << "'; ";
    if ( mStatus.isEngaged(lGuard) )
      oss << "currently in state machine '" << mStatus.getStateMachineId(lGuard) << "'";
    else
      oss << "NOT in any state machine";
    XCEPT_RAISE(ResourceInWrongStateMachine,oss.str());
  }

  // Throw if running action
  if ( mStatus.isBusy(lGuard) )
    XCEPT_RAISE(ActionableObjectIsBusy,"Cannot reset '"+mResource.getPath()+"', state machine '"+getId()+"'; busy running action '"+mStatus.getLastRunningAction(lGuard)->getPath()+"'");

  LOG4CPLUS_INFO(mResource.getLogger(), "Disengaging from state machine '" << getId() << "'");
  mStatus.setNoStateMachine(lGuard);
}


//------------------------------------------------------------------------------------
void StateMachine::reset(const GateKeeper& aGateKeeper)
{
  ActionableStatusGuard lGuard(mStatus);
  reset(aGateKeeper, lGuard);
}


//------------------------------------------------------------------------------------
void StateMachine::reset(const GateKeeper& aGateKeeper, const ActionableStatusGuard& aGuard)
{
  // Throw if currently in other state machine
  if ( mStatus.getStateMachineId(aGuard) != this->getId() ) {
    std::ostringstream oss;
    oss << "Cannot reset '" << mResource.getPath() << "', state machine '" << getId() << "'; ";
    if ( mStatus.isEngaged(aGuard) )
      oss << "currently in state machine '" << mStatus.getStateMachineId(aGuard) << "'";
    else
      oss << "NOT in any state machine";
    XCEPT_RAISE(ResourceInWrongStateMachine,oss.str());
  }

  // Throw if running action
  if ( mStatus.isBusy(aGuard) )
    XCEPT_RAISE(ActionableObjectIsBusy,"Cannot reset '"+mResource.getPath()+"', state machine '"+getId()+"'; busy running action '"+mStatus.getLastRunningAction(aGuard)->getPath()+"'");

  LOG4CPLUS_INFO(mResource.getLogger(), "Resetting state machine '" << getId() << "'; entering state '" << getInitialState() << "'");
  mStatus.setState(getInitialState(), aGuard);

  // Reset maskable objects (unmasked unless specified otherwise in gatekeeper)
  mResource.resetAndApplyMasks(aGateKeeper, aGuard);

  // Reset monitoring settings on children, and apply settings from gatekeeper
  resetMonitoringSettings();
  MonitoringSettings_t lMonSettings;
  extractMonitoringSettings(aGateKeeper, getInitialState(), lMonSettings);
  applyMonitoringSettings(lMonSettings);
}


//------------------------------------------------------------------------------------
Transition::Transition(const std::string& aId, const std::string& aAlias, StateMachine& aOp, ActionableStatus& aActionableStatus, const std::string& aStartState, const std::string& aEndState) :
  CommandVec(aId, aAlias, aOp.getActionable()),
  mStateMachine(aOp),
  mActionableStatus(aActionableStatus),
  mStartState(aStartState),
  mEndState(aEndState)
{
}


const std::string& Transition::getStartState() const
{
  return mStartState;
}


const std::string& Transition::getEndState() const
{
  return mEndState;
}


const StateMachine& Transition::getStateMachine() const
{
  return mStateMachine;
}


StateMachine& Transition::getStateMachine()
{
  return mStateMachine;
}


//------------------------------------------------------------------------------------
Transition& Transition::add(Command& aCmd, const std::string& aNamespace)
{
  addCommand(aCmd, aNamespace);
  return *this;
}


//------------------------------------------------------------------------------------
Transition& Transition::add(CommandSequence& aSequence)
{
  CommandVec::const_iterator lIt = aSequence.begin();
  std::vector<Command*> lCmds = aSequence.getCommands();
  for (auto lIt2=lCmds.begin(); lIt2!=lCmds.end(); lIt++, lIt2++) {
    addCommand(*(*lIt2), lIt->getNamespace());
  }
  return *this;
}


//------------------------------------------------------------------------------------
void Transition::exec(const GateKeeper& aGateKeeper, const bool& aUseThreadPool )
{
  exec(NULL, aGateKeeper, aUseThreadPool);
}


//------------------------------------------------------------------------------------
void Transition::exec(const BusyGuard* aOuterBusyGuard, const GateKeeper& aGateKeeper, const bool& aUseThreadPool )
{
  // 1) Extract parameters before creating busy guard (so that resource doesn't change states if parameter is missing, or parameter rule/constraint is violated)
  std::vector<core::ReadOnlyXParameterSet> lParamSets;
  std::vector<MissingParam> lMissingParams;
  MonitoringSettings_t lMonSettings;
  extractParameters(aGateKeeper, lParamSets, lMissingParams, true);

  std::vector<ParamRuleViolationList> lRuleViolations;
  checkForInvalidParameters(lParamSets, lRuleViolations);
  if (!lRuleViolations.empty()) {
    std::ostringstream lMsgStream;
    lMsgStream << "Parameters [" << core::join(lRuleViolations.at(0).violations.at(0).parameters, ", ") << "] for command '";
    lMsgStream << lRuleViolations.at(0).command << "' (index " <<  lRuleViolations.at(0).commandIdx; 
    lMsgStream << ") failed rule/constraint '" << lRuleViolations.at(0).violations.at(0).ruleDescription << "': ";
    lMsgStream << lRuleViolations.at(0).violations.at(0).details;
    XCEPT_RAISE(CommandParameterCheckFailed, lMsgStream.str());
  }

  mStateMachine.extractMonitoringSettings(aGateKeeper, getEndState(), lMonSettings);

  // 2) Check current state; if in correct state, then create busy guard and continue
  boost::shared_ptr<BusyGuard> lBusyGuard;
  {
    ActionableStatusGuard lGuard(mActionableStatus);
    if ( mActionableStatus.getStateMachineId(lGuard) != getStateMachine().getId())
      XCEPT_RAISE(ResourceInWrongState,"Resource '"+getActionable().getPath()+"' is not yet engaged in state machine '"+getStateMachine().getId()+"'");
    else if ( mActionableStatus.getState(lGuard) != getStartState() )
      XCEPT_RAISE(ResourceInWrongState,"Resource '"+getActionable().getPath()+"' is in state '"+mActionableStatus.getState(lGuard)+"'; transition '"+getId()+"' cannot be run");

    BusyGuard::Callback_t lCallback = boost::bind(&Transition::changeState, this, _1, _2);
    lBusyGuard.reset(new BusyGuard(*this, mActionableStatus, lGuard, lCallback, aOuterBusyGuard));
  }

// FIXME: Re-implement parameter cache at some future date; disabled by Tom on 28th August, since ...
//        ... current logic doesn't work correctly with different gatekeepers - need to change to ...
//        ... updating cache if either gatekeeper filename/runkey different or cache update timestamp older than gatekeeper.lastUpdated
//  // Is our cache of parameters up to date?
//  boost::posix_time::ptime lUpdateTime( aGateKeeper.lastUpdated() );
//  if( mParamUpdateTime != lUpdateTime )
//  {
//    updateParameterCache(aGateKeeper);
//    mParamUpdateTime = lUpdateTime; // We are up to date :)
//  }

  // 3) Reset this sequence's state variables
  reset(lParamSets);
  mCachedMonitoringSettings = lMonSettings;

  // 4) Run the commands
  if (aUseThreadPool) {
    scheduleAction<Transition>(this, &Transition::run, lBusyGuard);
  }
  else {
    // otherwise execute in same thread
    run(lBusyGuard);
  }
}


//------------------------------------------------------------------------------------
void Transition::run(boost::shared_ptr<BusyGuard> aGuard)
{
  // 1) Run the commands [as is done in a command sequence]
  CommandVec::runCommands(aGuard);

  // 2) Apply monitoring settings
  mStateMachine.applyMonitoringSettings(mCachedMonitoringSettings);

  // 3) The actionable object's state will change when BusyGuard gets destroyed
}


//------------------------------------------------------------------------------------
void Transition::changeState(const ActionableStatusGuard& aGuard, std::ostream& aLogMessageSuffix)
{
  State lActionState = getState();
  std::string lNewState = getEndState();
  if ((lActionState != kDone) && (lActionState != kWarning))
    lNewState = getStateMachine().getErrorState();

  mActionableStatus.setState(lNewState, aGuard);
  aLogMessageSuffix << "Entering state '" << lNewState << "'";
}


//------------------------------------------------------------------------------------
StateMachine::State::State(const std::string& aId) :
  Object(aId)
{
}


//------------------------------------------------------------------------------------
void StateMachine::State::addTransition(Transition* aTransition)
{
  addObj(aTransition);
  transitionMap[ aTransition->getId() ] = aTransition;
}


//------------------------------------------------------------------------------------
const StateMachine::State& StateMachine::getState(const std::string& aStateId) const
{
  std::map<std::string, State*>::const_iterator lIt = mStateMap.find(aStateId);
  if (lIt != mStateMap.end())
    return *lIt->second;
  else
    XCEPT_RAISE(StateNotDefined,"State '" + aStateId + "' does not exist in state machine '"+getPath()+"''");
}


//------------------------------------------------------------------------------------
StateMachine::State& StateMachine::getState(const std::string& aStateId)
{
  std::map<std::string, State*>::const_iterator lIt = mStateMap.find(aStateId);
  if (lIt != mStateMap.end())
    return *lIt->second;
  else
    XCEPT_RAISE(StateNotDefined,"State '" + aStateId + "' does not exist in system FSM '"+getPath()+"''");
}


//------------------------------------------------------------------------------------
void StateMachine::resetMonitoringSettings()
{
  for (Object::iterator lIt=getActionable().begin(); lIt != getActionable().end(); lIt++) {
    if (core::MonitorableObject* lMonObj = dynamic_cast<core::MonitorableObject*>(&*lIt)) {
      lMonObj->setMonitoringStatus(core::monitoring::kEnabled);
      std::vector<std::string> lMetricIds = lMonObj->getMetrics();
      for (std::vector<std::string>::const_iterator lIdIt=lMetricIds.begin(); lIdIt!=lMetricIds.end(); lIdIt++) {
        core::AbstractMetric& lMetric = lMonObj->getMetric(*lIdIt);
        lMetric.setMonitoringStatus(core::monitoring::kEnabled);
      }
    }
  }
}

//------------------------------------------------------------------------------------
void StateMachine::extractMonitoringSettings(const GateKeeper& aGateKeeper, const std::string& aState, MonitoringSettings_t& aMonSettings) const
{
  aMonSettings.clear();

  // get a list of all MonitorableObjects
  const ActionableObject& lResource = getActionable();
  std::vector<std::string> lDescendants = lResource.getDescendants();
  for (std::vector<std::string>::const_iterator lIt = lDescendants.begin(); lIt != lDescendants.end(); lIt++) {
    const core::MonitorableObject* lDescendant = lResource.getObjPtr<core::MonitorableObject>(*lIt);

    if (lDescendant) {
      // query the GateKeeper for relevant settings for each object
      std::string lPath = *lIt;
      const std::vector<std::string>& lContextsToLookIn = lResource.getGateKeeperContexts();
      const GateKeeper::MonitoringSetting_t lMonSetting = aGateKeeper.getMonitoringSetting(aState, lPath,
          lContextsToLookIn);
      if (lMonSetting)
        aMonSettings.push_back(*lMonSetting);

      // get settings for child metrics
      const std::vector<std::string> lMetricIds = lDescendant->getMetrics();
      for (auto lMetricIt = lMetricIds.begin(); lMetricIt != lMetricIds.end(); lMetricIt++) {
        const std::string lMetricPath = lPath + "." + *lMetricIt;
        const GateKeeper::MonitoringSetting_t lMetricSetting = aGateKeeper.getMonitoringSetting(aState, lMetricPath,
            lContextsToLookIn);
        if (lMetricSetting)
          aMonSettings.push_back(*lMetricSetting);
      }
    }
  }
}


//------------------------------------------------------------------------------------
void StateMachine::applyMonitoringSettings(const MonitoringSettings_t& aSettings)
{
  // get a list of all MonitorableObjects
  ActionableObject& lResource = getActionable();
  std::vector<std::string> lDescendants = lResource.getDescendants();

  for (std::vector<std::string>::const_iterator lIt = lDescendants.begin(); lIt != lDescendants.end(); lIt++) {
    core::MonitorableObject* lDescendant = lResource.getObjPtr<core::MonitorableObject>(*lIt);

    if (lDescendant) {
      // query the GateKeeper for relevant settings for each object
      std::string lPath = *lIt;

      for (auto lMonSetting = aSettings.begin(); lMonSetting != aSettings.end(); ++lMonSetting) {
//        LOG(logger::kDebug) << lPath << " " << lMonSetting->getId();

        if (lMonSetting->getId() == lPath) {
          lDescendant->setMonitoringStatus(lMonSetting->getStatus());
        }

        // Apply settings to child metrics
        const std::vector<std::string> lMetricIds = lDescendant->getMetrics();

        for (auto lMetricIt = lMetricIds.begin(); lMetricIt != lMetricIds.end(); lMetricIt++) {

          if (lMonSetting->getId() == (lPath + "." + *lMetricIt))
            lDescendant->getMetric(*lMetricIt).setMonitoringStatus(lMonSetting->getStatus());
        }
      }
    }
  }
}


}  //ns: action
}  //ns: swatch

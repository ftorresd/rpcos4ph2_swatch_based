/*
 * File:   SystemStateMachine.hpp
 * Author: tom
 *
 * Created on 18 September 2015, 15:27
 */

#ifndef __SWATCH_ACTION_SYSTEMSTATEMACHINE_HPP__
#define __SWATCH_ACTION_SYSTEMSTATEMACHINE_HPP__


// Standard headers
#include <stddef.h>                     // for size_t
#include <iosfwd>                       // for ostream
#include <iterator>                     // for iterator_traits
#include <map>                          // for map
#include <set>                          // for set
#include <string>                       // for string
#include <vector>                       // for vector, vector<>::iterator, etc

// boost headers
#include "boost/date_time/posix_time/ptime.hpp"  // for ptime
#include "boost/smart_ptr/shared_ptr.hpp"  // for shared_ptr
#include "boost/static_assert.hpp"      // for BOOST_STATIC_ASSERT_MSG
#include "boost/thread/pthread/mutex.hpp"  // for mutex
#include "boost/type_traits/is_convertible.hpp"  // for is_convertible

#include "swatch/action/ActionableStatus.hpp"
#include "swatch/action/ActionableSystem.hpp"
#include "swatch/action/CommandVec.hpp"
#include "swatch/core/exception.hpp"
#include "swatch/action/SystemFunctionoid.hpp"
#include "swatch/action/GateKeeper.hpp"
#include "swatch/core/Object.hpp"
#include "swatch/action/StateMachine.hpp"


namespace swatch {
namespace action {

class ActionableObject;
class SystemBusyGuard;
class SystemStateMachine;
class SystemTransitionSnapshot;


//! Represents transition  of a class that inherits from ActionableSystem, for an FSM modeled by SystemStateMachine
class SystemTransition : public SystemFunctionoid {
public:

  //! Represents a single step in a system transition - i.e. multiple transitions on child objects, executed in parallel
  class Step {
  public:
    Step(const std::vector<Transition*>& aTransitions);

    const std::vector<Transition*>& get();

    //FIXME: Shouldn't give non-const pointers in const method!
    const std::vector<Transition*>& cget() const;

  private:
    std::vector<Transition*> mTransitions;
  };

  SystemTransition(const std::string& aId, const std::string& aAlias, SystemStateMachine& aFSM, ActionableSystem::StatusContainer& aStatusMap, const std::string& aStartState, const std::string& aEndState);
  virtual ~SystemTransition();

  // Iteration over all steps in the transition
  typedef std::vector<Step>::const_iterator const_iterator;
  typedef std::vector<Step>::iterator iterator;

  iterator begin();
  iterator end();
  const_iterator begin() const;
  const_iterator end() const;

  //! Number of steps
  size_t size() const;

  //! Returns status of transition
  SystemTransitionSnapshot getStatus() const;

  //! State that this transition starts from
  const std::string& getStartState() const;

  //! State that this transition goes to (in case no error occurs)
  const std::string& getEndState() const;

  //! Returns FSM that this transition belongs to
  const SystemStateMachine& getStateMachine() const;

  //! Returns FSM that this transition belongs to
  SystemStateMachine& getStateMachine();

  /*!
   * @brief Add step consisting of transitions of children in range aBegin to aEnd, from same state for each child, with same ID for each child transition; uses this transition's FSM ID string for children as well
   * @tparam Iterator iterator over child objects that derive from ActionableObject ; must defererence to T*, where T is a class that inherits from ActionableObject
   * @param aBegin iterator to beginning of collection of child pointers
   * @param aEnd iterator to one-past-end of collection of child pointers
   * @param aFromState State that child transitions start from
   * @param aTransition ID of child transition
   */
  template<class Iterator>
  SystemTransition& add(Iterator aBegin, Iterator aEnd, const std::string& aFromState, const std::string& aTransition);

  /*!
   * @brief Add step consisting of transitions of children in range aBegin to aEnd, from same state for each child, with same ID for each child transition
   * @tparam Iterator iterator over child objects that derive from ActionableObject ; must defererence to T*, where T is a class that inherits from ActionableObject
   * @param aBegin iterator to beginning of collection of child pointers
   * @param aEnd iterator to one-past-end of collection of child pointers
   * @param aStateMachine ID of FSM for child transitions
   * @param aFromState State that child transitions start from
   * @param aTransition ID of child transition
   */
  template<class Iterator>
  SystemTransition& add(Iterator aBegin, Iterator aEnd, const std::string& aStateMachine, const std::string& aFromState, const std::string& aTransition);

  /*!
   * @brief Add step consisting of transitions of children, from same state for each child, with same ID for each child transition; uses this transition's FSM ID string for children as well
   * @tparam Collection collection of child objects that derive from ActionableObject ; iterators returned by begin() and end() methods must defererence to T*, where T is a class that inherits from ActionableObject
   * @param aCollection collection of child objects involved in this step
   * @param aFromState State that child transitions start from
   * @param aTransition ID of child transition
   */
  template<class Collection>
  SystemTransition& add(const Collection& aCollection, const std::string& aFromState, const std::string& aTransition);

  /*!
   * @brief Add step consisting of transitions of children, from same state for each child, with same ID for each child transition
   * @tparam Collection collection of child objects that derive from ActionableObject ; iterators returned by begin() and end() methods must defererence to T*, where T is a class that inherits from ActionableObject
   * @param aCollection collection of child objects involved in this step
   * @param aStateMachine ID of FSM for child transitions
   * @param aFromState State that child transitions start from
   * @param aTransition ID of child transition
   */
  template<class Collection>
  SystemTransition& add(const Collection& aCollection, const std::string& aStateMachine, const std::string& aFromState, const std::string& aTransition);

  /*!
   * @brief Add step consisting of transitions of children
   * @param aTransitions pointers to transitions on child objects
   */
  SystemTransition& add(const std::vector<Transition*>& aTransitions);

  void checkForMissingParameters(const GateKeeper& aGateKeeper, std::map< const Transition*, std::vector<CommandVec::MissingParam> >& aMissingParams) const;

  void checkForMissingParameters(const GateKeeper& aGateKeeper, std::map< const Transition*, std::vector<CommandVec::MissingParam> >& aMissingParams, const ActionableSystem::GuardMap_t& aGuardMap) const;

  void checkForInvalidParameters(const GateKeeper& aGateKeeper, std::map< const Transition*, std::vector<CommandVec::ParamRuleViolationList> >& aRuleViolation) const;
  void checkForInvalidParameters(const GateKeeper& aGateKeeper, std::map< const Transition*, std::vector<CommandVec::ParamRuleViolationList> >& aRuleViolation, const ActionableSystem::GuardMap_t& aGuardMap) const;

  typedef boost::function<std::string (const SystemTransitionSnapshot&)> SnapshotAnalyser_t;

  //! Registers a function that will be called at the end of this transition if an error occurs in one of the underlying commands
  void registerErrorAnalyser(const SnapshotAnalyser_t& aFunction);

  //! Registers a function that will be called at the end of this transition if an warning occurs in one of the underlying commands
  void registerWarningAnalyser(const SnapshotAnalyser_t& aFunction);

  /*!
   * @brief Run the transition, extracting the parameters for each child transition from the supplied gatekeeper
   *
   * @param aGateKeeper Gatekeeper that will be used to extract the
   * @param aUseThreadPool Run the transition asynchronously in the swatch::action::ThreadPool ; if equals false, then the sequence is run synchronously (i.e. in same thread)
   */
  void exec(const GateKeeper& aGateKeeper, const bool& aUseThreadPool = true );

private:
  //! Returns status of transition
  SystemTransitionSnapshot getStatus(const boost::unique_lock<boost::mutex>& aLockGuard) const;

  void runSteps(boost::shared_ptr<SystemBusyGuard> aGuard);

  void changeState(const ActionableStatusGuard& aGuard, std::ostream& aLogMessageSuffix);

  SystemStateMachine& mFSM;
  ActionableSystem::StatusContainer& mStatusMap;
  const std::string mStartState;
  const std::string mEndState;
  std::vector<Step> mSteps;

  SnapshotAnalyser_t mWarningAnalyser;
  SnapshotAnalyser_t mErrorAnalyser;

  mutable boost::mutex mMutex;
  const GateKeeper* mGateKeeper;
  State mState;
  std::vector<Step>::iterator mStepIt;
  boost::posix_time::ptime mExecStartTime;
  boost::posix_time::ptime mExecEndTime;
  std::set<const ActionableObject*> mEnabledChildren;
  std::vector< std::vector<boost::shared_ptr<const TransitionSnapshot> > > mStatusOfCompletedSteps;
  std::string mMessage;
  MonitoringSettings_t mCachedMonitoringSettings;
};


class SystemTransitionSnapshot : public ActionSnapshot {
public:
  typedef boost::shared_ptr<const TransitionSnapshot> ChildSnapshotRef_t;
  typedef std::vector<ChildSnapshotRef_t> Step_t;
  typedef std::vector<Step_t> StepVec_t;
  typedef StepVec_t::const_iterator const_iterator;

  SystemTransitionSnapshot(const IdAliasPair& aAction, const IdAliasPair& aActionable, State aState, float aRunningTime, const SystemTransition::Step* aCurrentStep, const StepVec_t& aFinishedStepStatuses, size_t aTotalNumSteps, const std::set<std::string>& aEnabledChildren, const std::string& aMessage);

  //! Returns fraction progress of transition - range [0,1] inclusive
  float getProgress() const;

  //! Returns count of steps that have already completed execution (regardless of whether their final state is kDone, kWarning, or kError)
  size_t getNumberOfCompletedSteps() const;

  //! Number of steps in the transition
  size_t getTotalNumberOfSteps() const;

  //! Returns ID paths of children that took part in this transition (i.e. only enabled children)
  const std::set<std::string>& getEnabledChildren() const;

  //! Returns iterator to snapshot of first step in transition (if it has started execution)
  const_iterator begin() const;

  //! Returns iterator 'one beyond' the snapshot of the last step that has started/completed execution
  const_iterator end() const;

  //! Returns snapshot of i'th step
  const Step_t& at(size_t aIndex) const;

  //! Returns number of steps that have started/completed execution as part of this transition (i.e. equal to this->end() - this->begin())
  size_t size() const;

  //! Returns final status message obtained from running system-level transition's registered error/warning analyser callbacks
  const std::string& getMessage() const;

private:
  size_t mTotalNumSteps;
  size_t mNumCompletedSteps;
  std::set<std::string> mEnabledChildren;
  StepVec_t mStepStatuses;
  std::string mMessage;
};


class SystemStateMachine : public core::Object {
public:
  SystemStateMachine(const std::string& aId, ActionableSystem& aSystem, ActionableSystem::StatusContainer& aStatusMap, const std::string& aInitialState, const std::string& aErrorState);

  virtual ~SystemStateMachine();

  //! Returns actionable system that this FSM belongs to
  const ActionableSystem& getActionable() const;

  //! Returns actionable system that this FSM belongs to
  ActionableSystem& getActionable();

  //! Returns ID of this FSM's initial state
  const std::string& getInitialState() const;

  //! Returns ID of this FSM's error state
  const std::string& getErrorState() const;

  //! Returns FSMs of children that are involved in this system FSM's transitions
  const std::set<const StateMachine*>& getParticipants() const;

  //! Returns IDs of all of this FSM's states
  const std::vector<std::string>& getStates() const;

  //FIXME: const method SHOULD NOT give non-pointer to Transition
  //! Returns map of all transitions (keyed by transition ID string) that start from the specified state
  const std::map<std::string, SystemTransition*>& getTransitions(const std::string& aStateId) const;

  //! Add state of specified ID
  void addState(const std::string& aState);


  /*!
   * @brief Add transition to this FSM
   * @param aTransitionId The new transition's ID
   * @param aFromState State that transition starts from
   * @param aToState State that transition goes to (in case no error occurs)
   */
  SystemTransition& addTransition(const std::string& aTransitionId, const std::string& aFromState, const std::string& aToState);

  SystemTransition& addTransition(const std::string& aTransitionId, const std::string& aAlias, const std::string& aFromState, const std::string& aToState);

  //! Engage this FSM, and FSMs of participating child objects
  void engage(const GateKeeper& aGateKeeper);

  //! Disengage this FSM, and FSMs of participating child objects
  void disengage();

  //! Reset this FSM, and FSMs of participating child objects, to initial state
  void reset(const GateKeeper& aGateKeeper);

private:

  //! Throws if system is not engaged in this state machine, or running transition; does NOT check children.
  void checkEngagedAndNotInTransition(const ActionableStatusGuard& aGuard, const std::string& aAction) const;

  //! Throws if child is not engaged in specified state machine, or running transition
  void checkChildEngagedAndNotInTransition(const StateMachine& aStateMachine, const ActionableStatusGuard& aGuard, const std::string& aAction) const;

  void disableChildren(const GateKeeper& aGateKeeper, const ActionableSystem::GuardMap_t& aGuardMap);

  struct State : public core::Object {
    State(const std::string& aId);
    void addTransition(SystemTransition* aTransition);
    std::map<std::string, SystemTransition*> transitionMap;
  };

  const State& getState(const std::string& aStateId) const;

  State& getState(const std::string& aStateId);

  void resetMonitoringSettings();

  void extractMonitoringSettings(const GateKeeper& aGateKeeper, const std::string& aState, MonitoringSettings_t& aSettings) const;

  void applyMonitoringSettings(const MonitoringSettings_t& aSettings);

  ActionableSystem& mResource;
  ActionableSystem::StatusContainer& mStatusMap;

  typedef std::vector<std::string> StateVec_t;
  typedef StateVec_t::const_iterator StateIt_t;
  const std::string mInitialState;
  const std::string mErrorState;
  std::vector<std::string> mStates;
  std::map<std::string, State*> mStateMap;
  std::set<const StateMachine*> mChildFSMs;
  std::set<StateMachine*> mNonConstChildFSMs;

  friend class SystemTransition;
  friend SystemTransition& SystemTransition::add(const std::vector<Transition*>& aTransitions);
};


// --------------------------------------------------------
template<class Iterator>
SystemTransition&
SystemTransition::add(Iterator aBegin, Iterator aEnd, const std::string& aFromState, const std::string& aTransition)
{
  typedef typename std::iterator_traits< Iterator >::value_type IteratorVal_t;
  BOOST_STATIC_ASSERT_MSG( (boost::is_convertible<IteratorVal_t, ActionableObject*>::value) , "Dereferencing type Iterator must result in a pointer to a type that inherits from swatch::action::ActionableObject");

  return add(aBegin, aEnd, getStateMachine().getId(), aFromState, aTransition);
}


// --------------------------------------------------------
template<class Iterator>
SystemTransition&
SystemTransition::add(Iterator aBegin, Iterator aEnd, const std::string& aFSM, const std::string& aFromState, const std::string& aTransition)
{
  typedef typename std::iterator_traits< Iterator >::value_type IteratorVal_t;
  BOOST_STATIC_ASSERT_MSG( (boost::is_convertible<IteratorVal_t, ActionableObject*>::value) , "Dereferencing type Iterator must result in a pointer to a type that inherits from swatch::action::ActionableObject");

  std::vector< Transition* > lTransitions;
  for (Iterator lIt=aBegin; lIt!=aEnd; lIt++)
    lTransitions.push_back( &(*lIt)->getStateMachine(aFSM).getTransition(aFromState, aTransition) );

  return add(lTransitions);
}


// --------------------------------------------------------
template<class Collection>
SystemTransition&
SystemTransition::add(const Collection& aCollection, const std::string& aFromState, const std::string& aTransition)
{
  return add( aCollection.begin(), aCollection.end(), aFromState, aTransition);
}


// --------------------------------------------------------
template<class Collection>
SystemTransition&
SystemTransition::add(const Collection& aCollection, const std::string& aFSMId, const std::string& aFromState, const std::string& aTransition)
{
  return add( aCollection.begin(), aCollection.end(), aFSMId, aFromState, aTransition);
}


SWATCH_DEFINE_EXCEPTION(InvalidSystemTransition)


}
}

#endif  /* __SWATCH_ACTION_SYSTEMSTATEMACHINE_HPP__ */


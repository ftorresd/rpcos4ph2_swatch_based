/*
 * File:   StateMachine.hpp
 * Author: tom
 *
 * Created on 18 August 2015, 22:58
 */

#ifndef __SWATCH_ACTION_STATEMACHINE_HPP__
#define __SWATCH_ACTION_STATEMACHINE_HPP__


// Standard headers
#include <iosfwd>                       // for ostream
#include <map>                          // for map
#include <string>                       // for string
#include <vector>                       // for vector

// boost headers
#include "boost/smart_ptr/shared_ptr.hpp"  // for shared_ptr

// SWATCH headers
#include "swatch/action/ActionableStatus.hpp"
#include "swatch/action/CommandVec.hpp"
#include "swatch/core/exception.hpp"
#include "swatch/action/MonitoringSetting.hpp"
#include "swatch/core/Object.hpp"


namespace swatch {
namespace action {


class ActionableObject;
class BusyGuard;
class Command;
class CommandSequence;
class GateKeeper;
class StateMachine;


class Transition : public CommandVec {
public:
  Transition(const std::string& aId, const std::string& aAlias, StateMachine& aFSM, ActionableStatus& aActionableStatus, const std::string& aStartState, const std::string& aEndState);

  //! State that this transition starts from
  const std::string& getStartState() const;

  //! State that this transition goes to (in case no error occurs)
  const std::string& getEndState() const;

  //! Returns FSM that this transition belongs to
  const StateMachine& getStateMachine() const;

  //! Returns FSM that this transition belongs to
  StateMachine& getStateMachine();

  /*!
   * @brief Add an individual command to this transition
   * @param aCmd the command
   * @param aNamespace Namespace used when retreving parameters from gatekeeper
   */
  Transition& add(Command& aCmd, const std::string& aNamespace="");

  //! Add all commands from specified sequence to this transition
  Transition& add(CommandSequence& aSequence);

  /**
   * Run the sequence, extracting the parameters for each command from the supplied gatekeeper
   *
   * @param aGateKeeper Gatekeeper that's used to extract the parameters
   * @param aUseThreadPool Run the sequence asynchronously in the swatch::action::ThreadPool ; if equals false, then the sequence is run synchronously
   */
  void exec(const GateKeeper& aGateKeeper, const bool& aUseThreadPool = true );

  /**
   * Run this sequence from another functionoid that already has control of resource, extracting the parameters for each command from the supplied gatekeeper
   *
   * @param aGateKeeper Gatekeeper that's used to extract the parameters
   * @param aUseThreadPool Run the command asynchronously in the swatch::action::ThreadPool ; if equals false, then the command is run synchronously
   */
  void exec(const BusyGuard* aGuard, const GateKeeper& aGateKeeper, const bool& aUseThreadPool = true );

private:
  void run(boost::shared_ptr<BusyGuard> aGuard);

  void changeState(const ActionableStatusGuard& aGuard, std::ostream& aLogMessageSuffix);

  StateMachine& mStateMachine;
  ActionableStatus& mActionableStatus;
  const std::string mStartState;
  const std::string mEndState;
  MonitoringSettings_t mCachedMonitoringSettings;
};


typedef CommandVecSnapshot TransitionSnapshot;


class StateMachine : public core::Object {
public:
  StateMachine(const std::string& aId, const std::string& aAlias, ActionableObject& aResource, ActionableStatus& aStatus, const std::string& aInitialState, const std::string& aErrorState);

  virtual ~StateMachine();

  //! Returns actionable object that this FSM belongs to
  const ActionableObject& getActionable() const;

  //! Returns actionable object that this FSM belongs to
  ActionableObject& getActionable();

  //! Returns ID of this FSM's initial state
  const std::string& getInitialState() const;

  //! Returns ID of this FSM's error state
  const std::string& getErrorState() const;

  //! Returns IDs of all of this FSM's states
  const std::vector<std::string>& getStates() const;

  //FIXME: const method SHOULD NOT give non-const pointer to Transition
  //! Returns map of all transitions (keyed by transition ID string) that start from the specified state
  const std::map<std::string, Transition*>& getTransitions(const std::string& aStateId) const;

  /*!
   * @brief Returns transition of specified ID, from specified state
   * @param aStateId ID string of start state
   * @param aTransitionId Transition's ID string
   */
  Transition& getTransition(const std::string& aStateId, const std::string& aTransitionId);

  //! Add state of specified ID
  void addState(const std::string& aState);

  /*!
   * @brief Add transition to this FSM
   * @param aTransitionId The new transition's ID
   * @param aFromState State that transition starts from
   * @param aToState State that transition goes to (in case no error occurs)
   */
  Transition& addTransition(const std::string& aTransitionId, const std::string& aFromState, const std::string& aToState);

  Transition& addTransition(const std::string& aTransitionId, const std::string& aAlias, const std::string& aFromState, const std::string& aToState);

  /*!
   * @brief Engage this FSM into initial state. Resets maskable descendants to unmasked, and monitoring settings to
   * enabled, then applies masks and monitoring settings from supplied gatekeeper
   * @param aGateKeeper source of monitoring settings and masks
   */
  void engage(const GateKeeper& aGateKeeper);

  /*!
   * @brief Engage this FSM into initial state, with actionable object's status already locked. Resets maskable descendants
   * to unmasked, and monitoring settings to enabled, then applies masks and monitoring settings from supplied gatekeeper
   * @param aGateKeeper source of monitoring settings and masks
   * @param aGuard Guard that's locking the status object of the associated actionable object
   */
  void engage(const GateKeeper& aGateKeeper, const ActionableStatusGuard& aGuard);

  //! Disengage this FSM
  void disengage();

  /*!
   * @brief Engage this FSM into initial state. Resets maskable descendants to unmasked, and monitoring settings to
   * enabled, then applies masks and monitoring settings from supplied gatekeeper
   * @param aGateKeeper source of monitoring settings and masks
   */
  void reset(const GateKeeper& aGateKeeper);

  /*!
   * @brief Reset this FSM into initial state, with actionable object's status already locked. Resets maskable descendants
   * to unmasked, and monitoring settings to enabled, then applies masks and monitoring settings from supplied gatekeeper
   * @param aGateKeeper source of monitoring settings and masks
   * @param aGuard Guard that's locking the status object of the associated actionable object
   */
  void reset(const GateKeeper& aGateKeeper, const ActionableStatusGuard& aGuard);

private:

  struct State : public Object {
    State(const std::string& aId);
    void addTransition(Transition* aTransition);
    std::map<std::string, Transition*> transitionMap;
  };

  const State& getState(const std::string& aStateId) const;

  State& getState(const std::string& aStateId);

  void resetMonitoringSettings();

  void extractMonitoringSettings(const GateKeeper& aGateKeeper, const std::string& aState, MonitoringSettings_t& aSettings) const;

  void applyMonitoringSettings(const MonitoringSettings_t& aSettings);

  ActionableObject& mResource;
  ActionableStatus& mStatus;

  typedef std::vector<std::string> StateVec_t;
  typedef StateVec_t::const_iterator StateIt_t;
  const std::string mInitialState;
  const std::string mErrorState;
  std::vector<std::string> mStates;

  std::map<std::string, State*> mStateMap;

  friend class Transition;
};


SWATCH_DEFINE_EXCEPTION(StateNotDefined)
SWATCH_DEFINE_EXCEPTION(StateAlreadyDefined)

SWATCH_DEFINE_EXCEPTION(TransitionNotDefined)
SWATCH_DEFINE_EXCEPTION(TransitionAlreadyDefined)

SWATCH_DEFINE_EXCEPTION(ResourceInWrongState)
SWATCH_DEFINE_EXCEPTION(ResourceInWrongStateMachine)


}
}


#endif /* __SWATCH_ACTION_STATEMACHINE_HPP__ */


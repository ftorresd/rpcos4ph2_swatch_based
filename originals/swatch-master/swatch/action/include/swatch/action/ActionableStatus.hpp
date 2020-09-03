/*
 * File:   ActionableStatus.hpp
 * Author: tom
 *
 * Created on 16 November 2015, 14:08
 */

#ifndef __SWATCH_ACTION_ACTIONABLESTATUS_HPP__
#define __SWATCH_ACTION_ACTIONABLESTATUS_HPP__


// STL headers
#include <iterator>                     // for iterator_traits
#include <stddef.h>                     // for size_t, NULL
#include <string>
#include <utility>                      // for pair
#include <vector>

// boost headers
#include "boost/static_assert.hpp"
#include "boost/smart_ptr/shared_ptr.hpp"        // for shared_ptr
#include "boost/thread/pthread/condition_variable_fwd.hpp"
#include "boost/type_traits/is_base_of.hpp"
#include "boost/type_traits/is_convertible.hpp"  // for is_convertible

// SWATCH headers
#include "swatch/core/AbstractMonitorableStatus.hpp"
#include "swatch/core/exception.hpp"


namespace swatch {
namespace action {

class Functionoid;
class ActionableStatus;


//! Represents the current aggregate state (engaged FSM, FSM state, running actions, ...) of an ActionableObject/ActionableSystem
class ActionableSnapshot {
public:

  ActionableSnapshot();

  /**
   * @brief      The object is alive i.e. it is not bound for destruction
   *
   * @return     True if alive
   */
  bool isAlive() const;

  /**
   * @brief      The object is engaged with a State Machine
   *
   * @return     True of engaged
   */
  bool isEngaged() const;

  /**
   * @brief      One or more actions are currently running
   *
   * @return     True if one or more actions/functionoid are running
   */
  bool isRunning() const;

  /**
   * @brief      The metrics update is being performed
   *
   * @return     True if the object is updating metrics
   */
  bool isUpdatingMetrics() const;

  //!
  const std::string& getState() const;

  //!
  const std::vector<const Functionoid*>& getRunningActions() const;


  const Functionoid* getLastRunningAction() const;

  //!
  template<class T>
  const T* getFirstRunningActionOfType() const;

  bool isActionWaitingToRun() const;

  //!
  const std::string& getStateMachineId() const;

  //! Returns true if corresponding actionable takes part in actions of higher-level actionables
  bool isEnabled() const;

  //!
  static const std::string kNullStateMachineId;

  //!
  static const std::string kNullStateId;

private:

  //! Indicates whether or not actions are allowed on this resource anymore (actions become disabled once the deleter is)
  bool mAlive;

  //!
  std::string mStateMachineId;

  //!
  std::string mState;

  //! Indicates which functionoids (Command/CommandSequence/(System)Transition) are currently active; NULL value indicates that no functionoids are currently active.
  std::vector<const Functionoid*> mRunningActions;

  bool mUpdatingMetrics;

  size_t mNumberOfWaitingMetricUpdates;
  size_t mNumberOfMetricReaders;

  bool mWaitingToRunAction;

  bool mEnabled;

  friend class ActionableStatus;
};


template<class T>
const T* ActionableSnapshot::getFirstRunningActionOfType() const
{
  BOOST_STATIC_ASSERT( (boost::is_base_of<swatch::action::Functionoid,T>::value) );

  for (std::vector<const Functionoid*>::const_iterator lIt=mRunningActions.begin(); lIt!=mRunningActions.end(); lIt++) {
    if (const T* lAction = dynamic_cast<const T*>(*lIt) )
      return lAction;
  }

  return NULL;
}


typedef core::MonitorableStatusGuard ActionableStatusGuard;

SWATCH_DEFINE_EXCEPTION(IncorrectActionableGuard)


typedef std::vector<boost::shared_ptr<ActionableStatusGuard> > ActionableStatusGuardVec_t;

//! Locks mutexes of all actionable status instances in supplied collection
template <typename Iterator>
ActionableStatusGuardVec_t lockMutexes(const Iterator& aBegin, const Iterator& aEnd);

//! Locks mutexes of all actionable status instances in supplied collection
ActionableStatusGuardVec_t lockMutexes(const std::vector<const ActionableStatus*>& aStatusMap);


class ActionableStatus : public core::AbstractMonitorableStatus {
public:
  ActionableStatus();
  ~ActionableStatus();

  ActionableSnapshot getSnapshot(const ActionableStatusGuard& aGuard) const;

  bool isAlive(const ActionableStatusGuard& aGuard) const;

  bool isEngaged(const ActionableStatusGuard& aGuard) const;

  const std::string& getStateMachineId(const ActionableStatusGuard& aGuard) const;

  const std::string& getState(const ActionableStatusGuard& aGuard) const;

  bool isBusy(const ActionableStatusGuard& aGuard) const;

  bool isUpdatingMetrics(const core::MonitorableStatusGuard& aGuard) const;

  const std::vector<const Functionoid*>& getRunningActions(const ActionableStatusGuard& aGuard) const;

  const Functionoid* getLastRunningAction(const ActionableStatusGuard& aGuard) const;

  /**
   * @brief      Gets the first running action of a given type.
   *
   * @param[in]  aGuard  A guard
   *
   * @tparam     T       { description }
   *
   * @return     The first running action of type.
   */
  template<class T>
  const T* getFirstRunningActionOfType(const ActionableStatusGuard& aGuard) const;

  /**
   *  @brief      Returns true if corresponding actionable takes part in actions
   *              of higher-level actionables
   * 
   *  @param[in]  aGuard  A guard
   *
   *  @return     True if enabled, False otherwise.
   */
  bool isEnabled(const ActionableStatusGuard& aGuard) const;

  void enable(const ActionableStatusGuard& aGuard);

  void disable(const ActionableStatusGuard& aGuard);

  void setStateMachine(const std::string& aStateMachine, const std::string& aState, const ActionableStatusGuard& aGuard);

  void setNoStateMachine(const ActionableStatusGuard& aGuard);

  void setState(const std::string& aState, const ActionableStatusGuard& aGuard);

  void addAction(const Functionoid& aAction, const ActionableStatusGuard& aGuard);

  void popAction(const ActionableStatusGuard& aGuard);

  //! Disables all future actions from running on this resource
  void kill(const ActionableStatusGuard& aGuard);

  /**
   * @brief      Blocks calling thread until no actions are running, no threads
   *             calling "waitUntilReadyToRunAction", *and* no threads reading
   *             metrics; then sets status to updating metrics before returning
   *             (DOESN'T THROW)
   *
   * @param      aGuard  A guard
   */
  virtual void waitUntilReadyToUpdateMetrics(core::MonitorableStatusGuard& aGuard);

  virtual void finishedUpdatingMetrics(const core::MonitorableStatusGuard& aGuard);

  /**
   * @brief      Blocks calling thread until no threads updating metrics, and no
   *             threads calling "waitUntilReadyToUpdateMetrics"; then sets
   *             status to "reading metrics" before returning (DOESN'T THROW)
   *
   * @param      aGuard  A guard
   */
  virtual void waitUntilReadyToReadMetrics(core::MonitorableStatusGuard& aGuard);

  virtual void finishedReadingMetrics(const core::MonitorableStatusGuard& aGuard);


  /**
   * @brief      Blocks calling thread until ready to run action (i.e.
   *             isUpdatingMetrics() = false), and then add specified
   *             Functionoid to "running actions" stack before returning
   * @throws     if    an action is already running, or an action is already queued
   *                   (another thread waiting in this method)
   *
   * @param[in]  aAction  A action
   * @param      aGuard   A guard
   */
  void waitUntilReadyToRunAction(const Functionoid& aAction, ActionableStatusGuard& aGuard);

  /**
   * @brief      Blocks calling thread until each status instance in vector is
   *             ready to run action (i.e. isUpdatingMetrics() = false), and
   *             then adds specified Functionoid to "running actions" stack of
   *             each status instance before returning
   * @throws     if    an action is already running, or an action is already queued
   *                   (another thread waiting in this method)
   *
   * @param[in]  <unnamed>  { parameter_description }
   */
  static void waitUntilReadyToRunAction(const std::vector<std::pair<ActionableStatus*, ActionableStatusGuard*> >&, const Functionoid& aAction);

private:
  //! Throws if guard is not for this status instance
  void throwIfWrongGuard(const core::MonitorableStatusGuard& aGuard) const;

  ActionableSnapshot mStatus;
  boost::condition_variable mConditionVar;

  friend ActionableStatusGuardVec_t lockMutexes(const std::vector<const ActionableStatus*>& aStatusMap);
};



template <typename Iterator>
ActionableStatusGuardVec_t lockMutexes(const Iterator& aBegin, const Iterator& aEnd)
{
  typedef typename std::iterator_traits<Iterator>::value_type IteratorVal_t;
  BOOST_STATIC_ASSERT_MSG( (boost::is_convertible<IteratorVal_t, const ActionableStatus*>::value) , "Dereferencing type 'Iterator' must result in a pointer to a type that inherits from swatch::action::ActionableStatus");

  std::vector<const ActionableStatus*> lStatusVec;
  for (Iterator lIt=aBegin; lIt != aEnd; lIt++) {
    lStatusVec.push_back(*lIt);
  }

  return lockMutexes(lStatusVec);
}


template<class T>
const T* ActionableStatus::getFirstRunningActionOfType(const ActionableStatusGuard& aGuard) const
{
  throwIfWrongGuard(aGuard);
  return mStatus.getFirstRunningActionOfType<T>();
}


} //end ns: action
} //end ns: swatch


#endif  /* __SWATCH_ACTION_ACTIONABLESTATUS_HPP__ */


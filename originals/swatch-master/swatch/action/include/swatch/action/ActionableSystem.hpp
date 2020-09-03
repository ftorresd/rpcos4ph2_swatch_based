/*
 * File:   ActionableSystem.hpp
 * Author: tom
 *
 * Created on 18 September 2015, 15:02
 */

#ifndef __SWATCH_ACTION_ACTIONABLESYSTEM_HPP__
#define __SWATCH_ACTION_ACTIONABLESYSTEM_HPP__


// Standard headers
#include <map>                          // for map
#include <set>                          // for set
#include <string>                       // for string

// boost headers
#include "boost/noncopyable.hpp"
#include "boost/smart_ptr/shared_ptr.hpp"  // for shared_ptr
#include "boost/unordered/unordered_map.hpp"  // for unordered_map

//log4cplus headers
#include <log4cplus/logger.h>

#include "swatch/action/ActionableStatus.hpp"
#include "swatch/core/exception.hpp"
#include "swatch/core/MonitorableObject.hpp"
#include "swatch/core/Object.hpp"


namespace swatch {
namespace action {

class ActionableObject;
class LeafObject;
class SystemStateMachine;


class ActionableSystem : public core::MonitorableObject {
public:
  typedef ActionableSnapshot Status_t;

  typedef std::map<const MonitorableObject*, boost::shared_ptr<ActionableStatusGuard> > GuardMap_t;

  class StatusContainer : boost::noncopyable {
  public:
    StatusContainer(const ActionableSystem& aSystem, ActionableStatus& aSysStatus);
    ~StatusContainer();

    const ActionableStatus& getSystemStatus() const;
    const ActionableStatus& getStatus(const ActionableObject& aChild ) const;

    ActionableStatus& getSystemStatus();
    ActionableStatus& getStatus(const ActionableObject& aChild );

    /*!
     * @brief Locks the mutexes for ActionableStatus object of the the system and all children known to this container.
     * @returns map containing the guards for all locked status objects; key is pointer to the associated actionable system/object
     */
    GuardMap_t lockMutexes() const;

  private:
    const ActionableSystem& mSystem;
    ActionableStatus& mSysStatus;
    std::map<const ActionableObject*, ActionableStatus*> mChildStatusMap;

    friend class ActionableSystem;
  };

  ActionableSystem(const std::string& aId, const std::string& aAlias, const std::string& aLoggerName);

  virtual ~ActionableSystem();

  /*!
   * Names of stored state machines.
   * @return set of state machine names
   */
  std::set<std::string> getStateMachines() const;

  //! Get registered state machine of specified ID
  SystemStateMachine& getStateMachine( const std::string& aId );


  Status_t getStatus() const;

  log4cplus::Logger& getLogger();

  typedef boost::unordered_map< std::string , SystemStateMachine* > StateMachineMap_t;
  typedef boost::unordered_map< std::string , ActionableObject* > ActionableChildMap_t;

  //! Get registered actionable children
  const ActionableChildMap_t& getActionableChildren();

  //! Deleter functor that only deletes the actionable system after all system-level actions have finished running
  class Deleter : public Object::Deleter {
  public:
    Deleter() {}
    ~Deleter() {}

    void operator()(LeafObject* aObject);
  };


protected:
  /*!
   * @brief Register a finite state machine in this system, with specified ID
   * @param aId State machine's ID
   * @param aInitialState The FSM's initial state
   * @param aErrorState The FSM's error state
   */
  SystemStateMachine& registerStateMachine(const std::string& aId, const std::string& aInitialState, const std::string& aErrorState );

  virtual void retrieveMetricValues() {}

  void addActionable(ActionableObject* aChildObject);

private:

  StateMachineMap_t mFSMs;
  ActionableChildMap_t mActionableChildren;

  ActionableStatus mStatus;
  StatusContainer mStatusMap;
  log4cplus::Logger mLogger;
};


SWATCH_DEFINE_EXCEPTION(ActionableSystemIsBusy)


}
}

#endif  /* __SWATCH_ACTION_ACTIONABLESYSTEM_HPP__ */


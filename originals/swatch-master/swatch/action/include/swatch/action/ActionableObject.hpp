/**
 * @file    ActionableObject.hpp
 * @author  Andy Rose
 * @brief   An object which exposes its Actionables
 * @date    August 2014
 *
 */

#ifndef __SWATCH_ACTION_ACTIONABLEOBJECT_HPP__
#define __SWATCH_ACTION_ACTIONABLEOBJECT_HPP__


// Standard headers
#include <algorithm>                    // for max
#include <functional>                   // for equal_to
#include <set>
#include <string>
#include <vector>                       // for vector

// boost headers
#include "boost/unordered/unordered_map.hpp"

// log4cplus headers
#include <log4cplus/logger.h>

// SWATCH headers
#include "swatch/action/ActionableStatus.hpp"
#include "swatch/core/exception.hpp"
#include "swatch/core/MonitorableObject.hpp"
#include "swatch/core/Object.hpp"


namespace swatch {
namespace action {

class ActionableSystem;
class Command;
class CommandSequence;
class GateKeeper;
class StateMachine;


/**
 * @class ActionableObject
 * @brief  An object representing a resource on which commands, command sequences, and transitions run
 */

class ActionableObject : public core::MonitorableObject {
public:
  typedef ActionableSnapshot Status_t;

  explicit ActionableObject( const std::string& aId, const std::string& aAlias, const std::string& aLoggerName );

  virtual ~ActionableObject();

  /**
    * Names of stored command sequences.
    * @return set of command sequence names
    */
  std::set< std::string > getSequences() const;

  /**
    * Names of stored commands.
    * @return set of command names
    */
  std::set<std::string> getCommands() const;

  /**
   * Names of stored state machines.
   * @return set of state machine names
   */
  std::set<std::string> getStateMachines() const;

  //! Get registered command sequence of specified ID
  CommandSequence& getSequence( const std::string& aId );

  //! Get registered command of specified ID
  Command& getCommand( const std::string& aId );

  //! Get registered state machine of specified ID
  StateMachine& getStateMachine( const std::string& aId );

  virtual const std::vector<std::string>& getGateKeeperContexts() const = 0;

  Status_t getStatus() const;

  log4cplus::Logger& getLogger();

  void enable();

  void enable( const ActionableStatusGuard& aGuard );

  void disable();

  void disable( const ActionableStatusGuard& aGuard );


  void resetAndApplyMasks(const GateKeeper& aGateKeeper);

  void resetAndApplyMasks(const GateKeeper& aGateKeeper, const ActionableStatusGuard& aGuard);


  typedef boost::unordered_map< std::string , CommandSequence* > CommandSequenceMap_t;
  typedef boost::unordered_map< std::string , Command* > CommandMap_t;
  typedef boost::unordered_map< std::string , StateMachine* > StateMachineMap_t;

  //! Deleter functor that only deletes the actionable object after all commands, command sequences and transitions have finished running
  class Deleter : public Object::Deleter {
  public:
    Deleter() {}
    ~Deleter() {}

    void operator()(LeafObject* aObject);
  };

protected:
  //! Register the supplied command class in this object, with specified ID; the class is constructed on the heap, using the ID as the only constructor argument.
  template< typename T>
  T& registerCommand( const std::string& aId );

  //! Register the supplied command class in this object, with specified ID and alias; the class is constructed on the heap, using the ID and alias as the only constructor arguments.
  template< typename T>
  T& registerCommand( const std::string& aId, const std::string& aAlias );

  //FIXME: Why is this method required; other registerCommand methods should be sufficent - check use in subsystem code, and make private if possible
  //! Register the supplied command in this object, with specified ID; this object takes ownership of the command
  Command& registerCommand(const std::string& aId , Command* aCommand );

  //! Register the a command sequence in this object, with specified ID
  CommandSequence& registerSequence(const std::string& aId, const std::string& aFirstCommandId, const std::string& aFirstCommandNamespace="");

  //! Register the a command sequence in this object, with specified ID and alias
  CommandSequence& registerSequence(const std::string& aId, const std::string& aAlias, const std::string& aFirstCommandId, const std::string& aFirstCommandNamespace);

  //! Register the a command sequence in this object, with specified ID
  CommandSequence& registerSequence(const std::string& aId, Command& aFirstCommand, const std::string& aFirstCommandNamespace="");

  //! Register the a command sequence in this object, with specified ID and alias
  CommandSequence& registerSequence(const std::string& aId, const std::string& aAlias, Command& aFirstCommand, const std::string& aFirstCommandNamespace="");

  /*!
   * @brief Register a finite state machine in this object, with specified ID
   * @param aId State machine's ID
   * @param aInitialState The FSM's initial state
   * @param aErrorState The FSM's error state
   */
  StateMachine& registerStateMachine(const std::string& aId, const std::string& aInitialState, const std::string& aErrorState );

  StateMachine& registerStateMachine(const std::string& aId, const std::string& aAlias, const std::string& aInitialState, const std::string& aErrorState );

private:
  CommandSequenceMap_t mCommandSequences;
  CommandMap_t mCommands;
  StateMachineMap_t mFSMs;

  ActionableStatus mStatus;
  log4cplus::Logger mLogger;

  friend class ActionableSystem;
  friend class Command;
};


SWATCH_DEFINE_EXCEPTION(CommandSequenceAlreadyExistsInActionableObject)
SWATCH_DEFINE_EXCEPTION(CommandAlreadyExistsInActionableObject)
SWATCH_DEFINE_EXCEPTION(StateMachineAlreadyExistsInActionableObject)

SWATCH_DEFINE_EXCEPTION(CommandSequenceNotFoundInActionableObject)
SWATCH_DEFINE_EXCEPTION(CommandNotFoundInActionableObject)
SWATCH_DEFINE_EXCEPTION(StateMachineNotFoundInActionableObject)

SWATCH_DEFINE_EXCEPTION(ActionableObjectIsBusy)


} // namespace action
} // namespace swatch

#include "swatch/action/ActionableObject.hxx"

#endif  /* __SWATCH_ACTION_ACTIONABLEOBJECT_HPP__ */


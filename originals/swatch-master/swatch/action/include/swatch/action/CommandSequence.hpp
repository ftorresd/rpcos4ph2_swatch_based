/**
 * @file    CommandSequence.hpp
 * @author  arose
 * @date    May 2015
 */

#ifndef __SWATCH_ACTION_COMMANDSEQUENCE_HPP__
#define __SWATCH_ACTION_COMMANDSEQUENCE_HPP__


#include <string>

#include "swatch/action/CommandVec.hpp"


namespace swatch {
namespace action {

class ActionableObject;
class ActionableStatus;
class BusyGuard;
class Command;
class GateKeeper;


//! Represents a sequence of commands, executed in succession.
class CommandSequence : public CommandVec {
public:
  CommandSequence( const std::string& aId, const std::string& aAlias, ActionableObject& aResource, ActionableStatus& aActionableStatus, const std::string& aFirstCommandId, const std::string& aFirstCommandNamespace="");
  CommandSequence( const std::string& aId, const std::string& aAlias, ActionableObject& aResource, ActionableStatus& aActionableStatus, Command& aFirstCommand, const std::string& aFirstCommandNamespace="");

  virtual ~CommandSequence();

  /**
    Utility function to add a command to the command sequence
    @param aCommand a command to add to the command sequence
    @param aAlias an alias to use for that command when looking up parameters in gatekeeper; the command's ID string is used if alias is empty
  */
  CommandSequence& run( Command& aCommand, const std::string& aNamespace="");
  CommandSequence& then( Command& aCommand, const std::string& aNamespace="");

  CommandSequence& run( const std::string& aCommand, const std::string& aNamespace="");
  CommandSequence& then( const std::string& aCommand, const std::string& aNamespace="");

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
  ActionableStatus& mActionableStatus;
};


typedef CommandVecSnapshot CommandSequenceSnapshot;

} /* namespace action */
} /* namespace swatch */

#endif /* __SWATCH_ACTION_COMMANDSEQUENCE_HPP__ */

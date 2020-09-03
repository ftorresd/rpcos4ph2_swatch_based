#include "swatch/action/CommandSequence.hpp"


#include <stddef.h>
#include <vector>

#include "boost/smart_ptr/shared_ptr.hpp"  // for shared_ptr

#include "swatch/action/ActionableObject.hpp"
#include "swatch/action/BusyGuard.hpp"
#include "swatch/action/GateKeeper.hpp"
#include "swatch/core/ReadOnlyXParameterSet.hpp"


namespace swatch {
namespace action {


CommandSequence::CommandSequence( const std::string& aId, const std::string& aAlias, ActionableObject& aResource, ActionableStatus& aActionableStatus, const std::string& aFirstCommandId, const std::string& aFirstCommandAlias) :
  CommandVec(aId, aAlias, aResource),
  mActionableStatus(aActionableStatus)
{
  run(aFirstCommandId, aFirstCommandAlias);
}


CommandSequence::CommandSequence( const std::string& aId, const std::string& aAlias, ActionableObject& aResource, ActionableStatus& aActionableStatus, Command& aFirstCommand, const std::string& aFirstCommandAlias) :
  CommandVec(aId, aAlias, aResource),
  mActionableStatus(aActionableStatus)
{
  run(aFirstCommand, aFirstCommandAlias);
}


CommandSequence::~CommandSequence()
{
}


CommandSequence& CommandSequence::run( Command& aCommand, const std::string& aNamespace )
{
  addCommand(aCommand, (aNamespace.empty() ? getId() : aNamespace) );
  return *this;
}


CommandSequence& CommandSequence::then ( Command& aCommand, const std::string& aNamespace )
{
  return run( aCommand, (aNamespace.empty() ? getId() : aNamespace) );
}


CommandSequence& CommandSequence::run( const std::string& aCommand, const std::string& aNamespace )
{
  addCommand(aCommand, (aNamespace.empty() ? getId() : aNamespace));
  return *this;
}


CommandSequence& CommandSequence::then ( const std::string& aCommand, const std::string& aNamespace )
{
  return run( aCommand, (aNamespace.empty() ? getId() : aNamespace) );
}


//------------------------------------------------------------------------------------
void CommandSequence::exec(const GateKeeper& aGateKeeper, const bool& aUseThreadPool )
{
  exec(NULL, aGateKeeper, aUseThreadPool);
}


//------------------------------------------------------------------------------------
void CommandSequence::exec(const BusyGuard* aOuterBusyGuard, const GateKeeper& aGateKeeper, const bool& aUseThreadPool )
{
  // 1) Extract parameters before creating busy guard (so that resource doesn't change states if parameter is missing)
  std::vector<core::ReadOnlyXParameterSet> lParamSets;
  std::vector<MissingParam> lMissingParams;
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

  // 2) Create busy guard
  boost::shared_ptr<BusyGuard> lBusyGuard(new BusyGuard(*this, mActionableStatus, aOuterBusyGuard));

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

  // 4) Run the commands
  if (aUseThreadPool) {
    scheduleAction<CommandSequence>(this, &CommandSequence::runCommands, lBusyGuard);
  }
  else {
    // otherwise execute in same thread
    this->runCommands(lBusyGuard);
  }
}


} /* namespace action */
} /* namespace swatch */

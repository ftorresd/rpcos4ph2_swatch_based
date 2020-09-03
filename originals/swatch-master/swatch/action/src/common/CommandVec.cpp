#include "swatch/action/CommandVec.hpp"


#include <iostream>
#include <exception>
#include <math.h>
#include <set>
#include <vector>

#include "boost/date_time/posix_time/posix_time_config.hpp"
#include "boost/date_time/posix_time/posix_time_types.hpp"
#include "boost/date_time/time.hpp"     // for base_time

#include "log4cplus/loggingmacros.h"

#include "swatch/action/ActionableObject.hpp"
#include "swatch/action/Command.hpp"
#include "swatch/action/GateKeeper.hpp"
#include "swatch/core/ReadWriteXParameterSet.hpp"


namespace swatch {
namespace action {


CommandVec::CommandVec( const std::string& aId, const std::string& aAlias, ActionableObject& aActionable) :
  ObjectFunctionoid( aId, aAlias, aActionable ),
  mCommands(),
  mCachedParameters(),
  mParamUpdateTime() ,
  mState( kInitial ),
  mCommandIt( mCommands.end() )
{
}


void CommandVec::addCommand(Command& aCommand, const std::string& aNamespace)
{
  if ( &aCommand.getActionable() != &getActionable() ) {
    XCEPT_RAISE(InvalidResource,"Cannot add command '" + aCommand.getId() + "' (resource: "+aCommand.getActionable().getPath()+") ' to sequence of resource '" + getActionable().getPath() + "'");
  }
  mCommands.push_back( Element(aCommand, aNamespace) );
  mCommandIt = mCommands.end();
}


void CommandVec::addCommand(const std::string& aCommand, const std::string& aNamespace)
{
  Command& lCommand = getActionable().getCommand( aCommand );
  mCommands.push_back( Element(lCommand, aNamespace) );
  mCommandIt = mCommands.end();
}


CommandVec::~CommandVec()
{
}


CommandVec::Element::Element(Command& aCommand, const std::string& aNamespace) :
  mCmd(&aCommand),
  mNamespace(aNamespace)
{
}


CommandVec::Element::~Element()
{
}

const std::string& CommandVec::Element::getNamespace() const
{
  return mNamespace;
}

const Command& CommandVec::Element::get() const
{
  return *mCmd;
}

Command& CommandVec::Element::get()
{
  return *mCmd;
}


size_t CommandVec::size() const
{
  return mCommands.size();
}


CommandVec::const_iterator CommandVec::begin() const
{
  return mCommands.begin();
}


CommandVec::const_iterator CommandVec::end() const
{
  return mCommands.end();
}


std::vector<Command*> CommandVec::getCommands()
{
  std::vector<Command*> lCmds;
  lCmds.reserve(size());
  for (auto lIt=mCommands.begin(); lIt!=mCommands.end(); lIt++)
    lCmds.push_back(&lIt->get());
  return lCmds;
}


Functionoid::State CommandVec::getState() const
{
  boost::unique_lock<boost::mutex> lock(mMutex);
  return mState;
}


CommandVecSnapshot CommandVec::getStatus() const
{
  boost::unique_lock<boost::mutex> lock(mMutex);

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

  const Command* currentCommand =  ( ((mCommandIt == mCommands.end()) || (mState == State::kError)) ? NULL : &mCommandIt->get());

  return CommandVecSnapshot(IdAliasPair(*this), IdAliasPair(getActionable()), mState, runningTime, currentCommand, mStatusOfCompletedCommands, mCommands.size());
}


void CommandVec::runCommands(boost::shared_ptr<BusyGuard> aGuard)
{
  // 1) Declare that I'm running
  {
    boost::unique_lock<boost::mutex> lock( mMutex );
    mExecStartTime = boost::posix_time::microsec_clock::universal_time();
    // Finish straight away if there aren't any commands to run
    if ( mCommands.empty() ) {
      mState = kDone;
      mCommandIt = mCommands.end();
      mExecEndTime = mExecStartTime;
      return;
    }
    else {
      mState = kRunning;
      mCommandIt = mCommands.begin();
    }
  }

  // 2) Run the commands
  try {
    ParameterSets_t::iterator lIt( mCachedParameters.begin() );

    while ( true ) {
      mCommandIt->get().exec(aGuard.get(), *lIt , false ); // False = run the commands in this thread!
      //FIXME: Make exec method return CommandStatus to remove any possibility of race condition ?

      CommandSnapshot status = mCommandIt->get().getStatus();
      boost::unique_lock<boost::mutex> lock(mMutex);
      mStatusOfCompletedCommands.push_back(status);

      // Don't execute any more commands if there was an error
      if ( status.getState() == kError ) {
        mState = kError;
        mExecEndTime = boost::posix_time::microsec_clock::universal_time();
        return;
      }

      // Increment the "current command" iterator
      ++mCommandIt;
      ++lIt;

      // Exit the loop if no more commands remain
      if ( mCommandIt == mCommands.end() ) {
        mState = kDone;
        for (std::vector<CommandSnapshot>::const_iterator statusIt=mStatusOfCompletedCommands.begin(); statusIt != mStatusOfCompletedCommands.end(); statusIt++) {
          if (statusIt->getState() == kWarning)
            mState = kWarning;
        }
        mExecEndTime = boost::posix_time::microsec_clock::universal_time();
        return;
      }
    }

  }
  catch (const std::exception& e) {
    std::cout << "An exception occurred in CommandVec::runCommands(): " << e.what() << std::endl;

    boost::unique_lock<boost::mutex> lock( mMutex );
    mState = kError;
    mExecEndTime = boost::posix_time::microsec_clock::universal_time();
  }

  // 3) The resource is released by destruction of BusyGuard
}


CommandVec::MissingParam::MissingParam(const std::string& aNamespace, const std::string& aCommand, const std::string& aParam) :
  nspace(aNamespace),
  command(aCommand),
  parameter(aParam)
{
}


void CommandVec::checkForMissingParameters(const GateKeeper& aGateKeeper, std::vector<core::ReadOnlyXParameterSet>& aParamSets, std::vector<MissingParam>& aMissingParams) const
{
  extractParameters(aGateKeeper, aParamSets, aMissingParams, false);
}

 
CommandVec::ParamRuleViolationList::ParamRuleViolationList(const std::string& aCommand, const size_t aCommandIdx, const core::ReadOnlyXParameterSet& aParamSet, const std::vector<Command::ParamRuleViolation>& aRuleViolations) :
  command(aCommand),
  commandIdx(aCommandIdx),
  paramSet(aParamSet),
  violations(aRuleViolations)
{
}


void CommandVec::checkForInvalidParameters(const std::vector<core::ReadOnlyXParameterSet>& aParamSets, std::vector<ParamRuleViolationList>& aRuleViolations) const
{
  if (aParamSets.size() < size()) {
    std::ostringstream lMsgStream;
    lMsgStream << "Parameters for only " << aParamSets.size() << " commands supplied";
    XCEPT_RAISE(ParameterNotFound, lMsgStream.str());
  }

  auto lParamSetIt = aParamSets.begin();
  for(size_t i=0; i<size(); i++, lParamSetIt++) {
    const Command& lCommand = mCommands.at(i).get();

    std::vector<Command::ParamRuleViolation> lRuleViolations;
    lCommand.checkRulesAndConstraints(*lParamSetIt, lRuleViolations);
    if (!lRuleViolations.empty())
      aRuleViolations.push_back( ParamRuleViolationList(lCommand.getId(), i, *lParamSetIt, lRuleViolations) );
  }
}


void CommandVec::reset(const ParameterSets_t& aParamSets)
{
  boost::unique_lock<boost::mutex> lock( mMutex );

  mState = kInitial;
  mCommandIt = mCommands.end();
  mCachedParameters = aParamSets;
  mStatusOfCompletedCommands.clear();
  mStatusOfCompletedCommands.reserve(mCommands.size());
}


void CommandVec::extractParameters(const GateKeeper& aGateKeeper, std::vector<core::ReadOnlyXParameterSet>& aParamSets, std::vector<MissingParam>& aMissingParams, bool aThrowOnMissing) const
{
  aParamSets.clear();
  aParamSets.reserve( mCommands.size() );

  aMissingParams.clear();

  for ( CommandVector_t::const_iterator lIt( mCommands.begin()) ; lIt != mCommands.end() ; ++lIt ) {
    const Command& lCommand( lIt->get() );
//    const std::string& lCommandAlias = (lIt->getAlias().empty() ? lCommand.getId() : lIt->getAlias());

    core::ReadOnlyXParameterSet lParams;
    std::set< std::string > lKeys( lCommand.getDefaultParams().keys() );
    for ( std::set< std::string >::iterator lIt2( lKeys.begin() ); lIt2!=lKeys.end(); ++lIt2 ) {
      GateKeeper::Parameter_t lData( aGateKeeper.get( lIt->getNamespace() , lCommand.getId() , *lIt2 , getActionable().getGateKeeperContexts() ) );
      if ( lData.get() != NULL ) {
        lParams.adopt( *lIt2 , lData );
      }
      else if (aThrowOnMissing) {
        std::ostringstream oss;
        oss << "Could not find value of parameter '" << *lIt2 << "' for command '" << lCommand.getId() << "' in namespace '" << lIt->getNamespace() << "' of resource '" << getActionable().getId() << "'";
//        LOG4CPLUS_ERROR(getActionable().getLogger(), oss.str());
        XCEPT_RAISE(ParameterNotFound,oss.str());
      }
      else {
        aMissingParams.push_back(MissingParam(lIt->getNamespace(), lCommand.getId(), *lIt2));
      }
    }
    aParamSets.push_back( lParams );
  }
}

std::ostream& operator << (std::ostream& aOstream, const CommandVec::MissingParam& aMissingParam )
{
  return (aOstream << aMissingParam.nspace << "." << aMissingParam.command << "." << aMissingParam.parameter);
}


bool operator !=(const CommandVec::MissingParam& aParam1, const CommandVec::MissingParam& aParam2)
{
  return !( (aParam1.nspace == aParam2.nspace) && (aParam1.command == aParam2.command) && (aParam1.parameter == aParam2.parameter));
}


std::ostream& operator << (std::ostream& aOstream, const CommandVec::ParamRuleViolationList& aViolationList )
{
  aOstream << "{" << aViolationList.commandIdx << ", " << aViolationList.command << ", pSet = " << aViolationList.paramSet << ", [";
  for(size_t i=0; i< aViolationList.violations.size(); i++) {
    const auto & lViolation = aViolationList.violations.at(i);
    if (i != 0)
      aOstream << ", ";
    aOstream << "{params [" << core::join(lViolation.parameters, ",")  << "],";
    aOstream << " '" << lViolation.ruleDescription << "', '" << lViolation.details << "'}";
  }
  aOstream << "]}";
  return aOstream;
}


bool operator !=(const CommandVec::ParamRuleViolationList& aList1, const CommandVec::ParamRuleViolationList& aList2)
{
  if (aList1.command != aList2.command)
    return true;
  if (aList1.commandIdx != aList2.commandIdx)
    return true;

  if (aList1.paramSet.keys() != aList2.paramSet.keys()) 
    return true;
  for (const auto& lKey : aList1.paramSet.keys()) {
    if (typeid(aList1.paramSet[lKey]) != typeid(aList2.paramSet[lKey]))
      return true;
    if (aList1.paramSet[lKey].equals(aList2.paramSet[lKey]) == 0)
      return true;
  }

  if (aList1.violations.size() != aList2.violations.size())
    return true;

  auto lIt1 = aList1.violations.begin();
  for (auto lIt2 = aList2.violations.begin(); lIt2 != aList2.violations.end(); lIt1++, lIt2++) {
    if (lIt1->ruleDescription != lIt2->ruleDescription)
      return true;
    if (lIt1->parameters != lIt2->parameters)
      return true;
    if (lIt1->details != lIt2->details)
      return true;
  }
  
  return false;
}



CommandVecSnapshot::CommandVecSnapshot(const IdAliasPair& aAction, const IdAliasPair& aActionable, State aState, float aRunningTime, const Command* aCurrentCommand, const std::vector<CommandSnapshot>& aStatusOfCompletedCommands,  size_t aTotalNumberOfCommands) :
  ActionSnapshot(aAction, aActionable, aState, aRunningTime),
  mTotalNumberOfCommands( aTotalNumberOfCommands ),
  mCommandStatuses(aStatusOfCompletedCommands)
{
  if (aCurrentCommand != NULL) {
    mCommandStatuses.push_back(aCurrentCommand->getStatus());
  }

  for (auto it = aStatusOfCompletedCommands.begin(); it != aStatusOfCompletedCommands.end(); it++)
    mResults.push_back(it->getResult());
}


float CommandVecSnapshot::getProgress() const
{
  if ((mTotalNumberOfCommands == 0) && (getState() == State::kDone))
    return 1.0;
  else if (mCommandStatuses.empty())
    return 0.0;
  else if ((getState() == State::kDone) || (getState() == State::kWarning))
    return 1.0;
  else
    return ( float(mCommandStatuses.size()-1) + mCommandStatuses.back().getProgress() ) / float(mTotalNumberOfCommands);
}


size_t CommandVecSnapshot::getNumberOfCompletedCommands() const
{
  return mResults.size();
}


size_t CommandVecSnapshot::getTotalNumberOfCommands() const
{
  return mTotalNumberOfCommands;
}


const std::vector<const xdata::Serializable*>& CommandVecSnapshot::getResults() const
{
  return mResults;
}


CommandVecSnapshot::const_iterator CommandVecSnapshot::begin() const
{
  return mCommandStatuses.begin();
}


CommandVecSnapshot::const_iterator CommandVecSnapshot::end() const
{
  return mCommandStatuses.end();
}


const CommandSnapshot& CommandVecSnapshot::at(size_t aIndex) const
{
  return mCommandStatuses.at(aIndex);
}


size_t CommandVecSnapshot::size() const
{
  return mCommandStatuses.size();
}


} /* namespace action */
} /* namespace swatch */

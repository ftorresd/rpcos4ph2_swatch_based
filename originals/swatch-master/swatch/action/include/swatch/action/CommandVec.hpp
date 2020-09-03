/**
 * @file    CommandSequence.hpp
 * @author  arose
 * @date    May 2015
 */

#ifndef __SWATCH_ACTION_COMMANDVEC_HPP__
#define __SWATCH_ACTION_COMMANDVEC_HPP__


#include <stddef.h>                     // for size_t
#include <iosfwd>                       // for ostream
#include <string>                       // for string
#include <vector>

#include "boost/date_time/posix_time/ptime.hpp"  // for ptime
#include "boost/function/function_fwd.hpp"  // for function
#include "boost/smart_ptr/shared_ptr.hpp"  // for shared_ptr
#include "boost/thread/lock_types.hpp"  // for unique_lock
#include "boost/thread/pthread/mutex.hpp"  // for mutex

#include "swatch/action/ActionableObject.hpp"
#include "swatch/action/Command.hpp"
#include "swatch/action/Functionoid.hpp"
#include "swatch/core/ReadOnlyXParameterSet.hpp"
#include "swatch/action/ThreadPool.hpp"


// Forward declarations
namespace xdata {
class Serializable;
}


namespace swatch {
namespace action {

class BusyGuard;
class CommandVecSnapshot;
class GateKeeper;

//! Represents a sequence of commands, executed in succession.
class CommandVec : public ObjectFunctionoid {
protected:
  CommandVec( const std::string& aId, const std::string& aAlias, ActionableObject& aResource);

  void addCommand(Command& aCommand, const std::string& aNamespace);

  void addCommand(const std::string& aCommand, const std::string& aNamespace);

public:
  virtual ~CommandVec();

  class Element {
  public:
    Element(Command& aCommand, const std::string& aNamespace);
    ~Element();

    const std::string& getNamespace() const;

    const Command& get() const;

    Command& get();

  private:
    Command* mCmd;
    std::string mNamespace;
  };

  //! Number of commands
  size_t size() const;

  typedef std::vector<Element>::const_iterator const_iterator;

  const_iterator begin() const;
  const_iterator end() const;

  std::vector<Command*> getCommands();

  //! Returns current state of this command sequence
  State getState() const;

  //! Returns snapshot of this command's current status (state flag value, running time, current command, overall progress fraction)
  CommandVecSnapshot getStatus() const;

  struct MissingParam {
    MissingParam(const std::string& aNamespace, const std::string& aCommand, const std::string& aParam);
    std::string nspace;
    std::string command;
    std::string parameter;
  };

  void checkForMissingParameters(const GateKeeper& aGateKeeper, std::vector<core::ReadOnlyXParameterSet>& aParamSets, std::vector<MissingParam>& aMissingParams) const;

  //! Data structure for the rule violations of a single command
  struct ParamRuleViolationList {
    ParamRuleViolationList(const std::string& aCommand, const size_t aCommandIdx, const core::ReadOnlyXParameterSet& aParamSet, const std::vector<Command::ParamRuleViolation>& aRuleViolation);
    std::string command;
    size_t commandIdx;
    core::ReadOnlyXParameterSet paramSet;
    std::vector<Command::ParamRuleViolation> violations;
  };
  
  void checkForInvalidParameters(const std::vector<core::ReadOnlyXParameterSet>& aParamSets, std::vector<ParamRuleViolationList>& aRuleViolations) const;

protected:
  typedef std::vector<core::ReadOnlyXParameterSet> ParameterSets_t;

  //! Resets this CommandVec's state variables
  void reset(const ParameterSets_t& aParamSets);

  /*!
   * Extracts from gatekeeper the parameter sets for running commands
   * @param aParamSets vector containing the extracted parameter sets
   * @param aMissingParams vector of missing parameters for each command
   * @param aThrowOnMissing if true, then throws ParameterNotFound if gatekeeper can't find value of any parameter
   */
  void extractParameters(const GateKeeper& aGateKeeper, std::vector<core::ReadOnlyXParameterSet>& aParamSets, std::vector<MissingParam>& aMissingParams, bool aThrowOnMissing) const;

  //! thread safe exception-catching wrapper for code()
  void runCommands(boost::shared_ptr<BusyGuard> aGuard);

  template<class OBJECT>
  void scheduleAction( OBJECT* cmd , boost::function<void(OBJECT*, boost::shared_ptr<BusyGuard>)> aFunction, const boost::shared_ptr<BusyGuard>& resourceGuard );


private:
  typedef std::vector< Element > CommandVector_t;
  CommandVector_t mCommands;

  ParameterSets_t mCachedParameters;

  /// The last time a table was updated from the Gatekeeper
  boost::posix_time::ptime mParamUpdateTime;

  mutable boost::mutex mMutex;
  State mState;

  CommandVector_t::iterator mCommandIt;

  boost::posix_time::ptime mExecStartTime;
  boost::posix_time::ptime mExecEndTime;

  std::vector<CommandSnapshot> mStatusOfCompletedCommands;
};


template<class OBJECT>
void CommandVec::scheduleAction( OBJECT* aAction , boost::function<void(OBJECT*, boost::shared_ptr<BusyGuard>)> aFunction, const boost::shared_ptr<BusyGuard>& aGuard )
{
  boost::unique_lock<boost::mutex> lLock(mMutex);
  mState = kScheduled;

  ThreadPool& lPool = ThreadPool::getInstance();
  lPool.addTask<OBJECT, BusyGuard>(aAction , aFunction, aGuard);
}


std::ostream& operator << (std::ostream& aOstream, const CommandVec::MissingParam& aMissingParam );

bool operator !=(const CommandVec::MissingParam& aParam1, const CommandVec::MissingParam& aParam2);

std::ostream& operator << (std::ostream& aOstream, const CommandVec::ParamRuleViolationList& aMissingParam );

bool operator !=(const CommandVec::ParamRuleViolationList& aParam1, const CommandVec::ParamRuleViolationList& aParam2);


//! Provides a snapshot of the progress/status of a swatch::action::CommandVec
class CommandVecSnapshot : public ActionSnapshot {
public:
  CommandVecSnapshot(const IdAliasPair& aAction, const IdAliasPair& aActionable, State aState, float aRunningTime, const Command* aCurrentCommand, const std::vector<CommandSnapshot>& aFinishedCommandStatuses, size_t aTotalNumberOfCommands);

  //! Returns fraction progress of sequence - range [0,1] inclusive
  float getProgress() const;

  //! Returns count of commands that have already completed execution (regardless of whether their final state is kDone, kWarning, or kError)
  size_t getNumberOfCompletedCommands() const;

  //! Number of commands in the sequence
  size_t getTotalNumberOfCommands() const;

  //! Returns results of the command in the sequence that have already completed execution
  const std::vector<const xdata::Serializable*>& getResults() const;

  typedef std::vector<CommandSnapshot>::const_iterator const_iterator;

  //! Returns iterator to snapshot of first command in this action (if it has started execution)
  const_iterator begin() const;

  //! Returns iterator 'one beyond' the snapshot of the last command that has started/completed execution
  const_iterator end() const;

  //! Returns snapshot of i'th command
  const CommandSnapshot& at(size_t aIndex) const;

  //! Returns number of commands that have started/completed execution as part of this action (i.e. equal to this->end() - this->begin())
  size_t size() const;

private:
  // const Command* mCurrentCommand;
  size_t mTotalNumberOfCommands;
  std::vector<const xdata::Serializable*> mResults;
  std::vector<CommandSnapshot> mCommandStatuses;
};

} /* namespace action */
} /* namespace swatch */

#endif /* __SWATCH_ACTION_COMMANDVEC_HPP__ */

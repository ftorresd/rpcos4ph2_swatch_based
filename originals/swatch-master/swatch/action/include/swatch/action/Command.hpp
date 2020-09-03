/**
 * @file    Command.hpp
 * @author  Alessandro Thea
 * @date    February 2015
 *
 */

#ifndef __SWATCH_ACTION_COMMAND_HPP__
#define __SWATCH_ACTION_COMMAND_HPP__

// C++ Headers
#include <string>
#include <sys/time.h>                   // for timeval

// boost headers
#include "boost/smart_ptr/shared_ptr.hpp"  // for shared_ptr
#include "boost/thread/pthread/mutex.hpp"  // for mutex

// SWATCH headers
#include "swatch/core/exception.hpp"
#include "swatch/action/ObjectFunctionoid.hpp"
#include "swatch/core/ReadOnlyXParameterSet.hpp"
#include "swatch/core/ReadWriteXParameterSet.hpp"
#include "swatch/action/DefaultCommandParRules.hpp"

// Forward declarations
namespace xdata {
class Serializable;
}


namespace swatch {

namespace core {
class XParameterSet;
class AbstractXRule;
class XPSetConstraint;
}

namespace action {

class ActionableStatus;
class BusyGuard;
class CommandSnapshot;


/**
 * @brief      Represents a one-shot action on a resource (a class that inherits
 *             from swatch::action::ActionableObject)
 */
class Command : public ObjectFunctionoid {
public:

  /**
   * @brief      Destroys the object.
   */
  virtual ~Command();

 /**
   * @brief      Run this command, using the supplied set of parameters
   *
   * @param[in]  aParams         Map of parameter values; any default parameters
   *                             for this command that aren't included in this
   *                             argument will be merged into the set supplied
   *                             to the code method
   * @param[in]  aUseThreadPool  aUseThreadPool Run the command asynchronously
   *                             in the swatch::action::ThreadPool ; if equals
   *                             false, then the command is run synchronously
   */
  void exec( const core::XParameterSet& aParams , bool aUseThreadPool = true );

  /**
   * @brief      Run this command, using the supplied set of parameters, from
   *             another functionoid that already has control of resource
   *
   * @param[in]  aOuterBusyGuard  A outer busy guard
   * @param      aParams          Map of parameter values; any default
   *                              parameters for this command that aren't
   *                              included in this argument will be merged into
   *                              the set supplied to the code method
   * @param      aUseThreadPool   Run the command asynchronously in the
   *                              swatch::action::ThreadPool ; if equals false,
   *                              then the command is run synchronously
   */
  void exec(const BusyGuard* aOuterBusyGuard, const core::XParameterSet& aParams , bool aUseThreadPool = true );

  /**
   * @brief      Returns current state of this command
   *
   * @return     The current state.
   */
  State getState() const;

  /**
   * @brief      Returns snapshot of this command's current status (state flag
   *             value, running time, progress fraction, status message and
   *             result) as a CommandStatus instance
   *
   * @return     A snapshot of the current status.
   */
  CommandSnapshot getStatus() const;

  
  /**
   * @brief      registers a new parameter for this command
   *
   * @param[in]  aName          The name of the new parameter
   * @param[in]  aDefaultValue  Default value for this command
   *
   * @tparam     T              Type of the parameter
   */
  template<typename T, typename R = typename DefaultCmdParRule<T>::type >
  void registerParameter(const std::string& aName, const T& aDefaultValue, R aRule = R()); 

  /**
   * @brief      Gets the default parameters.
   *
   * @return     The default parameters.
   */
  const core::ReadWriteXParameterSet& getDefaultParams() const;

  /**
   * @brief      Gets the default result.
   *
   * @return     The default result.
   */
  const xdata::Serializable& getDefaultResult() const;

  /**
   * @brief      De-register a parameter
   *
   * @param[in]  aName  Name of the parameter to de-register.
   */
  void unregisterParameter( const std::string aName );

  /**
   * @brief      Adds a new input parameter constraint.
   *
   * @param[in]  aName        Name of the new constraint.
   * @param[in]  aConstraint  Constraint object.
   *
   * @tparam     C            Constraint object class.
   */
  template<typename C>
  void addConstraint( const std::string& aName, const C& aConstraint );


  /**
   * @brief      Removes a parameter constraint.
   *
   * @param[in]  aName  Name of the constraint.
   */
  void removeConstraint( const std::string& aName );

  struct ParamRuleViolation {
    ParamRuleViolation(const std::vector<std::string>& aParams, const std::string& aRuleDescription, const std::string& aDetails);
    std::vector<std::string> parameters;
    //! Static description of the rule/constraint
    std::string ruleDescription;
    //! Details of rule violation from the XMatch object
    std::string details;
  };

  void checkRulesAndConstraints(const core::XParameterSet& aParams, std::vector<ParamRuleViolation>& aRuleViolations) const;

protected:

  /**
   * @brief      User-defined code for execution
   *
   * @param[in]  aParams  Input parameters.
   *
   * @return     Command execution status.
   */
  virtual State code( const core::XParameterSet& aParams ) = 0;

  /**
   * @brief      Templated command construcotr.
   *
   * @param[in]  aId        Name of the new command.
   * @param      aResource  Reference of the command target.
   * @param[in]  aDefault   Command default result.
   *
   * @tparam     T          Class of the command result object.
   */
  template<typename T>
  Command( const std::string& aId , ActionableObject& aResource, const T& aDefault );

  /**
   * @brief      Templated command construcotr.
   *
   * @param[in]  aId        Name of the new command.
   * @param[in]  aAlias     Alias for the new command (no character restrictions)
   * @param      aResource  Reference of the command target.
   * @param[in]  aDefault   Command default result.
   *
   * @tparam     T          Class of the command result object.
   */
  template<typename T>
  Command( const std::string& aId , const std::string& aAlias, ActionableObject& aResource, const T& aDefault );

  /**
   * @brief      Set command's execution progress.
   *
   * @param[in]  aProgress  Progress level, valid range [0,1]
   */
  void setProgress( float aProgress );

  /**
   * @brief      Set command's execution progress, and status message.
   *
   * @param[in]  aProgress  Progress level, valid range [0,1]
   * @param[in]  aMsg       Status message
   */
  void setProgress( float aProgress, const std::string& aMsg );

  /**
   * @brief      Set value of result from current command execution.
   *
   * @param[in]  aResult  Result object.
   */
  void setResult( const xdata::Serializable& aResult );

  /**
   * @brief      Set command's status message
   *
   * @param[in]  aMsg  A message
   */
  void setStatusMsg( const std::string& aMsg );

  /**
   * @brief      Adds serializable to command's "detailed info" XParameterSet for this invocation
   *
   * @param[in]  aId        A identifier
   * @param[in]  aInfoItem  A information item
   *
   * @tparam     T          Info object class.
   */
  template<typename T>
  void addExecutionDetails(const std::string& aId, const T& aInfoItem);

private:

  /**
   * @brief      Reset state, with state set to scheduled, and user-supplied
   *             parameter values used in next execution of command's 'code' method
   *
   * @param[in]  aParams  Parameters that will be used when running command
   */
  void resetForRunning(const core::XParameterSet& aParams);

  /**
   * @brief      Merges a parameter set with the default parameter set. Default
   *             values are only used if not present in params.
   *
   * @param[in]  aParams  Set of external parameters to merge with defaults
   *
   * @return     Merged parameter set
   */
  core::ReadOnlyXParameterSet mergeParametersWithDefaults(const core::XParameterSet& aParams) const;

  //! thread safe exception catching wrapper for code()
  void runCode(boost::shared_ptr<BusyGuard> aGuard, const core::XParameterSet& aParams );

  //! Signature of result cloner
  typedef xdata::Serializable* (*ResultXCloner_t)( const xdata::Serializable* );

  //! Container for parameter rules
  typedef boost::unordered_map<std::string, boost::shared_ptr<core::AbstractXRule> > XRuleMap_t;

  //! Container for parameter rules
  typedef boost::unordered_map<std::string, boost::shared_ptr<core::XPSetConstraint> > XConstraintMap_t;

  //! Reference to the Command's resource status
  ActionableStatus& mActionableStatus;

  //! Set of default parameters
  core::ReadWriteXParameterSet mDefaultParams;

  //! Set of parameters used for running
  core::ReadOnlyXParameterSet mRunningParams;

  XRuleMap_t mRules;

  XConstraintMap_t mConstraints;

  //! Default command's return value. 
  const xdata::Serializable* const mDefaultResult;

  // Current state of the command
  State mState;

  //! 
  timeval mExecStartTime;
  timeval mExecEndTime;

  //! Progress of the current command.
  float mProgress;

  //! Status string.
  std::string mStatusMsg;

  //! Result of the command execution
  boost::shared_ptr<xdata::Serializable> mResult;

  //! Detailed information about the command execution
  core::ReadOnlyXParameterSet mExecutionDetails;

  //! Mutex for thread safety of read-/write-access of member data
  mutable boost::mutex mMutex;

  //! Creates copy of xdata::Serializable object of type T on heap
  template<typename T>
  static xdata::Serializable* clone( const xdata::Serializable* other );

  //! Used to clone default result into result_ at start of execution, just before the code method is called
  ResultXCloner_t mResultCloner;
};


/**
 * @brief      Provides a snapshot of the progress/status of a swatch::action::Command
 */
class CommandSnapshot : public ActionSnapshot {
public:
  CommandSnapshot(const IdAliasPair& aCommand, const IdAliasPair& aActionable, State aState, float aRunningTime, float aProgress, const std::string& aStatusMsg, const core::ReadOnlyXParameterSet& aParamSet, const core::ReadOnlyXParameterSet& aExecutionDetails, const boost::shared_ptr<const xdata::Serializable>& aResult);

  //! Returns fractional progress of command; range [0,1]
  float getProgress() const;

  //! Returns command's status message
  const std::string& getStatusMsg() const;

  //! Returns parameter set used for running the command.
  const core::XParameterSet& getParameters() const;

  //! Returns the list of detailed info
  const core::XParameterSet& getExecutionDetails() const;

  //! Returns command's result; equal to NULL if command has not already completed execution
  const xdata::Serializable* getResult() const;

private:

  //! Progress status of the command
  float mProgress;

  //! Status message
  std::string mStatusMsg;

  //! List of parameters used to run the command
  core::ReadOnlyXParameterSet mParams;

  //! Additional details on the command status
  core::ReadOnlyXParameterSet mExecutionDetails;

  //! Result of the command execution
  boost::shared_ptr<const xdata::Serializable> mResult;
};

SWATCH_DEFINE_EXCEPTION(ParameterNotFound);

SWATCH_DEFINE_EXCEPTION(CommandParameterRegistrationFailed);

SWATCH_DEFINE_EXCEPTION(CommandConstraintRegistrationFailed);

SWATCH_DEFINE_EXCEPTION(CommandParameterCheckFailed);

SWATCH_DEFINE_EXCEPTION(OutOfRange);


} // namespace action
} // namespace swatch


#include "swatch/action/Command.hxx"


#endif /* __SWATCH_ACTION_COMMAND_HPP__ */

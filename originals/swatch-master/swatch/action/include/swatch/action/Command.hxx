/*
 * File:   Command.hxx
 * Author: kreczko
 *
 * Created on March 3, 2015
 */

#ifndef __SWATCH_ACTION_COMMAND_HXX__
#define __SWATCH_ACTION_COMMAND_HXX__


// IWYU pragma: private, include "swatch/action/Command.hpp"

#include "swatch/action/Command.hpp"
#include "swatch/action/ActionableObject.hpp"
#include "swatch/core/xoperators.hpp"
#include "swatch/core/XRule.hpp"
#include "swatch/core/XPSetConstraint.hpp"


namespace swatch {
namespace action {


// ----------------------------------------------------------------------------
// TODO (when migrated to full C++11) : Call other Command constructor to minimise duplication
template<typename T>
Command::Command( const std::string& aId , ActionableObject& aResource, const T& aDefault ) :
  ObjectFunctionoid( aId, "", aResource ),
  mActionableStatus(aResource.mStatus),
  mDefaultResult(new T(aDefault)),
  mState(kInitial),
  mProgress(0.),
  mStatusMsg("initialised"),
  mResult(),
  mMutex(),
  mResultCloner(static_cast<ResultXCloner_t>(clone<T>))
{
  // T must be derived from xdata::Serializable
  BOOST_STATIC_ASSERT((boost::is_base_of<xdata::Serializable, T>::value));
}


// ----------------------------------------------------------------------------
template<typename T>
Command::Command( const std::string& aId , const std::string& aAlias, ActionableObject& aResource, const T& aDefault ) :
  ObjectFunctionoid( aId, aAlias, aResource ),
  mActionableStatus(aResource.mStatus),
  mDefaultResult(new T(aDefault)),
  mState(kInitial),
  mProgress(0.),
  mStatusMsg("initialised"),
  mResult(),
  mMutex(),
  mResultCloner(static_cast<ResultXCloner_t>(clone<T>))
{
  // T must be derived from xdata::Serializable
  BOOST_STATIC_ASSERT((boost::is_base_of<xdata::Serializable, T>::value));
}


// ----------------------------------------------------------------------------
template<typename T, typename R>
void Command::registerParameter(const std::string& aName, const T& aDefaultValue, R aRule)
{
  // T must be derived from xdata::Serializable
  BOOST_STATIC_ASSERT_MSG( (boost::is_base_of<xdata::Serializable, T>::value) , "class T must be a descendant of xdata::Serializable" );
  
  // R must be derived from XRule<T>
  BOOST_STATIC_ASSERT_MSG( (std::is_base_of<core::XRule<T>, R>::value) , "class R must be a descendant of core::XRule<T>" );

  if ( getPath() != getId() ) {
    // The Command has already been registered. Parameters list cannot be modified
    XCEPT_RAISE(CommandParameterRegistrationFailed,"Registering parameter outside constructors is not allowed");
  }
  
  if (mDefaultParams.has(aName)) {
    std::ostringstream lExc;
    lExc << "Parameter " << aName << " is already registered";
    XCEPT_RAISE(CommandParameterRegistrationFailed,lExc.str());
  }

  core::XMatch lResult = aRule(aDefaultValue);
  if (!lResult.ok) {
    std::ostringstream lExc;
    lExc << "Command " << getId() << " - "
      << "The default value for parameter '" << aName << "' (" << aDefaultValue << ")"
      << " does not comply with rule '" << aRule << "'"; 
    XCEPT_RAISE(CommandParameterCheckFailed,lExc.str());
  }

  mDefaultParams.add(aName, aDefaultValue);

  mRules[aName] = boost::shared_ptr<core::AbstractXRule>(new R(aRule));
  
}


// ----------------------------------------------------------------------------
template<typename C>
void Command::addConstraint( const std::string& aName, const C& aConstraint ) {

  if ( getPath() != getId() ) {
    // The Command has already been registered. Parameters list cannot be modified
    XCEPT_RAISE(CommandConstraintRegistrationFailed,"Adding constraints outside constructors is not allowed");
  }

  if ( mConstraints.count(aName) ) {
    std::ostringstream lExc;
    lExc << "Command " << getId() << " - " 
      << "Constraint " << aName << " is already registered";
    XCEPT_RAISE(CommandConstraintRegistrationFailed,lExc.str());
  }

  core::XMatch lMatch = false;
  try {
    lMatch = aConstraint(mDefaultParams);
  } catch (const CommandParameterCheckFailed& lCheckFailed) {
    std::ostringstream lExc;
    lExc << "Command " << getId() << " - " 
         << "Default parameters do not satisfy the requirement for constraint " 
         << aName << "[" << aConstraint << "]" 
         << std::endl;
    lExc << lCheckFailed.what();
    XCEPT_RAISE(CommandConstraintRegistrationFailed,lExc.str());
  }

  if ( !lMatch.ok ) {
    // Complain here
    std::ostringstream lExc;
    lExc << "Default parameters do not comply with constraint " << aName << "[" << aConstraint << "]";
    XCEPT_RAISE(CommandConstraintRegistrationFailed,lExc.str());
    
  }
  
  mConstraints[aName] = boost::shared_ptr<C>(new C(aConstraint));
}


// ----------------------------------------------------------------------------
template<typename T>
void Command::addExecutionDetails(const std::string& aId, const T& aInfoItem)
{
  // T must be derived from xdata::Serializable
  BOOST_STATIC_ASSERT_MSG( (boost::is_base_of<xdata::Serializable, T>::value) , "class T must be a descendant of xdata::Serializable" );

  boost::unique_lock<boost::mutex> lLock(mMutex);
  if (mExecutionDetails.has(aId))
    mExecutionDetails.erase(aId);
  mExecutionDetails.adopt(aId, boost::shared_ptr<T>(new T(aInfoItem)));
}


// ----------------------------------------------------------------------------
template<typename T>
xdata::Serializable* Command::clone(const xdata::Serializable* aOther)
{
  // T must be derived from xdata::Serializable
  BOOST_STATIC_ASSERT((boost::is_base_of<xdata::Serializable, T>::value));

  const T* lXOther = dynamic_cast<const T*>(aOther);
  return new T(*lXOther);
}


} // namespace action
} // namespace swatch

#endif	/* __SWATCH_ACTION_COMMAND_HXX__ */

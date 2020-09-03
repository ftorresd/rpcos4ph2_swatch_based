
#include "swatch/action/SystemBusyGuard.hpp"


// Standard headers
#include <map>
#include <set>                          // for set, set<>::const_iterator
#include <sstream>                      // for operator<<, basic_ostream, etc
#include <stdexcept>                    // for runtime_error
#include <utility>                      // for pair
#include <vector>                       // for vector

// boost headers
#include "boost/lexical_cast.hpp"       // for lexical_cast

//log4cplus headers
#include <log4cplus/loggingmacros.h>

// SWATCH headers
#include "swatch/action/ActionableObject.hpp"
#include "swatch/action/ActionableStatus.hpp"
#include "swatch/action/BusyGuard.hpp"
#include "swatch/action/SystemFunctionoid.hpp"


namespace swatch {
namespace action {


//------------------------------------------------------------------------------------
SystemBusyGuard::SystemBusyGuard(SystemFunctionoid& aAction, ActionableSystem::StatusContainer& aStatusMap, const ActionableSystem::GuardMap_t& aStatusGuardMap, const Callback_t& aCallback) :
  mSystem(aAction.getActionable()),
  mSysStatus(aStatusMap.getSystemStatus()),
  mAction(aAction),
  mPostActionCallback(aCallback)
{
  ActionableStatusGuard& lSysGuard = *aStatusGuardMap.at(&mSystem);
  ActionableStatus& lSysStatus = aStatusMap.getSystemStatus();

  // 1) Check that the system is not busy
  ActionableSystem::Status_t lSysSnapshot = lSysStatus.getSnapshot(lSysGuard);
  if ( !lSysSnapshot.isAlive() || lSysSnapshot.isRunning() ) {
    std::ostringstream oss;
    oss << "Could not run action '" << mAction.getId() << "' on resource '" << mSystem.getPath() << "'. ";

    if ( lSysSnapshot.isRunning() )
      oss << "Resource currently busy running functionoid '" << lSysSnapshot.getLastRunningAction()->getId() << "'.";
    else
      oss << "Actions currently disabled on this resource.";

    LOG4CPLUS_INFO(mSystem.getLogger(), oss.str());
    XCEPT_RAISE(ActionableObjectIsBusy,oss.str());
  }

  // Generate list of ENABLED participating children
  std::set<ActionableObject*> lEnabledParticipants;
  typedef std::set<ActionableObject*>::const_iterator ChildIt_t;
  for (ChildIt_t lIt = aAction.getParticipants().begin(); lIt != aAction.getParticipants().end(); lIt++) {
    const ActionableStatusGuard& lChildGuard = *aStatusGuardMap.at(*lIt);
    if (aStatusMap.getStatus(**lIt).isEnabled(lChildGuard))
      lEnabledParticipants.insert(*lIt);
  }

  // 2) Check that none of the children are busy
  for (ChildIt_t lIt = lEnabledParticipants.begin(); lIt != lEnabledParticipants.end(); lIt++) {
    const ActionableStatus& lChildStatus = aStatusMap.getStatus(**lIt);
    const ActionableStatusGuard& lChildGuard = *aStatusGuardMap.at(*lIt);

    if ( !lChildStatus.isAlive(lChildGuard) || lChildStatus.isBusy(lChildGuard) ) {
      std::ostringstream oss;
      oss << "Could not run system action '" << mAction.getId() << "' due to child resource '" << (*lIt)->getPath() << "'. ";

      if ( lChildStatus.isBusy(lChildGuard) )
        oss << "Child currently busy running functionoid '" << lChildStatus.getLastRunningAction(lChildGuard)->getId() << "'.";
      else
        oss << "Actions currently disabled on this child.";

      LOG4CPLUS_INFO(mSystem.getLogger(), oss.str());
      XCEPT_RAISE(ActionableObjectIsBusy,oss.str());
    }
  }

  // 3) If got this far, then all is good:
  //    a) wait until actions ready to run on system and child objects; then ...
  //    b) create the busy guards for the children
  LOG4CPLUS_INFO(mSystem.getLogger(), "Starting system action '" << mAction.getId() << "'");

  std::vector<std::pair<ActionableStatus*, ActionableStatusGuard*> > lStatusVec;
  lStatusVec.push_back( std::pair<ActionableStatus*, ActionableStatusGuard*>(&lSysStatus, &lSysGuard) );
  for (ChildIt_t lIt = lEnabledParticipants.begin(); lIt != lEnabledParticipants.end(); lIt++)
    lStatusVec.push_back( std::pair<ActionableStatus*, ActionableStatusGuard*>(&aStatusMap.getStatus(**lIt), aStatusGuardMap.at(*lIt).get()) );
  ActionableStatus::waitUntilReadyToRunAction(lStatusVec, mAction);

  for (ChildIt_t lIt = lEnabledParticipants.begin(); lIt != lEnabledParticipants.end(); lIt++) {
    ActionableStatusGuard& lChildGuard = *aStatusGuardMap.at(*lIt).get();
    ActionableStatus& lChildStatus = aStatusMap.getStatus(**lIt);
    mChildGuardMap[ *lIt ] = ChildGuardPtr_t(new BusyGuard(**lIt, lChildStatus, lChildGuard, mAction, BusyGuard::Adopt()) );
  }
}


//------------------------------------------------------------------------------------
const BusyGuard& SystemBusyGuard::getChildGuard(const ActionableObject& aChild) const
{
  std::map<const ActionableObject*, ChildGuardPtr_t>::const_iterator lIt = mChildGuardMap.find(&aChild);
  if (lIt == mChildGuardMap.end())
    XCEPT_RAISE(core::RuntimeError,"Non-participating/disabled object '"+aChild.getPath()+"' passed to SystemBusyGuard::getChildGuard(...) for system '"+mSystem.getPath()+"', action '"+mAction.getId()+"'");
  return *(lIt->second);
}


//------------------------------------------------------------------------------------
SystemBusyGuard::~SystemBusyGuard()
{
  ActionableStatusGuard lSysGuard(mSysStatus);

  if ( mSysStatus.isBusy(lSysGuard) && (&mAction == mSysStatus.getLastRunningAction(lSysGuard)) ) {
    mSysStatus.popAction(lSysGuard);

    std::ostringstream lLogSuffixStream;
    if ( !mPostActionCallback.empty() )
      mPostActionCallback(lSysGuard, lLogSuffixStream);

    std::string lLogMsgSuffix = (lLogSuffixStream.tellp() > 0 ? ". " + lLogSuffixStream.str() : std::string());
    LOG4CPLUS_INFO(mSystem.getLogger(), "Finished system action '" << mAction.getId() << lLogMsgSuffix);
  }
  else {
    size_t lNrActions = mSysStatus.getRunningActions(lSysGuard).size();
    const std::string activeFuncId(lNrActions > 0 ? "NULL" : "'" + mSysStatus.getLastRunningAction(lSysGuard)->getId() + "' (innermost of "+boost::lexical_cast<std::string>(lNrActions)+")");
    LOG4CPLUS_ERROR(mSystem.getLogger(),
                    "unexpected active functionoid " << activeFuncId << "  in SystemBusyGuard destructor for action '" << mAction.getId() << "'");
  }
}


}
}

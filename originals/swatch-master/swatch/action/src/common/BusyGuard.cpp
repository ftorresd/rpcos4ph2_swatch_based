
#include "swatch/action/BusyGuard.hpp"


#include <algorithm>                    // for count
#include <sstream>                      // for operator<<, basic_ostream, etc
#include <stddef.h>                     // for NULL
#include <vector>                       // for vector

#include "boost/lexical_cast.hpp"       // for lexical_cast

#include "log4cplus/loggingmacros.h"

#include "swatch/action/ActionableObject.hpp"  // for ActionableObject, etc
#include "swatch/action/Command.hpp"
#include "swatch/action/CommandSequence.hpp"
#include "swatch/action/Functionoid.hpp"  // for Functionoid
#include "swatch/action/ObjectFunctionoid.hpp"  // for ObjectFunctionoid
#include "swatch/action/StateMachine.hpp"


using namespace std;

namespace swatch {
namespace action {


//------------------------------------------------------------------------------------
BusyGuard::BusyGuard(ObjectFunctionoid& aAction, ActionableStatus& aStatus, const BusyGuard* aOuterGuard) :
  mActionableObj(aAction.getActionable()),
  mStatus(aStatus),
  mAction(aAction),
  mOuterGuard(aOuterGuard)
{
  ActionableStatusGuard lGuard(mStatus);
  initialise(lGuard);
}


//------------------------------------------------------------------------------------
BusyGuard::BusyGuard(ObjectFunctionoid& aAction, ActionableStatus& aStatus, ActionableStatusGuard& aStatusGuard, const Callback_t& aCallback, const BusyGuard* aOuterGuard) :
  mActionableObj(aAction.getActionable()),
  mStatus(aStatus),
  mAction(aAction),
  mOuterGuard(aOuterGuard),
  mPostActionCallback(aCallback)
{
  initialise(aStatusGuard);
}


//------------------------------------------------------------------------------------
BusyGuard::BusyGuard(ActionableObject& aResource, ActionableStatus& aStatus, ActionableStatusGuard& aStatusGuard, const Functionoid& aAction, const BusyGuard* aOuterGuard) :
  mActionableObj(aResource),
  mStatus(aStatus),
  mAction(aAction),
  mOuterGuard(aOuterGuard)
{
  initialise(aStatusGuard);
}


//------------------------------------------------------------------------------------
BusyGuard::BusyGuard(ActionableObject& aResource, ActionableStatus& aStatus, ActionableStatusGuard& aStatusGuard, const Functionoid& aAction, const Adopt) :
  mActionableObj(aResource),
  mStatus(aStatus),
  mAction(aAction),
  mOuterGuard(NULL)
{
  if ( mStatus.getSnapshot(aStatusGuard).isActionWaitingToRun() )
    XCEPT_RAISE(WrongBusyGuard,"BusyGuard cannot adopt action '"+mAction.getPath()+"' on object '"+mActionableObj.getPath()+"' : Object is currently waiting for an action to run");
  else if ( mStatus.getRunningActions(aStatusGuard).size() != size_t(1) )
    XCEPT_RAISE(WrongBusyGuard,"BusyGuard cannot adopt action '"+mAction.getPath()+"' on object '"+mActionableObj.getPath()+"' : Action stack contains "+boost::lexical_cast<std::string>(mStatus.getRunningActions(aStatusGuard).size())+" actions");
  else if ( mStatus.getRunningActions(aStatusGuard).at(0) != &mAction )
    XCEPT_RAISE(WrongBusyGuard,"BusyGuard cannot adopt action '"+mAction.getPath()+"' on object '"+mActionableObj.getPath()+"' : Object is running "+ActionFmt(mStatus.getRunningActions(aStatusGuard).at(0)).str());

  LOG4CPLUS_INFO(mActionableObj.getLogger(), "Starting " << ActionFmt(&mAction) << " (adopted)");
}


//------------------------------------------------------------------------------------
void BusyGuard::initialise(ActionableStatusGuard& aStatusGuard)
{
  ActionableSnapshot lStatusSnapshot = mStatus.getSnapshot(aStatusGuard);
  // Consistency checks on outer busy guard
  if (mOuterGuard != NULL) {
    if (&mOuterGuard->mActionableObj != &mActionableObj)
      XCEPT_RAISE(WrongBusyGuard, "Incompatible outer BusyGuard, resource='"+mOuterGuard->mActionableObj.getPath()+"'. Inner guard resource is '"+mActionableObj.getPath() );
    else if ( lStatusSnapshot.getRunningActions().empty() )
      XCEPT_RAISE(WrongBusyGuard, "Outer BusyGuard used (resource: '"+mActionableObj.getPath()+"', action: '"+mOuterGuard->mAction.getId()+"'), but resource not busy");
    else if ( &mOuterGuard->mAction != lStatusSnapshot.getLastRunningAction() )
      XCEPT_RAISE(WrongBusyGuard, "Outer BusyGuard (resource: '"+mActionableObj.getPath()+"', action: '"+mOuterGuard->mAction.getId()+"') is not for current action '"+lStatusSnapshot.getLastRunningAction()->getId()+"'" );
  }

  // 1) Check that this this action isn't already running
  if (std::count(lStatusSnapshot.getRunningActions().begin(), lStatusSnapshot.getRunningActions().end(), &mAction) > 0 )
    XCEPT_RAISE(ActionableObjectIsBusy, "Action '"+mAction.getId()+"' is already running on resource '"+mActionableObj.getPath()+"'" );

  // 2) Claim the resource if free; else throw if can't get sole control of it
  if ( lStatusSnapshot.isAlive() && ( (mOuterGuard != NULL) || !lStatusSnapshot.isRunning() ) ) {
    if ( !lStatusSnapshot.isRunning() ) {
      LOG4CPLUS_INFO(mActionableObj.getLogger(), "Starting " << ActionFmt(&mAction));
      mStatus.waitUntilReadyToRunAction(mAction, aStatusGuard);
    }
    else {
      LOG4CPLUS_INFO(mActionableObj.getLogger(), "Starting " << ActionFmt(&mAction) << " within " << ActionFmt(lStatusSnapshot.getLastRunningAction()));
      mStatus.addAction(mAction, aStatusGuard);
    }
  }
  else {
    std::ostringstream oss;
    oss << "Could not run action '" << mAction.getId() << "' on resource '" << mActionableObj.getPath() << "'. ";

    if ( lStatusSnapshot.isRunning() )
      oss << "Resource currently busy running functionoid '" << lStatusSnapshot.getLastRunningAction()->getId() << "'.";
    else
      oss << "Actions currently disabled on this resource.";

    LOG4CPLUS_INFO(mActionableObj.getLogger(), oss.str());
    XCEPT_RAISE(ActionableObjectIsBusy,oss.str());
  }
}

//------------------------------------------------------------------------------------
BusyGuard::~BusyGuard()
{
  ActionableStatusGuard lGuard(mStatus);
  ActionableSnapshot lStatusSnapshot = mStatus.getSnapshot(lGuard);

  if ( lStatusSnapshot.isRunning() && (&mAction == lStatusSnapshot.getLastRunningAction()) ) {
    mStatus.popAction(lGuard);

    std::ostringstream lLogSuffixStream;
    if ( ! mPostActionCallback.empty() )
      mPostActionCallback(lGuard, lLogSuffixStream);

    std::string lLogMsgSuffix = (lLogSuffixStream.tellp() > 0 ? ". " + lLogSuffixStream.str() : std::string());
    LOG4CPLUS_INFO(mActionableObj.getLogger(), "Finished " << ActionFmt(&mAction) << lLogMsgSuffix);
  }
  else {
    size_t lNrActions = lStatusSnapshot.getRunningActions().size();
    const std::string activeFuncId(lNrActions > 0 ? "NULL" : "'" + lStatusSnapshot.getLastRunningAction()->getId() + "' (innermost of "+boost::lexical_cast<std::string>(lNrActions)+")");
    LOG4CPLUS_ERROR(mActionableObj.getLogger(), "Unexpected active functionoid " << activeFuncId << "  in BusyGuard destructor for " << ActionFmt(&mAction));
  }
}


BusyGuard::ActionFmt::ActionFmt(const Functionoid* aAction) :
  action(aAction)
{
}


BusyGuard::ActionFmt::~ActionFmt()
{
}


std::string BusyGuard::ActionFmt::str() const
{
  return boost::lexical_cast<std::string>(*this);
}

std::ostream& operator<<(std::ostream& aStream, const BusyGuard::ActionFmt& aActionFmt)
{
  if (dynamic_cast<const Command*>(aActionFmt.action) != NULL)
    aStream << "command";
  else if (dynamic_cast<const CommandSequence*>(aActionFmt.action) != NULL)
    aStream << "sequence";
  else if (dynamic_cast<const Transition*>(aActionFmt.action) != NULL)
    aStream << "transition";
  else
    aStream << "action";

  if ( aActionFmt.action == NULL )
    aStream << " NULL";
  else
    aStream << " '" << aActionFmt.action->getId() << "'";

  return aStream;
}

}
}


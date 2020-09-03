
#include "swatch/action/ActionableSystem.hpp"


// Standard headers
#include <map>
#include <sstream>                      // for operator<<, basic_ostream, etc
#include <stdexcept>                    // for runtime_error
#include <typeinfo>                     // for type_info
#include <utility>                      // for pair
#include <vector>                       // for vector

//log4cplus headers
#include <log4cplus/loggingmacros.h>
#include "log4cplus/logger.h"           // for Logger

// SWATCH headers
#include "swatch/logger/Logger.hpp"
#include "swatch/action/ActionableObject.hpp"
#include "swatch/action/ActionableStatus.hpp"
#include "swatch/core/LeafObject.hpp"
#include "swatch/action/SystemStateMachine.hpp"
#include "swatch/core/utilities.hpp"



namespace swatch {
namespace action {


ActionableSystem::StatusContainer::StatusContainer(const ActionableSystem& aSystem, ActionableStatus& aSysStatus) :
  mSystem(aSystem),
  mSysStatus(aSysStatus)
{
}

ActionableSystem::StatusContainer::~StatusContainer()
{
}

const ActionableStatus& ActionableSystem::StatusContainer::getSystemStatus() const
{
  return mSysStatus;
}

const ActionableStatus& ActionableSystem::StatusContainer::getStatus(const ActionableObject& aChild ) const
{
  return *mChildStatusMap.at(&aChild);
}

ActionableStatus& ActionableSystem::StatusContainer::getSystemStatus()
{
  return mSysStatus;
}


ActionableStatus& ActionableSystem::StatusContainer::getStatus(const ActionableObject& aChild )
{
  return *mChildStatusMap.at(&aChild);
}


ActionableSystem::GuardMap_t ActionableSystem::StatusContainer::lockMutexes() const
{
  // 1) Lock all of the actionable status instances
  std::vector<const ActionableStatus*> lStatusVec;
  lStatusVec.push_back(&mSysStatus);
  for (auto lIt = mChildStatusMap.begin(); lIt != mChildStatusMap.end(); lIt++)
    lStatusVec.push_back(lIt->second);
  ActionableStatusGuardVec_t lGuardVec = swatch::action::lockMutexes<>(lStatusVec.begin(), lStatusVec.end());

  // 2) Create map of actionable system/object to the corresponding actionable status guard instance
  ActionableSystem::GuardMap_t lGuardMap;
  lGuardMap[&mSystem] = *lGuardVec.begin();
  auto lChildStatusMapIt = mChildStatusMap.begin();
  for (auto lGuardIt = (lGuardVec.begin()+1); lGuardIt != lGuardVec.end(); lGuardIt++, lChildStatusMapIt++)
    lGuardMap[lChildStatusMapIt->first] = *lGuardIt;

  return lGuardMap;
}


//------------------------------------------------------------------------------------
ActionableSystem::ActionableSystem(const std::string& aId, const std::string& aAlias, const std::string& aLoggerName) :
  MonitorableObject(aId, aAlias),
  mStatus(),
  mStatusMap(*this, mStatus),
  mLogger(swatch::logger::Logger::getInstance(aLoggerName))
{
}


//------------------------------------------------------------------------------------
ActionableSystem::~ActionableSystem()
{
}

//------------------------------------------------------------------------------------
std::set<std::string> ActionableSystem::getStateMachines() const
{
  std::set<std::string> lNames;
  for (StateMachineMap_t::const_iterator lIt=mFSMs.begin(); lIt!=mFSMs.end(); lIt++)
    lNames.insert( lIt->first );
  return lNames;
}


//------------------------------------------------------------------------------------
SystemStateMachine& ActionableSystem::getStateMachine( const std::string& aId )
{
  try {
    return *(mFSMs.at( aId ));
  }
  catch ( const std::out_of_range& e ) {
    XCEPT_RAISE(StateMachineNotFoundInActionableObject,  "Unable to find state machine with ID '" + aId + "' in object '" + getPath() + "'");
  }
}


//------------------------------------------------------------------------------------
ActionableSystem::Status_t ActionableSystem::getStatus() const
{
  ActionableStatusGuard lGuard(mStatus);
  return mStatus.getSnapshot(lGuard);
}


//------------------------------------------------------------------------------------
const ActionableSystem::ActionableChildMap_t& ActionableSystem::getActionableChildren()
{
  return mActionableChildren;
}

//------------------------------------------------------------------------------------
SystemStateMachine& ActionableSystem::registerStateMachine( const std::string& aId, const std::string& aInitialState, const std::string& aErrorState )
{
  if (mFSMs.count(aId))
    XCEPT_RAISE(StateMachineAlreadyExistsInActionableObject,  "State machine With ID '"+aId+"' already exists" );

  SystemStateMachine* lFSM = new SystemStateMachine(aId, *this, mStatusMap, aInitialState, aErrorState);
  addObj(lFSM);
  mFSMs.insert( std::make_pair( aId , lFSM ) );
  return *lFSM;
}


//------------------------------------------------------------------------------------
void ActionableSystem::addActionable(ActionableObject* aChildActionable)
{
  addMonitorable(aChildActionable, ActionableObject::Deleter());

  mStatusMap.mChildStatusMap[aChildActionable] = &aChildActionable->mStatus;

  mActionableChildren[aChildActionable->getId()] = aChildActionable;
}


//------------------------------------------------------------------------------------
void ActionableSystem::Deleter::operator ()(LeafObject* aObject)
{
  if (ActionableSystem* lActionableSys = dynamic_cast<ActionableSystem*>(aObject)) {
    LOG4CPLUS_INFO(lActionableSys->getLogger(), aObject->getPath() << " : ActionableSystem deleter called");

    {
      ActionableStatusGuard lGuard(lActionableSys->mStatus);
      lActionableSys->mStatus.kill(lGuard);
    }

    //TODO (low-ish priority): Eventually replace this "spinning" do-loop with a more efficient implementation based on ActionableSystem/Functionoid methods that use conditional variables behind-the-scenes
    do {
    }
    while ( ! lActionableSys->getStatus().getRunningActions().empty() );

    LOG4CPLUS_INFO(lActionableSys->getLogger(), aObject->getPath() << " : ActionableSystem now being deleted");

    delete lActionableSys;
  }
  else {
    log4cplus::Logger lLogger = swatch::logger::Logger::getInstance("swatch.action.ActionableSystem");
    LOG4CPLUS_WARN(lLogger,
                   "ActionableSystem::Deleter being used on object '" << aObject->getPath() << "' of type '"
                   << core::demangleName(typeid(*aObject).name()) << "' that doesn't inherit from ActionableSystem");
    delete aObject;
  }
}


log4cplus::Logger& ActionableSystem::getLogger()
{
  return mLogger;
}

}
}

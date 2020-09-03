
#include "swatch/action/Functionoid.hpp"


#include <ostream>


namespace swatch {
namespace action {

Functionoid::Functionoid( const std::string& aId, const std::string& aAlias ) :
  LeafObject( aId, aAlias )
{
}


Functionoid::~Functionoid()
{
}


std::ostream& operator<<(std::ostream& aStream, Functionoid::State aState)
{
  switch (aState) {
    case Functionoid::kInitial   :
      aStream << "Initial";
      break;
    case Functionoid::kScheduled :
      aStream << "Scheduled";
      break;
    case Functionoid::kRunning   :
      aStream << "Running";
      break;
    case Functionoid::kWarning   :
      aStream << "Warning";
      break;
    case Functionoid::kError     :
      aStream << "Error";
      break;
    case Functionoid::kDone      :
      aStream << "Done";
      break;
    default :
      aStream << "Unknown value of swatch::action::Functionoid::State enum";
  }
  return aStream;
}




ActionSnapshot::ActionSnapshot(const IdAliasPair& aAction, const IdAliasPair& aActionable, State aState, float aRunningTime) :
  mPath(aAction.idPath),
  mAlias(aAction.alias),
  mActionableId(aActionable.idPath.substr(aActionable.idPath.rfind('.')+1)),
  mActionableAlias(aActionable.alias),
  mState(aState),
  mRunningTime(aRunningTime)
{}


ActionSnapshot::~ActionSnapshot()
{
}


const std::string& ActionSnapshot::getActionPath() const
{
  return mPath;
}


std::string ActionSnapshot::getActionId() const
{
  return mPath.substr(mPath.rfind('.')+1);
}


const std::string& ActionSnapshot::getActionableId() const
{
  return mActionableId;
}


ActionSnapshot::State ActionSnapshot::getState() const
{
  return mState;
}


float ActionSnapshot::getRunningTime() const
{
  return mRunningTime;
}


} /* namespace action */
} /* namespace swatch */

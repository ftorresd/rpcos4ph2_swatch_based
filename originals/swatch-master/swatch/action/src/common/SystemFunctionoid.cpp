
#include "swatch/action/SystemFunctionoid.hpp"



namespace swatch {
namespace action {


const ActionableSystem& SystemFunctionoid::getActionable() const
{
  return mActionable;
}

ActionableSystem& SystemFunctionoid::getActionable()
{
  return mActionable;
}

const std::set<ActionableObject*>& SystemFunctionoid::getParticipants()
{
  return mParticipants;
}

SystemFunctionoid::SystemFunctionoid(const std::string& aId, const std::string& aAlias, ActionableSystem& aActionable) :
  Functionoid(aId, aAlias),
  mActionable(aActionable)
{
}


void SystemFunctionoid::addParticipant(ActionableObject& aObj)
{
  mParticipants.insert(&aObj);
}


} // namespace action
} // namespace swatch


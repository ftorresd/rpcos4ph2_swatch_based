
#include "swatch/action/ObjectFunctionoid.hpp"


namespace swatch {
namespace action {


//------------------------------------------------------------------------------------
ObjectFunctionoid::ObjectFunctionoid(const std::string& aId, const std::string& aAlias, ActionableObject& aActionable) :
  Functionoid(aId, aAlias),
  mActionable(aActionable)
{
}


//------------------------------------------------------------------------------------
const ActionableObject& ObjectFunctionoid::getActionable() const
{
  return mActionable;
}


//------------------------------------------------------------------------------------
ActionableObject& ObjectFunctionoid::getActionable()
{
  return mActionable;
}


} // namespace action
} // namespace swatch


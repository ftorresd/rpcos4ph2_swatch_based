/*
 * File:   DummyCompositeActionable.cpp
 * Author: tom
 *
 * Created on 18 September 2015, 16:22
 */


#include "swatch/action/test/DummyActionableSystem.hpp"


namespace swatch {
namespace action {
namespace test {


DummyActionableSystem::DummyActionableSystem(const std::string& aId) :
  ActionableSystem(aId, "", "swatch.action.test." + aId)
{
}


DummyActionableSystem::DummyActionableSystem(const std::string& aId, const std::string& aAlias) :
  ActionableSystem(aId, aAlias, "swatch.action.test." + aId)
{
}


DummyActionableSystem::~DummyActionableSystem()
{
}


SystemStateMachine& DummyActionableSystem::registerStateMachine(const std::string& aId, const std::string& aInitialState, const std::string& aErrorState )
{
  return ActionableSystem::registerStateMachine(aId, aInitialState, aErrorState);
}

}
}
}

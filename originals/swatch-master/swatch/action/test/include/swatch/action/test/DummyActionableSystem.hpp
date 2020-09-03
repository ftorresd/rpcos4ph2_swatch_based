/*
 * File:   DummYActionableSystem.hpp
 * Author: tom
 *
 * Created on 18 September 2015, 16:22
 */

#ifndef __SWATCH_ACTION_TEST_DUMMYACTIONABLESYSTEM_HPP__
#define __SWATCH_ACTION_TEST_DUMMYACTIONABLESYSTEM_HPP__


#include "swatch/action/ActionableSystem.hpp"


namespace swatch {
namespace action {
namespace test {

class DummyActionableSystem : public ActionableSystem {
public:
  DummyActionableSystem(const std::string& aId);

  DummyActionableSystem(const std::string& aId, const std::string& aAlias);

  virtual ~DummyActionableSystem();

  // Expose protected method as public for unit testing ...
  SystemStateMachine& registerStateMachine(const std::string& aId, const std::string& aInitialState, const std::string& aErrorState );

  // Expose protected method as public for unit testing ...
  template< typename ObjType>
  ObjType& addActionable( ObjType* aChild);
};


template <class ObjType>
ObjType& DummyActionableSystem::addActionable(ObjType* aChild)
{
  BOOST_STATIC_ASSERT_MSG( (boost::is_base_of<ActionableObject, ObjType>::value) , "class ObjType must be derived from ActionableObject");

  const std::string& childId = aChild->getId();
  ActionableSystem::addActionable(aChild);
  return getObj<ObjType>(childId);
}

}
}
}


#endif  /* __SWATCH_ACTION_TEST_DUMMYACTIONABLESYSTEM_HPP__ */


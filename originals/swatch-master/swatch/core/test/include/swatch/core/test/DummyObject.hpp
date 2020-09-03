/**
 * @file    DummyObject.hpp
 * @author  Alessandro Thea
 * @brief   Dummy Object-derived class to exercise the Object interface
 * @date    20.08.2014
 *
 * Detailed description
 */

#ifndef __SWATCH_CORE_TEST_DUMMYOBJECT_HPP__
#define __SWATCH_CORE_TEST_DUMMYOBJECT_HPP__


#include "swatch/core/AbstractStub.hpp"
#include "swatch/core/Object.hpp"


namespace swatch {
namespace core {
namespace test {

//____________________________________________________________________________//
class DummyObject : public Object {
public:
  explicit DummyObject( const std::string& aId ) : Object( aId ) {}
  DummyObject( const std::string& aId, const std::string& aAlias ) :
    Object( aId, aAlias)
  {}
  DummyObject( const AbstractStub& aStub ) : Object( aStub.id ) {}
  virtual ~DummyObject() {}

  void add( swatch::core::Object* aChild )
  {
    this->Object::addObj(aChild);
  }
  void setValue( double aValue )
  {
    mValue = aValue;
  }
  double getValue()
  {
    return mValue;
  }

private:
  //! simple data member
  double mValue;
};

class DummyView : public  swatch::core::ObjectView {
public:
  explicit DummyView( const std::string& aId, const std::string& aAlias ) :
    swatch::core::ObjectView(aId, aAlias ) {}
  DummyView( const AbstractStub& aStub ) : swatch::core::ObjectView(aStub.id, aStub.alias) {}
  virtual ~DummyView() {}

  void add( swatch::core::Object* aChild, const std::string& aAlias )
  {
    this->ObjectView::addObj(aChild, aAlias);
  }
};

} // namespace swatch
} // namespace core
} // namespace test




#endif  /* SWATCH_CORE_TEST_SIMPLEOBJECT_HPP */


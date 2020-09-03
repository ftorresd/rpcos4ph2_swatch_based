#include <boost/test/unit_test.hpp>

// Swatch Headers
#include "swatch/core/XParameterSet.hpp"
#include "swatch/core/Object.hpp"
#include "swatch/core/AbstractFactory.hpp"
#include "swatch/core/test/DummyObject.hpp"

// Boost Headers
#include <boost/assign.hpp>
#include <boost/foreach.hpp>
#include "boost/scoped_ptr.hpp"


// Type definition
typedef swatch::core::AbstractFactory<swatch::core::Object> ObjFactory_t;

// Standard factory registration macros
#define SWATCH_TEST_REGISTER_OBJ( classname ) _SWATCH_ABSTRACT_REGISTER_CLASS( swatch::core::Object, classname )
#define SWATCH_TEST_REGISTER_OBJCREATOR( creatorname ) _SWATCH_ABSTRACT_REGISTER_CREATOR( swatch::core::Object, creatorname )


namespace swatch {
namespace core {
namespace test {

// Dummy Creator interface
/*
class SimpleCreator : public ObjFactory::CreatorInterface {
public:
    virtual swatch::core::Object* operator()(const std::string& aId, const swatch::core::XParameterSet& params);
};
*/
class DummyCreator : public ObjFactory_t::CreatorInterface {
public:
  virtual swatch::core::Object* operator()( const swatch::core::AbstractStub& aStub );
};

swatch::core::Object*
DummyCreator::operator ()( const swatch::core::AbstractStub& aStub )
{
  swatch::core::test::DummyObject* lObj = new swatch::core::test::DummyObject(aStub);
  lObj->setValue(1234.5678);
  return lObj;
}


SWATCH_TEST_REGISTER_OBJ(swatch::core::test::DummyObject)

SWATCH_TEST_REGISTER_OBJCREATOR(swatch::core::test::DummyCreator)


BOOST_AUTO_TEST_SUITE( FactoryTestSuite )

BOOST_AUTO_TEST_CASE( TestFactory )
{

  ObjFactory_t* lFactory = ObjFactory_t::get();
  swatch::core::AbstractStub lStub("d1");
//    swatch::core::XParameterSet none;
  boost::scoped_ptr<swatch::core::Object> lObj;
  lObj.reset(lFactory->make<swatch::core::Object>("swatch::core::test::DummyObject",lStub));

  BOOST_CHECK( typeid(lObj.get()) == typeid(swatch::core::Object*) );

  BOOST_CHECK( dynamic_cast<DummyObject*>(lObj.get()) != 0x0 );

  lObj.reset(lFactory->make<swatch::core::Object>("swatch::core::test::DummyCreator",lStub));

  BOOST_CHECK( typeid(lObj.get()) == typeid(swatch::core::Object*) );


  DummyObject* lDummyObj = dynamic_cast<DummyObject*>(lObj.get());

  BOOST_CHECK( lDummyObj != 0x0 );

  BOOST_CHECK( lDummyObj->getValue() == 1234.5678 );

}

BOOST_AUTO_TEST_SUITE_END() // CoreTestSuite

} /* namespace test */
} /* namespace core */
} /* namespace swatch */

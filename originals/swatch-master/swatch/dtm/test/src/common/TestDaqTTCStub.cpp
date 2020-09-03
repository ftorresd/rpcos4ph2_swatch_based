
#include <boost/test/unit_test.hpp>

// SWATCH headers
#include "swatch/dtm/DaqTTCStub.hpp"


namespace swatch {
namespace dtm {
namespace test {

struct DaqTTCStubTestSetup {
  DaqTTCStubTestSetup() {}
  ~DaqTTCStubTestSetup() {}

  DaqTTCStub createStub(const std::string& aId);
};

DaqTTCStub DaqTTCStubTestSetup::createStub(const std::string& aId)
{
  DaqTTCStub lStub(aId);

  lStub.loggerName = "myCoolLogger";
  lStub.creator = "MyDTMClass";
  lStub.role = "aDTMRole";
  lStub.uriT1 = "myT1uri";
  lStub.uriT2 = "myT2uri";
  lStub.addressTableT1 = "myT1addrTable";
  lStub.addressTableT2 = "myT2addrTable";
  lStub.crate = "myCoolCrate";
  lStub.crate = "myCoolCrate";
  lStub.fedId = 1;

  return lStub;
}


BOOST_AUTO_TEST_SUITE( DaqTTCStubTestSuite )


BOOST_AUTO_TEST_CASE(TestStubConstruction)
{
  // Check that fields are set to well-defined values by default constructor
  DaqTTCStub lStub("myTestDTM");

  BOOST_CHECK_EQUAL(lStub.id, "myTestDTM");
  BOOST_CHECK_EQUAL(lStub.alias, "");
  BOOST_CHECK_EQUAL(lStub.loggerName, "swatch");
  BOOST_CHECK_EQUAL(lStub.creator, "");
  BOOST_CHECK_EQUAL(lStub.role, "");
  BOOST_CHECK_EQUAL(lStub.uriT1, "");
  BOOST_CHECK_EQUAL(lStub.addressTableT1, "");
  BOOST_CHECK_EQUAL(lStub.uriT2, "");
  BOOST_CHECK_EQUAL(lStub.addressTableT2, "");
  BOOST_CHECK_EQUAL(lStub.crate, "");
  BOOST_CHECK_EQUAL(lStub.slot, uint32_t(13));
  BOOST_CHECK_EQUAL(lStub.fedId, uint16_t(0));

}


BOOST_FIXTURE_TEST_CASE(TestStubEqualOperator, DaqTTCStubTestSetup)
{
  // Testing strategy:
  //    - Create two identical DTM stubs, change value of field in one of them, check that they're no longer equal
  //    - Repeat for each field within DTM stub

  DaqTTCStub lStubA = createStub("myCoolDTM");
  DaqTTCStub lStubB = createStub("myCoolDTM");
  BOOST_REQUIRE(lStubA == lStubB);

  // DaqTTCStub::id
  lStubB.id = "anotherId";
  BOOST_CHECK_EQUAL(lStubA == lStubB, false);

  // DaqTTCStub::loggerName
  lStubB = createStub("myCoolDTM");
  BOOST_REQUIRE(lStubA == lStubB);
  lStubB.loggerName = "anotherLogger";
  BOOST_CHECK_EQUAL(lStubA == lStubB, false);

  // DaqTTCStub::creator
  lStubB = createStub("myCoolDTM");
  BOOST_REQUIRE(lStubA == lStubB);
  lStubB.creator = "anotherClass";
  BOOST_CHECK_EQUAL(lStubA == lStubB, false);

  // DaqTTCStub::role
  lStubB = createStub("myCoolDTM");
  BOOST_REQUIRE(lStubA == lStubB);
  lStubB.role = "anotherRole";
  BOOST_CHECK_EQUAL(lStubA == lStubB, false);

  // DaqTTCStub::uriT1
  lStubB = createStub("myCoolDTM");
  BOOST_REQUIRE(lStubA == lStubB);
  lStubB.uriT1 = "anotherURI";
  BOOST_CHECK_EQUAL(lStubA == lStubB, false);

  // DaqTTCStub::uriT2
  lStubB = createStub("myCoolDTM");
  BOOST_REQUIRE(lStubA == lStubB);
  lStubB.uriT2 = "anotherURI";
  BOOST_CHECK_EQUAL(lStubA == lStubB, false);

  // DaqTTCStub::addressTableT1
  lStubB = createStub("myCoolDTM");
  BOOST_REQUIRE(lStubA == lStubB);
  lStubB.addressTableT1 = "file://anotherAddrTable";
  BOOST_CHECK_EQUAL(lStubA == lStubB, false);

  // DaqTTCStub::addressTableT2
  lStubB = createStub("myCoolDTM");
  BOOST_REQUIRE(lStubA == lStubB);
  lStubB.addressTableT2 = "file://anotherAddrTable";
  BOOST_CHECK_EQUAL(lStubA == lStubB, false);

  // DaqTTCStub::crate
  lStubB = createStub("myCoolDTM");
  BOOST_REQUIRE(lStubA == lStubB);
  lStubB.crate = "anotherCrate";
  BOOST_CHECK_EQUAL(lStubA == lStubB, false);

  // DaqTTCStub::slot
  lStubB = createStub("myCoolDTM");
  BOOST_REQUIRE(lStubA == lStubB);
  lStubB.slot = uint32_t(1);
  BOOST_CHECK_EQUAL(lStubA == lStubB, false);

  // DaqTTCStub::fedId
  lStubB = createStub("myCoolDTM");
  BOOST_REQUIRE(lStubA == lStubB);
  lStubB.fedId = 42;
  BOOST_CHECK_EQUAL(lStubA == lStubB, false);
}

BOOST_AUTO_TEST_SUITE_END() // DaqTTCStubTestSuite


} // end ns: test
} // end ns: dtm
} // end ns: swatch

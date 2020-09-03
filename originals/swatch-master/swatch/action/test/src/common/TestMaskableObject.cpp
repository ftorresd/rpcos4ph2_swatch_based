/**
 * @file    TestMaskableObject.cpp
 * @author  Tom Williams
 * @date    December 2015
 */

// boost headers
#include "boost/test/unit_test.hpp"

// SWATCH headers
#include "swatch/action/MaskableObject.hpp"
#include "swatch/action/test/DummyMaskableObject.hpp"


namespace swatch {
namespace action {
namespace test {


BOOST_AUTO_TEST_SUITE( MaskableObjectTestSuite )


BOOST_AUTO_TEST_CASE(TestConstruction)
{
  DummyMaskableObject lObj("testMaskable");
  BOOST_CHECK_EQUAL( lObj.getId(), "testMaskable");
  BOOST_CHECK_EQUAL( lObj.getAlias(), "");
  BOOST_CHECK_EQUAL( lObj.isMasked(), false);

  DummyMaskableObject lObjWithAlias("testMaskable", "a useful alias");
  BOOST_CHECK_EQUAL( lObjWithAlias.getId(), "testMaskable");
  BOOST_CHECK_EQUAL( lObjWithAlias.getAlias(), "a useful alias");
  BOOST_CHECK_EQUAL( lObjWithAlias.isMasked(), false);
}


BOOST_AUTO_TEST_CASE(TestSetMasked)
{
  DummyMaskableObject lObj("testMaskable");

  // 1) setMasked() - no arguments - should change mask to true
  BOOST_REQUIRE_EQUAL(lObj.isMasked(), false);
  lObj.setMasked();
  BOOST_CHECK_EQUAL(lObj.isMasked(), true);

  // 2) Test setMasked method with argument
  BOOST_REQUIRE_EQUAL(lObj.isMasked(), true);
  lObj.setMasked(false);
  BOOST_CHECK_EQUAL(lObj.isMasked(), false);

  BOOST_REQUIRE_EQUAL(lObj.isMasked(), false);
  lObj.setMasked(true);
  BOOST_CHECK_EQUAL(lObj.isMasked(), true);
}


BOOST_AUTO_TEST_SUITE_END() // MaskableObjectTestSuite


} // namespace test
} // namespace action
} // namespace swatch

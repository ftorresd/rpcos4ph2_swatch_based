// Boost Unit Test includes
#include <boost/test/unit_test.hpp>

// boost headers
#include "boost/scoped_ptr.hpp"

// XDAQ headers
#include <xdata/Integer.h>
#include <xdata/UnsignedInteger.h>
#include <xdata/UnsignedInteger64.h>
#include <xdata/String.h>
#include <xdata/Boolean.h>
#include <xdata/Float.h>

// swatch headers
#include "swatch/xml/SimpleSerializer.hpp"

//others
#include "pugixml.hpp"


namespace swatch {
namespace xml {
namespace test {


BOOST_AUTO_TEST_SUITE( TestSimpleSerializer )
// Test all supported types
BOOST_AUTO_TEST_CASE( TestUInt )
{
  pugi::xml_document lDoc;
  lDoc.load("<param id='clkErrorTimeout' type='uint'>40</param>");
  pugi::xml_node lNode = lDoc.child("param");
  SimpleSerializer<xdata::UnsignedInteger> lSerializer;
  BOOST_CHECK_EQUAL(lSerializer.type(), "unsigned int");

  boost::scoped_ptr<xdata::Serializable> lResult(lSerializer.import(lNode));
  BOOST_CHECK_EQUAL(lResult->type(), "unsigned int");
  BOOST_CHECK_EQUAL(lResult->toString(), "40");
}

BOOST_AUTO_TEST_CASE( TestUInt64 )
{
  pugi::xml_document lDoc;
  lDoc.load("<param id='clkErrorTimeout' type='uint64'>0x2800000000</param>");
  pugi::xml_node lNode = lDoc.child("param");
  SimpleSerializer<xdata::UnsignedInteger64> lSerializer;
  BOOST_CHECK_EQUAL(lSerializer.type(), "unsigned int 64");

  boost::scoped_ptr<xdata::Serializable> lResult(lSerializer.import(lNode));
  BOOST_CHECK_EQUAL(lResult->type(), "unsigned int 64");
  BOOST_CHECK_EQUAL(lResult->toString(), "171798691840");
}

BOOST_AUTO_TEST_CASE( TestInt )
{
  pugi::xml_document lDoc;
  lDoc.load("<param id='test' type='int'>-2</param>");
  pugi::xml_node lNode = lDoc.child("param");
  SimpleSerializer<xdata::Integer> lSerializer;
  BOOST_CHECK_EQUAL(lSerializer.type(), "int");

  boost::scoped_ptr<xdata::Serializable> lResult(lSerializer.import(lNode));
  BOOST_CHECK_EQUAL(lResult->type(), "int");
  BOOST_CHECK_EQUAL(lResult->toString(), "-2");
}

BOOST_AUTO_TEST_CASE( TestBool )
{
  pugi::xml_document lDoc;
  lDoc.load("<param id='test' type='bool'>true</param>");
  pugi::xml_node lNode = lDoc.child("param");
  SimpleSerializer<xdata::Boolean> lSerializer;
  BOOST_CHECK_EQUAL(lSerializer.type(), "bool");

  boost::scoped_ptr<xdata::Serializable> lResult(lSerializer.import(lNode));
  BOOST_CHECK_EQUAL(lResult->type(), "bool");
  BOOST_CHECK_EQUAL(lResult->toString(), "true");
}

BOOST_AUTO_TEST_CASE( TestFloat )
{
  pugi::xml_document lDoc;
  lDoc.load("<param id='test' type='float'>2.0</param>");
  pugi::xml_node lNode = lDoc.child("param");
  SimpleSerializer<xdata::Float> lSerializer;
  BOOST_CHECK_EQUAL(lSerializer.type(), "float");

  boost::scoped_ptr<xdata::Serializable> lResult(lSerializer.import(lNode));
  BOOST_CHECK_EQUAL(lResult->type(), "float");
  // xdata::Float will change the string to a more generic format
  BOOST_CHECK_EQUAL(lResult->toString(), "2.00000e+00");

  // further tests to understand xdata::Float
  boost::scoped_ptr<xdata::Serializable> test2(new xdata::Float());
  test2->fromString(lResult->toString());
  BOOST_ASSERT(lResult->equals(*test2));
  BOOST_CHECK_EQUAL(lResult->toString(), test2->toString());
}

BOOST_AUTO_TEST_CASE( TestString )
{
  pugi::xml_document lDoc;
  lDoc.load("<param id='test' type='string'>hello</param>");
  pugi::xml_node lNode = lDoc.child("param");
  SimpleSerializer<xdata::String> lSerializer;
  BOOST_CHECK_EQUAL(lSerializer.type(), "string");

  boost::scoped_ptr<xdata::Serializable> lResult(lSerializer.import(lNode));
  BOOST_CHECK_EQUAL(lResult->type(), "string");
  BOOST_CHECK_EQUAL(lResult->toString(), "hello");
}

BOOST_AUTO_TEST_CASE( TestInvalid )
{
  pugi::xml_document lDoc;
  lDoc.load("<param id='test' type='string'>hello</param>");
  pugi::xml_node lNode = lDoc.child("param");
  SimpleSerializer<xdata::Integer> lSerializer;
  BOOST_CHECK_EQUAL(lSerializer.type(), "int");

  BOOST_CHECK_THROW(lSerializer.import(lNode), swatch::xml::ValueError);
}


BOOST_AUTO_TEST_SUITE_END() // TestSimpleSerializer

} //ns: test
}//ns: xml
} //ns: swatch

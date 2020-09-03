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
#include "swatch/xml/VectorSerializer.hpp"

//others
#include "pugixml.hpp"


namespace swatch {
namespace xml {
namespace test {

BOOST_AUTO_TEST_SUITE( TestVectorSerializer )

BOOST_AUTO_TEST_CASE( TestUInt )
{
  pugi::xml_document lDoc;
  lDoc.load("<param id='test' type='vector:uint'>40, 30, 1, 2</param>");
  pugi::xml_node lNode = lDoc.child("param");
  VectorSerializer<xdata::UnsignedInteger> lSerializer;

  boost::scoped_ptr<xdata::Serializable> lResult(lSerializer.import(lNode));
  BOOST_CHECK_EQUAL(lResult->type(), "vector");
  BOOST_CHECK_EQUAL(((xdata::Vector<xdata::UnsignedInteger>*)lResult.get())->getElementType(), "unsigned int");

  BOOST_CHECK_EQUAL(lResult->toString(), "[40,30,1,2]");
}

BOOST_AUTO_TEST_CASE( TestUInt64 )
{
  pugi::xml_document lDoc;
  lDoc.load("<param id='test' type='vector:uint64'>0x2800000000, 30, 1, 2</param>");
  pugi::xml_node lNode = lDoc.child("param");
  VectorSerializer<xdata::UnsignedInteger64> lSerializer;

  boost::scoped_ptr<xdata::Serializable> lResult(lSerializer.import(lNode));
  BOOST_CHECK_EQUAL(lResult->type(), "vector");
  BOOST_CHECK_EQUAL(((xdata::Vector<xdata::UnsignedInteger64>*)lResult.get())->getElementType(), "unsigned int 64");

  BOOST_CHECK_EQUAL(lResult->toString(), "[171798691840,30,1,2]");
}

BOOST_AUTO_TEST_CASE( TestInt )
{
  pugi::xml_document lDoc;
  lDoc.load("<param id='test' type='vector:int'>-2</param>");
  pugi::xml_node lNode = lDoc.child("param");
  VectorSerializer<xdata::Integer> lSerializer;

  boost::scoped_ptr<xdata::Serializable> lResult(lSerializer.import(lNode));
  BOOST_CHECK_EQUAL(lResult->type(), "vector");
  BOOST_CHECK_EQUAL(((xdata::Vector<xdata::Integer>*)lResult.get())->getElementType(), "int");
  BOOST_CHECK_EQUAL(lResult->toString(), "[-2]");
}

BOOST_AUTO_TEST_CASE( TestBool )
{
  pugi::xml_document lDoc;
  lDoc.load("<param id='test' type='vector:bool'>true,false</param>");
  pugi::xml_node lNode = lDoc.child("param");
  VectorSerializer<xdata::Boolean> lSerializer;

  boost::scoped_ptr<xdata::Serializable> lResult(lSerializer.import(lNode));
  BOOST_CHECK_EQUAL(lResult->type(), "vector");
  BOOST_CHECK_EQUAL(((xdata::Vector<xdata::Boolean>*)lResult.get())->getElementType(), "bool");
  BOOST_CHECK_EQUAL(lResult->toString(), "[true,false]");
}

BOOST_AUTO_TEST_CASE( TestFloat )
{
  pugi::xml_document lDoc;
  lDoc.load("<param id='test' type='vector:float'>2.0</param>");
  pugi::xml_node lNode = lDoc.child("param");
  VectorSerializer<xdata::Float> lSerializer;

  boost::scoped_ptr<xdata::Serializable> lResult(lSerializer.import(lNode));
  BOOST_CHECK_EQUAL(lResult->type(), "vector");
  BOOST_CHECK_EQUAL(((xdata::Vector<xdata::Float>*)lResult.get())->getElementType(), "float");
  BOOST_CHECK_EQUAL(lResult->toString(), "[2.00000e+00]");
}

BOOST_AUTO_TEST_CASE( TestString )
{
  pugi::xml_document lDoc;
  lDoc.load("<param id='test' type='vector:string'>hello, world, and, all</param>");
  pugi::xml_node lNode = lDoc.child("param");
  VectorSerializer<xdata::String> lSerializer;

  boost::scoped_ptr<xdata::Serializable> lResult(lSerializer.import(lNode));
  BOOST_CHECK_EQUAL(lResult->type(), "vector");
  BOOST_CHECK_EQUAL(((xdata::Vector<xdata::String>*)lResult.get())->getElementType(), "string");
  BOOST_CHECK_EQUAL(lResult->toString(), "[hello,world,and,all]");
}

BOOST_AUTO_TEST_CASE( TestCustomDelimiter )
{
  pugi::xml_document lDoc;
  lDoc.load("<param id='test' type='vector:string' delimiter='|'>hello, world| and, all</param>");
  pugi::xml_node lNode = lDoc.child("param");
  VectorSerializer<xdata::String> lSerializer;

  boost::scoped_ptr<xdata::Serializable> lResult(lSerializer.import(lNode));
  BOOST_CHECK_EQUAL(lResult->type(), "vector");
  BOOST_CHECK_EQUAL(((xdata::Vector<xdata::String>*)lResult.get())->getElementType(), "string");
  BOOST_CHECK_EQUAL(lResult->toString(), "[hello, world,and, all]");
}

BOOST_AUTO_TEST_CASE( TestInvalid )
{
  pugi::xml_document lDoc;
  lDoc.load("<param id='test' type='string'>hello, test</param>");
  pugi::xml_node lNode = lDoc.child("param");
  VectorSerializer<xdata::Integer> lSerializer;
  BOOST_CHECK_EQUAL(lSerializer.type(), "vector:int");

  BOOST_CHECK_THROW(lSerializer.import(lNode), swatch::xml::ValueError);
}


BOOST_AUTO_TEST_SUITE_END() // TestVectorSerializer

} //ns: test
}//ns: xml
} //ns: swatch

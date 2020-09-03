// Boost Unit Test includes
#include <boost/test/unit_test.hpp>

// boost headers
#include "boost/scoped_ptr.hpp"

// XDAQ headers
#include <xdata/Table.h>

// swatch headers
#include "swatch/xml/TableSerializer.hpp"
//others
#include "pugixml.hpp"


namespace swatch {
namespace xml {
namespace test {

BOOST_AUTO_TEST_SUITE( TestTableSerializer )

BOOST_AUTO_TEST_CASE( TestTable )
{
  pugi::xml_document lDoc;
  lDoc.load(
    "<param id='test' type='table'>"
    "   <columns>aaa,bbb,ccc,ddd</columns>"
    "   <types>int,uint,uint64,float</types>"
    "   <rows>"
    "      <row>1,2,3,4</row>"
    "      <row>5,6,7,8</row>"
    "   </rows>"
    "</param>");
  pugi::xml_node lNode = lDoc.child("param");
  TableSerializer lSerializer;
  BOOST_CHECK_EQUAL(lSerializer.type(), "table");

  boost::scoped_ptr<xdata::Table> lResult(dynamic_cast<xdata::Table*>(lSerializer.import(lNode)));
  // Check columns
  std::vector<std::string> lExpColumns = {"aaa","bbb","ccc","ddd"};
  std::vector<std::string> lColumns = lResult->getColumns();

  BOOST_CHECK_EQUAL_COLLECTIONS(lColumns.begin(),lColumns.end(),lExpColumns.begin(),lExpColumns.end());
  std::string lExpected="{\"rows\":2,\"cols\":4,\"definition\":[[\"aaa\",\"int\"],[\"bbb\",\"unsigned int\"],[\"ccc\",\"unsigned int 64\"],[\"ddd\",\"float\"]],\"data\":[[1,2,3,4.00000e+00],[5,6,7,8.00000e+00]]}";

  BOOST_CHECK_EQUAL(lResult->toString(), lExpected);
}

BOOST_AUTO_TEST_CASE( TestTableDelimiter )
{
  pugi::xml_document doc;
  doc.load(
    "<param id='test' type='table' delimiter='|'>"
    "   <columns>aaa|bbb|ccc</columns>"
    "   <types>uint|uint|uint</types>"
    "   <rows>"
    "      <row>1|2|4</row>"
    "      <row>5|6|7</row>"
    "   </rows>"
    "</param>");
  pugi::xml_node lNode = doc.child("param");
  TableSerializer lSerializer;
  BOOST_CHECK_EQUAL(lSerializer.type(), "table");

  boost::scoped_ptr<xdata::Serializable> result(lSerializer.import(lNode));

  std::string expected="{\"rows\":2,\"cols\":3,\"definition\":[[\"aaa\",\"unsigned int\"],[\"bbb\",\"unsigned int\"],[\"ccc\",\"unsigned int\"]],\"data\":[[1,2,4],[5,6,7]]}";

  BOOST_CHECK_EQUAL(result->toString(), expected);
}

BOOST_AUTO_TEST_CASE( TestIntruderTag )
{
  pugi::xml_document doc;
  doc.load(
    "<param id='test' type='table'>"
    "   <columns>aaa,bbb,ccc</columns>"
    "   <rows>"
    "      <row>1,2,4</row>"
    "      <not_a_row>5,6,7</not_a_row>"
    "   </rows>"
    "</param>");
  pugi::xml_node lNode = doc.child("param");
  TableSerializer lSerializer;

  BOOST_CHECK_THROW(lSerializer.import(lNode), swatch::xml::ValueError);
}


BOOST_AUTO_TEST_CASE( TestMissingTag )
{
  pugi::xml_document doc;
  doc.load(
    "<param id='test' type='table'>"
    "   <columns>aaa,bbb,ccc</columns>"
    "   <rows>"
    "      <row>1,2,4</row>"
    "      <row>5,6,7</row>"
    "   </rows>"
    "</param>");
  pugi::xml_node lNode = doc.child("param");
  TableSerializer lSerializer;

  BOOST_CHECK_THROW(lSerializer.import(lNode), swatch::xml::ValueError);
}

BOOST_AUTO_TEST_CASE( TestColRowMismatch )
{
  pugi::xml_document doc;
  doc.load(
    "<param id='test' type='table'>"
    "   <columns>aaa,bbb,ccc</columns>"
    "   <rows>"
    "      <row>1,2</row>"
    "      <row>5,6,7</row>"
    "   </rows>"
    "</param>");
  pugi::xml_node lNode = doc.child("param");
  TableSerializer lSerializer;

  BOOST_CHECK_THROW(lSerializer.import(lNode), swatch::xml::ValueError);
}

BOOST_AUTO_TEST_SUITE_END() // TestTableSerializer

} // namespace swatch
} // namespace xml
} // namespace test

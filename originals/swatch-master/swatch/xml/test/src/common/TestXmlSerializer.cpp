// Boost Unit Test includes
#include <boost/test/unit_test.hpp>

// boost headers
#include "boost/scoped_ptr.hpp"

// xdaq
#include <xdata/Integer.h>
#include <xdata/UnsignedInteger.h>
#include <xdata/String.h>
#include <xdata/Boolean.h>
#include <xdata/Float.h>
#include <xdata/Vector.h>

// SWATCH headers
#include "swatch/xml/XmlSerializer.hpp"
#include "swatch/xml/VectorSerializer.hpp"

//others
#include "pugixml.hpp"


namespace swatch {
namespace xml {
namespace test {

struct XmlSerializerTestSetup {
  boost::scoped_ptr<XmlSerializer> serializer;
  std::vector<std::string> inputs, outputs, types;


  XmlSerializerTestSetup() :
    serializer(new XmlSerializer()), inputs(), outputs(), types()
  {
    inputs = {
      // vectors
      "<param id='testUintV' type='vector:uint'>40, 30, 1, 2</param>",
      "<param id='testUint64V' type='vector:uint64'>0x2800000000, 30, 1, 2</param>",
      "<param id='testIntV' type='vector:int'>-2, 2, -42</param>",
      "<param id='testFloatV' type='vector:float'>-2.2, 2.3, -42.0</param>",
      "<param id='testBoolV' type='vector:bool'>false, false, true</param>",
      "<param id='testStringV' type='vector:string'>hello, is, it, me, you're, looking, for</param>",
      // single objects
      "<param id='testUint' type='uint'>40</param>",
      "<param id='testUint64' type='uint64'>0x2800000000</param>",
      "<param id='testInt' type='int'>-2</param>",
      "<param id='testFloat' type='float'>-2.2</param>",
      "<param id='testBool' type='bool'>false</param>",
      "<param id='testString' type='string'>hello</param>",
      "<param id='test' type='table' delimiter='|'>"
      "   <columns>aaa|bbb|ccc</columns>"
      "   <types>uint|uint|uint</types>"
      "   <rows>"
      "      <row>1|2|4</row>"
      "      <row>5|6|7</row>"
      "   </rows>"
      "</param>"
    };
    outputs = {
      "[40,30,1,2]",
      "[171798691840,30,1,2]",
      "[-2,2,-42]",
      "[-2.20000e+00,2.30000e+00,-4.20000e+01]",
      "[false,false,true]",
      "[hello,is,it,me,you're,looking,for]",
      "40",
      "171798691840",
      "-2",
      "-2.20000e+00",
      "false",
      "hello",
      "{\"rows\":2,\"cols\":3,\"definition\":[[\"aaa\",\"unsigned int\"],[\"bbb\",\"unsigned int\"],[\"ccc\",\"unsigned int\"]],\"data\":[[1,2,4],[5,6,7]]}"
    };
    types = {
      "vector",
      "vector",
      "vector",
      "vector",
      "vector",
      "vector",
      "unsigned int",
      "unsigned int 64",
      "int",
      "float",
      "bool",
      "string",
      "table"
    };
    BOOST_REQUIRE_EQUAL(inputs.size(), outputs.size());
    BOOST_REQUIRE_EQUAL(inputs.size(), types.size());
  }
};


BOOST_AUTO_TEST_SUITE(TestXmlSerializer)

BOOST_FIXTURE_TEST_CASE(TestImport, XmlSerializerTestSetup)
{
  for (unsigned int i = 0; i < inputs.size(); ++i) {
    pugi::xml_document lDoc;
    BOOST_REQUIRE_EQUAL(lDoc.load(inputs.at(i).c_str()), true);
    pugi::xml_node lNode = lDoc.child("param");
    BOOST_REQUIRE_EQUAL(lNode.empty(), false);
    boost::scoped_ptr<xdata::Serializable> lResult(serializer->import(lNode));

    BOOST_CHECK_EQUAL(lResult->type(), types.at(i));
    BOOST_CHECK_EQUAL(lResult->toString(), outputs.at(i));
  }
}


BOOST_FIXTURE_TEST_CASE(TestInvalid, XmlSerializerTestSetup)
{
  std::string lInput = "<param id='test' type='vector:uint'>40, error, 1, 2</param>";
  pugi::xml_document lDoc;
  BOOST_REQUIRE_EQUAL(lDoc.load(lInput.c_str()), true);
  pugi::xml_node lNode = lDoc.child("param");

  BOOST_CHECK_THROW(serializer->import(lNode), swatch::xml::ValueError);
}


BOOST_FIXTURE_TEST_CASE(TestUnkownType, XmlSerializerTestSetup)
{
  std::string lInput = "<param id='test' type='unknown'>I can't do that Bob.</param>";
  pugi::xml_document lDoc;
  BOOST_REQUIRE_EQUAL(lDoc.load(lInput.c_str()), true);
  pugi::xml_node lNode = lDoc.child("param");

  BOOST_CHECK_THROW(serializer->import(lNode), swatch::xml::UnknownDataType);
}

BOOST_AUTO_TEST_SUITE_END() // TestXmlSerializer

}//ns: test
}//ns: xml
} //ns: swatch

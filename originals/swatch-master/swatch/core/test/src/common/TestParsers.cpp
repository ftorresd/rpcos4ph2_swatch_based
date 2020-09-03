/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

// Boost Unit Test includes
#include <boost/test/unit_test.hpp>
#include "boost/test/test_tools.hpp"
#include "swatch/core/toolbox/IdSliceGrammar.hpp"

#include <swatch/core/toolbox/IdSliceParser.hpp>

namespace swatch {
namespace core {
namespace test {

BOOST_AUTO_TEST_SUITE( ParserTestSuite )

BOOST_AUTO_TEST_CASE(IdSliceBasic)
{

  std::string lSimple = "Rx01";

  std::vector<std::string> lIds = toolbox::IdSliceParser::parse(lSimple);

  BOOST_CHECK_EQUAL( lIds.size(), (size_t)1);
  BOOST_CHECK_EQUAL( lIds[0], "Rx01");
}

BOOST_AUTO_TEST_CASE(IdSliceRange)
{

  std::string simple = "Rx[01:05]";
  std::vector<std::string> expected = {"Rx01","Rx02","Rx03","Rx04"};

  std::vector<std::string> ids = toolbox::IdSliceParser::parse(simple);

  BOOST_CHECK_EQUAL( ids.size(), (size_t)4);
  BOOST_CHECK_EQUAL_COLLECTIONS(ids.begin(), ids.end(),
                                expected.begin(), expected.end());
}

BOOST_AUTO_TEST_CASE(IdSliceRangePostfix)
{

  std::string simple = "Rx[01:05]post";
  std::vector<std::string> expected = {"Rx01post","Rx02post","Rx03post","Rx04post"};

  std::vector<std::string> ids = toolbox::IdSliceParser::parse(simple);

  BOOST_CHECK_EQUAL( ids.size(), (size_t)4);
  BOOST_CHECK_EQUAL_COLLECTIONS(ids.begin(), ids.end(),
                                expected.begin(), expected.end());
}


BOOST_AUTO_TEST_CASE(IdSliceRangeException)
{

  std::string simple = "Rx[05:01]";
  BOOST_CHECK_THROW(toolbox::IdSliceParser::parse(simple), toolbox::InvalidSliceRange);

}

//____________________________________________________________________________//
BOOST_AUTO_TEST_CASE(SliceSyntaxParsingTests)
{
  std::cout << "ProcessorTestSuite.SliceSyntaxParsingTests" << std::endl;
  std::vector<std::string> expected, returned;
  std::vector<std::string> expectedSet, returnedSet;


  // Very basic strings (no prefix / suffix)
  expected = {"0", "1"};
  returned = toolbox::IdSliceParser::parse("[0:2]");
  BOOST_CHECK_EQUAL_COLLECTIONS(returned.begin(), returned.end(), expected.begin(), expected.end());
  returned = toolbox::IdSliceParser::parse("[0:2:1]");
  BOOST_CHECK_EQUAL_COLLECTIONS(returned.begin(), returned.end(), expected.begin(), expected.end());

  expected = {"0", "3", "6"};
  returned = toolbox::IdSliceParser::parse("[0:7:3]");
  BOOST_CHECK_EQUAL_COLLECTIONS(returned.begin(), returned.end(), expected.begin(), expected.end());
  returned = toolbox::IdSliceParser::parse("[0:9:3]");
  BOOST_CHECK_EQUAL_COLLECTIONS(returned.begin(), returned.end(), expected.begin(), expected.end());

  expected = {"12", "24", "36"};
  returned = toolbox::IdSliceParser::parse("[12:37:12]");
  BOOST_CHECK_EQUAL_COLLECTIONS(returned.begin(), returned.end(), expected.begin(), expected.end());
  returned = toolbox::IdSliceParser::parse("[12:48:12]");
  BOOST_CHECK_EQUAL_COLLECTIONS(returned.begin(), returned.end(), expected.begin(), expected.end());


  // Very basic strings, decreasing numbers
  expected = {"12", "11", "10"};
  returned = toolbox::IdSliceParser::parse("[12:9:-1]");
  BOOST_CHECK_EQUAL_COLLECTIONS(returned.begin(), returned.end(), expected.begin(), expected.end());

  expected = {"12", "09", "06"};
  returned = toolbox::IdSliceParser::parse("[12:3:-3]");
  BOOST_CHECK_EQUAL_COLLECTIONS(returned.begin(), returned.end(), expected.begin(), expected.end());
  returned = toolbox::IdSliceParser::parse("[12:5:-3]");
  BOOST_CHECK_EQUAL_COLLECTIONS(returned.begin(), returned.end(), expected.begin(), expected.end());


  // Very basic strings (no prefix / suffix), fixed width
  expected = {"00", "01"};
  returned = toolbox::IdSliceParser::parse("[00:2]");
  BOOST_CHECK_EQUAL_COLLECTIONS(returned.begin(), returned.end(), expected.begin(), expected.end());
  returned = toolbox::IdSliceParser::parse("[0:02]");
  BOOST_CHECK_EQUAL_COLLECTIONS(returned.begin(), returned.end(), expected.begin(), expected.end());


  // Strings with prefix + suffix
  expected = {"abc_rx00_def", "abc_rx05_def", "abc_rx10_def"};
  returned = toolbox::IdSliceParser::parse("abc_rx[0:15:5]_def");
  BOOST_CHECK_EQUAL_COLLECTIONS(returned.begin(), returned.end(), expected.begin(), expected.end());

  // STrings with spaces
  expected = {"string for rx 00 with spaces", "string for rx 01 with spaces", "string for rx 02 with spaces"};
  returned = toolbox::IdSliceParser::parse("string for rx [00:03] with spaces");
  BOOST_CHECK_EQUAL_COLLECTIONS(returned.begin(), returned.end(), expected.begin(), expected.end());

  // Finally, strings that don't contain port slice syntax
  BOOST_CHECK_THROW(toolbox::IdSliceParser::parse("abc[0]def"), swatch::core::toolbox::SliceParsingFailed);

  BOOST_CHECK_THROW(toolbox::IdSliceParser::parse("abc[0:2:3:4]def"), swatch::core::toolbox::SliceParsingFailed);

  BOOST_CHECK_THROW(toolbox::IdSliceParser::parse("[a:b:c]"), swatch::core::toolbox::SliceParsingFailed);

  BOOST_CHECK_THROW(toolbox::IdSliceParser::parse("abc0:4]"), swatch::core::toolbox::SliceParsingFailed);

  BOOST_CHECK_THROW(toolbox::IdSliceParser::parseSet("0, 0"), swatch::core::toolbox::SliceSetParsingDuplicatesFound);
}

BOOST_AUTO_TEST_SUITE_END() // ObjectTestSuite

} //namespace test
} //namespace core
} //namespace swatch
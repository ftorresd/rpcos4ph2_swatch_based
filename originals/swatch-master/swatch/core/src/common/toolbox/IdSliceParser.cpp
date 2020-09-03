/*
 * File:   IdSliceParser.cpp
 * Author: ale
 *
 * Created on November 16, 2015, 4:58 PM
 */

#include "swatch/core/toolbox/IdSliceParser.hpp"


// Standard headers
#include <iterator>                     // for distance, ostream_iterator
#include <set>
#include <sstream>                      // for basic_ostringstream
#include <string>                       // for basic_string, string, etc
#include <vector>                       // for vector

// SWATCH headers
#include "swatch/core/toolbox/IdSliceGrammar.hpp"


namespace swatch {
namespace core {
namespace toolbox {

// --------------------------------------------------------
std::vector<std::string>
IdSliceParser::parse(const std::string& aStringSlice)
{

  namespace qi = boost::spirit::qi;
  namespace ascii = boost::spirit::ascii;

  std::string::const_iterator lBegin(aStringSlice.begin()), lEnd(aStringSlice.end());

  IdSliceGrammar lGrammar;

  std::vector<std::string> lIds;
  bool lSuccess = qi::parse ( lBegin , lEnd , lGrammar , lIds );
  if ( lSuccess ) {

    // Throw if slice not fully parsed
    if ( lBegin != lEnd ) {
      std::ostringstream lMsg;
      std::ostream_iterator<char> lOutIt (lMsg,"");
      lMsg << "Slice parsing stopped at position " << std::distance(aStringSlice.begin(), lBegin) << " of string '" << aStringSlice << "'";

      XCEPT_RAISE(SliceParsingFailed,lMsg.str());
    }

  }
  else {
    // TODO: Should throw here
    return std::vector<std::string>();
  }

  return lIds;
}


std::vector<std::string>
IdSliceParser::parseList(const std::string& aStringSlice)
{
  namespace qi = boost::spirit::qi;
  namespace ascii = boost::spirit::ascii;

  std::string::const_iterator lBegin(aStringSlice.begin()), lEnd(aStringSlice.end());

  IdSliceListGrammar lGrammar;

  std::vector<std::string> lIds;
  bool lSuccess = qi::phrase_parse ( lBegin , lEnd , lGrammar , ascii::space , lIds );
  if ( lSuccess ) {

    // Throw if slice not fully parsed
    if ( lBegin != lEnd ) {
      std::ostringstream lMsg;
      lMsg << "SliceList parsing stopped at position " << std::distance(aStringSlice.begin(), lBegin) << " of string '" << aStringSlice << "'";

      XCEPT_RAISE(SliceListParsingFailed,lMsg.str());
    }

  }
  else {
    // TODO: Should throw here
    return std::vector<std::string>();
  }

  // Special case for empty strings. They are parsed into a 1-element vector with a single empty string.
  // If so return an empty vector
  if (lIds.size() == 1 && lIds.front().empty())
    lIds.clear();

  return lIds;
}


std::set<std::string> IdSliceParser::parseSet(const std::string& aStringSlice)
{
  std::vector<std::string> lIdList = parseList(aStringSlice);

  std::set<std::string> lIdSet(lIdList.begin(), lIdList.end());

  if ( lIdList.size() != lIdSet.size()) {
    XCEPT_RAISE(SliceSetParsingDuplicatesFound,"");
  }

  return lIdSet;
}




} // namespace toolbox
} // namespace core
} // namespace swatch

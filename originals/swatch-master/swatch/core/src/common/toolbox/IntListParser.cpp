/*
 * File:   IntListGrammar.cpp
 * Author: ale
 *
 * Created on September 15, 2015, 7:21 AM
 */


#include "swatch/core/toolbox/IntListParser.hpp"

// SWATCH Headers
#include "swatch/core/toolbox/IntListGrammar.hpp"

namespace swatch {
namespace core {
namespace toolbox {

// --------------------------------------------------------
std::vector<int>
IntListParser::parse(const std::string& aStringList)
{

  namespace qi = boost::spirit::qi;
  namespace ascii = boost::spirit::ascii;

  IntListGrammar lGrammar;

  std::vector<int> lIds;
  bool lSuccess = qi::phrase_parse ( aStringList.begin() , aStringList.end() , lGrammar , ascii::space , lIds );
  if ( not lSuccess ) {
    return std::vector<int32_t>();
  }
  return lIds;
}

// --------------------------------------------------------
std::vector<uint32_t>
UIntListParser::parse(const std::string& aStringList)
{

  namespace qi = boost::spirit::qi;
  namespace ascii = boost::spirit::ascii;

  IntListGrammar lGrammar;

  std::vector<uint32_t> lIds;
  bool lSuccess = qi::phrase_parse ( aStringList.begin() , aStringList.end() , lGrammar , ascii::space , lIds );
  if ( not lSuccess ) {
    return std::vector<uint32_t>();
  }
  return lIds;
}

} // namespace toolbox
} // namespace core
} // namespace swatch

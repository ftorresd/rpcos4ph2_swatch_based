/*
 * File:   IdSliceGrammar.cpp
 * Author: ale
 *
 * Created on November 16, 2015, 4:25 PM
 */

#include "swatch/core/toolbox/IdSliceGrammar.hpp"

// Standard headers
#include <algorithm>                    // for max
#include <iomanip>
#include <stddef.h>                     // for size_t
#include <stdint.h>                     // for int32_t
#include <sstream>                      // for basic_ostream, etc

// Boost Headers
#include <boost/lexical_cast.hpp>
#include <boost/spirit/include/qi.hpp>
//#include <boost/spirit/include/qi_grammar.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_bind.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_statement.hpp>


namespace swatch {
namespace core {
namespace toolbox {

// --------------------------------------------------------
IdSliceGrammar::IdSliceGrammar() :
  IdSliceGrammar::base_type(query)
{

  namespace qi = boost::spirit::qi;
  namespace phx = boost::phoenix;

  query = (range | literal);
  literal = *(qi::char_ - ',' - '[' - ':' - ']');
  number = *(qi::char_("0-9"));
  step = qi::int_;

  range =
    (literal >> '[' >> number >> ':' >> number >> -(':' >> step) >> ']' >> literal )[
      phx::bind(&pushBackRange, qi::_val, qi::_1, qi::_2, qi::_3, qi::_4, qi::_5)
    ];
}


// --------------------------------------------------------
void IdSliceGrammar::pushBackRange(std::vector<std::string>& aResult, const std::string& aPrefix, const std::string& aFirst, const std::string& aLast, const boost::optional<int32_t>& aStep, const boost::optional<std::string>& aPostfix)
{

  const int32_t lFirstId = boost::lexical_cast<int32_t>(aFirst);
  const int32_t lLastId = boost::lexical_cast<int32_t>(aLast);
  const int32_t lStep = (aStep ? *aStep : 1);

  const size_t lWidth = std::max(aFirst.size(), aLast.size());

  // Step 0? Not good
  if ( !lStep )  {
    XCEPT_RAISE(InvalidSliceStep,"Invalid step 0");
  }

  // Check that the loop is not infinite
  if ( (lFirstId < lLastId) ^ (lStep > 0 ) )
    XCEPT_RAISE(InvalidSliceRange,
      "First index ("+boost::lexical_cast<std::string>(aFirst)+
      ") bigger than last ("+boost::lexical_cast<std::string>(aLast)+")"
    );


  std::ostringstream lOSS;

  for ( int32_t i(lFirstId); (lLastId-i)/(float)lStep > 0 ; i += lStep) {
    // Reset stream
    lOSS.str("");
    lOSS << aPrefix << std::setw(lWidth) << std::setfill('0') << i;
    if (aPostfix)
      lOSS << *aPostfix;

    aResult.push_back( lOSS.str());

  }

}


// --------------------------------------------------------
IdSliceListGrammar::IdSliceListGrammar() :
  IdSliceListGrammar::base_type(query)
{
  namespace qi = boost::spirit::qi;
  namespace phx = boost::phoenix;

  query = element % ',';
  element = (range | literal);
  literal = *(qi::char_ - ',' - '[' - ':' - ']');
}



} // namespace toolbox
} // namespace core
} // namespace swatch


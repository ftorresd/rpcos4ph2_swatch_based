#include "swatch/core/toolbox/IntListGrammar.hpp"

#include <boost/iterator/counting_iterator.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_bind.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>


namespace swatch {
namespace core {
namespace toolbox {

// --------------------------------------------------------
IntListGrammar::IntListGrammar() :
  IntListGrammar::base_type(query)
{
  namespace qi = boost::spirit::qi;
  namespace phx = boost::phoenix;

  query = element % ',';

  element = (range | literal);
  literal = qi::int_;
  range = (qi::int_ >> '-' >> qi::int_)[phx::bind(&push_back_range, qi::_val, qi::_1, qi::_2) ];

  //  BOOST_SPIRIT_DEBUG_NODES((query));
}


// --------------------------------------------------------
void IntListGrammar::push_back_range(std::vector<int>& aVector, int aFirst, int aLast)
{
  boost::counting_iterator<int> lBegin(aFirst);
  // +1 to include last element
  boost::counting_iterator<int> lEnd(aLast + 1);

  aVector.insert(aVector.end(), lBegin, lEnd);
}


} // namespace toolbox
} // namespace core
} // namespace swatch


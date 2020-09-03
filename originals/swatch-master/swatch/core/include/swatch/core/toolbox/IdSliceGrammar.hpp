/*
 * File:   IdSliceGrammar.hpp
 * Author: ale
 *
 * Created on September 15, 2015, 9:23 AM
 */

#ifndef __SWATCH_CORE_TOOLBOX_IDSLICEGRAMMAR_HPP__
#define __SWATCH_CORE_TOOLBOX_IDSLICEGRAMMAR_HPP__


// Standard headers
#include <stdint.h>                     // for int32_t
#include <string>                       // for string
#include <vector>                       // for vector

// boost headers
#include "boost/optional/optional.hpp"  // for optional
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_grammar.hpp>

#include "swatch/core/exception.hpp"


namespace swatch {
namespace core {
namespace toolbox {

//-----
struct IdSliceGrammar : boost::spirit::qi::grammar<std::string::const_iterator, std::vector<std::string>() > {
  IdSliceGrammar();

  boost::spirit::qi::rule<std::string::const_iterator, std::vector<std::string>() > query;
  boost::spirit::qi::rule<std::string::const_iterator, std::string()> literal;
  boost::spirit::qi::rule<std::string::const_iterator, std::string()> number;
  boost::spirit::qi::rule<std::string::const_iterator, int32_t()> step;
  boost::spirit::qi::rule<std::string::const_iterator, std::vector<std::string>() > range;

  static void pushBackRange(std::vector<std::string>& aResult, const std::string& aPrefix, const std::string& aFirst, const std::string& aLast, const boost::optional<int32_t>& aStep, const boost::optional<std::string>& aPostfix);

};

SWATCH_DEFINE_EXCEPTION(InvalidSliceStep)
SWATCH_DEFINE_EXCEPTION(InvalidSliceRange)

struct IdSliceListGrammar : boost::spirit::qi::grammar<std::string::const_iterator, std::vector<std::string>(), boost::spirit::ascii::space_type > {
  IdSliceListGrammar();

  boost::spirit::qi::rule<std::string::const_iterator, std::vector<std::string>(), boost::spirit::ascii::space_type > query;
  boost::spirit::qi::rule<std::string::const_iterator, std::vector<std::string>(), boost::spirit::ascii::space_type > element;
  boost::spirit::qi::rule<std::string::const_iterator, std::string(), boost::spirit::ascii::space_type> literal;
  IdSliceGrammar range;
};

} // namespace tools
} // namespace core
} // namespace swatch



#endif /* __SWATCH_CORE_TOOLBOX_IDSLICEGRAMMAR_HPP__ */


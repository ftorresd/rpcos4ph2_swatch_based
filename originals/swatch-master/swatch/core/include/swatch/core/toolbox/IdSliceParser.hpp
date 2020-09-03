/*
 * File:   IdSliceParser.hpp
 * Author: ale
 *
 * Created on November 16, 2015, 4:58 PM
 */

#ifndef __SWATCH_CORE_TOOLBOX_IDSLICEPARSER_HPP__
#define __SWATCH_CORE_TOOLBOX_IDSLICEPARSER_HPP__


// Standard headers
#include <set>                          // for set
#include <string>
#include <vector>

// SWATCH headers
#include "swatch/core/exception.hpp"


namespace swatch {
namespace core {
namespace toolbox {

struct IdSliceParser {
  static std::vector<std::string> parse( const std::string& aStringSlice );
  static std::vector<std::string> parseList( const std::string& aStringSlice );
  static std::set<std::string> parseSet( const std::string& aStringSlice );

};

SWATCH_DEFINE_EXCEPTION(SliceParsingFailed)
SWATCH_DEFINE_EXCEPTION(SliceListParsingFailed)
SWATCH_DEFINE_EXCEPTION(SliceSetParsingDuplicatesFound)

} // namespace toolbox
} // namespace core
} // namespace swatch

#endif /* __SWATCH_CORE_TOOLBOX_IDSLICEPARSER_HPP__ */

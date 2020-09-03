/**
 * @file    GateKeeperView.hxx
 * @author  Luke Kreczko
 * @date    February 2016
 */

#ifndef __SWATCH_ACTION_GATEKEEPERVIEW_HXX__
#define __SWATCH_ACTION_GATEKEEPERVIEW_HXX__


// IWYU pragma: private, include "swatch/action/GateKeeperView.hpp"

#include <string>

#include <boost/foreach.hpp>

#include "swatch/action/GateKeeperView.hpp"


namespace swatch {
namespace action {

template<typename T>
const std::vector<std::string> GateKeeperView::extractMapKeys(const boost::unordered_map<std::string, T>& aMap) const
{
  std::vector<std::string> lKeys;
  // the BOOST_FOREACH macro is not happy when using th full type, hence typedef
  typedef std::pair<std::string, T> Pair_t;
  BOOST_FOREACH(const Pair_t lIt, aMap) {
    lKeys.push_back(lIt.first);
  }

  return lKeys;
}

} // namespace action
} // namespace swatch

#endif	/* __SWATCH_ACTION_GATEKEEPERVIEW_HPP__ */


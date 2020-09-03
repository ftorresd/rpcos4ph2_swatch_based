/**
 * @file    StatusFlag.hpp
 * @author  Tom Williams
 * @date    May 2015
 */

#ifndef __SWATCH_CORE_STATUSFLAG_HPP__
#define __SWATCH_CORE_STATUSFLAG_HPP__


#include <iosfwd>


namespace swatch {
namespace core {


enum StatusFlag {
  kGood,
  kWarning,
  kError,
  kNoLimit,
  kUnknown
};


std::ostream& operator<<(std::ostream& aOut, swatch::core::StatusFlag aValue);


swatch::core::StatusFlag operator& (const swatch::core::StatusFlag& aFlag1, const swatch::core::StatusFlag& aFlag2);


} // namespace core
} // namespace swatch


#endif  /* __SWATCH_CORE_STATUSFLAG_HPP__ */


/*
 * File:   StatusFlag.cpp
 * Author: Tom Williams
 * Date:   May 2015
 */

#include "swatch/core/StatusFlag.hpp"
#include "swatch/core/exception.hpp"


#include <iostream>
#include <stdexcept>


namespace swatch {
namespace core {


std::ostream& operator<<(std::ostream& aOut, swatch::core::StatusFlag aValue)
{
  switch (aValue) {
    case kGood :
      return (aOut << "Good");
    case kWarning :
      return (aOut << "Warning");
    case kError :
      return (aOut << "Error");
    case kNoLimit :
      return (aOut << "NoLimit");
    case kUnknown :
      return (aOut << "Unknown");
    default :
      XCEPT_RAISE(RuntimeError,"Invalid StatusFlag enum value in ostream operator<<");
  }
}


swatch::core::StatusFlag operator& (const swatch::core::StatusFlag& aFlag1, const swatch::core::StatusFlag& aFlag2)
{
  if (aFlag1 == kNoLimit && aFlag2 == kNoLimit)
    return kNoLimit;
  else if (aFlag1 == kError || aFlag2 == kError)
    return kError;
  else if (aFlag1 == kUnknown || aFlag2 == kUnknown)
    return kUnknown;
  else if (aFlag1 == kWarning || aFlag2 == kWarning)
    return kWarning;
  else
    return kGood;
}


} //ns core
} //ns swatch

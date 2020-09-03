/*
 * File:   Utilities.hpp
 * Author: ale
 *
 * Created on October 12, 2015, 12:21 PM
 */

#ifndef __SWATCH_AMC13_UTILITIES_HPP__
#define __SWATCH_AMC13_UTILITIES_HPP__


#include <stdint.h>
#include <string>                       // for string

#include "amc13/AMC13.hh"
#include "amc13/AMC13Simple.hh"         // for AMC13Simple::Board

namespace swatch {
namespace amc13 {

uint64_t read64bCounter(::amc13::AMC13& aDriver, ::amc13::AMC13::Board aBoard, const std::string& aName );

} // namespace amc13
} // namespace swatch


#endif  /* __SWATCH_AMC13_UTILITIES_HPP__ */

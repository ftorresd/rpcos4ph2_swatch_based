/*
 * File:   ComponentState.hpp
 * Author: tom
 * Date:   February 2016
 */

#ifndef __SWATCH_DUMMY_COMPONENTSTATE_HPP__
#define __SWATCH_DUMMY_COMPONENTSTATE_HPP__


namespace swatch {
namespace dummy {

//! Represents the state (good/warning/error/unreachable) of the components within dummy AMC13s &processors
enum ComponentState {
  kGood,
  kWarning,
  kError,
  kNotReachable
};


} // end ns: dummy
} // end ns: swatch

#endif  /* _SWATCH_DUMMY_COMPONENTSTATE_HPP_ */


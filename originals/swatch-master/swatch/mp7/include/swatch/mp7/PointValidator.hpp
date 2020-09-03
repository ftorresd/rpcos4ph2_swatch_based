/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   Utilities.hpp
 * Author: ale
 *
 * Created on November 17, 2015, 4:49 PM
 */

#ifndef __SWATCH_MP7_POINTVALIDATOR_HPP__
#define __SWATCH_MP7_POINTVALIDATOR_HPP__

// Boost Headers
#include <boost/foreach.hpp>

// C++ Headers
#include <deque>
#include <string>
//#include <vector>
#include <iterator>

// XDAQ Headers
// #include "xdata/UnsignedInteger.h"
#include "swatch/xsimpletypedefs.hpp"
#include "swatch/core/exception.hpp"

#include "mp7/Orbit.hpp"

// Forward declaration
namespace mp7 {
namespace orbit {
class Metric;
} // namespace orbit
} // namespace mp7

// Orbit related functions
// TODO: move them into a separate library?

namespace swatch {
namespace mp7 {
namespace orbit {

/**
 *
 * @class PointValidator
 * @brief Helper class
 */
class PointValidator {
public:
  PointValidator(const XUInt_t& aBx, const XUInt_t& aCycle, const ::mp7::orbit::Metric& aMetric);

  bool isNull() const;
  void throwIfInvalid() const;

  ::mp7::orbit::Point point() const;
  operator ::mp7::orbit::Point() const;

private:
  bool isValid() const;

  //!
  const XUInt_t& mBx;

  //!
  const XUInt_t& mCycle;

  //!
  const ::mp7::orbit::Metric& mMetric;
};

SWATCH_DEFINE_EXCEPTION(InvalidPointError);
SWATCH_DEFINE_EXCEPTION(PointError);

}

} // namespace mp7
} // namespace swatch


#endif /* __SWATCH_MP7_POINTVALIDATOR_HPP__ */


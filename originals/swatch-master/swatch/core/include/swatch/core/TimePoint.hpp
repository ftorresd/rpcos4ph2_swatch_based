
#ifndef __SWATCH_CORE_TIMEPOINT_HPP__
#define __SWATCH_CORE_TIMEPOINT_HPP__

#include "boost/chrono/system_clocks.hpp"


namespace swatch {
namespace core {


typedef boost::chrono::steady_clock::time_point SteadyTimePoint_t ;
typedef boost::chrono::system_clock::time_point SystemTimePoint_t ;


struct TimePoint {
public:
  TimePoint();
  ~TimePoint();

  static TimePoint now();

  SteadyTimePoint_t steady;
  SystemTimePoint_t system;
};


} // end ns: core
} // end ns: swatch

#endif /* __SWATCH_CORE_TIMEPOINT_HPP__ */

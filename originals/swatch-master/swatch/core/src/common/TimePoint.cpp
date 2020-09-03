
#include "swatch/core/TimePoint.hpp"


namespace swatch {
namespace core {


TimePoint::TimePoint() :
  steady(),
  system()
{
}


TimePoint::~TimePoint()
{
}


TimePoint TimePoint::now()
{
  TimePoint lNow;
  lNow.steady = SteadyTimePoint_t::clock::now();
  lNow.system = SystemTimePoint_t::clock::now();
  return lNow;
}


} // end ns: core
} // end ns: swatch

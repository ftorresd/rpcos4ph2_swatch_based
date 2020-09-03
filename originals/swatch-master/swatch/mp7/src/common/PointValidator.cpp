
#include "swatch/mp7/PointValidator.hpp"


#include <sstream>

#include "swatch/xsimpletypedefs.hpp"

// MP7 Headers
#include "mp7/Orbit.hpp"

namespace swatch {
namespace mp7 {

namespace orbit {

// ----------------------------------------------------------------------------
PointValidator::PointValidator(const XUInt_t& aBx, const XUInt_t& aCycle, const ::mp7::orbit::Metric& aMetric) :
  mBx(aBx),
  mCycle(aCycle),
  mMetric(aMetric)

{
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
bool PointValidator::isValid() const
{
  return (
           // Bx is not finite or bx value out of range
           (mBx.isFinite() && mBx.value_ < mMetric.bunchCount()) &&
           // Cycle is not finite or out of range
           (mCycle.isFinite() && mCycle.value_ < mMetric.clockRatio())
         );
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
bool PointValidator::isNull() const
{
  // Null if neither bx nor cycle are numbers
  return (mBx.isNaN() && mCycle.isNaN());
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
::mp7::orbit::Point PointValidator::point() const
{
  if (isNull()) XCEPT_RAISE(PointError, "Failed to create an mp7::Point object");
  else if (!isValid()) XCEPT_RAISE(PointError, "Invalid point!");
  return ::mp7::orbit::Point(mBx, mCycle);
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
orbit::PointValidator::operator ::mp7::orbit::Point() const
{
  return point();
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
void PointValidator::throwIfInvalid() const
{
  if ( isValid() ) return;

  std::ostringstream lMsg;
  lMsg << "Invalid orbit point parameters (" << (uint32_t)mBx << ", " << (uint32_t)mCycle << ")";
  XCEPT_RAISE(InvalidPointError,lMsg.str());
}
// ----------------------------------------------------------------------------

} // namespace orbit


} // namespace mp7
} // namespace swatch

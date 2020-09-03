#include "swatch/mp7/cmds/OrbitConstraint.hpp"

#include "swatch/xsimpletypedefs.hpp"
#include "swatch/core/XParameterSet.hpp"

namespace swatch {
namespace mp7 {
namespace cmds {

// ----------------------------------------------------------------------------
OrbitConstraint::OrbitConstraint( const std::string& aBxName, const std::string& aCycleName, ::mp7::orbit::Metric aMetric, bool aAllowNull) :
  mBxName(aBxName),
  mCycleName(aCycleName),
  mMetric(aMetric),
  mAllowNull(aAllowNull)
{
  require<XUInt_t>(mBxName);
  require<XUInt_t>(mCycleName);
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
void OrbitConstraint::describe(std::ostream& aStream) const {
  if (mAllowNull) 
    aStream << "(";

  aStream << mBxName << "<" << mMetric.bunchCount() << " && " << mCycleName << "<" << mMetric.clockRatio(); 

  if (mAllowNull)
    aStream << ") || (isNaN(" << mBxName << ") && isNaN(" << mCycleName << "))"; 
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
core::XMatch OrbitConstraint::verify(const core::XParameterSet& aParams) const {

  const auto& lBx = aParams.get<XUInt_t>(mBxName);
  const auto& lCycle = aParams.get<XUInt_t>(mCycleName);

  return (
           // Bx is not finite or bx value out of range
          (lBx.isFinite() && lBx < mMetric.bunchCount()) &&
           // Cycle is not finite or out of range
          (lCycle.isFinite() && lCycle < mMetric.clockRatio())
         ) || ( 
            mAllowNull && lBx.isNaN() && lCycle.isNaN()
         );
  }
// ----------------------------------------------------------------------------


} // namespace cmds
} // namespace mp7
} // namespace swatch
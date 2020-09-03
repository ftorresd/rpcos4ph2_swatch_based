#include "swatch/core/rules/InRange.hpp"

#include "xdata/Boolean.h"
#include "xdata/Integer.h"
#include "xdata/Integer32.h"
#include "xdata/Integer64.h"
#include "xdata/UnsignedInteger.h"
#include "xdata/UnsignedInteger32.h"
#include "xdata/UnsignedInteger64.h"
#include "xdata/UnsignedShort.h"
#include "xdata/UnsignedLong.h"
#include "xdata/Float.h"
#include "xdata/Double.h"
#include "xdata/Vector.h"
#include "xdata/Table.h"


namespace swatch {
namespace core {
namespace rules {

// ----------------------------------------------------------------------------
template<typename T>
InRange<T>::InRange( const T& aLowerBound, const T& aUpperBound ) : mLowerBound(aLowerBound), mUpperBound(aUpperBound) {
	
	if (mLowerBound >= mUpperBound ) {
		std::ostringstream lMsg;
		lMsg << "Upper bound (" << mUpperBound << ") is smaller than lower bound (" << mLowerBound << ")";
		XCEPT_RAISE(XRuleArgumentError, lMsg.str());
	}
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
template<typename T>
XMatch InRange<T>::verify( const T& aValue ) const
{
  // const T& lValue = dynamic_cast<const T&>(aValue);
  return XMatch(mLowerBound < aValue && aValue < mUpperBound);
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template<typename T>
void InRange<T>::describe(std::ostream& aStream) const
{
  aStream << mLowerBound << " < x < " << mUpperBound;
}
// ----------------------------------------------------------------------------

template class InRange<xdata::Boolean>;   
template class InRange<xdata::Integer>;   
template class InRange<xdata::Integer32>;   
template class InRange<xdata::Integer64>;   
template class InRange<xdata::UnsignedInteger>;   
template class InRange<xdata::UnsignedInteger32>;   
template class InRange<xdata::UnsignedInteger64>;   
template class InRange<xdata::UnsignedShort>;   
template class InRange<xdata::UnsignedLong>;   
template class InRange<xdata::Float>;   
template class InRange<xdata::Double>;   


} // namespace rules
} // namespace core
} // namespace swatch

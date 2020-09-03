#include "swatch/core/rules/OutOfRange.hpp"

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
OutOfRange<T>::OutOfRange( const T& aLowerBound, const T& aUpperBound ) : mLowerBound(aLowerBound), mUpperBound(aUpperBound) {
	
	if (mLowerBound >= mUpperBound ) {
		std::ostringstream lMsg;
		lMsg << "Upper bound (" << mUpperBound << ") is smaller than lower bound (" << mLowerBound << ")";
		XCEPT_RAISE(XRuleArgumentError, lMsg.str());
	}
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
template<typename T>
XMatch OutOfRange<T>::verify( const T& aValue ) const
{
  // const T& lValue = dynamic_cast<const T&>(aValue);
  return XMatch(aValue < mLowerBound || mUpperBound < aValue);
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template<typename T>
void OutOfRange<T>::describe(std::ostream& aStream) const
{
  aStream << "(x < " << mLowerBound << " || x > " << mUpperBound << ")";
}
// ----------------------------------------------------------------------------

template class OutOfRange<xdata::Boolean>;   
template class OutOfRange<xdata::Integer>;   
template class OutOfRange<xdata::Integer32>;   
template class OutOfRange<xdata::Integer64>;   
template class OutOfRange<xdata::UnsignedInteger>;   
template class OutOfRange<xdata::UnsignedInteger32>;   
template class OutOfRange<xdata::UnsignedInteger64>;   
template class OutOfRange<xdata::UnsignedShort>;   
template class OutOfRange<xdata::UnsignedLong>;   
template class OutOfRange<xdata::Float>;   
template class OutOfRange<xdata::Double>;   


} // namespace rules
} // namespace core
} // namespace swatch

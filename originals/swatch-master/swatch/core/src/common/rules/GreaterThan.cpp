#include "swatch/core/rules/GreaterThan.hpp"

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
XMatch GreaterThan<T>::verify( const T& aValue ) const
{
  // const T& lValue = dynamic_cast<const T&>(aValue);
  return XMatch(aValue > lLowerBound);
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template<typename T>
void GreaterThan<T>::describe(std::ostream& aStream) const
{
  aStream << "x > " << lLowerBound;
}
// ----------------------------------------------------------------------------

template class GreaterThan<xdata::Boolean>;   
template class GreaterThan<xdata::Integer>;   
template class GreaterThan<xdata::Integer32>;   
template class GreaterThan<xdata::Integer64>;   
template class GreaterThan<xdata::UnsignedInteger>;   
template class GreaterThan<xdata::UnsignedInteger32>;   
template class GreaterThan<xdata::UnsignedInteger64>;   
template class GreaterThan<xdata::UnsignedShort>;   
template class GreaterThan<xdata::UnsignedLong>;   
template class GreaterThan<xdata::Float>;   
template class GreaterThan<xdata::Double>;   


} // namespace rules
} // namespace core
} // namespace swatch

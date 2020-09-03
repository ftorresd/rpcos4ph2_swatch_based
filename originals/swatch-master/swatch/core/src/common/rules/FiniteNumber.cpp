#include "swatch/core/rules/FiniteNumber.hpp"

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
XMatch FiniteNumber<T>::verify( const T& aValue ) const
{
  const T& lValue = dynamic_cast<const T&>(aValue);
  return XMatch(lValue.isFinite());
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template<typename T>
void FiniteNumber<T>::describe(std::ostream& aStream) const
{
  aStream << "isFinite(x)";
}
// ----------------------------------------------------------------------------

template class FiniteNumber<xdata::Boolean>;   
template class FiniteNumber<xdata::Integer>;   
template class FiniteNumber<xdata::Integer32>;   
template class FiniteNumber<xdata::Integer64>;   
template class FiniteNumber<xdata::UnsignedInteger>;   
template class FiniteNumber<xdata::UnsignedInteger32>;   
template class FiniteNumber<xdata::UnsignedInteger64>;   
template class FiniteNumber<xdata::UnsignedShort>;   
template class FiniteNumber<xdata::UnsignedLong>;   
template class FiniteNumber<xdata::Float>;   
template class FiniteNumber<xdata::Double>;   


} // namespace rules
} // namespace core
} // namespace swatch

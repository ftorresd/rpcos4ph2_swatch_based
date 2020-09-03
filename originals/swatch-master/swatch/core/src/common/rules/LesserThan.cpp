#include "swatch/core/rules/LesserThan.hpp"

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
XMatch LesserThan<T>::verify( const T& aValue ) const
{
  // const T& lValue = dynamic_cast<const T&>(aValue);
  return XMatch(aValue < lLowerBound);
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template<typename T>
void LesserThan<T>::describe(std::ostream& aStream) const
{
  aStream << "x < " << lLowerBound;
}
// ----------------------------------------------------------------------------

template class LesserThan<xdata::Boolean>;   
template class LesserThan<xdata::Integer>;   
template class LesserThan<xdata::Integer32>;   
template class LesserThan<xdata::Integer64>;   
template class LesserThan<xdata::UnsignedInteger>;   
template class LesserThan<xdata::UnsignedInteger32>;   
template class LesserThan<xdata::UnsignedInteger64>;   
template class LesserThan<xdata::UnsignedShort>;   
template class LesserThan<xdata::UnsignedLong>;   
template class LesserThan<xdata::Float>;   
template class LesserThan<xdata::Double>;   


} // namespace rules
} // namespace core
} // namespace swatch

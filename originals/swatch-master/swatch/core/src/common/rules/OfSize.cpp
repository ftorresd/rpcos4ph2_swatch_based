#include "swatch/core/rules/OfSize.hpp"

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
XMatch OfSize<T>::verify(const T& aValue) const 
{
  return aValue.size() == mSize;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template<typename T>
void OfSize<T>::describe(std::ostream& aStream) const
{
  aStream << "len(x) = " << mSize;
}
// ----------------------------------------------------------------------------

template class OfSize<xdata::Vector<xdata::Boolean>>;
template class OfSize<xdata::Vector<xdata::Integer>>;
template class OfSize<xdata::Vector<xdata::Integer32>>;
template class OfSize<xdata::Vector<xdata::Integer64>>;
template class OfSize<xdata::Vector<xdata::UnsignedInteger>>;
template class OfSize<xdata::Vector<xdata::UnsignedInteger32>>;
template class OfSize<xdata::Vector<xdata::UnsignedInteger64>>;
template class OfSize<xdata::Vector<xdata::UnsignedShort>>;
template class OfSize<xdata::Vector<xdata::UnsignedLong>>;
template class OfSize<xdata::Vector<xdata::Float>>;
template class OfSize<xdata::Vector<xdata::Double>>;

} // namespace rules
} // namespace core
} // namespace swatch

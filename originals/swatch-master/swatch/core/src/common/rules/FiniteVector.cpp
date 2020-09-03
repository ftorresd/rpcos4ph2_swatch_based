#include "swatch/core/rules/FiniteVector.hpp"

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
XMatch FiniteVector<T>::verify(const T& aValue) const 
{
  const T& lValue = dynamic_cast<const T&>(aValue);
  return {std::all_of(
      lValue.begin(), lValue.end(),
      [] ( const typename T::value_type& aX ) { return aX.isFinite(); }
    ),
    ""
  };
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template<typename T>
void FiniteVector<T>::describe(std::ostream& aStream) const
{
  aStream << "all(x,isFinite)";
}
// ----------------------------------------------------------------------------

template class FiniteVector<xdata::Vector<xdata::Boolean>>;
template class FiniteVector<xdata::Vector<xdata::Integer>>;
template class FiniteVector<xdata::Vector<xdata::Integer32>>;
template class FiniteVector<xdata::Vector<xdata::Integer64>>;
template class FiniteVector<xdata::Vector<xdata::UnsignedInteger>>;
template class FiniteVector<xdata::Vector<xdata::UnsignedInteger32>>;
template class FiniteVector<xdata::Vector<xdata::UnsignedInteger64>>;
template class FiniteVector<xdata::Vector<xdata::UnsignedShort>>;
template class FiniteVector<xdata::Vector<xdata::UnsignedLong>>;
template class FiniteVector<xdata::Vector<xdata::Float>>;
template class FiniteVector<xdata::Vector<xdata::Double>>;

} // namespace rules
} // namespace core
} // namespace swatch

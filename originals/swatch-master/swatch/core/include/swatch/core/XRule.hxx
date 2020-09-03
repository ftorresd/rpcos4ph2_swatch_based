#ifndef __SWATCH_CORE_XRULE_HXX__
#define __SWATCH_CORE_XRULE_HXX__

#include "XRule.hpp"

namespace swatch {
namespace core {

// ----------------------------------------------------------------------------
template<typename T>
const std::type_info& XRule<T>::type() const
{ 
  return typeid(T);
};
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template<typename T>
XMatch XRule<T>::operator()(const xdata::Serializable& aSerializable) const
{
  try {
    const T& aValue = dynamic_cast<const T&>(aSerializable);
    return verify(aValue);
  } catch ( const std::bad_cast& lExc) {
    // Do something?
    XCEPT_RAISE(XRuleTypeMismatch, "Mismatch between expected Serializable type and parameter type.");
  }
}
// ----------------------------------------------------------------------------


} // namespace core
} // namespace swatch

#endif /* __SWATCH_CORE_XRULE_HXX__ */
#ifndef __SWATCH_CORE_RULES_NONE_HPP__
#define __SWATCH_CORE_RULES_NONE_HPP__


#include "swatch/core/XRule.hpp"


namespace swatch {
namespace core {
namespace rules {

// ----------------------------------------------------------------------------


//! Class representing no rule - i.e. all xdata::Serializable-derived objects pass this rule.
template <typename T>
class None: public XRule<T> {
public:

  virtual XMatch verify( const T& aValue ) const final;

private:

  virtual void describe(std::ostream& aStream) const final;

};
// ----------------------------------------------------------------------------



// ----------------------------------------------------------------------------
template<typename T>
XMatch None<T>::verify( const T& aValue ) const 
{
  return XMatch(true);
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
template<typename T>
void None<T>::describe(std::ostream& aStream) const 
{
  aStream << "true"; 
}
// ----------------------------------------------------------------------------

} // namespace rules
} // namespace core
} // namespace swatch

#endif /* __SWATCH_CORE_RULES_NONE_HPP__ */
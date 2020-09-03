#ifndef __SWATCH_CORE_RULES_NONEMPTYSTRING_HPP__
#define __SWATCH_CORE_RULES_NONEMPTYSTRING_HPP__

#include "swatch/core/XRule.hpp"
#include "xdata/String.h"

namespace swatch {
namespace core {
namespace rules {

class NonEmptyString : public XRule<xdata::String> {

public:
  NonEmptyString() {};
  ~NonEmptyString() {};

  virtual XMatch verify( const xdata::String& aValue ) const;

private:

  virtual void describe(std::ostream& aStream) const;
  
};

} // namespace rules	
} // namespace core
} // namespace swatch

#endif /* __SWATCH_CORE_RULES_NONEMPTYSTRING_HPP__ */
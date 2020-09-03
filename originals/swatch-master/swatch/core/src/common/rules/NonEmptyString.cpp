#include "swatch/core/rules/NonEmptyString.hpp"

namespace swatch {
namespace core {
namespace rules {

// ----------------------------------------------------------------------------
XMatch NonEmptyString::verify(const xdata::String& aValue) const 
{
	return !aValue.value_.empty();
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
void NonEmptyString::describe(std::ostream& aStream) const
{
  aStream << "!x.empty()";
}
// ----------------------------------------------------------------------------

} // namespace rules
} // namespace core
} // namespace swatch

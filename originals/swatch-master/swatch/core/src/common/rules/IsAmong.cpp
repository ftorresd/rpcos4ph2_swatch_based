#include "swatch/core/rules/IsAmong.hpp"


// SWATCH headers
#include "swatch/core/utilities.hpp"


namespace swatch {
namespace core {
namespace rules {


// ----------------------------------------------------------------------------
XMatch IsAmong::verify( const xdata::String& aValue ) const
{
//  return (std::find(mChoices.begin(), mChoices.end(), aValue.toString()) != mChoices.end());
  return {std::find(mChoices.begin(), mChoices.end(), aValue.toString()) != mChoices.end(), ""};
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
void IsAmong::describe(std::ostream& aStream) const
{
  aStream << "x in {" << core::join(mChoices, ", ") << "}";
}
// ----------------------------------------------------------------------------


} // namespace rules
} // namespace core
} // namespace swatch



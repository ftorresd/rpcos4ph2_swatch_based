
#include "swatch/core/XRule.hpp"


namespace swatch {
namespace core {

// ----------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& aOut, const AbstractXRule& aRule)
{
  aRule.describe(aOut);
  return aOut;
}
// ----------------------------------------------------------------------------


} // namespace core
} // namespace swatch

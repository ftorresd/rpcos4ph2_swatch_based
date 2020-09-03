#include "swatch/core/XMatch.hpp"

namespace swatch {
namespace core {


// ----------------------------------------------------------------------------
XMatch::XMatch(bool ok, std::string details) :
ok(ok), details(details)
{
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
bool XMatch::operator!=(const XMatch& right) const
{
  bool result = !(*this == right); // Reuse equals operator
  return result;
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
bool XMatch::operator==(const XMatch& right) const
{
  bool result = (this->ok == right.ok && this->details == right.details); // Compare right and *this here
  return result;
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& os, const XMatch& obj)
{
  // Write obj to stream
  os << obj.ok;
  return os;
}
// ----------------------------------------------------------------------------

} // namespace core
} // namespace swatch

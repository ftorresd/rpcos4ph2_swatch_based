#include "swatch/core/rules/PathExists.hpp"

#include <boost/filesystem.hpp>

namespace swatch {
namespace core {
namespace rules {

// ----------------------------------------------------------------------------
XMatch PathExists::verify(const xdata::String& aValue) const 
{

	if ( aValue.value_.empty() ) {
		return XMatch(false, "Empty path");
	}


	boost::filesystem::path lPath = boost::filesystem::path(aValue);

	if (!mPrefix.empty()) 
		lPath = boost::filesystem::path(mPrefix) / lPath;

	if (!mExtension.empty())
		lPath.replace_extension(mExtension);

	return (boost::filesystem::exists(lPath));
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
void PathExists::describe(std::ostream& aStream) const
{
  aStream << "exists(x)";
}
// ----------------------------------------------------------------------------

} // namespace rules
} // namespace core
} // namespace swatch

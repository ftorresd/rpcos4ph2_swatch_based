/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "swatch/core/XPSetConstraint.hpp"

#include "swatch/core/utilities.hpp"
#include "swatch/core/XParameterSet.hpp"
#include "swatch/core/ReadOnlyXParameterSet.hpp"
#include "xdata/Serializable.h"

namespace swatch {
namespace core {


// ----------------------------------------------------------------------------
std::set<std::string> XPSetConstraint::getParameterNames() const
{
  std::set<std::string> lNames;
  std::transform(mParameterTypeMap.begin(), mParameterTypeMap.end(), 
    std::inserter(lNames, lNames.end()), 
    boost::bind(&ParameterTypeMap_t::value_type::first, _1)
  );
  return lNames;
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
XMatch XPSetConstraint::operator() ( const XParameterSet& aParams ) const 
{
  core::ReadOnlyXParameterSet lParams(aParams);

  throwIfNotApplicable(lParams);
  

  // Create a minimal parameter set to feed verify
  core::ReadOnlyXParameterSet lOnlyRequired;
  for ( const std::string& lName : getParameterNames() ) {
    lOnlyRequired.adopt(lName, lParams);
  }

  // Additional try-catch block wrapping usercode "just in case"
  try {
    return verify( lOnlyRequired );
  } catch ( const exception& lExc ) {
    std::ostringstream lExcMsg;
    lExcMsg << "Failed to apply constraint" << *this << ": " << lExc.what();
    XCEPT_RAISE(ConstraintError, lExcMsg.str());
  }
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
void XPSetConstraint::throwIfNotApplicable( const XParameterSet& aParams ) const
{
  std::set<std::string> lNotFound;
  std::map<std::string, std::pair<const std::type_info*, const std::type_info*> > lTypeMismatches;

  for( const auto& x : mParameterTypeMap ) {

    const xdata::Serializable* lParamPtr;
    try {
      lParamPtr = &(aParams.get(x.first));
    } catch ( const XParameterNotFound& lExc ) {
      // std::cout << x.first << " is missing!" << std::endl;
      lNotFound.insert(x.first);
      continue;
    }

    // std::cout << x.first << " - " << x.second << " - " << &typeid(*lParamPtr) << std::endl;

    if ( *(x.second) != typeid(*lParamPtr)  ) {
        lTypeMismatches[x.first] = { x.second, &typeid(*lParamPtr) };
    }
  }

  if ( !(lNotFound.empty() && lTypeMismatches.empty()) ) {

    std::ostringstream lExcMsg;
    lExcMsg << "Error detected while applying XParameterSet rules." << std::endl;

    // Fill the parameters not found section
    if ( !lNotFound.empty() ) {
      lExcMsg << "Missing parameters: ";
      lExcMsg << "'" << *lNotFound.begin() << "'";

      std::for_each(
          std::next(lNotFound.begin()), lNotFound.end(), 
          [&lExcMsg](const std::string& lStr ) { lExcMsg << ", '" <<  lStr << "'"; } );

      lExcMsg << std::endl;
    }

    // Fill the type mismatch section
    if ( !lTypeMismatches.empty() ) {
      lExcMsg << "Mismatching parameter types" << std::endl;
      for( const auto& lP : lTypeMismatches ) {
        lExcMsg << "  " << lP.first << ":"
                << " expected '" << demangleName(lP.second.first->name()) << "'"
                << " found '" << demangleName(lP.second.second->name()) << "'"
                << std::endl; 
      }
    }

    XCEPT_RAISE(ConstraintIncompatibleParameter,lExcMsg.str());

  }
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& aOut, const XPSetConstraint& aConstraint)
{
  aConstraint.describe(aOut);
  return aOut;
}
// ----------------------------------------------------------------------------

} // namespace core
} // namespace swatch

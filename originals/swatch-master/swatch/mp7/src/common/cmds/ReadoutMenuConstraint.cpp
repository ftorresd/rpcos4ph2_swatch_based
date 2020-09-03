#include "swatch/mp7/cmds/ReadoutMenuConstraint.hpp"

#include "swatch/core/XParameterSet.hpp"
#include "swatch/core/rules/None.hpp"
#include "swatch/mp7/cmds/ReadoutMenuHelper.hpp"


#include "mp7/exception.hpp"

#include <boost/lexical_cast.hpp>

// #include "xdata/Boolean.h"
// #include "xdata/UnsignedInteger.h"
// #include "swatch/xsimpletypedefs.hpp"


namespace swatch {
namespace mp7 {
namespace cmds {

// ----------------------------------------------------------------------------
ReadoutMenuConstraint::ReadoutMenuConstraint(uint32_t aBanks, uint32_t aModes, uint32_t aCaptures) : 
  mBanks(aBanks),
  mModes(aModes),
  mCaptures(aCaptures)
{

  ReadoutMenuHelper(mBanks, mModes, mCaptures).addRequirements(*this);

}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
void ReadoutMenuConstraint::describe(std::ostream& aStream) const {
    aStream << "x.isGood()"; 
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
core::XMatch ReadoutMenuConstraint::verify(const core::XParameterSet& aParams) const {

  // Use the helper to import the readout menu from the input parameters
  ::mp7::ReadoutMenu lMenu = ReadoutMenuHelper(mBanks, mModes, mCaptures).import(aParams);
  
  // And the validator to ensure that evetything is alright
  try {
    ::mp7::ReadoutMenuValidator(lMenu).verify();
  } catch ( const ::mp7::ReadoutMenuConsistencyCheckFailed& e ) {
    return core::XMatch(false, e.what());
  }
  return true;
}
// ----------------------------------------------------------------------------


} // namespace cmds
} // namespace mp7
} // namespace swatch
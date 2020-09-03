#include "swatch/mp7/cmds/ReadoutMenuHelper.hpp"

#include "swatch/xsimpletypedefs.hpp"

#include "swatch/core/exception.hpp"
#include "swatch/action/Command.hpp"
#include "swatch/mp7/cmds/ReadoutMenuConstraint.hpp"

namespace swatch {
namespace mp7 {
namespace cmds {


// ----------------------------------------------------------------------------
ReadoutMenuHelper::ReadoutMenuHelper(uint32_t aBanks, uint32_t aModes, uint32_t aCaptures): 
  mBanks(aBanks),
  mModes(aModes),
  mCaptures(aCaptures)
{
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
ReadoutMenuHelper::~ReadoutMenuHelper()
{
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
::mp7::ReadoutMenu ReadoutMenuHelper::import(const ::swatch::core::XParameterSet& aParams)
{

  ::mp7::ReadoutMenu lMenu(mBanks, mModes, mCaptures);

  // Bank IDs
  std::string bankStr, modeStr, capStr;
  for ( uint32_t iB(0); iB < mBanks; ++iB ) {
    bankStr = "bank"+std::to_string(iB)+":";
    const XUInt_t& bxOffset = aParams.get<XUInt_t>(bankStr+"wordsPerBx");
    if ( bxOffset.isFinite()) lMenu.bank(iB).wordsPerBx = bxOffset;

  }

  for ( uint32_t iM(0); iM < mModes; ++iM ) {

    // Retrieve Mode parameters
    modeStr = "mode"+std::to_string(iM)+":";
    const XUInt_t& eventSize = aParams.get<XUInt_t>(modeStr+"eventSize");
    const XUInt_t& eventToTrigger = aParams.get<XUInt_t>(modeStr+"eventToTrigger");
    const XUInt_t& eventType = aParams.get<XUInt_t>(modeStr+"eventType");

    // Insert parameters in the menu
    ::mp7::ReadoutMenu::Mode& lMode = lMenu.mode(iM);
    if ( eventSize.isFinite()) lMode.eventSize = eventSize;
    if ( eventToTrigger.isFinite()) lMode.eventToTrigger = eventToTrigger;
    if ( eventType.isFinite()) lMode.eventType = eventType;


    // Loop over capture modes
    for ( uint32_t iC(0); iC < mCaptures; ++iC ) {

      // Retrieve capture more parameters
      capStr = modeStr+"capture"+std::to_string(iC)+":";

      const XBool_t& enable = aParams.get<XBool_t>(capStr+"enable");
      const XUInt_t& id = aParams.get<XUInt_t>(capStr+"id");
      const XUInt_t& bankId = aParams.get<XUInt_t>(capStr+"bankId");
      const XUInt_t& length = aParams.get<XUInt_t>(capStr+"length");
      const XUInt_t& delay = aParams.get<XUInt_t>(capStr+"delay");
      const XUInt_t& readoutLength = aParams.get<XUInt_t>(capStr+"readoutLength");

      // Insert the parameters in the menu
      ::mp7::ReadoutMenu::Capture& lCapture = lMode[iC];
      if ( enable.isFinite()) lCapture.enable = enable;
      if ( id.isFinite()) lCapture.id = id;
      if ( bankId.isFinite()) lCapture.bankId = bankId;
      if ( length.isFinite()) lCapture.length = length;
      if ( delay.isFinite()) lCapture.delay = delay;
      if ( readoutLength.isFinite()) lCapture.readoutLength = readoutLength;

    }
  }
  
  return lMenu;
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
void ReadoutMenuHelper::registerParameters(swatch::action::Command& aCommand)
{
  // const auto& str = boost::lexical_cast<std::string, uint32_t>;
 
  // Default initial values for unsigned integers
  XUInt_t lDefaultUint;
  core::rules::None<XUInt_t> lNoruleUint;

  // Default initial values for bools
  XBool_t lDefaultBool;
  core::rules::None<XBool_t> lNoruleBool;


  std::string bankStr, modeStr, capStr;
  for ( uint32_t iB(0); iB < mBanks; ++iB ) {
    bankStr = "bank"+std::to_string(iB)+":";
    aCommand.registerParameter(bankStr+"wordsPerBx", lDefaultUint, lNoruleUint);
  }

  for ( uint32_t iM(0); iM < mModes; ++iM ) {

    modeStr = "mode"+std::to_string(iM)+":";
    aCommand.registerParameter(modeStr+"eventSize", lDefaultUint, lNoruleUint);
    aCommand.registerParameter(modeStr+"eventToTrigger", lDefaultUint, lNoruleUint);
    aCommand.registerParameter(modeStr+"eventType", lDefaultUint, lNoruleUint);

    for ( uint32_t iC(0); iC < mCaptures; ++iC ) {

      capStr = modeStr+"capture"+std::to_string(iC)+":";

      aCommand.registerParameter(capStr+"enable", lDefaultBool, lNoruleBool);
      aCommand.registerParameter(capStr+"id", lDefaultUint, lNoruleUint);
      aCommand.registerParameter(capStr+"bankId", lDefaultUint, lNoruleUint);
      aCommand.registerParameter(capStr+"length", lDefaultUint, lNoruleUint);
      aCommand.registerParameter(capStr+"delay", lDefaultUint, lNoruleUint);
      aCommand.registerParameter(capStr+"readoutLength", lDefaultUint, lNoruleUint);

    }
  }
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
void ReadoutMenuHelper::addRequirements(ReadoutMenuConstraint& aConstraint)
{

  if ( aConstraint.mBanks != mBanks || aConstraint.mModes != mModes  || aConstraint.mCaptures != mCaptures ) {
    XCEPT_RAISE(core::RuntimeError, "Mismatch metween ");
  }

  std::string bankStr, modeStr, capStr;
  for ( uint32_t iB(0); iB < mBanks; ++iB ) {
    bankStr = "bank"+std::to_string(iB)+":";
    aConstraint.require<XUInt_t>(bankStr+"wordsPerBx");
  }


  for ( uint32_t iM(0); iM < mModes; ++iM ) {

    modeStr = "mode"+std::to_string(iM)+":";
    aConstraint.require<XUInt_t>(modeStr+"eventSize");
    aConstraint.require<XUInt_t>(modeStr+"eventToTrigger");
    aConstraint.require<XUInt_t>(modeStr+"eventType");

    for ( uint32_t iC(0); iC < mCaptures; ++iC ) {

      capStr = modeStr+"capture"+std::to_string(iC)+":";

      aConstraint.require<XBool_t>(capStr+"enable");
      aConstraint.require<XUInt_t>(capStr+"id");
      aConstraint.require<XUInt_t>(capStr+"bankId");
      aConstraint.require<XUInt_t>(capStr+"length");
      aConstraint.require<XUInt_t>(capStr+"delay");
      aConstraint.require<XUInt_t>(capStr+"readoutLength");

    }
  }

}
// ----------------------------------------------------------------------------


} // namespace cmds
} // namespace mp7
} // namespace swatch
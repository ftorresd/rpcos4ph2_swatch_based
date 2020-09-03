#include "swatch/mp7/cmds/LoadZeroSuppressionMenu.hpp"

// Boost HEaders
#include <boost/format.hpp>

// log4cplus headers
#include "log4cplus/loggingmacros.h"

// SWATCH headers
#include "swatch/xvectortypedefs.hpp"
#include "swatch/mp7/MP7Processor.hpp"
#include "swatch/core/rules/And.hpp"
#include "swatch/core/rules/FiniteVector.hpp"
#include "swatch/core/rules/OfSize.hpp"

#include "mp7/ZeroSuppressionNode.hpp"

namespace swatch {
namespace mp7 {
namespace cmds {

// ----------------------------------------------------------------------------
LoadZeroSuppressionMenu::LoadZeroSuppressionMenu(const std::string& aId, swatch::action::ActionableObject& aActionable):
  action::Command(aId, aActionable, XUInt_t()),
  mZSInstantiated(false),
  mNumCaptureIds(0)
{

  ::mp7::MP7Controller& lDriver = getActionable<MP7Processor>().driver();

  // Is the firmware ZS capable?
  // Check #1: Has this fw got the ZS node?
  
  if ( lDriver.getReadout().getNodes("readout_zs").empty() ) {
    // The readout_zs is not present. We're in a pre-2.2.1
    // Set flag and quit
    mZSInstantiated = false;
    return;
  }

  const ::mp7::ZeroSuppressionNode& zs = lDriver.getReadout().getNode< ::mp7::ZeroSuppressionNode>("readout_zs");

  // Check if the ZS block is enabled
  uhal::ValWord<uint32_t> lZsEnabled = zs.getNode("csr.info.zs_enabled").read();
  zs.getClient().dispatch();

  mZSInstantiated = (lZsEnabled == 1);
  
  if ( !mZSInstantiated ) return;

  registerParameter("validationMode", XUInt_t(0x0));



  core::rules::FiniteVector<XVectorUInt_t> lFiniteVec;
  core::rules::OfSize<XVectorUInt_t> lSize(::mp7::ZeroSuppressionNode::kBxMaskSize);
  core::rules::And<XVectorUInt_t >lFiniteAndSize6(lFiniteVec,lSize);


  mNumCaptureIds = ::mp7::ZeroSuppressionNode::kNumCapIds;
  // Register enable parameters and masks
  XBool_t lDefaultEnable(false), lDefaultInvert(false);
  XVectorUInt_t lDefaultMask;
  for ( size_t k(0); k<::mp7::ZeroSuppressionNode::kBxMaskSize; ++k) 
    lDefaultMask.emplace_back(XUInt_t(0x0));
  // std::copy (lDefaultMask.begin(), lDefaultMask.end(), std::back_inserter(std::vector<XUInt_t>(0x0, 6)));

  for ( uint32_t lCapId(0); lCapId < mNumCaptureIds; ++lCapId ) {
      std::string lParBase = boost::str( boost::format("capId[%x]:") % lCapId );
      registerParameter(lParBase+"enable", lDefaultEnable);
      registerParameter(lParBase+"invert", lDefaultInvert);
      registerParameter(lParBase+"mask", lDefaultMask, lFiniteAndSize6);
}

}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
action::Command::State LoadZeroSuppressionMenu::code(const ::swatch::core::XParameterSet& aParams)
{
  if (!mZSInstantiated) {
    setStatusMsg("ZeroSuppression is not enabled in the current firmware");
    return State::kError;
  }

  ::mp7::ZeroSuppressionMenu lMenu;

  // Populate menu
  const XUInt_t& lZSValMode = aParams.get<XUInt_t>("validationMode");
  lMenu.setValidationMode(lZSValMode);
  
  for ( uint32_t lCapId(0); lCapId < mNumCaptureIds; ++lCapId ) {
      std::string lParBase = boost::str( boost::format("capId[%x]:") % lCapId );
      const XBool_t& lEnableCap = aParams.get<XBool_t>(lParBase+"enable");
      const XBool_t& lInvertMask = aParams.get<XBool_t>(lParBase+"invert");

      if ( lEnableCap.isNaN() ) {
        std::string lMsg = boost::str(
          boost::format("Enable flag is undefined for capture id %x")
            % lCapId
            );
        XCEPT_RAISE(core::RuntimeError,lMsg);
      }
      
      const XVectorUInt_t& lMasksCap = aParams.get<XVectorUInt_t>(lParBase+"mask");
      
      if (lMasksCap.size() != ::mp7::ZeroSuppressionNode::kBxMaskSize ) {
        std::string lMsg = boost::str(
            boost::format("Unexpected mask size for capture id %x: expected %d, found %d") 
            % lCapId 
            % ::mp7::ZeroSuppressionNode::kBxMaskSize 
            % lMasksCap.size()
            );
        XCEPT_RAISE(core::RuntimeError,lMsg);
      }
      ::mp7::ZeroSuppressionMenu::Mask& lMask = lMenu[lCapId];
      lMask.enable = lEnableCap;
      lMask.invert = lInvertMask;
      for( size_t k(0); k < ::mp7::ZeroSuppressionNode::kBxMaskSize; ++k) {
        lMask.data[k] = lMasksCap[k];
      }
  }
  
  
  ::mp7::MP7Controller& lDriver = getActionable<MP7Processor>().driver();
  const ::mp7::ZeroSuppressionNode& zs = lDriver.getReadout().getNode< ::mp7::ZeroSuppressionNode>("readout_zs");

  // Apply menu
  zs.configureMenu(lMenu);

  return State::kDone;
}
// ----------------------------------------------------------------------------



} // namespace cmds
} // namespace mp7
} // namespace swatch


/*
 * File:   IOChannelSelector.cpp
 * Author: ale
 *
 * Created on February 4, 2016, 11:57 AM
 */

#include "swatch/mp7/IOChannelSelector.hpp"


// Standard headers
#include <stdint.h>                     // for uint32_t
#include <set>                          // for set, etc

// boost headers
#include <boost/lambda/lambda.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/range/adaptor/map.hpp>

// XDAQ headers
#include <xdata/String.h>

// MP7 headers
#include "mp7/MP7MiniController.hpp"

// SWATCH headers
#include "swatch/action/Command.hpp"      // for Command
#include "swatch/core/XParameterSet.hpp"  // for XParameterSet
#include "swatch/core/toolbox/IdSliceParser.hpp"
#include "swatch/core/rules/IsAmong.hpp"  // for IsAmong
#include "swatch/mp7/ChannelDescriptor.hpp"  // for ChannelDescriptor
#include "swatch/mp7/MP7AbstractProcessor.hpp"


namespace swatch {
namespace mp7 {

const ChannelRule_t IOChannelSelector::kAlwaysTrue = boost::lambda::constant(true);
const std::string IOChannelSelector::kIdSelection = "ids";


// ----------------------------------------------------------------------------
IOChannelSelector::IOChannelSelector(swatch::action::Command& aCommand) :
  mCommand(aCommand),
  mProcessor(aCommand.getActionable<MP7AbstractProcessor>())
{

}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
IOChannelSelector::~IOChannelSelector()
{
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
void IOChannelSelector::registerParameters()
{
  mCommand.registerParameter(kIdSelection, xdata::String());
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
std::string
IOChannelSelector::getIdSelection( const swatch::core::XParameterSet& aParams ) const
{
  return (std::string)aParams.get<xdata::String>(kIdSelection);
}


// ----------------------------------------------------------------------------
const ChannelRule_t&
IOChannelSelector::getMaskFilter(const swatch::core::XParameterSet& aParams) const
{
  return kAlwaysTrue;
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
std::vector<std::string> IOChannelSelector::extractIds(const ::mp7::ChannelManager& aManager) const
{
  return getDescriptors().chansToIds(aManager.getDescriptor().pickAllIDs().channels());

}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
::mp7::ChannelManager
IOChannelSelector::manager(const swatch::core::XParameterSet& aParams) const
{
  // Parse the list of selected ports
  const std::set<std::string>& lSelIdsSet = swatch::core::toolbox::IdSliceParser::parseSet(getIdSelection(aParams));

  // Get descriptor collection for this selector
  const ChannelDescriptorCollection& chans = getDescriptors();

  // If selected ids are empty, fall back on the full list of ids
  std::vector<std::string> lIds = ( not lSelIdsSet.empty() ? std::vector<std::string>(lSelIdsSet.begin(), lSelIdsSet.end()) : chans.ids());

  // Ensure lids satisfy the group rule for this selector
  chans.ensureIdsExistAndMatchRule(lIds,getGroupFilter());

  // Apply mask on ports
  std::vector<std::string> lEnabledIds = chans.filterIds(lIds, getMaskFilter(aParams));

  // Convert obj ids to channel ids
  std::vector<uint32_t> lEnabledChans = chans.idsToChans(lEnabledIds);


//  return IOChannelManager( mProcessor.driver(), lIdChanMap);
  return mProcessor.driver().channelMgr(lEnabledChans);
}

// ----------------------------------------------------------------------------
::mp7::MP7MiniController&
IOChannelSelector::getDriver()
{

  return mProcessor.driver();

}
// ----------------------------------------------------------------------------


//
// RxChannelSelector
//

const std::string RxChannelSelector::kMaskSelection = "masks";
const std::string RxChannelSelector::kApplyMasks = "apply";
const std::string RxChannelSelector::kInvertMasks = "invert";
const std::string RxChannelSelector::kIgnoreMasks = "ignore";

// ----------------------------------------------------------------------------
RxChannelSelector::RxChannelSelector(swatch::action::Command& aCommand, const ChannelRule_t& aFilter) :
  IOChannelSelector(aCommand),
  mRxGroupFilter(aFilter),
  mApplyMaskFilter(!boost::bind(&ChannelDescriptor::isMasked, _1)),
  mInvertMaskFilter(boost::bind(&ChannelDescriptor::isMasked, _1))
{
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
void RxChannelSelector::registerParameters()
{
  IOChannelSelector::registerParameters();
  mCommand.registerParameter(
    kMaskSelection, 
    xdata::String(kApplyMasks), 
    core::rules::IsAmong({kApplyMasks, kInvertMasks, kIgnoreMasks})
  );
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
const ChannelRule_t& RxChannelSelector::getGroupFilter() const
{
  return mRxGroupFilter;
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
const ChannelDescriptorCollection& RxChannelSelector::getDescriptors() const
{
  return mProcessor.getRxDescriptors();
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
const ChannelRule_t& RxChannelSelector::getMaskFilter(const swatch::core::XParameterSet& aParams) const
{
  const std::string& masks = aParams.get<xdata::String>(kMaskSelection).value_;

  if ( masks == kApplyMasks ) {
    return mApplyMaskFilter;
  }
  else if ( masks == kInvertMasks ) {
    return mInvertMaskFilter;
  }
  else if ( masks == kIgnoreMasks ) {
    return kAlwaysTrue;
  }
  else {
    XCEPT_RAISE(ApplyMaskOptionInvalid,"Unknown option "+masks);
  }
}
// ----------------------------------------------------------------------------

//
// TxChannelSelector
//

// ----------------------------------------------------------------------------
TxChannelSelector::TxChannelSelector(swatch::action::Command& aCommand, const ChannelRule_t& aFilter) :
  IOChannelSelector(aCommand),
  mTxGroupFilter(aFilter)
{
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
const ChannelRule_t& TxChannelSelector::getGroupFilter() const
{
  return mTxGroupFilter;
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
const ChannelDescriptorCollection& TxChannelSelector::getDescriptors() const
{
  return mProcessor.getTxDescriptors();
}
// ----------------------------------------------------------------------------


//
// RxMGTSelector
//
// ----------------------------------------------------------------------------
RxMGTSelector::RxMGTSelector(swatch::action::Command& aCommand) :
  RxChannelSelector(aCommand, boost::bind(&ChannelDescriptor::hasMGT, _1))
{
}
// ----------------------------------------------------------------------------

//
// TxMGTSelector
//

// ----------------------------------------------------------------------------
TxMGTSelector::TxMGTSelector(swatch::action::Command& aCommand) :
  TxChannelSelector(aCommand, boost::bind(&ChannelDescriptor::hasMGT, _1))
{
}
// ----------------------------------------------------------------------------


//
// RxBufferSelector
//

// ----------------------------------------------------------------------------
RxBufferSelector::RxBufferSelector(swatch::action::Command& aCommand) :
  RxChannelSelector(aCommand, boost::bind(&ChannelDescriptor::hasBuffer, _1))
{
}
// ----------------------------------------------------------------------------


//
// TxBufferSelector
//
// ----------------------------------------------------------------------------
TxBufferSelector::TxBufferSelector(swatch::action::Command& aCommand) :
  TxChannelSelector(aCommand, boost::bind(&ChannelDescriptor::hasBuffer, _1))
{
}
// ----------------------------------------------------------------------------





} // namespace mp7
} // namespace swatch

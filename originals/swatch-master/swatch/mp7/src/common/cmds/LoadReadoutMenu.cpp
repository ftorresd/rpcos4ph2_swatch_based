#include "swatch/mp7/cmds/LoadReadoutMenu.hpp"

// log4cplus headers
#include "log4cplus/loggingmacros.h"

// SWATCH headers
#include "swatch/xsimpletypedefs.hpp"
#include "swatch/mp7/MP7Processor.hpp"
#include "swatch/mp7/cmds/ReadoutMenuHelper.hpp"

namespace swatch {
namespace mp7 {
namespace cmds {


// ----------------------------------------------------------------------------
LoadReadoutMenu::LoadReadoutMenu( const std::string& aId, swatch::action::ActionableObject& aActionable ) :
  swatch::action::Command(aId, aActionable, XUInt_t())
{

  ::mp7::MP7Controller& lDriver = getActionable<MP7Processor>().driver();
  const ::mp7::ReadoutCtrlNode& rc = lDriver.getReadout().getNode< ::mp7::ReadoutCtrlNode>("readout_control");

  mBanks = rc.readNumBanks();
  mModes = rc.readNumModes();
  mCaptures = rc.readNumCaptures();

  ReadoutMenuHelper(mBanks, mModes, mCaptures).registerParameters(*this);

  addConstraint("menuconsistency", ReadoutMenuConstraint(mBanks, mModes, mCaptures));
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
LoadReadoutMenu::~LoadReadoutMenu()
{
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
action::Command::State
LoadReadoutMenu::code(const ::swatch::core::XParameterSet& aParams)
{

  ::mp7::ReadoutMenu lMenu = ReadoutMenuHelper(mBanks, mModes, mCaptures).import(aParams);

  ::mp7::MP7Controller& driver = getActionable<MP7Processor>().driver();
  const ::mp7::ReadoutCtrlNode& rc = driver.getReadout().getNode< ::mp7::ReadoutCtrlNode >("readout_control");


  std::map<uint32_t,uint32_t> lEventSizes = driver.computeEventSizes(lMenu);
  for ( uint32_t iM(0); iM < mModes; ++iM ) {
    ::mp7::ReadoutMenu::Mode& lMode = lMenu.mode(iM);
    if ( lMode.eventSize == 0xfffff ) continue;
    lMode.eventSize = lEventSizes.at(iM);
    LOG4CPLUS_INFO(getActionable().getLogger(), "Readout mode " << iM << " event size set to " << lMode.eventSize);
  }

  LOG4CPLUS_INFO(getActionable().getLogger(), "Configuring with readout menu: " << lMenu);

  rc.configureMenu(lMenu);

  return State::kDone;
}
// ----------------------------------------------------------------------------



} // namespace cmds
} // namespace mp7
} // namespace swatch


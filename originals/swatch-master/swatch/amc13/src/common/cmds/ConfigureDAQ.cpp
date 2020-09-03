#include "swatch/amc13/cmds/ConfigureDAQ.hpp"

#include "swatch/xsimpletypedefs.hpp"
#include "swatch/amc13/AMC13Manager.hpp"
#include "swatch/dtm/AMCPortCollection.hpp"
#include "swatch/dtm/AMCPort.hpp"       // for AMCPort

// AMC13 Headers
#include "amc13/AMC13.hh"
#include "amc13/AMC13Simple.hh"         // for AMC13Simple, etc


namespace swatch {
namespace amc13 {
namespace cmds {


// ----------------------------------------------------------------------------
ConfigureDAQ::ConfigureDAQ(const std::string& aId, swatch::action::ActionableObject& aActionable) :
  action::Command(aId, aActionable, xdata::String())
{
  // slots, fedId, slink, localTtc=False
  // Slinks
  registerParameter("slinkMask", XUInt_t(0x0));
  // Bunch counter reset offset
  registerParameter("bcnOffset", XUInt_t(0xdec-24));
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
action::Command::State
ConfigureDAQ::code(const core::XParameterSet& aParams)
{

  uint32_t slinkMask = aParams.get<XUInt_t>("slinkMask").value_;
  uint32_t bcnOffset = aParams.get<XUInt_t>("bcnOffset").value_;

  AMC13Manager& amc13mgr = getActionable<AMC13Manager>();

  // Extract list of AMCPorts
  const dtm::AMCPortCollection& amcPorts = amc13mgr.getAMCPorts();

  ::amc13::AMC13& board = amc13mgr.driver();

  // TODO: replace with a proper parameters
  uint32_t bitmask = 0x0;

  std::ostringstream oss;
  for(const dtm::AMCPort* p : amcPorts.getPorts()) {
    // Skip the slot if masked.
    if ( p->isMasked() ) continue;
    // Add it to the bitmask, otherwise.
    bitmask |= ( 1<< (p->getSlot()-1) ) ;
    oss << p->getSlot() << " ";
  }

  board.AMCInputEnable(bitmask);

  // Set FED ID
  board.setFEDid(amc13mgr.getStub().fedId);

  // Enable SFPs
  board.sfpOutputEnable(slinkMask);

  // Enable daq link if any of the SFPs is enabled
  board.daqLinkEnable( slinkMask != 0);

  // Bunch counter offset
  board.setBcnOffset(bcnOffset);

  // Reset counters
  board.resetCounters();

  // Reset T1, just in case
  board.reset(::amc13::AMC13Simple::T1);

  setResult(xdata::String("Enabled amports: "+oss.str()));

  return State::kDone;
}
// ----------------------------------------------------------------------------


} // namespace cmds
} // namespace amc13
} // namespace swatch

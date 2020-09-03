#include "swatch/amc13/cmds/ConfigureTTC.hpp"

#include "swatch/xsimpletypedefs.hpp"
#include "swatch/amc13/AMC13Manager.hpp"

// AMC13 Headers
#include "amc13/AMC13.hh"
#include "amc13/AMC13Simple.hh"         // for AMC13Simple, etc


namespace swatch {
namespace amc13 {
namespace cmds {

// --------------------------------------------------------
ConfigureTTC::ConfigureTTC(const std::string& aId, swatch::action::ActionableObject& aActionable) :
  Command(aId, aActionable, XInt_t())
{

  // TTC orbit counter encoding
  registerParameter("resyncCmd", XUInt_t(0x4));
  // TTC Resync encoding
  registerParameter("ocrCmd", XUInt_t(0x8));
  // Local ttc configuration
  registerParameter("localTTC", XBool_t(false));
}
// --------------------------------------------------------


// --------------------------------------------------------
action::Command::State ConfigureTTC::code(const core::XParameterSet& aParams)
{

  AMC13Manager& amc13mgr = getActionable<AMC13Manager>();

  uint32_t resyncCmd = aParams.get<XUInt_t>("resyncCmd");
  uint32_t ocrCmd    = aParams.get<XUInt_t>("ocrCmd");
  uint32_t localTTC  = aParams.get<XBool_t>("localTTC");

  ::amc13::AMC13& board = amc13mgr.driver();

  // configure TTC commands
  board.setOcrCommand(ocrCmd);

  // Replace with python bindings when they come out...
  board.setResyncCommand(resyncCmd);

  // Do we need this in this command?
  if ( localTTC ) board.localTtcSignalEnable(true);

  // activate TTC output to all AMCs
  board.enableAllTTC();

  return State::kDone;
}
// --------------------------------------------------------


} // namespace cmds
} // namespace amc13
} // namespace swatch
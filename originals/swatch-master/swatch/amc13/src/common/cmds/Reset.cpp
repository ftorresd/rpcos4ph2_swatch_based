#include "swatch/amc13/cmds/Reset.hpp"

#include "swatch/xsimpletypedefs.hpp"
#include "swatch/amc13/AMC13Manager.hpp"

// AMC13 Headers
#include "amc13/AMC13.hh"
#include "amc13/AMC13Simple.hh"         // for AMC13Simple, etc

namespace swatch {
namespace amc13 {
namespace cmds {

// ----------------------------------------------------------------------------
Reset::Reset(const std::string& aId, swatch::action::ActionableObject& aActionable) :
  Command(aId, aActionable, XInt_t())
{
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
action::Command::State Reset::code(const core::XParameterSet& aParams)
{

  AMC13Manager& amc13mgr = getActionable<AMC13Manager>();

  ::amc13::AMC13& board = amc13mgr.driver();

  // Reset T1 chip
  board.reset(::amc13::AMC13Simple::T1);

  // Reset T2 chip
  board.reset(::amc13::AMC13Simple::T2);

  // Take the board out of run mode
  board.endRun();

  // Disable SFP outputs
  board.sfpOutputEnable(0);

  // Disable AMC13-AMC connection
  board.AMCInputEnable(0);

  // Disable any TTS mask
  board.ttsDisableMask(0x0);

  // Disable local triggers
  board.localTtcSignalEnable(false);

  // Disable fake data generation
  board.fakeDataEnable(false);

  // Disable monitor buffer backpressure
  board.monBufBackPressEnable(false);

  // activate TTC output to all AMCs
  board.enableAllTTC();

  return State::kDone;
}
// ----------------------------------------------------------------------------


} // namespace cmds
} // namespace amc13
} // namespace swatch

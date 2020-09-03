#include "swatch/mp7/cmds/AutoAlign.hpp"

#include "swatch/xsimpletypedefs.hpp"
#include "swatch/mp7/MP7AbstractProcessor.hpp"
#include "swatch/mp7/PointValidator.hpp"
#include "swatch/core/utilities.hpp"

namespace swatch {
namespace mp7 {
namespace cmds {

// ----------------------------------------------------------------------------
AutoAlign::AutoAlign(const std::string& aId, swatch::action::ActionableObject& aActionable) :
  swatch::action::Command(aId, aActionable, xdata::String()),
  mRxMGTSelector(*this)
{
  mRxMGTSelector.registerParameters();

  registerParameter("margin", XUInt_t(3));
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
action::Command::State AutoAlign::code(const core::XParameterSet& aParams)
{
  const XUInt_t& lMargin = aParams.get<XUInt_t>("margin");

  ::mp7::ChannelManager cm = mRxMGTSelector.manager(aParams);

  setProgress(0.01, "Starting auto-align with margin=" + boost::lexical_cast<std::string>(lMargin.value_));

  try {
    cm.minimizeAndAlign(lMargin.value_);
  }
  catch (const ::mp7::AlignmentFailed& lExc) {

    std::vector<std::string> lIdsFailingAlignment = mRxMGTSelector.getDescriptors().chansToIds(lExc.channels());

    std::ostringstream lErr;
    lErr << "Alignment failed on input ports: " << core::joinAny(lIdsFailingAlignment);
    setStatusMsg(lErr.str());
    return State::kError;

  }
  catch (const ::mp7::exception& lExc) {

    std::ostringstream lErr;
    lErr << "Exception caught in alignment: " << lExc.what();
    setStatusMsg(lErr.str());
    return State::kError;

  }
  catch (const std::exception& lExc) {

    std::ostringstream lErr;
    lErr << "Exception caught in alignment: " << lExc.what();
    setStatusMsg(lErr.str());
    return State::kError;

  }

  // TODO: run post-mortem checks, if necessary
  // setProgress(0.6, "Checking input ports...");

  // Check Rx channel states before continuing
  //  std::map<std::uint32_t, ::mp7::RxChannelStatus> lRxStates = cm.readRxStatus();
  // Analyze channel status

  setStatusMsg("Align MGTs completed : "+core::joinAny(mRxMGTSelector.extractIds(cm)));

  return State::kDone;
}
// ----------------------------------------------------------------------------
// 

} // namespace cmds
} // namespace mp7
} // namespace swatch

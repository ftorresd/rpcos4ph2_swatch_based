#include "swatch/mp7/cmds/AlignRxsTo.hpp"

#include "log4cplus/loggingmacros.h"

#include "mp7/MP7MiniController.hpp"    // for MP7MiniController

#include "swatch/xsimpletypedefs.hpp"
#include "swatch/mp7/MP7AbstractProcessor.hpp"
#include "swatch/mp7/PointValidator.hpp"
#include "swatch/mp7/cmds/OrbitConstraint.hpp"


#include "swatch/core/XParameterSet.hpp"
#include "swatch/core/XPSetConstraint.hpp"

namespace swatch {
namespace mp7 {
namespace cmds {


// ----------------------------------------------------------------------------
AlignRxsTo::AlignRxsTo(const std::string& aId, swatch::action::ActionableObject& aActionable) :
  swatch::action::Command(aId, aActionable, xdata::String()),
  mRxMGTSelector(*this)
{

  mRxMGTSelector.registerParameters();

  registerParameter("bx", XUInt_t(0x0));
  registerParameter("cycle", XUInt_t(0x0));

  // Query the MP7 driver
  ::mp7::MP7MiniController& lDriver = getActionable<MP7AbstractProcessor>().driver();
  // to access the metric
  ::mp7::orbit::Metric lMetric = lDriver.getMetric();
  
  // Add a constraint based on the metric
  addConstraint("pointCheck", OrbitConstraint("bx", "cycle", lMetric));

}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
action::Command::State
AlignRxsTo::code(const swatch::core::XParameterSet& aParams)
{

  const XUInt_t& bx = aParams.get<XUInt_t>("bx");
  const XUInt_t& cycle = aParams.get<XUInt_t>("cycle");

  // FIXME: Embed this check somewhere
  ::mp7::MP7MiniController& lDriver = getActionable<MP7AbstractProcessor>().driver();
  ::mp7::orbit::Metric lMetric = lDriver.getMetric();

  // Check validity of target alignment point
  orbit::PointValidator lAlignTarget(bx, cycle, lMetric);

  // Ensure that the alignment target is valid
  lAlignTarget.throwIfInvalid();

  // Get a Channel Manager with user channel selection
  ::mp7::ChannelManager cm = mRxMGTSelector.manager(aParams);

  std::ostringstream lMsg;
  lMsg << "Aligning MGTs to " << lAlignTarget.point() << "...";
  setProgress(0., lMsg.str());

  // Clean msg stream
  lMsg.str("");
  bool lAlignmentDone = true;

  // Align Rx ports to
  try {
    cm.align(lAlignTarget);
  }
  catch (const ::mp7::AlignmentFailed& lExc) {
    lAlignmentDone = false;
    lMsg << "Alignment failed on input ports: " << core::joinAny(mRxMGTSelector.getDescriptors().chansToIds(lExc.channels()));

  }
  catch (const ::mp7::exception& lExc) {
    lAlignmentDone = false;
    lMsg << "Exception caught in alignment: " << lExc.what();

  }
  catch (const std::exception& lExc) {

    lAlignmentDone = false;
    lMsg << "Exception caught in alignment: " << lExc.what();

  }

  if ( !lAlignmentDone ) {
    // Run post mortem check
    for( auto lRxEntry : cm.readRxStatus() ) {
      // Get a reference to channel status
      const ::mp7::RxChannelStatus& lRxStatus = lRxEntry.second;
      // Resolve port ID
      std::string lId = mRxMGTSelector.getDescriptors().getByChannelId(lRxEntry.first).getId();
      if ( !lRxStatus.pllLocked )
        LOG4CPLUS_ERROR(getActionable().getLogger(), lId << " > pll unlocked");

      if ( lRxStatus.crcChecked == 0 or lRxStatus.crcErrors != 0 )
        LOG4CPLUS_ERROR(getActionable().getLogger(), lId << " > CRCs : Checked=" <<  lRxStatus.crcChecked << " Errors=" << lRxStatus.crcErrors);

      if ( !lRxStatus.fsmResetDone or lRxStatus.usrReset )
        LOG4CPLUS_ERROR(getActionable().getLogger(), lId << " > Reset : Done=" <<  lRxStatus.fsmResetDone << " User Reset=" << lRxStatus.usrReset);

    }
    setStatusMsg(lMsg.str());
    return State::kError;

  }

  setStatusMsg("Align MGTs completed : "+core::joinAny(mRxMGTSelector.extractIds(cm)));
  return State::kDone;
}
// ----------------------------------------------------------------------------

} // namespace cmds
} // namespace mp7
} // namespace swatch

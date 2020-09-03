#include "swatch/mp7/cmds/DatavalidFormatter.hpp"


// MP7 Core Headers
#include "mp7/MP7Controller.hpp"

// SWATCH Headers
#include "swatch/xsimpletypedefs.hpp"
#include "swatch/mp7/MP7AbstractProcessor.hpp"
#include "swatch/mp7/cmds/OrbitConstraint.hpp"
#include "swatch/mp7/PointValidator.hpp"


namespace swatch {
namespace mp7 {
namespace cmds {

// ----------------------------------------------------------------------------
DatavalidFormatter::DatavalidFormatter(const std::string& aId, swatch::action::ActionableObject& aActionable) :
  swatch::action::Command(aId, aActionable, xdata::String()),
  mFmtSelector(*this, boost::bind(&ChannelDescriptor::getFormatterKind, _1) == ::mp7::kDemuxFormatter)
{

  // add default parameters
  mFmtSelector.registerParameters();

  registerParameter("startBx", XUInt_t(0x0));
  registerParameter("startCycle", XUInt_t(0x0));
  registerParameter("stopBx", XUInt_t(0x0));
  registerParameter("stopCycle", XUInt_t(0x0));

  // Query the MP7 driver
  ::mp7::MP7MiniController& lDriver = getActionable<MP7AbstractProcessor>().driver();
  // to access the metric
  ::mp7::orbit::Metric lMetric = lDriver.getMetric();
  
  // Add a constraint based on the metric
  addConstraint("startPointCheck", OrbitConstraint("startBx", "startCycle", lMetric, true));
  addConstraint("stopPointCheck", OrbitConstraint("stopBx", "stopCycle", lMetric, true));

}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
swatch::action::Command::State DatavalidFormatter::code(const swatch::core::XParameterSet& aParams)
{

  const XUInt_t& startBx = aParams.get<XUInt_t>("startBx");
  const XUInt_t& startCycle = aParams.get<XUInt_t>("startCycle");
  const XUInt_t& stopBx = aParams.get<XUInt_t>("stopBx");
  const XUInt_t& stopCycle = aParams.get<XUInt_t>("stopCycle");

  ::mp7::MP7MiniController& lDriver = mFmtSelector.getDriver();
  ::mp7::orbit::Metric lMetric = lDriver.getMetric();

  ::mp7::ChannelManager lCM = mFmtSelector.manager(aParams);

  // Check dv begin orbitpoint
  orbit::PointValidator lStart( startBx, startCycle, lMetric);

  // Ensure that the start point is valid
  lStart.throwIfInvalid();

  // Check dv end orbitpoint
  orbit::PointValidator lStop( stopBx, stopCycle, lMetric);

  // Ensure that the end point is valid
  lStop.throwIfInvalid();

  // Fixme: this is always true
  if (not (lStart.isNull() and lStop.isNull() ) ) {

    setProgress(0.4, "Configuring data valid override...");
    lCM.configureDVFormatters(lStart, lStop);
  }
  else {
    setProgress(0.4, "Disabling data valid override...");
    lCM.disableDVFormatters();
  }
  setStatusMsg("Configure data valid complete");
  return State::kDone;
}
// ----------------------------------------------------------------------------


} // namespace cmds
} // namespace mp7
} // namespace swatch


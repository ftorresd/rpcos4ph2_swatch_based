#include "swatch/mp7/cmds/ZeroInputs.hpp"

#include "mp7/MP7MiniController.hpp"    // for MP7MiniController
#include "mp7/Logger.hpp"

#include "swatch/mp7/MP7AbstractProcessor.hpp" // for MP7AbstractProcessor


namespace swatch {
namespace mp7 {
namespace cmds {

// ----------------------------------------------------------------------------
ZeroInputs::ZeroInputs(const std::string& aId, swatch::action::ActionableObject& aActionable) :
  swatch::action::Command(aId, aActionable, xdata::String())
{

}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
swatch::action::Command::State ZeroInputs::code(const ::swatch::core::XParameterSet& aParams)
{
  // Extract the MP7 driver
  ::mp7::MP7MiniController& driver = getActionable<MP7AbstractProcessor>().driver();
  ::mp7::orbit::Metric metric = driver.getMetric();

  // Create a base configurator
  ::mp7::TestPathConfigurator lZeroConfigurator = ::mp7::TestPathConfigurator(::mp7::TestPathConfigurator::kZeroes, ::mp7::orbit::Point(), metric);

  // Configure
  ::mp7::ChannelManager cm = driver.channelMgr();

  //
  std::ostringstream zeroedIds;
  zeroedIds << core::shortVecFmt(cm.getDescriptor().pickRxBufferIDs(::mp7::kBuffer).channels());
  setStatusMsg("Masking MP7 hardware inputs channels" + zeroedIds.str());

  // Zero
  cm.configureBuffers(::mp7::kRx, lZeroConfigurator);

  setResult(xdata::String("Masked MP7 inputs "+zeroedIds.str()));

  return State::kDone;
}
// ----------------------------------------------------------------------------

} // namespace cmds
} // namespace mp7
} // namespace swatch


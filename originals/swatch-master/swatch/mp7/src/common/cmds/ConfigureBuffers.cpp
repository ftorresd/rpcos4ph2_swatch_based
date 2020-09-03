#include "swatch/mp7/cmds/ConfigureBuffers.hpp"

#include <log4cplus/loggingmacros.h>

#include <boost/range/algorithm/copy.hpp>
#include <boost/range/adaptor/map.hpp>

// SWATCH headers
#include "swatch/xsimpletypedefs.hpp"
#include "swatch/mp7/cmds/BufferTraits.hpp"
#include "swatch/mp7/PointValidator.hpp"
#include "swatch/mp7/cmds/OrbitConstraint.hpp"
#include "swatch/mp7/MP7AbstractProcessor.hpp"
#include "swatch/core/utilities.hpp"

#include "swatch/core/rules/None.hpp"
#include "swatch/core/rules/IsAmong.hpp"

// MP7 Core Headers
#include "mp7/MP7MiniController.hpp"    // for MP7MiniController

namespace swatch {
namespace mp7 {
namespace cmds {

// ----------------------------------------------------------------------------
// Static initialization
template<class Selector> const std::map< std::string, ::mp7::TestPathConfigurator::Mode > ConfigureBuffers<Selector>::kBufferModeMap = {
  {"Latency", ::mp7::TestPathConfigurator::kLatency},
  {"Capture", ::mp7::TestPathConfigurator::kCapture},
  {"PlayOnce", ::mp7::TestPathConfigurator::kPlayOnce},
  {"PlayLoop", ::mp7::TestPathConfigurator::kPlayLoop},
  {"Pattern", ::mp7::TestPathConfigurator::kPattern},
  {"Zeroes", ::mp7::TestPathConfigurator::kZeroes},
  {"CaptureStrobe", ::mp7::TestPathConfigurator::kCaptureStrobe},
  {"Pattern3G", ::mp7::TestPathConfigurator::kPattern3G},
  {"PlayOnceStrobe", ::mp7::TestPathConfigurator::kPlayOnceStrobe},
  {"PlayOnce3G", ::mp7::TestPathConfigurator::kPlayOnce3G}
};
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
template<class Selector>
ConfigureBuffers<Selector>::ConfigureBuffers(const std::string& aId, swatch::action::ActionableObject& aActionable) :
  swatch::action::Command(aId, aActionable, xdata::String()),
  mBufferSelector(*this)
{
// 
  mBufferSelector.registerParameters();

  // Collect the names of all known modes for late use
  std::vector<std::string> lModeNames;
  boost::copy(kBufferModeMap | boost::adaptors::map_keys, std::back_inserter(lModeNames));

  // Parameter registration
  registerParameter("startBx", XUInt_t(0x0));
  registerParameter("startCycle", XUInt_t(0x0));
  registerParameter("stopBx", XUInt_t(), core::rules::None<XUInt_t>());
  registerParameter("stopCycle", XUInt_t(), core::rules::None<XUInt_t>());
  registerParameter("payload", xdata::String(""));
  registerParameter("mode", xdata::String("Latency"), core::rules::IsAmong(lModeNames));

  // Query the MP7 driver
  ::mp7::MP7MiniController& lDriver = getActionable<MP7AbstractProcessor>().driver();
  // to access the metric
  ::mp7::orbit::Metric lMetric = lDriver.getMetric();
  // Add a constraint based on the metric
  addConstraint("startPointCheck", OrbitConstraint("startBx", "startCycle", lMetric));
  addConstraint("stopPointCheck", OrbitConstraint("stopBx", "stopCycle", lMetric, true));

}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
template<class Selector>
action::Command::State
ConfigureBuffers<Selector>::code(const ::swatch::core::XParameterSet& aParams)
{

  const ::mp7::RxTxSelector bKind = BufferTraits<Selector>::kRxTxSelector;

  // Extract parameter values
  std::string lBufModeStr = aParams.get<xdata::String>("mode");
  std::string lPayload = aParams.get<xdata::String>("payload");

  const XUInt_t& startBx = aParams.get<XUInt_t>("startBx");
  const XUInt_t& startCycle = aParams.get<XUInt_t>("startCycle");
  const XUInt_t& stopBx = aParams.get<XUInt_t>("stopBx");
  const XUInt_t& stopCycle = aParams.get<XUInt_t>("stopCycle");

  // Instantiate the message streamer once
  std::ostringstream lMsg;
  // Extract the MP7 driver
  ::mp7::MP7MiniController& driver = getActionable<MP7AbstractProcessor>().driver();
  ::mp7::orbit::Metric metric = driver.getMetric();

  orbit::PointValidator lStart(startBx, startCycle, metric);

  // Ensure that the start point is valid
  lStart.throwIfInvalid();

  size_t bufferSize = driver.getBuffer().getBufferSize();

  lMsg.str("");
  lMsg << "Configuring " << bKind << " buffer...";
  setProgress(0., lMsg.str());

  // Reset lMsg
  lMsg.str("");
  // Populate message
  lMsg << "Mode set: " << lBufModeStr;

  if (!lPayload.empty())
    lMsg << " reading from: " << lPayload;

  setStatusMsg(lMsg.str());

  ::mp7::ChannelManager cm = mBufferSelector.manager(aParams);
  setProgress(0.2, "Generating BoardData object...");

  // Reset lMsg
  lMsg.str("");
  // Populate message
  lMsg << "Setting range : start point " << lStart.point();
  setProgress(0.4, lMsg.str());

  ::mp7::TestPathConfigurator::Mode lBufMode;
  try {
    lBufMode = kBufferModeMap.at(lBufModeStr);
  }
  catch (std::out_of_range& e) {
    setStatusMsg("Unknown buffer mode: '"+lBufModeStr+"'");
    return State::kError;
  }

  // Create a base configurator
  ::mp7::TestPathConfigurator lPathCfg = ::mp7::TestPathConfigurator(lBufMode, lStart, metric);

  orbit::PointValidator lStop(stopBx, stopCycle, metric);

  // If stop is defined, update it
  if (!lStop.isNull()) {

    // Ensure that the stop point is invalid
    lStop.throwIfInvalid();

    lMsg << "Setting range : stop point " << lStop.point();

    // Update configurator with fixed range
    lPathCfg = ::mp7::TestPathConfigurator(lBufMode, lStart, lStop, metric);

  }

  setProgress(0.6, "Configuring buffers...");


  // Apply configuration
  cm.configureBuffers(bKind, lPathCfg);

  if (!lPayload.empty()) {
    LOG4CPLUS_INFO(getActionable().getLogger(), "loading buffers with payload " << lPayload);

    ::mp7::BoardData data = ::mp7::BoardDataFactory::generate(lPayload, bufferSize, true);
    cm.clearBuffers(bKind);
    cm.loadPatterns(bKind, data);

  }
  else {
    cm.clearBuffers(bKind, ::mp7::ChanBufferNode::kCapture);
  }

  setStatusMsg("Buffers configuration completed : "+core::joinAny(mBufferSelector.extractIds(cm)) );

  return State::kDone;
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
template class ConfigureBuffers<RxBufferSelector>;
template class ConfigureBuffers<TxBufferSelector>;
// ----------------------------------------------------------------------------

} // namespace cmds
} // namespace mp7
} // namespace swatch
  // 
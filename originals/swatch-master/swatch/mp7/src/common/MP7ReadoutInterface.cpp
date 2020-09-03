
#include "swatch/mp7/MP7ReadoutInterface.hpp"


// Standard headers
#include <stdint.h>                     // for uint32_t, uint64_t

// SWATCH headers
#include "swatch/core/TTSUtils.hpp"

// uHAL headers
#include "uhal/ClientInterface.hpp"     // for ClientInterface
#include "uhal/Node.hpp"                // for Node
#include "uhal/ValMem.hpp"              // for ValWord

// MP7 Headers
#include "mp7/MP7Controller.hpp"
#include "mp7/ReadoutNode.hpp"


namespace swatch {
namespace mp7 {


MP7ReadoutInterface::MP7ReadoutInterface(::mp7::MP7Controller& aController) :
  mDriver(aController),
  mMetricUpTime(registerMetric<float>("upTime")),
  mMetricBusyTime(registerMetric<float>("busyTime")),
  mMetricReadyTime(registerMetric<float>("readyTime")),
  mMetricWarnTime(registerMetric<float>("warnTime")),
  mMetricOOSTime(registerMetric<float>("oosTime"))

{
}


MP7ReadoutInterface::~MP7ReadoutInterface()
{
}


void MP7ReadoutInterface::retrieveMetricValues()
{
  const ::mp7::ReadoutNode& readOut = mDriver.getReadout();

  const float kClk40 = 40e6;

  setMetricValue<>(mMetricTTS, static_cast<core::tts::State>(readOut.readTTSState()));
  setMetricValue<>(mMetricAMCCoreReady, (bool)readOut.isAMC13LinkReady());
  setMetricValue<>(mMetricEventCounter,readOut.readEventCounter());

  uhal::ValWord<uint32_t> upCounterLo, upCounterHi, busyCounterLo, busyCounterHi, readyCounterLo, readyCounterHi, warnCounterLo, warnCounterHi, oosCounterLo, oosCounterHi;
  upCounterHi = readOut.getNode("tts_ctrs.uptime_ctr_h").read();
  upCounterLo = readOut.getNode("tts_ctrs.uptime_ctr_l").read();
  busyCounterHi = readOut.getNode("tts_ctrs.busy_ctr_h").read();
  busyCounterLo = readOut.getNode("tts_ctrs.busy_ctr_l").read();
  readyCounterHi = readOut.getNode("tts_ctrs.ready_ctr_h").read();
  readyCounterLo = readOut.getNode("tts_ctrs.ready_ctr_l").read();
  warnCounterHi = readOut.getNode("tts_ctrs.warn_ctr_h").read();
  warnCounterLo = readOut.getNode("tts_ctrs.warn_ctr_l").read();
  oosCounterHi = readOut.getNode("tts_ctrs.oos_ctr_h").read();
  oosCounterLo = readOut.getNode("tts_ctrs.oos_ctr_l").read();

  readOut.getClient().dispatch();

  setMetricValue<>(mMetricUpTime,(((uint64_t)upCounterHi.value() << 32) + (uint32_t)upCounterLo)/kClk40);
  setMetricValue<>(mMetricBusyTime,(((uint64_t)busyCounterHi.value() << 32) + (uint32_t)busyCounterLo)/kClk40);
  setMetricValue<>(mMetricReadyTime,(((uint64_t)readyCounterHi.value() << 32) + (uint32_t)readyCounterLo)/kClk40);
  setMetricValue<>(mMetricWarnTime,(((uint64_t)warnCounterHi.value() << 32) + (uint32_t)warnCounterLo)/kClk40);
  setMetricValue<>(mMetricOOSTime,(((uint64_t)oosCounterHi.value() << 32) + (uint32_t)oosCounterLo)/kClk40);
}

} // namespace mp7
} // namespace swatch

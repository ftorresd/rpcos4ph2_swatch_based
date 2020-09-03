/*
 * File:   ReadoutInterface.cpp
 * Author: Tom Williams
 * Created August 2015
 */

#include "swatch/processor/ReadoutInterface.hpp"

#include "swatch/core/MetricConditions.hpp"
#include "swatch/core/TTSUtils.hpp"


namespace swatch {
namespace processor {

// Static Members Initialization
const std::vector<std::string> ReadoutInterface::kDefaultMetrics = {"tts","amcCoreReady"};


ReadoutInterface::ReadoutInterface() :
  core::MonitorableObject("readout"),
  mMetricTTS( registerMetric<core::tts::State>("tts") ),
  mMetricAMCCoreReady( registerMetric<bool>("amcCoreReady") ),
  mMetricEventCounter( registerMetric<uint32_t>("eventCounter") )
{

  // Error if in OOS, warning if not Ready
  setConditions(mMetricTTS,
                core::EqualCondition<core::tts::State>(core::tts::kError),
                core::NotEqualCondition<core::tts::State>(core::tts::kReady));

  // Error if AMCCore is not ready
  setErrorCondition(mMetricAMCCoreReady, core::EqualCondition<bool>(false));
}


ReadoutInterface::~ReadoutInterface()
{
}




} // end ns core
} // end ns swatch

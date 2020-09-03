
#ifndef __SWATCH_PROCESSOR_READOUTINTERFACE_HPP__
#define __SWATCH_PROCESSOR_READOUTINTERFACE_HPP__


// Standard headers
#include <stdint.h>                     // for uint32_t
#include <string>                       // for string
#include <vector>                       // for vector

// SWATCH headers
#include "swatch/core/MonitorableObject.hpp"
#include "swatch/core/TTSUtils.hpp"


namespace swatch {

namespace core {
template <typename DataType> class SimpleMetric;
}


namespace processor {

//! Abstract class defining the readout component interface of a processor
class ReadoutInterface: public core::MonitorableObject {
protected:

  ReadoutInterface();

public:

  virtual ~ReadoutInterface();

  static const std::vector<std::string>  kDefaultMetrics;

protected:
  core::SimpleMetric<core::tts::State>& mMetricTTS;
  core::SimpleMetric<bool>& mMetricAMCCoreReady;
  core::SimpleMetric<uint32_t>& mMetricEventCounter;
};


} // namespace processor
} // namespace swatch

#endif /* __SWATCH_PROCESSOR_READOUTINTERFACE_HPP__ */

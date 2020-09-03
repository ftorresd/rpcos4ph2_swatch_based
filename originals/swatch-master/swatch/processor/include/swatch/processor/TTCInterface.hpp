/**
 * @file    TTCInterface.hpp
 * @author  Maxime Guilbaud
 * @brief   Brief description
 * @date    24/07/2014
 */

#ifndef __SWATCH_PROCESSOR_TTCINTERFACE_HPP__
#define __SWATCH_PROCESSOR_TTCINTERFACE_HPP__


// Standard headers
#include <stdint.h>
#include <string>                       // for string
#include <vector>                       // for vector

// SWATCH headers
#include "swatch/core/MonitorableObject.hpp"


namespace swatch {

namespace core {
template <typename DataType> class SimpleMetric;
}

namespace processor {

//! Abstract class defining the TTC component interface of a processor
class TTCInterface: public core::MonitorableObject {
protected:
  TTCInterface();

public:
  virtual ~TTCInterface() {}


protected:
  //! Metric containing l1a counter values
  core::SimpleMetric<uint32_t>& mMetricL1ACounter;

  //! Metric containing bunch counter value
  core::SimpleMetric<uint32_t>& mMetricBunchCounter;

  //! Metric containing orbit counter value
  core::SimpleMetric<uint32_t>& mMetricOrbitCounter;

  //! Metric indicating if clk40 is locked
  core::SimpleMetric<bool>& mMetricIsClock40Locked;

  //! Metric indicating if clk40 has stopped
  core::SimpleMetric<bool>& mMetricHasClock40Stopped;

  //! Metric indicating if BC0 is locked
  core::SimpleMetric<bool>& mMetricIsBC0Locked;

  //! Metric representing single-bit error counter
  core::SimpleMetric<uint32_t>& mMetricSingleBitErrors;

  //! Metric representing double-bit error counter
  core::SimpleMetric<uint32_t>& mMetricDoubleBitErrors;

public:
  static const std::vector<std::string> kDefaultMetrics;

};

} // namespace processor
} // namespace swatch
#endif /* SWATCH_PROCESSOR_TTCINTERFACE_HPP */

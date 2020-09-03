/*
 * File:   MP7TCCInterface.hpp
 * Author: Alessandro Thea
 *
 * Created on November 6, 2014, 4:50 PM
 */

#ifndef __SWATCH_MP7_MP7TTCINTERFACE_HPP__
#define __SWATCH_MP7_MP7TTCINTERFACE_HPP__


#include <stdint.h>                     // for uint32_t

// Swatch Headers
#include "swatch/processor/TTCInterface.hpp"


namespace mp7 {
class MP7MiniController;
}


namespace swatch {

namespace core {
template <typename DataType> class SimpleMetric;
}

namespace mp7 {

class MP7TTCInterface : public swatch::processor::TTCInterface {
public:
  MP7TTCInterface( ::mp7::MP7MiniController& );
  virtual ~MP7TTCInterface();

  virtual void clearCounters();

  virtual void clearErrors();

protected:
  virtual void retrieveMetricValues();

private:

  ::mp7::MP7MiniController& mDriver;

  core::SimpleMetric<uint32_t>& mMetricBC0Counter;
  core::SimpleMetric<uint32_t>& mMetricEC0Counter;
  core::SimpleMetric<uint32_t>& mMetricOC0Counter;
  core::SimpleMetric<uint32_t>& mMetricResyncCounter;
  core::SimpleMetric<uint32_t>& mMetricStartCounter;
  core::SimpleMetric<uint32_t>& mMetricStopCounter;
  core::SimpleMetric<uint32_t>& mMetricTestCounter;
  core::SimpleMetric<uint32_t>& mMetricTestEnableCounter;
  core::SimpleMetric<uint32_t>& mMetricHardResetCounter;

};

} // namespace mp7
} // namespace swatch

#endif /* __SWATCH_MP7_MP7TTCINTERFACE_HPP__ */


#ifndef _RPCOS4PH2_DUMMY_DUMMYRXPORT_HPP__
#define _RPCOS4PH2_DUMMY_DUMMYRXPORT_HPP__


#include <string>

#include "swatch/processor/Port.hpp"


namespace rpcos4ph2 {
namespace dummy {


class DummyProcDriver;

//! Dummy input port implementation (used for testing)
class DummyRxPort : public swatch::processor::InputPort {
public:
  DummyRxPort(const std::string& aId, uint32_t aNumber, DummyProcDriver& aDriver);

  virtual ~DummyRxPort();

  virtual void retrieveMetricValues();

private:
  uint32_t mChannelId;
  DummyProcDriver& mDriver;
  swatch::core::SimpleMetric<bool>& mWarningSign;
};


} // namespace dummy
} // namespace rpcos4ph2

#endif /* SWATCH_DUMMY_DUMMYRXPORT_HPP */

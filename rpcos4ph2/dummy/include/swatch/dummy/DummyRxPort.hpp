
#ifndef __SWATCH_DUMMY_DUMMYRXPORT_HPP__
#define __SWATCH_DUMMY_DUMMYRXPORT_HPP__


#include <string>

#include "swatch/processor/Port.hpp"


namespace swatch {
namespace dummy {


class DummyProcDriver;

//! Dummy input port implementation (used for testing)
class DummyRxPort : public processor::InputPort {
public:
  DummyRxPort(const std::string& aId, uint32_t aNumber, DummyProcDriver& aDriver);

  virtual ~DummyRxPort();

  virtual void retrieveMetricValues();

private:
  uint32_t mChannelId;
  DummyProcDriver& mDriver;
  core::SimpleMetric<bool>& mWarningSign;
};


} // namespace dummy
} // namespace swatch

#endif /* SWATCH_DUMMY_DUMMYRXPORT_HPP */

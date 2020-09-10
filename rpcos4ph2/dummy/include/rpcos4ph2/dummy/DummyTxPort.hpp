
#ifndef _RPCOS4PH2_DUMMY_DUMMYTXPORT_HPP__
#define _RPCOS4PH2_DUMMY_DUMMYTXPORT_HPP__


#include "swatch/processor/Port.hpp"


namespace rpcos4ph2 {
namespace dummy {


class DummyProcDriver;

//! Dummy output port implementation (used for testing)
class DummyTxPort : public swatch::processor::OutputPort {
public:
  DummyTxPort (const std::string& aId, uint32_t aNumber, DummyProcDriver& aDriver);
  virtual ~DummyTxPort ();

  virtual void retrieveMetricValues();

private:
  uint32_t mChannelId;
  DummyProcDriver& mDriver;
  swatch::core::SimpleMetric<bool>& mWarningSign;
};


} // namespace dummy
} // namespace rpcos4ph2

#endif /* SWATCH_DUMMY_DUMMYTXPORT_HPP */


#ifndef _RPCOS4PH2_DUMMY_DUMMYTTC_HPP__
#define _RPCOS4PH2_DUMMY_DUMMYTTC_HPP__


#include "swatch/processor/TTCInterface.hpp"


namespace rpcos4ph2 {
namespace dummy {

class DummyProcDriver;

//! Dummy TTC interface implementation (used for testing)
class DummyTTC : public swatch::processor::TTCInterface {
public:
  DummyTTC(DummyProcDriver& aDriver);

  virtual ~DummyTTC();

private:
  virtual void retrieveMetricValues();

  DummyProcDriver& mDriver;
  swatch::core::SimpleMetric<bool>& mWarningSign;
};

} // namespace dummy
} // namespace rpcos4ph2

#endif /* SWATCH_DUMMY_DUMMYTTC_HPP */

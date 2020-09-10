
#ifndef _RPCOS4PH2_DUMMY_DUMMYALGO_HPP__
#define _RPCOS4PH2_DUMMY_DUMMYALGO_HPP__


#include <string>

#include "swatch/processor/AlgoInterface.hpp"


namespace rpcos4ph2 {
namespace dummy {

class DummyProcDriver;

//! Dummy algo interface implementation (used for testing)
class DummyAlgo : public swatch::processor::AlgoInterface {
public:
  DummyAlgo(DummyProcDriver& aDriver);

  virtual ~DummyAlgo();

  virtual void retrieveMetricValues();

private:
  DummyProcDriver& mDriver;

  swatch::core::SimpleMetric<float>& mRateCounterA;
  swatch::core::SimpleMetric<float>& mRateCounterB;
};

} // namespace dummy
} // namespace rpcos4ph2

#endif /* SWATCH_DUMMY_DUMMYALGO_HPP */

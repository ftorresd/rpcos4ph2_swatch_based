
#ifndef __SWATCH_DUMMY_DUMMYALGO_HPP__
#define __SWATCH_DUMMY_DUMMYALGO_HPP__


#include <string>

#include "swatch/processor/AlgoInterface.hpp"


namespace swatch {
namespace dummy {

class DummyProcDriver;

//! Dummy algo interface implementation (used for testing)
class DummyAlgo : public processor::AlgoInterface {
public:
  DummyAlgo(DummyProcDriver& aDriver);

  virtual ~DummyAlgo();

  virtual void retrieveMetricValues();

private:
  DummyProcDriver& mDriver;

  core::SimpleMetric<float>& mRateCounterA;
  core::SimpleMetric<float>& mRateCounterB;
};

} // namespace dummy
} // namespace swatch

#endif /* SWATCH_DUMMY_DUMMYALGO_HPP */

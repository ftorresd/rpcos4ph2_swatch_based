
#ifndef __SWATCH_PROCESSOR_TEST_DUMMYPROCESSOR_HPP__
#define __SWATCH_PROCESSOR_TEST_DUMMYPROCESSOR_HPP__


// C++ headers
#include <vector>

// SWATCH headers
#include "swatch/processor/AlgoInterface.hpp"
#include "swatch/processor/ReadoutInterface.hpp"
#include "swatch/processor/TTCInterface.hpp"
#include "swatch/processor/Port.hpp"
#include "swatch/processor/Processor.hpp"


namespace swatch {
namespace processor {
namespace test {

//----------------------------------------------------------------------------//

class DummyAlgo : public AlgoInterface {
public:
  DummyAlgo() {}
  virtual ~DummyAlgo() {}
private:
  void retrieveMetricValues() {}
};


class DummyReadoutInterface : public ReadoutInterface {
public:
  DummyReadoutInterface() {}
  virtual ~DummyReadoutInterface() {}
private:
  void retrieveMetricValues() {}
};


class DummyTTC : public TTCInterface {
public:
  DummyTTC() {}
  virtual ~DummyTTC() {}
private:
  void retrieveMetricValues() {}
};


class DummyRxPort : public InputPort {
public:
  DummyRxPort(const std::string& aId, size_t aIdx) : InputPort(aId) {}
  virtual ~DummyRxPort() {}
private:
  void retrieveMetricValues() {}
};


class DummyTxPort : public OutputPort {
public:
  DummyTxPort(const std::string& aId, size_t aIdx) : OutputPort(aId) {}
  virtual ~DummyTxPort() {}
private:
  void retrieveMetricValues() {}
};


class DummyProcessor : public Processor {
public:
  DummyProcessor( const swatch::core::AbstractStub& aStub );
  virtual ~DummyProcessor();

  static ProcessorStub generateParams( const std::string& aId );

protected:
  virtual void retrieveMetricValues();

private:
  static ProcessorPortStub getPortBag(const std::string& aName, size_t aNumber);
};


} // namespace test
} // namespace processor
} // namespace swatch

#endif  /* SWATCH_PROCESSOR_TEST_DUMMYPROCESSOR_HPP */


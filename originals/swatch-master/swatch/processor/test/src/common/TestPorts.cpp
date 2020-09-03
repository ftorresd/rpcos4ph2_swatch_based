
#include <boost/test/unit_test.hpp>

// SWATCH headers
#include "swatch/processor/Port.hpp"
#include "swatch/processor/PortCollection.hpp"
#include "swatch/processor/test/DummyProcessor.hpp"



namespace swatch {
namespace processor {
namespace test {

BOOST_AUTO_TEST_SUITE( ProcessorTestSuite )


//____________________________________________________________________________//
BOOST_AUTO_TEST_CASE(TestInputPortDefaultMetricsList)
{
  std::cout << "ProcessorTestSuite.TestInputPortDefaultMetricsList" << std::endl;

  DummyProcessor p(DummyProcessor::generateParams("dummy"));

  for (auto it=InputPort::kDefaultMetrics.begin(); it != InputPort::kDefaultMetrics.end(); it++) {
    BOOST_CHECK_NO_THROW(p.getInputPorts().getPort("rxA").getMetric(*it));
  }
  BOOST_CHECK_NO_THROW(p.getInputPorts().getPort("rxA").getMetric<bool>(InputPort::kMetricIdIsAligned));
  BOOST_CHECK_NO_THROW(p.getInputPorts().getPort("rxA").getMetric<bool>(InputPort::kMetricIdIsLocked));
  BOOST_CHECK_NO_THROW(p.getInputPorts().getPort("rxA").getMetric<uint32_t>(InputPort::kMetricIdCRCErrors));
}


//____________________________________________________________________________//
BOOST_AUTO_TEST_CASE(TestOutputPortDefaultMetricsList)
{
  std::cout << "ProcessorTestSuite.TestOutputPortDefaultMetricsList" << std::endl;

  DummyProcessor p(DummyProcessor::generateParams("dummy"));

  for (auto it=OutputPort::kDefaultMetrics.begin(); it != OutputPort::kDefaultMetrics.end(); it++) {
    BOOST_CHECK_NO_THROW(p.getOutputPorts().getPort("txA").getMetric(*it));
  }
  BOOST_CHECK_NO_THROW(p.getOutputPorts().getPort("txA").getMetric<bool>(OutputPort::kMetricIdIsOperating));
}


BOOST_AUTO_TEST_SUITE_END() // ProcessorTestSuite

} /* namespace test */
} /* namespace core */
} /* namespace swatch */

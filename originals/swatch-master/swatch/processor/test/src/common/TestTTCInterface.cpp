
#include <boost/test/unit_test.hpp>

// SWATCH headers
#include "swatch/processor/TTCInterface.hpp"
#include "swatch/processor/test/DummyProcessor.hpp"



namespace swatch {
namespace processor {
namespace test {

BOOST_AUTO_TEST_SUITE( ProcessorTestSuite )


//____________________________________________________________________________//
BOOST_AUTO_TEST_CASE(TestTTCDefaultMetricsList)
{
  std::cout << "ProcessorTestSuite.TestTTCDefaultMetricsList" << std::endl;

  DummyProcessor p(DummyProcessor::generateParams("dummy"));

  for (auto it=TTCInterface::kDefaultMetrics.begin(); it != TTCInterface::kDefaultMetrics.end(); it++) {
    BOOST_CHECK_NO_THROW(p.getTTC().getMetric(*it));
  }
}


BOOST_AUTO_TEST_SUITE_END() // ProcessorTestSuite

} /* namespace test */
} /* namespace core */
} /* namespace swatch */

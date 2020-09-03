
#include <boost/test/unit_test.hpp>

// SWATCH headers
#include "swatch/processor/ReadoutInterface.hpp"
#include "swatch/processor/test/DummyProcessor.hpp"



namespace swatch {
namespace processor {
namespace test {

BOOST_AUTO_TEST_SUITE( ProcessorTestSuite )


//____________________________________________________________________________//
BOOST_AUTO_TEST_CASE(TestReadoutDefaultMetricsList)
{
  std::cout << "ProcessorTestSuite.TestReadoutDefaultMetricsList" << std::endl;

  DummyProcessor p(DummyProcessor::generateParams("dummy"));

  for (auto it=ReadoutInterface::kDefaultMetrics.begin(); it != ReadoutInterface::kDefaultMetrics.end(); it++) {
    BOOST_CHECK_NO_THROW(p.getReadout().getMetric(*it));
  }
}


BOOST_AUTO_TEST_SUITE_END() // ProcessorTestSuite

} /* namespace test */
} /* namespace core */
} /* namespace swatch */

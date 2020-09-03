
#include <boost/test/unit_test.hpp>

// SWATCH headers
#include "swatch/processor/PortCollection.hpp"
#include "swatch/processor/test/DummyProcessor.hpp"



namespace swatch {
namespace processor {
namespace test {

BOOST_AUTO_TEST_SUITE( ProcessorTestSuite )


//____________________________________________________________________________//
BOOST_AUTO_TEST_CASE(TestProcessorComponentAccess)
{
  std::cout << "ProcessorTestSuite.TestProcessorComponentAccess" << std::endl;

  DummyProcessor p(DummyProcessor::generateParams("dummy"));

  BOOST_CHECK( typeid(p.getAlgo()) == typeid(DummyAlgo) );
  BOOST_CHECK( typeid(p.getReadout()) == typeid(DummyReadoutInterface) );
  BOOST_CHECK( typeid(p.getTTC()) == typeid(DummyTTC) );
  BOOST_CHECK( typeid(p.getInputPorts()) == typeid(swatch::processor::InputPortCollection) );
  BOOST_CHECK( typeid(p.getOutputPorts()) == typeid(swatch::processor::OutputPortCollection) );
}


//____________________________________________________________________________//
BOOST_AUTO_TEST_CASE(CreationTest)
{
  std::cout << "ProcessorTestSuite.CreationTest" << std::endl;

  DummyProcessor p(DummyProcessor::generateParams("dummy"));

  BOOST_CHECK_EQUAL(p.getSlot(), uint32_t(0));
  BOOST_CHECK_EQUAL(p.getSlot(), p.getStub().slot);
  BOOST_CHECK_EQUAL(p.getCrateId(), "s2g20-10");
  BOOST_CHECK_EQUAL(p.getCrateId(), p.getStub().crate);

  // Input and output ports
  BOOST_CHECK_EQUAL(p.getInputPorts().getNumPorts(), size_t(5));
  BOOST_CHECK_NO_THROW(p.getInputPorts().getPort("rxA"));
  BOOST_CHECK_NO_THROW(p.getInputPorts().getPort("rxB"));
  BOOST_CHECK_NO_THROW(p.getInputPorts().getPort("rxC"));
  BOOST_CHECK_NO_THROW(p.getInputPorts().getPort("rxD"));
  BOOST_CHECK_NO_THROW(p.getInputPorts().getPort("rxE"));

  BOOST_CHECK_EQUAL(p.getOutputPorts().getNumPorts(), size_t(3));
  BOOST_CHECK_NO_THROW(p.getOutputPorts().getPort("txA"));
  BOOST_CHECK_NO_THROW(p.getOutputPorts().getPort("txB"));
  BOOST_CHECK_NO_THROW(p.getOutputPorts().getPort("txC"));
}


//____________________________________________________________________________//
BOOST_AUTO_TEST_CASE(TestDefaultMetricsList)
{
  std::cout << "ProcessorTestSuite.TestDefaultMetricsList" << std::endl;

  DummyProcessor p(DummyProcessor::generateParams("dummy"));

  for (auto it=Processor::kDefaultMetrics.begin(); it != Processor::kDefaultMetrics.end(); it++) {
    BOOST_CHECK_NO_THROW(p.getMetric(*it));
  }
}


//____________________________________________________________________________//
BOOST_AUTO_TEST_CASE(TestDefaultMonitorableObjectsList)
{
  std::cout << "ProcessorTestSuite.TestDefaultMonitorableObjectsList" << std::endl;

  DummyProcessor p(DummyProcessor::generateParams("dummy"));

  for (auto it=Processor::kDefaultMonitorableObjects.begin(); it != Processor::kDefaultMonitorableObjects.end(); it++) {
    BOOST_CHECK_NE(p.getObjPtr<core::MonitorableObject>(*it), (swatch::core::MonitorableObject*)NULL);
  }
}


BOOST_AUTO_TEST_SUITE_END() // ProcessorTestSuite

} /* namespace test */
} /* namespace core */
} /* namespace swatch */

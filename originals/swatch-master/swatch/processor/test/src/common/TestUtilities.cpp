
#include <boost/test/unit_test.hpp>

// standard headers
#include <iomanip>

// SWATCH headers
#include "swatch/core/exception.hpp"
#include "swatch/processor/ProcessorStub.hpp"
#include "swatch/processor/test/utilities.hpp"


namespace swatch {
namespace processor {
namespace test {

BOOST_AUTO_TEST_SUITE( ProcessorTestSuite )


BOOST_AUTO_TEST_CASE(SliceSyntaxPortStubTests)
{
  std::cout << "ProcessorTestSuite.SliceSyntaxPortStubTests" << std::endl;

  std::vector<swatch::processor::ProcessorPortStub> lResult, lExpected;

//  expandPortSliceSyntax("myPort_rx[0:10]", "[10:60:5]", result);
  pushBackPortStubs(lResult, "myPort_rx[0:10]", "[10:60:5]");

  // Generated expected vector (i used for port name, j is port number)
  size_t j=10;
  for (size_t i=0; i<10; i++, j+=5) {
    std::ostringstream lMsg;
    lMsg << "myPort_rx" << std::setw(2) << std::setfill('0') << i;

    ProcessorPortStub lStub(lMsg.str());
    lStub.number = j;
    lExpected.push_back(lStub);
  }

  BOOST_CHECK_EQUAL(lResult.size(), lExpected.size());
  for ( size_t i = 0; i < std::min(lResult.size(), lExpected.size()); i++ ) {
    BOOST_CHECK_EQUAL(std::string(lResult[i].id), std::string(lExpected[i].id));
    BOOST_CHECK_EQUAL(unsigned(lResult[i].number), unsigned(lExpected[i].number));
  }


  lResult.clear();

  // Check for throwing when lengths of name and index sequence aren't equal
  BOOST_CHECK_THROW(pushBackPortStubs(lResult, "myPort_rx[0:3]", "[0:2]"), swatch::core::RuntimeError);
  BOOST_CHECK( lResult.empty() );

  // Check that exception thrown when the expanded port index strings can't be cast to an unsigned
  BOOST_CHECK_THROW(pushBackPortStubs(lResult, "myPort_rx[0:3]", "a[0:2]"), swatch::core::RuntimeError);
  BOOST_CHECK( lResult.empty() );

}


BOOST_AUTO_TEST_SUITE_END() // ProcessorTestSuite

} /* namespace test */
} /* namespace core */
} /* namespace swatch */

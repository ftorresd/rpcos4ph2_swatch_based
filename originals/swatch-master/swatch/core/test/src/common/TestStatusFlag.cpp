// Boost Unit Test includes
#include <boost/test/unit_test.hpp>
#include "boost/test/test_tools.hpp"

// Swatch Headers
#include "swatch/core/StatusFlag.hpp"

// Boost Headers
#include <boost/lexical_cast.hpp>


BOOST_AUTO_TEST_SUITE( StatusFlagTestSuite )

BOOST_AUTO_TEST_CASE(StatusFlagStreamNoNamespace)
{
  BOOST_CHECK_EQUAL( boost::lexical_cast<std::string>(swatch::core::kError) , "Error" );

  std::ostringstream lMsg;
  lMsg << swatch::core::kError;
  BOOST_CHECK_EQUAL( lMsg.str(), "Error" );
}


BOOST_AUTO_TEST_CASE(StatusFlagStreamUsingNamespaceSwatchCore)
{
  using namespace swatch::core;

  BOOST_CHECK_EQUAL( boost::lexical_cast<std::string>(swatch::core::kError) , "Error" );

  std::ostringstream lMsg;
  lMsg << kError;
  BOOST_CHECK_EQUAL( lMsg.str(), "Error" );
}


namespace testArbitrary {

BOOST_AUTO_TEST_CASE(StatusFlagStreamInArbitraryNamespace)
{
  BOOST_CHECK_EQUAL( boost::lexical_cast<std::string>(swatch::core::kGood), "Good" );
  BOOST_CHECK_EQUAL( boost::lexical_cast<std::string>(swatch::core::kWarning), "Warning" );
  BOOST_CHECK_EQUAL( boost::lexical_cast<std::string>(swatch::core::kError) , "Error" );
  BOOST_CHECK_EQUAL( boost::lexical_cast<std::string>(swatch::core::kNoLimit), "NoLimit" );
  BOOST_CHECK_EQUAL( boost::lexical_cast<std::string>(swatch::core::kUnknown), "Unknown" );

  std::ostringstream lMsg;
  lMsg << swatch::core::kError;
  BOOST_CHECK_EQUAL( lMsg.str(), "Error" );
}

}


BOOST_AUTO_TEST_CASE(StatusFlagAndOperator)
{
  namespace swco = swatch::core;

  BOOST_CHECK_EQUAL ( swco::kGood & swco::kGood , swco::kGood );
  BOOST_CHECK_EQUAL ( swco::kGood & swco::kWarning, swco::kWarning );
  BOOST_CHECK_EQUAL ( swco::kGood & swco::kError, swco::kError );
  BOOST_CHECK_EQUAL ( swco::kGood & swco::kNoLimit, swco::kGood );
  BOOST_CHECK_EQUAL ( swco::kGood & swco::kUnknown, swco::kUnknown );

  BOOST_CHECK_EQUAL ( swco::kWarning & swco::kGood, swco::kWarning );
  BOOST_CHECK_EQUAL ( swco::kWarning & swco::kWarning, swco::kWarning );
  BOOST_CHECK_EQUAL ( swco::kWarning & swco::kError, swco::kError );
  BOOST_CHECK_EQUAL ( swco::kWarning & swco::kNoLimit, swco::kWarning );
  BOOST_CHECK_EQUAL ( swco::kWarning & swco::kUnknown, swco::kUnknown );

  BOOST_CHECK_EQUAL ( swco::kError & swco::kGood, swco::kError );
  BOOST_CHECK_EQUAL ( swco::kError & swco::kWarning, swco::kError );
  BOOST_CHECK_EQUAL ( swco::kError & swco::kError, swco::kError );
  BOOST_CHECK_EQUAL ( swco::kError & swco::kNoLimit, swco::kError );
  BOOST_CHECK_EQUAL ( swco::kError & swco::kUnknown, swco::kError );

  BOOST_CHECK_EQUAL ( swco::kNoLimit & swco::kGood, swco::kGood );
  BOOST_CHECK_EQUAL ( swco::kNoLimit & swco::kWarning, swco::kWarning );
  BOOST_CHECK_EQUAL ( swco::kNoLimit & swco::kError, swco::kError );
  BOOST_CHECK_EQUAL ( swco::kNoLimit & swco::kNoLimit, swco::kNoLimit );
  BOOST_CHECK_EQUAL ( swco::kNoLimit & swco::kUnknown, swco::kUnknown );

  BOOST_CHECK_EQUAL ( swco::kUnknown & swco::kGood, swco::kUnknown );
  BOOST_CHECK_EQUAL ( swco::kUnknown & swco::kWarning, swco::kUnknown );
  BOOST_CHECK_EQUAL ( swco::kUnknown & swco::kError, swco::kError );
  BOOST_CHECK_EQUAL ( swco::kUnknown & swco::kNoLimit, swco::kUnknown );
  BOOST_CHECK_EQUAL ( swco::kUnknown & swco::kUnknown, swco::kUnknown );
}


BOOST_AUTO_TEST_SUITE_END() // StatusFlagTestSuite



#include <boost/test/unit_test.hpp>


#include "boost/lexical_cast.hpp"

#include "xdata/String.h"
#include "xdata/UnsignedInteger.h"

#include "swatch/core/ReadOnlyXParameterSet.hpp"
#include "swatch/core/ReadWriteXParameterSet.hpp"
#include "swatch/core/XParameterSet.hpp"
#include "swatch/core/XPSetConstraint.hpp"


namespace swatch {
namespace core {
namespace test {



class DummyNullConstraint : public XPSetConstraint
{
public:
  DummyNullConstraint() = default; 
  ~DummyNullConstraint() = default;
  
  // Expose 'define' method as public method for unit tests
  template <typename T>
  void require(const std::string& aName) { this->XPSetConstraint::require<T>(aName);}
  
private:
  void describe(std::ostream& aStream) const override
  {
    aStream << "null constraint";
  }
  
  XMatch verify(const XParameterSet& aParams) const override
  {
    return true;
  }
};

// ----------------------------------------------------------------------------
class DummySumSmallerThan : public XPSetConstraint
{
public:
  DummySumSmallerThan( const std::string& a, const std::string& b, size_t max) :
    a(a), 
    b(b),
    max(max)
  {
    require<xdata::UnsignedInteger>(a);
    require<xdata::UnsignedInteger>(b);
  }
  virtual ~DummySumSmallerThan() = default;

private:
  void describe(std::ostream& aStream) const override
  {
    aStream << "(" << a << " + " << b << ") < " << max;
  }
  
  XMatch verify(const XParameterSet& aParams) const override
  {
    return ( aParams.get<xdata::UnsignedInteger>(a) + aParams.get<xdata::UnsignedInteger>(b) ) < max;
  }

  const std::string a;  
  const std::string b;
  const size_t max;

};
// ----------------------------------------------------------------------------



BOOST_AUTO_TEST_SUITE(ConstraintTestSuite)

BOOST_AUTO_TEST_CASE(TestStreamOperator)
{
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(DummyNullConstraint()), "null constraint");
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(DummySumSmallerThan("a", "b", 42)), "(a + b) < 42");
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(DummySumSmallerThan("param1", "anotherParam", 56)), "(param1 + anotherParam) < 56");
}


BOOST_AUTO_TEST_CASE(TestGetParameterNames)
{
  std::set<std::string> lExpectedNames = {"param1", "anotherParam"};
  std::set<std::string> lReturnedNames = DummySumSmallerThan("param1", "anotherParam", 42).getParameterNames();
  BOOST_CHECK_EQUAL_COLLECTIONS(lReturnedNames.begin(), lReturnedNames.end(), lExpectedNames.begin(), lExpectedNames.end());

  DummyNullConstraint lNullConstraint;
  lReturnedNames = lNullConstraint.getParameterNames();
  lExpectedNames = {};
  BOOST_CHECK_EQUAL_COLLECTIONS(lReturnedNames.begin(), lReturnedNames.end(), lExpectedNames.begin(), lExpectedNames.end());
  
  lNullConstraint.require<xdata::UnsignedInteger>("someNumber");
  lReturnedNames = lNullConstraint.getParameterNames();
  lExpectedNames = {"someNumber"};
  BOOST_CHECK_EQUAL_COLLECTIONS(lReturnedNames.begin(), lReturnedNames.end(), lExpectedNames.begin(), lExpectedNames.end());

  lNullConstraint.require<xdata::UnsignedInteger>("anotherNumber");
  lReturnedNames = lNullConstraint.getParameterNames();
  lExpectedNames.insert("anotherNumber");
  BOOST_CHECK_EQUAL_COLLECTIONS(lReturnedNames.begin(), lReturnedNames.end(), lExpectedNames.begin(), lExpectedNames.end());

  lNullConstraint.require<xdata::String>("andAString");
  lReturnedNames = lNullConstraint.getParameterNames();
  lExpectedNames.insert("andAString");
  BOOST_CHECK_EQUAL_COLLECTIONS(lReturnedNames.begin(), lReturnedNames.end(), lExpectedNames.begin(), lExpectedNames.end());  
}


BOOST_AUTO_TEST_CASE(TestFunctionCallOperator)
{
  const DummyNullConstraint lNullConstraint;
  const DummySumSmallerThan lSmallerThan50("paramA", "paramB", 50);
  ReadWriteXParameterSet lParamSet;

  // Check that correct value is returned if given the correct types
  BOOST_CHECK_EQUAL(lNullConstraint(ReadOnlyXParameterSet()), true);
  BOOST_CHECK_EQUAL(lNullConstraint(lParamSet), true);

  lParamSet.add("paramA", xdata::UnsignedInteger(40));
  lParamSet.add("paramB", xdata::UnsignedInteger(4));
  BOOST_CHECK_EQUAL(lSmallerThan50(lParamSet), true);
  lParamSet.erase("paramB");
  lParamSet.add("paramB", xdata::UnsignedInteger(14));
  BOOST_CHECK_EQUAL(lSmallerThan50(lParamSet), false);
  
  // Check that exception is thrown if items in input XParameterSet are of incorrect type
  lParamSet.erase("paramB");
  lParamSet.add("paramB", xdata::String("Hello there!"));
  BOOST_CHECK_THROW(lSmallerThan50(lParamSet), ConstraintIncompatibleParameter);

  // Check that exception is thrown if some parameters missing
  BOOST_CHECK_THROW(lSmallerThan50(ReadOnlyXParameterSet()), ConstraintIncompatibleParameter);
  lParamSet.erase("paramA");
  lParamSet.erase("paramB");
  BOOST_REQUIRE_EQUAL(lParamSet.size(), size_t(0));
  BOOST_CHECK_THROW(lSmallerThan50(lParamSet), ConstraintIncompatibleParameter);
  lParamSet.add("paramA", xdata::UnsignedInteger(30));
  BOOST_CHECK_THROW(lSmallerThan50(lParamSet), ConstraintIncompatibleParameter);
  lParamSet.add("ParamB", xdata::UnsignedInteger(3));
  BOOST_CHECK_THROW(lSmallerThan50(lParamSet), ConstraintIncompatibleParameter);  
}


BOOST_AUTO_TEST_SUITE_END() // ConstraintTestSuite


} /* namespace test */
} /* namespace core */
} /* namespace swatch */

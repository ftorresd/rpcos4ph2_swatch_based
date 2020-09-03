#include <boost/test/unit_test.hpp>

#include "swatch/xvectortypedefs.hpp"
#include "swatch/core/ReadWriteXParameterSet.hpp"
#include "swatch/core/rules/FiniteVector.hpp"
#include "swatch/core/rules/OfSize.hpp"
#include "swatch/core/rules/And.hpp"
#include "swatch/mp7/cmds/OrbitConstraint.hpp"
#include "swatch/mp7/cmds/ReadoutMenuConstraint.hpp"

namespace swatch {
namespace mp7 {
namespace test {

struct RuleAndConstraintSetup {
};

BOOST_AUTO_TEST_SUITE( MP7TestSuite )


BOOST_FIXTURE_TEST_CASE(OrbitConstraintTest, RuleAndConstraintSetup)
{
	::mp7::orbit::Metric lLHCMetric(3564, 6);
	cmds::OrbitConstraint lOrbConstr("bx", "cycle", lLHCMetric);
	cmds::OrbitConstraint lOrbConstrNull("bx", "cycle", lLHCMetric, true);

	core::ReadWriteXParameterSet lPSetOK;
  	lPSetOK.add("bx", XUInt_t(40));
  	lPSetOK.add("cycle", XUInt_t(4));
  	BOOST_CHECK_EQUAL(lOrbConstr(lPSetOK), true);

	core::ReadWriteXParameterSet lPSetCycleError;
  	lPSetCycleError.add("bx", XUInt_t(40));
  	lPSetCycleError.add("cycle", XUInt_t(10));
  	BOOST_CHECK_EQUAL(lOrbConstr(lPSetCycleError), false);

	core::ReadWriteXParameterSet lPSetNull;
  	lPSetNull.add("bx", XUInt_t());
  	lPSetNull.add("cycle", XUInt_t());
  	BOOST_CHECK_EQUAL(lOrbConstr(lPSetNull), false);
  	BOOST_CHECK_EQUAL(lOrbConstrNull(lPSetNull), true);
}

BOOST_FIXTURE_TEST_CASE(ROMenuConstraintTest, RuleAndConstraintSetup)
{
	core::ReadWriteXParameterSet lPSetMenuOK;
	core::ReadWriteXParameterSet lPSetMenuBad;


	lPSetMenuOK.add("bank0:wordsPerBx", XUInt_t(6));

	lPSetMenuOK.add("mode0:eventSize", XUInt_t(0));
	lPSetMenuOK.add("mode0:eventToTrigger", XUInt_t(0));
	lPSetMenuOK.add("mode0:eventType", XUInt_t(0));

	lPSetMenuOK.add("mode0:capture0:enable", XBool_t(true));
	lPSetMenuOK.add("mode0:capture0:id", XUInt_t(1));
	lPSetMenuOK.add("mode0:capture0:bankId", XUInt_t(1));
	lPSetMenuOK.add("mode0:capture0:length", XUInt_t(1));
	lPSetMenuOK.add("mode0:capture0:delay", XUInt_t(1));
	lPSetMenuOK.add("mode0:capture0:readoutLength", XUInt_t(1));

	lPSetMenuOK.add("mode0:capture1:enable", XBool_t(true));
	lPSetMenuOK.add("mode0:capture1:id", XUInt_t(1));
	lPSetMenuOK.add("mode0:capture1:bankId", XUInt_t(1));
	lPSetMenuOK.add("mode0:capture1:length", XUInt_t(1));
	lPSetMenuOK.add("mode0:capture1:delay", XUInt_t(1));
	lPSetMenuOK.add("mode0:capture1:readoutLength", XUInt_t(1));

	lPSetMenuBad.deepCopyFrom(lPSetMenuOK);
	lPSetMenuBad.get<XBool_t>("mode0:capture0:enable") = XBool_t(false);
	cmds::ReadoutMenuConstraint lConstraint(1,1,2);

	BOOST_CHECK_EQUAL(lConstraint(lPSetMenuOK), true);
	BOOST_CHECK_EQUAL(lConstraint(lPSetMenuBad), core::XMatch(false, "Menu consistency check failed. Capture modes are enabled but preceded by a disabled capture modes.Mode 0: captures 1"));

}

BOOST_FIXTURE_TEST_CASE(ZSMenuRuleTest, RuleAndConstraintSetup)
{
  core::rules::FiniteVector<XVectorUInt_t> lFiniteVec;
  core::rules::OfSize<XVectorUInt_t> lSize6(6);
  core::rules::And<XVectorUInt_t> lFiniteAndSize6(lFiniteVec,lSize6);

  XVectorUInt_t lMaskOf6, lMaskOf6NaN, lMaskOf7;
	lMaskOf6.push_back(XUInt_t(0xff));
	lMaskOf6.push_back(XUInt_t(0xff));
	lMaskOf6.push_back(XUInt_t(0xff));
	lMaskOf6.push_back(XUInt_t(0xff));
	lMaskOf6.push_back(XUInt_t(0xff));
	lMaskOf6.push_back(XUInt_t(0xff));

  lMaskOf6NaN = lMaskOf6;
  lMaskOf6NaN[3] = XUInt_t();

  lMaskOf7 = lMaskOf6;
  lMaskOf7.push_back(XUInt_t(0xff));

	BOOST_CHECK_EQUAL(lFiniteAndSize6(lMaskOf6), true);
	BOOST_CHECK_EQUAL(lFiniteAndSize6(lMaskOf6NaN), false);
	BOOST_CHECK_EQUAL(lFiniteAndSize6(lMaskOf7), false);




}


BOOST_AUTO_TEST_SUITE_END()

}
}
}
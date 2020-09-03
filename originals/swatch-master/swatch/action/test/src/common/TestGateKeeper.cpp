/**
 * \brief Test suite for swatch/action/GateKeeper
 * \author kreczko
 */
// Boost Unit Test includes
#include <boost/test/unit_test.hpp>

// swatch headers
#include "swatch/action/GateKeeper.hpp"
#include "swatch/action/test/DummyGateKeeper.hpp"

//xdaq headers
#include <xdata/Integer.h>
#include <xdata/String.h>


namespace swatch {
namespace action {
namespace test {

struct TestGateKeeperSetup {
  TestGateKeeperSetup() :
    gk()
  {
    typedef boost::shared_ptr<const xdata::Serializable> ParamPtr_t;
    typedef GateKeeper::Parameters_t Parameters_t;
    typedef boost::shared_ptr<MonitoringSetting> MonSettingPtr_t;
    typedef GateKeeper::MonitoringSettings_t MonSettings_t;
    typedef GateKeeper::Masks_t Masks_t;

    GateKeeper::ParametersContext_t lCommonParams(new Parameters_t{
      {"hello", ParamPtr_t(new xdata::String("World"))},
      {"answer", ParamPtr_t(new xdata::Integer(42))}
    });
    gk.addContext("dummy_sys.common", lCommonParams);

    GateKeeper::ParametersContext_t lChildA1Params(new Parameters_t{
      {"hello", ParamPtr_t(new xdata::String("World! (childA1)"))}
    });
    gk.addContext("dummy_sys.childA1", lChildA1Params);

    GateKeeper::ParametersContext_t lChildTypeAParams(new GateKeeper::Parameters_t{
      {"sequence_1.command_1.parameter_1", ParamPtr_t(new xdata::String("sequence"))},
      {"command_1.parameter_1", ParamPtr_t(new xdata::String("command"))}
    });
    gk.addContext("dummy_sys.childTypeA", lChildTypeAParams);

    // monitoring status
    MonSettings_t lMetricSettings {
      {"criticalMetric", MonSettingPtr_t(new MonitoringSetting("criticalMetric", core::monitoring::kEnabled))},
      {"weird_state.criticalMetric", MonSettingPtr_t(new MonitoringSetting("weird_state.criticalMetric", core::monitoring::kDisabled))},
      {"nonCriticalMetric", MonSettingPtr_t(new MonitoringSetting("nonCriticalMetric", core::monitoring::kNonCritical))}
    };
    gk.addSettingsContext("dummy_sys.childTypeA", lMetricSettings);

    // Masks
    gk.addMasksContext("dummy_sys.common", Masks_t{"componentA"});
    gk.addMasksContext("dummy_sys.childTypeA", Masks_t{"componentB"});
    gk.addMasksContext("dummy_sys.childA1", Masks_t{"componentC"});

    // Disable some IDs
    gk.addDisabledId("dummy_sys.childA1");
    gk.addDisabledId("dummy_sys.child42");
  }

  DummyGateKeeper gk;
};


BOOST_AUTO_TEST_SUITE( TestGateKeeper )


BOOST_FIXTURE_TEST_CASE ( TestCommonParameters, TestGateKeeperSetup )
{
  std::vector<std::string> lContextsToLookIn;
  lContextsToLookIn.push_back("dummy_sys.common");

  GateKeeper::Parameter_t lParam(gk.get("", "", "hello", lContextsToLookIn));
  BOOST_REQUIRE(lParam != NULL);
  BOOST_CHECK_EQUAL(lParam->toString(), "World");

  GateKeeper::Parameter_t lParam2(gk.get("", "", "answer", lContextsToLookIn));
  BOOST_REQUIRE(lParam2 != NULL);
  BOOST_CHECK_EQUAL(lParam2->toString(), "42");
}

BOOST_FIXTURE_TEST_CASE ( TestChildParameters, TestGateKeeperSetup )
{
  std::vector<std::string> lContextsToLookIn {"dummy_sys.childA1", "dummy_sys.common"};

  GateKeeper::Parameter_t p(gk.get("", "", "hello", lContextsToLookIn));
  BOOST_REQUIRE(p != NULL);
  BOOST_CHECK_EQUAL(p->toString(), "World! (childA1)");
}

BOOST_FIXTURE_TEST_CASE ( TestSequenceParameters, TestGateKeeperSetup )
{
  std::vector<std::string> lContextsToLookIn;
  lContextsToLookIn.push_back("dummy_sys.childTypeA");

  GateKeeper::Parameter_t p(
    gk.get("sequence_1", "command_1", "parameter_1", lContextsToLookIn));
  BOOST_REQUIRE(p != NULL);
  BOOST_CHECK_EQUAL(p->toString(), "sequence");

  GateKeeper::Parameter_t p2(
    gk.get("", "command_1", "parameter_1", lContextsToLookIn));
  BOOST_REQUIRE(p != NULL);
  BOOST_CHECK_EQUAL(p2->toString(), "command");
}

BOOST_FIXTURE_TEST_CASE ( TestMonitoringSettings, TestGateKeeperSetup )
{
  std::vector<std::string> lContextsToLookIn;
  lContextsToLookIn.push_back("dummy_sys.childTypeA");

  GateKeeper::MonitoringSetting_t p(
    gk.getMonitoringSetting("", "nonCriticalMetric", lContextsToLookIn));
  BOOST_REQUIRE(p != NULL);
  BOOST_CHECK_EQUAL(p->getStatus(), core::monitoring::kNonCritical);

  GateKeeper::MonitoringSetting_t p2(
    gk.getMonitoringSetting("", "criticalMetric", lContextsToLookIn));
  BOOST_REQUIRE(p2 != NULL);
  BOOST_CHECK_EQUAL(p2->getStatus(), core::monitoring::kEnabled);

  GateKeeper::MonitoringSetting_t p3(
    gk.getMonitoringSetting("weird_state", "criticalMetric",
                            lContextsToLookIn));
  BOOST_REQUIRE(p3 != NULL);
  BOOST_CHECK_EQUAL(p3->getStatus(), core::monitoring::kDisabled);
}


BOOST_FIXTURE_TEST_CASE(TestMask, TestGateKeeperSetup)
{
  std::vector<std::string> lContextsToLookIn;

  // 1) When tables vector is empty,  GateKeeper::getMask method should always return false
  BOOST_CHECK_EQUAL(gk.getMask("componentA", lContextsToLookIn), false);
  BOOST_CHECK_EQUAL(gk.getMask("componentB", lContextsToLookIn), false);
  BOOST_CHECK_EQUAL(gk.getMask("componentC", lContextsToLookIn), false);
  BOOST_CHECK_EQUAL(gk.getMask("otherComponent", lContextsToLookIn), false);

  // 2) Contexts vector only contains common table
  lContextsToLookIn.push_back("dummy_sys.common");
  BOOST_CHECK_EQUAL(gk.getMask("componentA", lContextsToLookIn), true);
  BOOST_CHECK_EQUAL(gk.getMask("componentB", lContextsToLookIn), false);
  BOOST_CHECK_EQUAL(gk.getMask("componentC", lContextsToLookIn), false);
  BOOST_CHECK_EQUAL(gk.getMask("otherComponent", lContextsToLookIn), false);

  // 3) Contexts vector contains proc1 and common tables
  lContextsToLookIn = std::vector<std::string> {"dummy_sys.childA1", "dummy_sys.common"};
  BOOST_CHECK_EQUAL(gk.getMask("componentA", lContextsToLookIn), true);
  BOOST_CHECK_EQUAL(gk.getMask("componentB", lContextsToLookIn), false);
  BOOST_CHECK_EQUAL(gk.getMask("componentC", lContextsToLookIn), true);
  BOOST_CHECK_EQUAL(gk.getMask("otherComponent", lContextsToLookIn), false);

  // 4) Contexts vector contains processors and common tables
  lContextsToLookIn = std::vector<std::string> {"dummy_sys.childTypeA", "dummy_sys.common"};
  BOOST_CHECK_EQUAL(gk.getMask("componentA", lContextsToLookIn), true);
  BOOST_CHECK_EQUAL(gk.getMask("componentB", lContextsToLookIn), true);
  BOOST_CHECK_EQUAL(gk.getMask("componentC", lContextsToLookIn), false);
  BOOST_CHECK_EQUAL(gk.getMask("otherComponent", lContextsToLookIn), false);

  // 5) Contexts vector contains all tables
  lContextsToLookIn = std::vector<std::string> {"dummy_sys.childA1", "dummy_sys.childTypeA", "dummy_sys.common"};
  BOOST_CHECK_EQUAL(gk.getMask("componentA", lContextsToLookIn), true);
  BOOST_CHECK_EQUAL(gk.getMask("componentB", lContextsToLookIn), true);
  BOOST_CHECK_EQUAL(gk.getMask("componentC", lContextsToLookIn), true);
  BOOST_CHECK_EQUAL(gk.getMask("otherComponent", lContextsToLookIn), false);
}


BOOST_FIXTURE_TEST_CASE(TestDisabled, TestGateKeeperSetup)
{
  BOOST_CHECK_EQUAL(gk.isEnabled("dummy_sys.childA1"), false);
  BOOST_CHECK_EQUAL(gk.isEnabled("dummy_sys.childA2"), true);
  BOOST_CHECK_EQUAL(gk.isEnabled("dummy_sys.unkownChild"), true);
  BOOST_CHECK_EQUAL(gk.isEnabled("dummy_sys.child42"), false);
}


BOOST_AUTO_TEST_SUITE_END() // TestGateKeeper

} //ns: test
} //ns: action
} //ns: swatch

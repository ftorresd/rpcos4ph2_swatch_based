/*
 * FSM_test.cpp
 *
 *  Created on: 18 Nov 2014
 *      Author: kreczko
 */
#include <boost/test/unit_test.hpp>

// swatch headers
#include "swatch/core/SimpleMetric.hpp"
#include "swatch/core/MetricConditions.hpp"
#include "swatch/core/Object.hpp"
#include "swatch/core/test/DummyMetric.hpp"
#include "swatch/core/test/DummyMonitorableObjects.hpp"


namespace swatch {
namespace core {
namespace test {


BOOST_AUTO_TEST_SUITE( MetricTestSuite )

BOOST_AUTO_TEST_CASE(InvalidMetricId)
{
  // 1) Construction without metric conditions
  BOOST_CHECK_THROW(SimpleMetric<int> m("", ""), InvalidObjectId);
  BOOST_CHECK_THROW(SimpleMetric<int> m(".some_invalid_id", ""), InvalidObjectId);
  BOOST_CHECK_THROW(SimpleMetric<int> m("some.invalid_id", ""), InvalidObjectId);
  BOOST_CHECK_THROW(SimpleMetric<int> m("some_invalid_id.", ""), InvalidObjectId);
  BOOST_CHECK_NO_THROW(SimpleMetric<int> m("some_valid_id", ""));

  // 2) Construction with metric conditions (should give same results, but double-checking since diff CTOR)
  BOOST_CHECK_THROW(SimpleMetric<int> m("", "", new EqualCondition<int>(42)), InvalidObjectId);
  BOOST_CHECK_THROW(SimpleMetric<int> m(".some_invalid_id", "", new EqualCondition<int>(42)), InvalidObjectId);
  BOOST_CHECK_THROW(SimpleMetric<int> m("some.invalid_id", "", new EqualCondition<int>(42)), InvalidObjectId);
  BOOST_CHECK_THROW(SimpleMetric<int> m("some_invalid_id.", "", new EqualCondition<int>(42)), InvalidObjectId);
  BOOST_CHECK_NO_THROW(SimpleMetric<int> m("some_valid_id", "", new EqualCondition<int>(42)));
}


BOOST_AUTO_TEST_CASE(TestIdAliasAndIdPathGetters)
{
  DummyMonitorableObject lGrandparent("topLevelMonObj");
  DummyMonitorableObject& lParent = dynamic_cast<DummyMonitorableObject&>(
                                      lGrandparent.addMonitorable(new DummyMonitorableObject("aMonChild")) );
  SimpleMetric<bool>& lMetricWithoutAlias = lParent.registerMetric<bool>("coolDummyMetric1");
  SimpleMetric<bool>& lMetricWithAlias = lParent.registerMetric<bool>("coolDummyMetric2", "alias for dummy metric");

  // 1) Check metric object's ID getter methods
  BOOST_CHECK_EQUAL(lMetricWithoutAlias.getId(), "coolDummyMetric1");
  BOOST_CHECK_EQUAL(lMetricWithoutAlias.getPath(), "topLevelMonObj.aMonChild.coolDummyMetric1");
  BOOST_CHECK_EQUAL(lMetricWithoutAlias.getAlias(), "");
  BOOST_CHECK_EQUAL(lMetricWithAlias.getId(), "coolDummyMetric2");
  BOOST_CHECK_EQUAL(lMetricWithAlias.getPath(), "topLevelMonObj.aMonChild.coolDummyMetric2");
  BOOST_CHECK_EQUAL(lMetricWithAlias.getAlias(), "alias for dummy metric");

  // 2) Check metric snapshot's ID getter methods
  MetricSnapshot lSnapshot = lMetricWithoutAlias.getSnapshot();
  BOOST_CHECK_EQUAL(lSnapshot.getPath(), "topLevelMonObj.aMonChild.coolDummyMetric1");
  BOOST_CHECK_EQUAL(lSnapshot.getMetricId(), "coolDummyMetric1");
  BOOST_CHECK_EQUAL(lSnapshot.getAlias(), "");

  BOOST_CHECK_EQUAL(lSnapshot.getMonitorableObjectId(), "aMonChild");
  BOOST_CHECK_EQUAL(lSnapshot.getMonitorableObjectPath(), "topLevelMonObj.aMonChild");

  lSnapshot = lMetricWithAlias.getSnapshot();
  BOOST_CHECK_EQUAL(lSnapshot.getPath(), "topLevelMonObj.aMonChild.coolDummyMetric2");
  BOOST_CHECK_EQUAL(lSnapshot.getMetricId(), "coolDummyMetric2");
  BOOST_CHECK_EQUAL(lSnapshot.getAlias(), "alias for dummy metric");
  
  BOOST_CHECK_EQUAL(lSnapshot.getMonitorableObjectId(), "aMonChild");
  BOOST_CHECK_EQUAL(lSnapshot.getMonitorableObjectPath(), "topLevelMonObj.aMonChild");
}


BOOST_AUTO_TEST_CASE(SimpleIntMetric)
{
  DummyMetric<int> m("myCoolDummyMetric", "");

  // 1) Check construction (value should be unknown)
  BOOST_CHECK_EQUAL(m.getId(), "myCoolDummyMetric");
  BOOST_CHECK_EQUAL(m.getStatus().first, swatch::core::kUnknown);
  BOOST_CHECK_EQUAL(m.getStatus().second, monitoring::kEnabled);

  MetricSnapshot ms = m.getSnapshot();
  BOOST_CHECK_EQUAL(ms.getPath(), "myCoolDummyMetric");
  BOOST_CHECK_EQUAL(ms.getMetricId(), "myCoolDummyMetric");
  BOOST_CHECK_EQUAL(ms.getMonitorableObjectId(), "");
  BOOST_CHECK_EQUAL(ms.getMonitorableObjectPath(), "");
  BOOST_CHECK_EQUAL(ms.getStatusFlag(), swatch::core::kUnknown);
  BOOST_CHECK(!ms.isValueKnown());
  BOOST_CHECK_THROW(ms.getValue<int>(), MetricValueNotKnown);
  BOOST_CHECK_THROW(ms.getValue<bool>(), MetricValueNotKnown);
  BOOST_CHECK_THROW(ms.getValueAsString(), MetricValueNotKnown);
  BOOST_CHECK(ms.getWarningCondition() == NULL);
  BOOST_CHECK(ms.getErrorCondition() == NULL);
  BOOST_CHECK_EQUAL(ms.getMonitoringStatus(), monitoring::kEnabled);

  // 2) Set value, and check new snapshot
  m.setValue(1);
  ms = m.getSnapshot();
  BOOST_CHECK_EQUAL(ms.getStatusFlag(), swatch::core::StatusFlag::kNoLimit);
  BOOST_CHECK(ms.isValueKnown());
  BOOST_CHECK_EQUAL(ms.getValue<int>(), 1);
  BOOST_CHECK_THROW(ms.getValue<bool>(), MetricValueFailedCast);
  BOOST_CHECK_EQUAL(ms.getValueAsString(), "1");
  BOOST_CHECK(ms.getWarningCondition() == NULL);
  BOOST_CHECK(ms.getErrorCondition() == NULL);
  BOOST_CHECK_EQUAL(ms.getMonitoringStatus(), monitoring::kEnabled);
}


BOOST_AUTO_TEST_CASE(SimpleBooleanMetric)
{
  DummyMetric<bool> m("myCoolDummyMetric", "");

  // 1) Check construction (value should be unknown)
  BOOST_CHECK_EQUAL(m.getId(), "myCoolDummyMetric");
  BOOST_CHECK_EQUAL(m.getStatus().first, swatch::core::kUnknown);
  BOOST_CHECK_EQUAL(m.getStatus().second, monitoring::kEnabled);

  MetricSnapshot ms = m.getSnapshot();
  BOOST_CHECK_EQUAL(ms.getPath(), "myCoolDummyMetric");
  BOOST_CHECK_EQUAL(ms.getMetricId(), "myCoolDummyMetric");
  BOOST_CHECK_EQUAL(ms.getMonitorableObjectId(), "");
  BOOST_CHECK_EQUAL(ms.getMonitorableObjectPath(), "");
  BOOST_CHECK_EQUAL(ms.getStatusFlag(), swatch::core::kUnknown);
  BOOST_CHECK(!ms.isValueKnown());
  BOOST_CHECK_THROW(ms.getValue<bool>(), MetricValueNotKnown);
  BOOST_CHECK_THROW(ms.getValue<int>(), MetricValueNotKnown);
  BOOST_CHECK_THROW(ms.getValueAsString(), MetricValueNotKnown);
  BOOST_CHECK(ms.getWarningCondition() == NULL);
  BOOST_CHECK(ms.getErrorCondition() == NULL);
  BOOST_CHECK_EQUAL(ms.getMonitoringStatus(), monitoring::kEnabled);

  // 2) Set value, and check new snapshot
  m.setValue(true);
  ms = m.getSnapshot();
  BOOST_CHECK_EQUAL(ms.getStatusFlag(), swatch::core::StatusFlag::kNoLimit);
  BOOST_CHECK(ms.isValueKnown());
  BOOST_CHECK_EQUAL(ms.getValue<bool>(), true);
  BOOST_CHECK_THROW(ms.getValue<int>(), MetricValueFailedCast);
  BOOST_CHECK_EQUAL(ms.getValueAsString(), "true");
  BOOST_CHECK(ms.getWarningCondition() == NULL);
  BOOST_CHECK(ms.getErrorCondition() == NULL);
  BOOST_CHECK_EQUAL(ms.getMonitoringStatus(), monitoring::kEnabled);
}


BOOST_AUTO_TEST_CASE(MetricWithErrorCondition)
{
  DummyMetric<int> m("aDummyMetric", "", 1, new LessThanCondition<int>(0));

  // 1) With good value, status flag should be good
  BOOST_CHECK_EQUAL(m.getId(), "aDummyMetric");
  MetricSnapshot ms = m.getSnapshot();
  BOOST_CHECK_EQUAL(ms.getPath(), "aDummyMetric");
  BOOST_CHECK_EQUAL(ms.getMetricId(), "aDummyMetric");
  BOOST_CHECK_EQUAL(ms.getMonitorableObjectId(), "");
  BOOST_CHECK_EQUAL(ms.getMonitorableObjectPath(), "");
  BOOST_CHECK_EQUAL(ms.isValueKnown(), true);
  BOOST_CHECK_EQUAL(ms.getValue<int>(), 1);
  BOOST_CHECK_EQUAL(ms.getStatusFlag(), StatusFlag::kGood);

  // 2) Update value to a "bad" value: status flag should go to "error"
  m.setValue(-1);
  ms = m.getSnapshot();
  BOOST_CHECK_EQUAL(ms.isValueKnown(), true);
  BOOST_CHECK_EQUAL(ms.getValue<int>(), -1);
  BOOST_CHECK_EQUAL(ms.getStatusFlag(), StatusFlag::kError);
}


BOOST_AUTO_TEST_CASE(NonCriticalMetric)
{
  DummyMetric<int> m("myCoolDummyMetric", "", 1);
  m.setMonitoringStatus(monitoring::kNonCritical);

  BOOST_CHECK_EQUAL(m.getId(), "myCoolDummyMetric");
  BOOST_CHECK_EQUAL(m.getStatus().second, monitoring::kNonCritical);

  MetricSnapshot ms = m.getSnapshot();
  BOOST_CHECK(ms.isValueKnown());
  BOOST_CHECK_EQUAL(ms.getValue<int>(), 1);
  BOOST_CHECK_EQUAL(ms.getMonitoringStatus(), monitoring::kNonCritical);
}

BOOST_AUTO_TEST_CASE(DisabledMetric)
{
  SimpleMetric<int> m("myCoolDummyMetric", "");

  BOOST_CHECK_EQUAL(m.getId(), "myCoolDummyMetric");
  m.setMonitoringStatus(monitoring::kDisabled);

  MetricSnapshot ms = m.getSnapshot();
  BOOST_CHECK(!ms.isValueKnown());
  BOOST_CHECK_THROW(ms.getValue<int>(), MetricValueNotKnown);
  BOOST_CHECK_EQUAL(ms.getMonitoringStatus(), monitoring::kDisabled);
  BOOST_CHECK_EQUAL(ms.getStatusFlag(), StatusFlag::kNoLimit);
}

BOOST_AUTO_TEST_CASE(DisabledMetricWithErrorCondition)
{
  DummyMetric<int> m("myCoolDummyMetric", "", 1, new LessThanCondition<int>(0));
  m.setMonitoringStatus(monitoring::kDisabled);
  BOOST_CHECK_EQUAL(m.getStatus().second, monitoring::kDisabled);

  MetricSnapshot ms = m.getSnapshot();
  BOOST_CHECK(ms.isValueKnown());
  BOOST_CHECK_EQUAL(ms.getValue<int>(), 1);
  BOOST_CHECK_EQUAL(ms.getMonitoringStatus(), monitoring::kDisabled);
  BOOST_CHECK_EQUAL(ms.getStatusFlag(), StatusFlag::kNoLimit);

  m.setValue(-1);
  ms = m.getSnapshot();
  BOOST_CHECK(ms.isValueKnown());
  BOOST_CHECK_EQUAL(ms.getValue<int>(), -1);
  BOOST_CHECK_EQUAL(ms.getStatusFlag(), StatusFlag::kNoLimit);
}

BOOST_AUTO_TEST_SUITE_END() // MetricTestSuite
} /* namespace test */
} /* namespace core */
} /* namespace swatch */

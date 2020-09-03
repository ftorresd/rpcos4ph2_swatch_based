/*
 * FSM_test.cpp
 *
 *  Created on: 18 Nov 2014
 *      Author: kreczko
 */
#include <boost/test/unit_test.hpp>


// boost headers
#include "boost/chrono/chrono_io.hpp"

// SWATCH headers
#include "swatch/core/MetricConditions.hpp"
#include "swatch/core/test/DummyMonitorableObjects.hpp"


namespace swatch {
namespace core {
namespace test {


struct MonitorableObjectTestSetup {
  MonitorableObjectTestSetup();
  ~MonitorableObjectTestSetup();

  typedef DummyMonitorableObject::IntegerMetricDataType_t IntegerMetricDataType_t;
  typedef SimpleMetric<IntegerMetricDataType_t> IntegerMetric_t;
  DummyMasterMonitorableObject masterObj;
  DummyMonitorableObject& childObj;
  IntegerMetric_t& childMetricA;
  IntegerMetric_t& childMetricB;
  IntegerMetric_t& grandchildMetricA;
  IntegerMetric_t& grandchildMetricB;
  const std::vector<const IntegerMetric_t*> childMetrics;
  const std::vector<const IntegerMetric_t*> grandchildMetrics;
  const std::vector<const IntegerMetric_t*> allMetrics;
};


MonitorableObjectTestSetup::MonitorableObjectTestSetup() :
  masterObj("myObj"),
  childObj(dynamic_cast<DummyMonitorableObject&>(masterObj.addMonitorable(new DummyMonitorableObject("childObj")))),
  childMetricA(masterObj.registerMetric<IntegerMetricDataType_t>("metricA")),
  childMetricB(masterObj.registerMetric<IntegerMetricDataType_t>("metricB")),
  grandchildMetricA(childObj.registerMetric<IntegerMetricDataType_t>("metricA")),
  grandchildMetricB(childObj.registerMetric<IntegerMetricDataType_t>("metricB")),
  childMetrics{&childMetricA, &childMetricB},
  grandchildMetrics{&grandchildMetricA, &grandchildMetricB},
  allMetrics{&childMetricA, &childMetricB, &grandchildMetricA, &grandchildMetricB}
{
  masterObj.setConditions(childMetricB, GreaterThanCondition<DummyMonitorableObject::IntegerMetricDataType_t>(0), EqualCondition<DummyMonitorableObject::IntegerMetricDataType_t>(0));
  childObj.setConditions(grandchildMetricB, GreaterThanCondition<DummyMonitorableObject::IntegerMetricDataType_t>(0), EqualCondition<DummyMonitorableObject::IntegerMetricDataType_t>(0));
}

MonitorableObjectTestSetup::~MonitorableObjectTestSetup()
{
}




BOOST_AUTO_TEST_SUITE( MonitorableObjectTestSuite )


BOOST_AUTO_TEST_CASE(TestConstruction)
{
  // Check that object's initial state after construction is as expected
  DummyMonitorableObject lObj("testMonObj");

  BOOST_CHECK_EQUAL(lObj.getId(), "testMonObj");
  BOOST_CHECK_EQUAL(lObj.getPath(), "testMonObj");
  BOOST_CHECK_EQUAL(lObj.getAlias(), "");
  BOOST_CHECK(lObj.getChildren().empty());

  BOOST_CHECK_EQUAL(lObj.getMetrics().size(), size_t(0));
  BOOST_CHECK_EQUAL(lObj.getStatusFlag(), swatch::core::kNoLimit);
  BOOST_CHECK_EQUAL(lObj.getMonitoringStatus(), swatch::core::monitoring::kEnabled);
  
  MonitorableObjectSnapshot lSnapshot = lObj.getStatus();
  BOOST_CHECK_EQUAL(lSnapshot.getId(), lObj.getId());
  BOOST_CHECK_EQUAL(lSnapshot.getPath(), lObj.getPath());
  BOOST_CHECK_EQUAL(lSnapshot.getStatusFlag(), swatch::core::kNoLimit);
  BOOST_CHECK_EQUAL(lSnapshot.getMonitoringStatus(), swatch::core::monitoring::kEnabled);
}

BOOST_AUTO_TEST_CASE(TestConstructionWithAlias)
{
  // Check that object's initial state after construction is as expected
  DummyMonitorableObject lObj("testMonObj", "my cool alias");

  BOOST_CHECK_EQUAL(lObj.getId(), "testMonObj");
  BOOST_CHECK_EQUAL(lObj.getPath(), "testMonObj");
  BOOST_CHECK_EQUAL(lObj.getAlias(), "my cool alias");
  BOOST_CHECK(lObj.getChildren().empty());
}

BOOST_AUTO_TEST_CASE(RegisterMetricWithoutConditions)
{
  DummyMonitorableObject lObj("myMonObj");

  // Check that metric isn't already registered
  BOOST_REQUIRE_EQUAL(lObj.getMetrics().size(), size_t(0));
  BOOST_REQUIRE_THROW(lObj.getMetric("myTestMetric"), MetricNotFoundInMonitorableObject);
  BOOST_REQUIRE_THROW(lObj.getMetric<bool>("myTestMetric"), MetricNotFoundInMonitorableObject);
  BOOST_REQUIRE_THROW(lObj.getObj("myTestMetric"), ObjectDoesNotExist);

  // Register the metric
  SimpleMetric<bool>& lMetric = lObj.registerMetric<bool>("myTestMetric");

  // Check that now can't register another metric of the same ID
  BOOST_CHECK_THROW(lObj.registerMetric<bool>("myTestMetric"), ObjectOfSameIdAlreadyExists);
  BOOST_CHECK_THROW(lObj.registerMetric<uint32_t>("myTestMetric"), ObjectOfSameIdAlreadyExists);

  // Test access from monitorable object, and monitorable object's status flag
  BOOST_CHECK_EQUAL(&lObj.getMetric("myTestMetric"), &lMetric);
  BOOST_CHECK_EQUAL(&lObj.getMetric<bool>("myTestMetric"), &lMetric);
  BOOST_CHECK_EQUAL(&lObj.getObj("myTestMetric"), &lMetric);
  BOOST_CHECK_EQUAL(lObj.getStatusFlag(), swatch::core::kUnknown);
  BOOST_CHECK_EQUAL(lObj.getStatus().getStatusFlag(), swatch::core::kUnknown);

  // Test properties of metric class
  BOOST_CHECK_EQUAL(lMetric.getId(), "myTestMetric");
  BOOST_CHECK_EQUAL(lMetric.getPath(), "myMonObj.myTestMetric");
  BOOST_CHECK_EQUAL(lMetric.getUpdateTime(), SteadyTimePoint_t());
  BOOST_CHECK_EQUAL(lMetric.getStatus().first, kUnknown);
  BOOST_CHECK_EQUAL(lMetric.getStatus().second, monitoring::kEnabled);

  // Check information in metric snapshot
  MetricSnapshot lSnapshot = lMetric.getSnapshot();
  BOOST_CHECK_EQUAL(lSnapshot.getPath(), "myMonObj.myTestMetric");
  BOOST_CHECK_EQUAL(lSnapshot.getMetricId(), "myTestMetric");
  BOOST_CHECK_EQUAL(lSnapshot.getMonitorableObjectId(), "myMonObj");
  BOOST_CHECK_EQUAL(lSnapshot.getMonitorableObjectPath(), "myMonObj");
  BOOST_CHECK_EQUAL(lSnapshot.isValueKnown(), false);
  BOOST_CHECK_THROW(lSnapshot.getValue<bool>(), MetricValueNotKnown);
  BOOST_CHECK_THROW(lSnapshot.getValue<int>(), MetricValueNotKnown);
  BOOST_CHECK_THROW(lSnapshot.getValueAsString(), MetricValueNotKnown);
  BOOST_CHECK_EQUAL(lSnapshot.getStatusFlag(), kUnknown);
  BOOST_CHECK_EQUAL(lSnapshot.getUpdateTimestamp().steady, SteadyTimePoint_t());
  BOOST_CHECK_EQUAL(lSnapshot.getUpdateTimestamp().system, SystemTimePoint_t());
  BOOST_CHECK_EQUAL(lSnapshot.getMonitoringStatus(), monitoring::kEnabled);
  BOOST_CHECK_EQUAL(lSnapshot.getErrorCondition(), (const AbstractMetricCondition*) NULL);
  BOOST_CHECK_EQUAL(lSnapshot.getWarningCondition(), (const AbstractMetricCondition*) NULL);

  // Check that getMetric method throws when requesting metric of unknown ID
  BOOST_CHECK_THROW(lObj.getMetric("non_existant_metric"), MetricNotFoundInMonitorableObject);
  BOOST_CHECK_THROW(lObj.getMetric<bool>("non_existant_metric"), MetricNotFoundInMonitorableObject);
}


BOOST_AUTO_TEST_CASE(RegisterMetricWithErrorCondition)
{
  DummyMonitorableObject lObj("myMonObj");

  // Check that metric isn't already registered
  BOOST_REQUIRE_EQUAL(lObj.getMetrics().size(), size_t(0));
  BOOST_REQUIRE_THROW(lObj.getMetric("myTestMetric"), MetricNotFoundInMonitorableObject);
  BOOST_REQUIRE_THROW(lObj.getMetric<bool>("myTestMetric"), MetricNotFoundInMonitorableObject);
  BOOST_REQUIRE_THROW(lObj.getObj("myTestMetric"), ObjectDoesNotExist);

  // Register the metric
  SimpleMetric<bool>& lMetric = lObj.registerMetric<bool>("myTestMetric", core::EqualCondition<bool>(true));

  // Check that now can't register another metric of the same ID
  BOOST_CHECK_THROW(lObj.registerMetric<bool>("myTestMetric"), ObjectOfSameIdAlreadyExists);
  BOOST_CHECK_THROW(lObj.registerMetric<uint32_t>("myTestMetric"), ObjectOfSameIdAlreadyExists);

  // Test access from monitorable object, and monitorable object's status flag
  BOOST_CHECK_EQUAL(&lObj.getMetric("myTestMetric"), &lMetric);
  BOOST_CHECK_EQUAL(&lObj.getMetric<bool>("myTestMetric"), &lMetric);
  BOOST_CHECK_EQUAL(&lObj.getObj("myTestMetric"), &lMetric);
  BOOST_CHECK_EQUAL(lObj.getStatusFlag(), swatch::core::kUnknown);
  BOOST_CHECK_EQUAL(lObj.getStatus().getStatusFlag(), swatch::core::kUnknown);

  // Test properties of metric class
  BOOST_CHECK_EQUAL(lMetric.getId(), "myTestMetric");
  BOOST_CHECK_EQUAL(lMetric.getPath(), "myMonObj.myTestMetric");
  BOOST_CHECK_EQUAL(lMetric.getUpdateTime(), SteadyTimePoint_t());
  BOOST_CHECK_EQUAL(lMetric.getStatus().first, kUnknown);
  BOOST_CHECK_EQUAL(lMetric.getStatus().second, monitoring::kEnabled);

  // Check information in metric snapshot
  MetricSnapshot lSnapshot = lMetric.getSnapshot();
  BOOST_CHECK_EQUAL(lSnapshot.getPath(), "myMonObj.myTestMetric");
  BOOST_CHECK_EQUAL(lSnapshot.getMetricId(), "myTestMetric");
  BOOST_CHECK_EQUAL(lSnapshot.getMonitorableObjectId(), "myMonObj");
  BOOST_CHECK_EQUAL(lSnapshot.getMonitorableObjectPath(), "myMonObj");
  BOOST_CHECK_EQUAL(lSnapshot.isValueKnown(), false);
  BOOST_CHECK_THROW(lSnapshot.getValue<bool>(), MetricValueNotKnown);
  BOOST_CHECK_THROW(lSnapshot.getValue<int>(), MetricValueNotKnown);
  BOOST_CHECK_THROW(lSnapshot.getValueAsString(), MetricValueNotKnown);
  BOOST_CHECK_EQUAL(lSnapshot.getUpdateTimestamp().steady, SteadyTimePoint_t());
  BOOST_CHECK_EQUAL(lSnapshot.getUpdateTimestamp().system, SystemTimePoint_t());
  BOOST_CHECK_EQUAL(lSnapshot.getMonitoringStatus(), monitoring::kEnabled);
  BOOST_CHECK_EQUAL(lSnapshot.getWarningCondition(), (const AbstractMetricCondition*) NULL);
  BOOST_REQUIRE_NE(lSnapshot.getErrorCondition(), (const AbstractMetricCondition*) NULL);
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(*lSnapshot.getErrorCondition()), " == 1");

  // Check that getMetric method throws when requesting metric of unknown ID
  BOOST_CHECK_THROW(lObj.getMetric("non_existant_metric"), MetricNotFoundInMonitorableObject);
  BOOST_CHECK_THROW(lObj.getMetric<bool>("non_existant_metric"), MetricNotFoundInMonitorableObject);
}


BOOST_AUTO_TEST_CASE(RegisterMetricWithBothConditions)
{
  DummyMonitorableObject lObj("myMonObj");

  // Check that metric isn't already registered
  BOOST_REQUIRE_EQUAL(lObj.getMetrics().size(), size_t(0));
  BOOST_REQUIRE_THROW(lObj.getMetric("myTestMetric"), MetricNotFoundInMonitorableObject);
  BOOST_REQUIRE_THROW(lObj.getMetric<bool>("myTestMetric"), MetricNotFoundInMonitorableObject);
  BOOST_REQUIRE_THROW(lObj.getObj("myTestMetric"), ObjectDoesNotExist);

  // Register the metric
  SimpleMetric<uint32_t>& lMetric = lObj.registerMetric<uint32_t>("myTestMetric", GreaterThanCondition<uint32_t>(100), NotEqualCondition<uint32_t>(0));

  // Check that now can't register another metric of the same ID
  BOOST_CHECK_THROW(lObj.registerMetric<bool>("myTestMetric"), ObjectOfSameIdAlreadyExists);
  BOOST_CHECK_THROW(lObj.registerMetric<uint32_t>("myTestMetric"), ObjectOfSameIdAlreadyExists);

  // Test access from monitorable object, and monitorable object's status flag
  BOOST_CHECK_EQUAL(&lObj.getMetric("myTestMetric"), &lMetric);
  BOOST_CHECK_EQUAL(&lObj.getMetric<uint32_t>("myTestMetric"), &lMetric);
  BOOST_CHECK_EQUAL(&lObj.getObj("myTestMetric"), &lMetric);
  BOOST_CHECK_EQUAL(lObj.getStatusFlag(), swatch::core::kUnknown);
  BOOST_CHECK_EQUAL(lObj.getStatus().getStatusFlag(), swatch::core::kUnknown);

  // Test properties of metric class
  BOOST_CHECK_EQUAL(lMetric.getId(), "myTestMetric");
  BOOST_CHECK_EQUAL(lMetric.getPath(), "myMonObj.myTestMetric");
  BOOST_CHECK_EQUAL(lMetric.getUpdateTime(), SteadyTimePoint_t());
  BOOST_CHECK_EQUAL(lMetric.getStatus().first, kUnknown);
  BOOST_CHECK_EQUAL(lMetric.getStatus().second, monitoring::kEnabled);

  // Check information in metric snapshot
  MetricSnapshot lSnapshot = lMetric.getSnapshot();
  BOOST_CHECK_EQUAL(lSnapshot.getPath(), "myMonObj.myTestMetric");
  BOOST_CHECK_EQUAL(lSnapshot.getMetricId(), "myTestMetric");
  BOOST_CHECK_EQUAL(lSnapshot.getMonitorableObjectId(), "myMonObj");
  BOOST_CHECK_EQUAL(lSnapshot.getMonitorableObjectPath(), "myMonObj");
  BOOST_CHECK_EQUAL(lSnapshot.isValueKnown(), false);
  BOOST_CHECK_THROW(lSnapshot.getValue<uint32_t>(), MetricValueNotKnown);
  BOOST_CHECK_THROW(lSnapshot.getValue<bool>(), MetricValueNotKnown);
  BOOST_CHECK_THROW(lSnapshot.getValueAsString(), MetricValueNotKnown);
  BOOST_CHECK_EQUAL(lSnapshot.getUpdateTimestamp().steady, SteadyTimePoint_t());
  BOOST_CHECK_EQUAL(lSnapshot.getUpdateTimestamp().system, SystemTimePoint_t());
  BOOST_CHECK_EQUAL(lSnapshot.getMonitoringStatus(), monitoring::kEnabled);
  BOOST_REQUIRE_NE(lSnapshot.getWarningCondition(), (const AbstractMetricCondition*) NULL);
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(*lSnapshot.getWarningCondition()), " != 0");
  BOOST_REQUIRE_NE(lSnapshot.getErrorCondition(), (const AbstractMetricCondition*) NULL);
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(*lSnapshot.getErrorCondition()), "> 100");

  // Check that getMetric method throws when requesting metric of unknown ID
  BOOST_CHECK_THROW(lObj.getMetric("non_existant_metric"), MetricNotFoundInMonitorableObject);
  BOOST_CHECK_THROW(lObj.getMetric<bool>("non_existant_metric"), MetricNotFoundInMonitorableObject);
}


BOOST_AUTO_TEST_CASE(SetMetricConditionsAfterRegistration)
{
  DummyMonitorableObject lObj("myMonObj");

  // Register the metric
  SimpleMetric<uint32_t>& lMetric = lObj.registerMetric<uint32_t>("myTestMetric");

  // Check that metric doesn't have any conditions, before adding them ...
  MetricSnapshot lSnapshot = lMetric.getSnapshot();
  BOOST_REQUIRE_EQUAL(lSnapshot.getWarningCondition(), (const AbstractMetricCondition*) NULL);
  BOOST_REQUIRE_EQUAL(lSnapshot.getErrorCondition(), (const AbstractMetricCondition*) NULL);

  // Add warning condition
  lObj.setWarningCondition(lMetric, EqualCondition<uint32_t>(0));
  lSnapshot = lMetric.getSnapshot();
  BOOST_REQUIRE_NE(lSnapshot.getWarningCondition(), (const AbstractMetricCondition*) NULL);
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(*lSnapshot.getWarningCondition()), " == 0");
  BOOST_CHECK_EQUAL(lSnapshot.getErrorCondition(), (const AbstractMetricCondition*) NULL);

  // Add error condition
  lObj.setErrorCondition(lMetric, GreaterThanCondition<uint32_t>(42));
  lSnapshot = lMetric.getSnapshot();
  BOOST_REQUIRE_NE(lSnapshot.getWarningCondition(), (const AbstractMetricCondition*) NULL);
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(*lSnapshot.getWarningCondition()), " == 0");
  BOOST_REQUIRE_NE(lSnapshot.getErrorCondition(), (const AbstractMetricCondition*) NULL);
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(*lSnapshot.getErrorCondition()), "> 42");

  // Change both error & warning conditions
  lObj.setConditions(lMetric, LessThanCondition<uint32_t>(10), NotEqualCondition<uint32_t>(42));
  lSnapshot = lMetric.getSnapshot();
  BOOST_REQUIRE_NE(lSnapshot.getWarningCondition(), (const AbstractMetricCondition*) NULL);
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(*lSnapshot.getWarningCondition()), " != 42");
  BOOST_REQUIRE_NE(lSnapshot.getErrorCondition(), (const AbstractMetricCondition*) NULL);
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(*lSnapshot.getErrorCondition()), "< 10");
}


const bool* complexBooleanMetricTestCalcFunction(const std::vector<MetricSnapshot>& aSnapshots)
{
  return new bool(true);
}

ComplexMetric<bool>::CalculateFunction_t lComplexBooleanMetricTestCalcFunction = &complexBooleanMetricTestCalcFunction;

const uint32_t* complexUint32MetricTestCalcFunction(const std::vector<MetricSnapshot>& aSnapshots)
{
  return new uint32_t(42);
}

ComplexMetric<uint32_t>::CalculateFunction_t lComplexUint32MetricTestCalcFunction = &complexUint32MetricTestCalcFunction;


BOOST_AUTO_TEST_CASE(RegisterComplexMetricSuccessful)
{
  DummyMonitorableObject lObj("myMonObj");

  // Check that metric isn't already registered
  BOOST_REQUIRE_EQUAL(lObj.getMetrics().size(), size_t(0));
  BOOST_REQUIRE_THROW(lObj.getMetric("myTestMetric"), MetricNotFoundInMonitorableObject);
  BOOST_REQUIRE_THROW(lObj.getMetric<bool>("myTestMetric"), MetricNotFoundInMonitorableObject);
  BOOST_REQUIRE_THROW(lObj.getObj("myTestMetric"), ObjectDoesNotExist);

  // Register the metric
  SimpleMetric<uint32_t>& lSourceMetric = lObj.registerMetric<uint32_t>("aMetric");
  std::vector<AbstractMetric*> lSourceMetrics = {&lSourceMetric};
  ComplexMetric<bool>& lMetric = lObj.registerComplexMetric<bool>("myTestMetric", lSourceMetrics.begin(), lSourceMetrics.end(), lComplexBooleanMetricTestCalcFunction);

  // Check that now can't register another metric of the same ID
  BOOST_CHECK_THROW(lObj.registerMetric<bool>("myTestMetric"), ObjectOfSameIdAlreadyExists);
  BOOST_CHECK_THROW(lObj.registerMetric<uint32_t>("myTestMetric"), ObjectOfSameIdAlreadyExists);
  BOOST_CHECK_THROW(lObj.registerComplexMetric<bool>("myTestMetric", lSourceMetrics.begin(), lSourceMetrics.end(), lComplexBooleanMetricTestCalcFunction), ObjectOfSameIdAlreadyExists);
  BOOST_CHECK_THROW(lObj.registerComplexMetric<uint32_t>("myTestMetric", lSourceMetrics.begin(), lSourceMetrics.end(), lComplexUint32MetricTestCalcFunction), ObjectOfSameIdAlreadyExists);

  // Test access from monitorable object, and monitorable object's status flag
  BOOST_CHECK_EQUAL(&lObj.getMetric("myTestMetric"), &lMetric);
  BOOST_CHECK_EQUAL(&lObj.getComplexMetric<bool>("myTestMetric"), &lMetric);
  BOOST_CHECK_EQUAL(&lObj.getObj("myTestMetric"), &lMetric);
  BOOST_CHECK_EQUAL(lObj.getStatusFlag(), swatch::core::kUnknown);
  BOOST_CHECK_EQUAL(lObj.getStatus().getStatusFlag(), swatch::core::kUnknown);

  // Test properties of metric class
  BOOST_CHECK_EQUAL(lMetric.getId(), "myTestMetric");
  BOOST_CHECK_EQUAL(lMetric.getPath(), "myMonObj.myTestMetric");
  BOOST_CHECK_EQUAL(lMetric.getUpdateTime(), SteadyTimePoint_t());
  BOOST_CHECK_EQUAL(lMetric.getStatus().first, kUnknown);
  BOOST_CHECK_EQUAL(lMetric.getStatus().second, monitoring::kEnabled);

  // Check information in metric snapshot
  MetricSnapshot lSnapshot = lMetric.getSnapshot();
  BOOST_CHECK_EQUAL(lSnapshot.getPath(), "myMonObj.myTestMetric");
  BOOST_CHECK_EQUAL(lSnapshot.getMetricId(), "myTestMetric");
  BOOST_CHECK_EQUAL(lSnapshot.getMonitorableObjectId(), "myMonObj");
  BOOST_CHECK_EQUAL(lSnapshot.getMonitorableObjectPath(), "myMonObj");
  BOOST_CHECK_EQUAL(lSnapshot.isValueKnown(), false);
  BOOST_CHECK_THROW(lSnapshot.getValue<bool>(), MetricValueNotKnown);
  BOOST_CHECK_THROW(lSnapshot.getValue<int>(), MetricValueNotKnown);
  BOOST_CHECK_THROW(lSnapshot.getValueAsString(), MetricValueNotKnown);
  BOOST_CHECK_EQUAL(lSnapshot.getStatusFlag(), kUnknown);
  BOOST_CHECK_EQUAL(lSnapshot.getUpdateTimestamp().steady, SteadyTimePoint_t());
  BOOST_CHECK_EQUAL(lSnapshot.getUpdateTimestamp().system, SystemTimePoint_t());
  BOOST_CHECK_EQUAL(lSnapshot.getMonitoringStatus(), monitoring::kEnabled);
  BOOST_CHECK_EQUAL(lSnapshot.getErrorCondition(), (const AbstractMetricCondition*) NULL);
  BOOST_CHECK_EQUAL(lSnapshot.getWarningCondition(), (const AbstractMetricCondition*) NULL);

  // Check that getMetric method throws when requesting metric of unknown ID
  BOOST_CHECK_THROW(lObj.getMetric("non_existant_metric"), MetricNotFoundInMonitorableObject);
  BOOST_CHECK_THROW(lObj.getMetric<bool>("non_existant_metric"), MetricNotFoundInMonitorableObject);
}


BOOST_AUTO_TEST_CASE(RegisterComplexMetricFailed_NullFunctionPointer)
{
  DummyMonitorableObject lObj("myMonObj");

  // Register the metric - should fail if 'value calculating' function pointer is null
  SimpleMetric<uint32_t>& lSourceMetric = lObj.registerMetric<uint32_t>("aMetric");
  std::vector<AbstractMetric*> lSourceMetrics = {&lSourceMetric};
  BOOST_CHECK_THROW(lObj.registerComplexMetric<bool>("myTestMetric", lSourceMetrics.begin(), lSourceMetrics.end(), ComplexMetric<bool>::CalculateFunction_t(NULL)), MetricRegistrationError);

  // Check that the complex metric wasn't registered
  BOOST_CHECK_EQUAL(lObj.getMetrics().size(), size_t(1));
  BOOST_CHECK_THROW(lObj.getMetric("myTestMetric"), MetricNotFoundInMonitorableObject);
  BOOST_CHECK_THROW(lObj.getComplexMetric<bool>("myTestMetric"), MetricNotFoundInMonitorableObject);
  BOOST_CHECK_THROW(lObj.getObj("myTestMetric"), ObjectDoesNotExist);
}


BOOST_AUTO_TEST_CASE(RegisterComplexMetricFailed_NullMetricPointer)
{
  DummyMonitorableObject lObj("myMonObj");

  // Register the metric - should fail if value calculating function pointer is null
  SimpleMetric<uint32_t>& lSourceMetric = lObj.registerMetric<uint32_t>("aMetric");
  std::vector<AbstractMetric*> lSourceMetrics = {&lSourceMetric, NULL};
  BOOST_CHECK_THROW(lObj.registerComplexMetric<bool>("myTestMetric", lSourceMetrics.begin(), lSourceMetrics.end(), lComplexBooleanMetricTestCalcFunction), MetricRegistrationError);

  // Check that the complex metric wasn't registered
  BOOST_CHECK_EQUAL(lObj.getMetrics().size(), size_t(1));
  BOOST_CHECK_THROW(lObj.getMetric("myTestMetric"), MetricNotFoundInMonitorableObject);
  BOOST_CHECK_THROW(lObj.getComplexMetric<bool>("myTestMetric"), MetricNotFoundInMonitorableObject);
  BOOST_CHECK_THROW(lObj.getObj("myTestMetric"), ObjectDoesNotExist);
}


BOOST_AUTO_TEST_CASE(RegisterComplexMetricFailed_ZeroMetrics)
{
  DummyMonitorableObject lObj("myMonObj");

  // Register the metric - should fail if metric pointer collection is empty
  std::vector<AbstractMetric*> lSourceMetrics = {};
  BOOST_CHECK_THROW(lObj.registerComplexMetric<bool>("myTestMetric", lSourceMetrics.begin(), lSourceMetrics.end(), lComplexBooleanMetricTestCalcFunction), MetricRegistrationError);

  // Check that the complex metric wasn't registered
  BOOST_CHECK_EQUAL(lObj.getMetrics().size(), size_t(0));
  BOOST_CHECK_THROW(lObj.getMetric("myTestMetric"), MetricNotFoundInMonitorableObject);
  BOOST_CHECK_THROW(lObj.getComplexMetric<bool>("myTestMetric"), MetricNotFoundInMonitorableObject);
  BOOST_CHECK_THROW(lObj.getObj("myTestMetric"), ObjectDoesNotExist);
}


BOOST_AUTO_TEST_CASE(RegisterComplexMetricFailed_DuplicateMetricPointer)
{
  DummyMonitorableObject lObj("myMonObj");

  // Register the metric - should fail if a metric pointer appears twice in collection
  SimpleMetric<uint32_t>& lSourceMetric1 = lObj.registerMetric<uint32_t>("aMetric");
  SimpleMetric<uint32_t>& lSourceMetric2 = lObj.registerMetric<uint32_t>("anotherMetric");
  std::vector<AbstractMetric*> lSourceMetrics = {&lSourceMetric1, &lSourceMetric2, &lSourceMetric1};
  BOOST_CHECK_THROW(lObj.registerComplexMetric<bool>("myTestMetric", lSourceMetrics.begin(), lSourceMetrics.end(), lComplexBooleanMetricTestCalcFunction), MetricRegistrationError);

  // Check that the complex metric wasn't registered
  BOOST_CHECK_EQUAL(lObj.getMetrics().size(), size_t(2));
  BOOST_CHECK_THROW(lObj.getMetric("myTestMetric"), MetricNotFoundInMonitorableObject);
  BOOST_CHECK_THROW(lObj.getComplexMetric<bool>("myTestMetric"), MetricNotFoundInMonitorableObject);
  BOOST_CHECK_THROW(lObj.getObj("myTestMetric"), ObjectDoesNotExist);
}


BOOST_AUTO_TEST_CASE(RegisterComplexMetricFailed_SourceMetricIsNotDescendant)
{
  DummyMonitorableObject lObj1("myMonObj");
  DummyMonitorableObject lObj2("myMonObj");

  // Register the metric - should fail if any of the source metrics is not a descendant of the metric's parent
  lObj1.registerMetric<uint32_t>("aMetric");
  SimpleMetric<uint32_t>& lSourceMetric2 = lObj2.registerMetric<uint32_t>("aMetric");
  std::vector<AbstractMetric*> lSourceMetrics = {&lSourceMetric2};
  BOOST_CHECK_THROW(lObj1.registerComplexMetric<bool>("myTestMetric", lSourceMetrics.begin(), lSourceMetrics.end(), lComplexBooleanMetricTestCalcFunction), MetricRegistrationError);

  // Check that the complex metric wasn't registered
  BOOST_CHECK_EQUAL(lObj1.getMetrics().size(), size_t(1));
  BOOST_CHECK_THROW(lObj1.getMetric("myTestMetric"), MetricNotFoundInMonitorableObject);
  BOOST_CHECK_THROW(lObj1.getComplexMetric<bool>("myTestMetric"), MetricNotFoundInMonitorableObject);
  BOOST_CHECK_THROW(lObj1.getObj("myTestMetric"), ObjectDoesNotExist);
}


const bool* complexBooleanMetricTestCalcFunction2(const std::vector<MonitorableObjectSnapshot>& aSnapshots)
{
  return new bool(true);
}

ComplexMetric<bool>::CalculateFunction2_t lComplexBooleanMetricTestCalcFunction2 = &complexBooleanMetricTestCalcFunction2;

const uint32_t* complexUint32MetricTestCalcFunction2(const std::vector<MonitorableObjectSnapshot>& aSnapshots)
{
  return new uint32_t(42);
}

ComplexMetric<uint32_t>::CalculateFunction2_t lComplexUint32MetricTestCalcFunction2 = &complexUint32MetricTestCalcFunction2;


BOOST_AUTO_TEST_CASE(RegisterComplexMetric2Successful)
{
  DummyMonitorableObject lObj("myMonObj");

  // Check that metric isn't already registered
  BOOST_REQUIRE_EQUAL(lObj.getMetrics().size(), size_t(0));
  BOOST_REQUIRE_THROW(lObj.getMetric("myTestMetric"), MetricNotFoundInMonitorableObject);
  BOOST_REQUIRE_THROW(lObj.getMetric<bool>("myTestMetric"), MetricNotFoundInMonitorableObject);
  BOOST_REQUIRE_THROW(lObj.getObj("myTestMetric"), ObjectDoesNotExist);

  // Register the metric
  MonitorableObject& lSourceMonObj = lObj.addMonitorable(new DummyMonitorableObject("aChildObj"));
  std::vector<MonitorableObject*> lSourceMonObjs = {&lSourceMonObj};
  ComplexMetric<bool>& lMetric = lObj.registerComplexMetric<bool>("myTestMetric", lSourceMonObjs.begin(), lSourceMonObjs.end(), lComplexBooleanMetricTestCalcFunction2);

  // Check that now can't register another metric of the same ID
  BOOST_CHECK_THROW(lObj.registerMetric<bool>("myTestMetric"), ObjectOfSameIdAlreadyExists);
  BOOST_CHECK_THROW(lObj.registerMetric<uint32_t>("myTestMetric"), ObjectOfSameIdAlreadyExists);
  BOOST_CHECK_THROW(lObj.registerComplexMetric<bool>("myTestMetric", lSourceMonObjs.begin(), lSourceMonObjs.end(), lComplexBooleanMetricTestCalcFunction2), ObjectOfSameIdAlreadyExists);
  BOOST_CHECK_THROW(lObj.registerComplexMetric<uint32_t>("myTestMetric", lSourceMonObjs.begin(), lSourceMonObjs.end(), lComplexUint32MetricTestCalcFunction2), ObjectOfSameIdAlreadyExists);

  // Test access from monitorable object, and monitorable object's status flag
  BOOST_CHECK_EQUAL(&lObj.getMetric("myTestMetric"), &lMetric);
  BOOST_CHECK_EQUAL(&lObj.getComplexMetric<bool>("myTestMetric"), &lMetric);
  BOOST_CHECK_EQUAL(&lObj.getObj("myTestMetric"), &lMetric);
  BOOST_CHECK_EQUAL(lObj.getStatusFlag(), swatch::core::kUnknown);
  BOOST_CHECK_EQUAL(lObj.getStatus().getStatusFlag(), swatch::core::kUnknown);

  // Test properties of metric class
  BOOST_CHECK_EQUAL(lMetric.getId(), "myTestMetric");
  BOOST_CHECK_EQUAL(lMetric.getPath(), "myMonObj.myTestMetric");
  BOOST_CHECK_EQUAL(lMetric.getUpdateTime(), SteadyTimePoint_t());
  BOOST_CHECK_EQUAL(lMetric.getStatus().first, kUnknown);
  BOOST_CHECK_EQUAL(lMetric.getStatus().second, monitoring::kEnabled);

  // Check information in metric snapshot
  MetricSnapshot lSnapshot = lMetric.getSnapshot();
  BOOST_CHECK_EQUAL(lSnapshot.getPath(), "myMonObj.myTestMetric");
  BOOST_CHECK_EQUAL(lSnapshot.getMetricId(), "myTestMetric");
  BOOST_CHECK_EQUAL(lSnapshot.getMonitorableObjectId(), "myMonObj");
  BOOST_CHECK_EQUAL(lSnapshot.getMonitorableObjectPath(), "myMonObj");
  BOOST_CHECK_EQUAL(lSnapshot.isValueKnown(), false);
  BOOST_CHECK_THROW(lSnapshot.getValue<bool>(), MetricValueNotKnown);
  BOOST_CHECK_THROW(lSnapshot.getValue<int>(), MetricValueNotKnown);
  BOOST_CHECK_THROW(lSnapshot.getValueAsString(), MetricValueNotKnown);
  BOOST_CHECK_EQUAL(lSnapshot.getStatusFlag(), kUnknown);
  BOOST_CHECK_EQUAL(lSnapshot.getUpdateTimestamp().steady, SteadyTimePoint_t());
  BOOST_CHECK_EQUAL(lSnapshot.getUpdateTimestamp().system, SystemTimePoint_t());
  BOOST_CHECK_EQUAL(lSnapshot.getMonitoringStatus(), monitoring::kEnabled);
  BOOST_CHECK_EQUAL(lSnapshot.getErrorCondition(), (const AbstractMetricCondition*) NULL);
  BOOST_CHECK_EQUAL(lSnapshot.getWarningCondition(), (const AbstractMetricCondition*) NULL);

  // Check that getMetric method throws when requesting metric of unknown ID
  BOOST_CHECK_THROW(lObj.getMetric("non_existant_metric"), MetricNotFoundInMonitorableObject);
  BOOST_CHECK_THROW(lObj.getMetric<bool>("non_existant_metric"), MetricNotFoundInMonitorableObject);
}


BOOST_AUTO_TEST_CASE(RegisterComplexMetric2Failed_NullFunctionPointer)
{
  DummyMonitorableObject lObj("myMonObj");

  // Register the metric - should fail if 'value calculating' function pointer is null
  MonitorableObject& lSourceMonObj = lObj.addMonitorable(new DummyMonitorableObject("aChildObj"));
  std::vector<MonitorableObject*> lSourceMonObjs = {&lSourceMonObj};
  BOOST_CHECK_THROW(lObj.registerComplexMetric<bool>("myTestMetric", lSourceMonObjs.begin(), lSourceMonObjs.end(), ComplexMetric<bool>::CalculateFunction2_t(NULL)), MetricRegistrationError);

  // Check that the complex metric wasn't registered
  BOOST_CHECK_EQUAL(lObj.getMetrics().size(), size_t(0));
  BOOST_CHECK_THROW(lObj.getMetric("myTestMetric"), MetricNotFoundInMonitorableObject);
  BOOST_CHECK_THROW(lObj.getComplexMetric<bool>("myTestMetric"), MetricNotFoundInMonitorableObject);
  BOOST_CHECK_THROW(lObj.getObj("myTestMetric"), ObjectDoesNotExist);
}


BOOST_AUTO_TEST_CASE(RegisterComplexMetric2Failed_NullMonObjPointer)
{
  DummyMonitorableObject lObj("myMonObj");

  // Register the metric - should fail if value calculating function pointer is null
  MonitorableObject& lSourceMonObj = lObj.addMonitorable(new DummyMonitorableObject("aChildObj"));
  std::vector<MonitorableObject*> lSourceMonObjs = {&lSourceMonObj, NULL};
  BOOST_CHECK_THROW(lObj.registerComplexMetric<bool>("myTestMetric", lSourceMonObjs.begin(), lSourceMonObjs.end(), lComplexBooleanMetricTestCalcFunction2), MetricRegistrationError);

  // Check that the complex metric wasn't registered
  BOOST_CHECK_EQUAL(lObj.getMetrics().size(), size_t(0));
  BOOST_CHECK_THROW(lObj.getMetric("myTestMetric"), MetricNotFoundInMonitorableObject);
  BOOST_CHECK_THROW(lObj.getComplexMetric<bool>("myTestMetric"), MetricNotFoundInMonitorableObject);
  BOOST_CHECK_THROW(lObj.getObj("myTestMetric"), ObjectDoesNotExist);
}


BOOST_AUTO_TEST_CASE(RegisterComplexMetric2Failed_ZeroMonObjs)
{
  DummyMonitorableObject lObj("myMonObj");

  // Register the metric - should fail if metric pointer collection is empty
  std::vector<MonitorableObject*> lSourceMonObjs = {};
  BOOST_CHECK_THROW(lObj.registerComplexMetric<bool>("myTestMetric", lSourceMonObjs.begin(), lSourceMonObjs.end(), lComplexBooleanMetricTestCalcFunction2), MetricRegistrationError);

  // Check that the complex metric wasn't registered
  BOOST_CHECK_EQUAL(lObj.getMetrics().size(), size_t(0));
  BOOST_CHECK_THROW(lObj.getMetric("myTestMetric"), MetricNotFoundInMonitorableObject);
  BOOST_CHECK_THROW(lObj.getComplexMetric<bool>("myTestMetric"), MetricNotFoundInMonitorableObject);
  BOOST_CHECK_THROW(lObj.getObj("myTestMetric"), ObjectDoesNotExist);
}


BOOST_AUTO_TEST_CASE(RegisterComplexMetric2Failed_DuplicateMetricPointer)
{
  DummyMonitorableObject lObj("myMonObj");

  // Register the metric - should fail if a metric pointer appears twice in collection
  MonitorableObject& lSourceMonObj1 = lObj.addMonitorable(new DummyMonitorableObject("aChildObj1"));
  MonitorableObject& lSourceMonObj2 = lObj.addMonitorable(new DummyMonitorableObject("aChildObj2"));
  std::vector<MonitorableObject*> lSourceMonObjs = {&lSourceMonObj1, &lSourceMonObj2, &lSourceMonObj1};
  BOOST_CHECK_THROW(lObj.registerComplexMetric<bool>("myTestMetric", lSourceMonObjs.begin(), lSourceMonObjs.end(), lComplexBooleanMetricTestCalcFunction2), MetricRegistrationError);

  // Check that the complex metric wasn't registered
  BOOST_CHECK_EQUAL(lObj.getMetrics().size(), size_t(0));
  BOOST_CHECK_THROW(lObj.getMetric("myTestMetric"), MetricNotFoundInMonitorableObject);
  BOOST_CHECK_THROW(lObj.getComplexMetric<bool>("myTestMetric"), MetricNotFoundInMonitorableObject);
  BOOST_CHECK_THROW(lObj.getObj("myTestMetric"), ObjectDoesNotExist);
}


BOOST_AUTO_TEST_CASE(RegisterComplexMetric2Failed_SourceMonObjIsNotDescendant)
{
  DummyMonitorableObject lObj1("myMonObj");
  DummyMonitorableObject lObj2("myMonObj");

  // Register the metric - should fail if any of the source metrics is not a descendant of the metric's parent
  MonitorableObject& lObj2child = lObj2.addMonitorable(new DummyMonitorableObject("aChildObj2"));
  std::vector<MonitorableObject*> lSourceMonObjs = {&lObj2child};
  BOOST_CHECK_THROW(lObj1.registerComplexMetric<bool>("myTestMetric", lSourceMonObjs.begin(), lSourceMonObjs.end(), lComplexBooleanMetricTestCalcFunction2), MetricRegistrationError);

  // Check that the complex metric wasn't registered
  BOOST_CHECK_EQUAL(lObj1.getMetrics().size(), size_t(0));
  BOOST_CHECK_THROW(lObj1.getMetric("myTestMetric"), MetricNotFoundInMonitorableObject);
  BOOST_CHECK_THROW(lObj1.getComplexMetric<bool>("myTestMetric"), MetricNotFoundInMonitorableObject);
  BOOST_CHECK_THROW(lObj1.getObj("myTestMetric"), ObjectDoesNotExist);
}


BOOST_FIXTURE_TEST_CASE(UpdateMetricsSuccessful, MonitorableObjectTestSetup)
{
  // Check assumption that all metric start off being unknown ...
  for (auto lIt = allMetrics.begin(); lIt != allMetrics.end(); lIt++)  {
    const IntegerMetric_t& lMetric = **lIt;
    BOOST_REQUIRE_EQUAL(lMetric.getUpdateTime(), SteadyTimePoint_t());
    BOOST_REQUIRE_EQUAL(lMetric.getSnapshot().getUpdateTimestamp().steady, SteadyTimePoint_t());
    BOOST_REQUIRE_EQUAL(lMetric.getSnapshot().getUpdateTimestamp().system, SystemTimePoint_t());
    BOOST_REQUIRE_EQUAL(lMetric.getSnapshot().isValueKnown(), false);
    BOOST_REQUIRE_THROW(lMetric.getSnapshot().getValue<IntegerMetricDataType_t>(), MetricValueNotKnown);
  }

  // 1) Update values of child metrics (but not grandchildren yet)
  masterObj.setNextMetricValues( std::map<std::string, IntegerMetricDataType_t> {{"metricA", 0}, {"metricB", 42}});
  TimePoint lTimeBeforeLastUpdate = TimePoint::now();
  masterObj.updateMetrics( MetricUpdateGuard(masterObj) );

  // Values of child metrics shown now be known
  for (auto lIt = childMetrics.begin(); lIt != childMetrics.end(); lIt++) {
    const IntegerMetric_t& lMetric = **lIt;
    BOOST_CHECK_LE(lTimeBeforeLastUpdate.steady, lMetric.getUpdateTime());
    BOOST_CHECK_LE(lTimeBeforeLastUpdate.steady, lMetric.getSnapshot().getUpdateTimestamp().steady);
    BOOST_CHECK_LE(lTimeBeforeLastUpdate.system, lMetric.getSnapshot().getUpdateTimestamp().system);
    BOOST_CHECK_EQUAL(lMetric.getSnapshot().isValueKnown(), true);
    BOOST_CHECK_THROW(lMetric.getSnapshot().getValue<bool>(), MetricValueFailedCast);
  }
  BOOST_CHECK_EQUAL(childMetricA.getSnapshot().getValue<IntegerMetricDataType_t>(), 0);
  BOOST_CHECK_EQUAL(childMetricB.getSnapshot().getValue<IntegerMetricDataType_t>(), 42);

  // But values of grandchild metrics should still be unknown
  for (auto lIt = grandchildMetrics.begin(); lIt != grandchildMetrics.end(); lIt++) {
    const IntegerMetric_t& lMetric = **lIt;
    BOOST_CHECK_EQUAL(lMetric.getUpdateTime(), SteadyTimePoint_t());
    BOOST_CHECK_EQUAL(lMetric.getSnapshot().getUpdateTimestamp().steady, SteadyTimePoint_t());
    BOOST_CHECK_EQUAL(lMetric.getSnapshot().getUpdateTimestamp().system, SystemTimePoint_t());
    BOOST_CHECK_EQUAL(lMetric.getSnapshot().isValueKnown(), false);
    BOOST_CHECK_THROW(lMetric.getSnapshot().getValue<IntegerMetricDataType_t>(), MetricValueNotKnown);
    BOOST_CHECK_THROW(lMetric.getSnapshot().getValue<bool>(), MetricValueNotKnown);
  }

  // 2) Now update values of grandchild metrics (but leave children as they are)
  childObj.setNextMetricValues( std::map<std::string, IntegerMetricDataType_t> {{"metricA", 99}, {"metricB", -100}});
  masterObj.setNextMetricValues( std::map<std::string, IntegerMetricDataType_t> {});
  lTimeBeforeLastUpdate = TimePoint::now();
  childObj.updateMetrics( MetricUpdateGuard(childObj));

  // Child metrics should still have their previous values
  for (auto lIt = childMetrics.begin(); lIt != childMetrics.end(); lIt++) {
    const IntegerMetric_t& lMetric = **lIt;
    BOOST_CHECK_GT(lTimeBeforeLastUpdate.steady, lMetric.getUpdateTime());
    BOOST_CHECK_GT(lTimeBeforeLastUpdate.steady, lMetric.getSnapshot().getUpdateTimestamp().steady);
    BOOST_CHECK_GT(lTimeBeforeLastUpdate.system, lMetric.getSnapshot().getUpdateTimestamp().system);
  }
  // Grandchild metrics should been updated during "childObj.updateMetrics()" invocation
  for (auto lIt = grandchildMetrics.begin(); lIt != grandchildMetrics.end(); lIt++) {
    const IntegerMetric_t& lMetric = **lIt;
    BOOST_CHECK_LE(lTimeBeforeLastUpdate.steady, lMetric.getUpdateTime());
    BOOST_CHECK_LE(lTimeBeforeLastUpdate.steady, lMetric.getSnapshot().getUpdateTimestamp().steady);
    BOOST_CHECK_LE(lTimeBeforeLastUpdate.system, lMetric.getSnapshot().getUpdateTimestamp().system);
  }
  // Values of all metrics should be known
  for (auto lIt = allMetrics.begin(); lIt != allMetrics.end(); lIt++) {
    const IntegerMetric_t& lMetric = **lIt;
    BOOST_CHECK_EQUAL(lMetric.getSnapshot().isValueKnown(), true);
    BOOST_CHECK_THROW(lMetric.getSnapshot().getValue<bool>(), MetricValueFailedCast);
  }
  BOOST_CHECK_EQUAL(childMetricA.getSnapshot().getValue<IntegerMetricDataType_t>(), 0);
  BOOST_CHECK_EQUAL(childMetricB.getSnapshot().getValue<IntegerMetricDataType_t>(), 42);
  BOOST_CHECK_EQUAL(grandchildMetricA.getSnapshot().getValue<IntegerMetricDataType_t>(), 99);
  BOOST_CHECK_EQUAL(grandchildMetricB.getSnapshot().getValue<IntegerMetricDataType_t>(), -100);
}


void checkUpdateMetricsResetsForgottenMetricsToUnknown(bool aThrowInRetrieveMetrics)
{
  MonitorableObjectTestSetup lSetup;
  typedef MonitorableObjectTestSetup::IntegerMetric_t IntegerMetric_t;
  typedef MonitorableObjectTestSetup::IntegerMetricDataType_t IntegerMetricDataType_t;

  // Check assumption that all metric start off being unknown ...
  for (auto lIt = lSetup.allMetrics.begin(); lIt != lSetup.allMetrics.end(); lIt++)  {
    const IntegerMetric_t& lMetric = **lIt;
    BOOST_REQUIRE_EQUAL(lMetric.getUpdateTime(), SteadyTimePoint_t());
    BOOST_REQUIRE_EQUAL(lMetric.getSnapshot().getUpdateTimestamp().steady, SteadyTimePoint_t());
    BOOST_REQUIRE_EQUAL(lMetric.getSnapshot().getUpdateTimestamp().system, SystemTimePoint_t());
    BOOST_REQUIRE_EQUAL(lMetric.getSnapshot().isValueKnown(), false);
    BOOST_REQUIRE_THROW(lMetric.getSnapshot().getValue<IntegerMetricDataType_t>(), MetricValueNotKnown);
  }

  // 1) Update values of one child metric, leaving other one untouched (but not grandchildren yet)
  lSetup.masterObj.setNextMetricValues( std::map<std::string, IntegerMetricDataType_t> {{"metricA", 0}});
  TimePoint lTimeBeforeLastUpdate = TimePoint::now();
  lSetup.masterObj.throwAfterRetrievingMetricValues(aThrowInRetrieveMetrics);
  lSetup.masterObj.updateMetrics( MetricUpdateGuard(lSetup.masterObj) );

  // Values of child metric A shown now be known, B still unknown, but both with updated timestamps
  for (auto lIt = lSetup.childMetrics.begin(); lIt != lSetup.childMetrics.end(); lIt++) {
    const IntegerMetric_t& lMetric = **lIt;
    BOOST_CHECK_LE(lTimeBeforeLastUpdate.steady, lMetric.getUpdateTime());
    BOOST_CHECK_LE(lTimeBeforeLastUpdate.steady, lMetric.getSnapshot().getUpdateTimestamp().steady);
    BOOST_CHECK_LE(lTimeBeforeLastUpdate.system, lMetric.getSnapshot().getUpdateTimestamp().system);
  }
  BOOST_CHECK_EQUAL(lSetup.childMetricA.getSnapshot().isValueKnown(), true);
  BOOST_CHECK_EQUAL(lSetup.childMetricA.getSnapshot().getValue<IntegerMetricDataType_t>(), 0);
  BOOST_CHECK_THROW(lSetup.childMetricA.getSnapshot().getValue<bool>(), MetricValueFailedCast);
  BOOST_CHECK_EQUAL(lSetup.childMetricB.getSnapshot().isValueKnown(), false);
  BOOST_CHECK_THROW(lSetup.childMetricB.getSnapshot().getValue<IntegerMetricDataType_t>(), MetricValueNotKnown);
  BOOST_CHECK_THROW(lSetup.childMetricB.getSnapshot().getValue<bool>(), MetricValueNotKnown);

  // But values of grandchild metrics should still be unknown, with unset timestamps
  for (auto lIt = lSetup.grandchildMetrics.begin(); lIt != lSetup.grandchildMetrics.end(); lIt++) {
    const IntegerMetric_t& lMetric = **lIt;
    BOOST_CHECK_EQUAL(lMetric.getUpdateTime(), SteadyTimePoint_t());
    BOOST_CHECK_EQUAL(lMetric.getSnapshot().getUpdateTimestamp().steady, SteadyTimePoint_t());
    BOOST_CHECK_EQUAL(lMetric.getSnapshot().getUpdateTimestamp().system, SystemTimePoint_t());
    BOOST_CHECK_EQUAL(lMetric.getSnapshot().isValueKnown(), false);
    BOOST_CHECK_THROW(lMetric.getSnapshot().getValue<IntegerMetricDataType_t>(), MetricValueNotKnown);
    BOOST_CHECK_THROW(lMetric.getSnapshot().getValue<bool>(), MetricValueNotKnown);
  }

  // 2) Now update values of one of grandchild metrics (but leave children as they are)
  lSetup.childObj.setNextMetricValues( std::map<std::string, IntegerMetricDataType_t> {{"metricB", -100}});
  lSetup.masterObj.setNextMetricValues( std::map<std::string, IntegerMetricDataType_t> {});
  lTimeBeforeLastUpdate = TimePoint::now();
  lSetup.childObj.throwAfterRetrievingMetricValues(aThrowInRetrieveMetrics);
  lSetup.childObj.updateMetrics( MetricUpdateGuard(lSetup.childObj));

  // Child metrics should still have their previous values
  for (auto lIt = lSetup.childMetrics.begin(); lIt != lSetup.childMetrics.end(); lIt++) {
    const IntegerMetric_t& lMetric = **lIt;
    BOOST_CHECK_GT(lTimeBeforeLastUpdate.steady, lMetric.getUpdateTime());
    BOOST_CHECK_GT(lTimeBeforeLastUpdate.steady, lMetric.getSnapshot().getUpdateTimestamp().steady);
    BOOST_CHECK_GT(lTimeBeforeLastUpdate.system, lMetric.getSnapshot().getUpdateTimestamp().system);
  }
  // Grandchild metric timestamps should been updated during "childObj.updateMetrics()" invocation
  for (auto lIt = lSetup.grandchildMetrics.begin(); lIt != lSetup.grandchildMetrics.end(); lIt++) {
    const IntegerMetric_t& lMetric = **lIt;
    BOOST_CHECK_LE(lTimeBeforeLastUpdate.steady, lMetric.getUpdateTime());
    BOOST_CHECK_LE(lTimeBeforeLastUpdate.steady, lMetric.getSnapshot().getUpdateTimestamp().steady);
    BOOST_CHECK_LE(lTimeBeforeLastUpdate.system, lMetric.getSnapshot().getUpdateTimestamp().system);
  }
  // Values of childMetricA & grandchildMetrcB should be known; others still unknown
  std::vector<const IntegerMetric_t*> lMetricsWithKnownValues {&lSetup.childMetricA, &lSetup.grandchildMetricB};
  std::vector<const IntegerMetric_t*> lMetricsWithUnknownValues {&lSetup.childMetricB, &lSetup.grandchildMetricA};
  for (auto lIt = lMetricsWithKnownValues.begin(); lIt != lMetricsWithKnownValues.end(); lIt++) {
    const IntegerMetric_t& lMetric = **lIt;
    BOOST_CHECK_EQUAL(lMetric.getSnapshot().isValueKnown(), true);
    BOOST_CHECK_THROW(lMetric.getSnapshot().getValue<bool>(), MetricValueFailedCast);
  }
  BOOST_CHECK_EQUAL(lSetup.childMetricA.getSnapshot().getValue<IntegerMetricDataType_t>(), 0);
  BOOST_CHECK_EQUAL(lSetup.grandchildMetricB.getSnapshot().getValue<IntegerMetricDataType_t>(), -100);

  for (auto lIt = lMetricsWithUnknownValues.begin(); lIt != lMetricsWithUnknownValues.end(); lIt++) {
    const IntegerMetric_t& lMetric = **lIt;
    BOOST_CHECK_EQUAL(lMetric.getSnapshot().isValueKnown(), false);
    BOOST_CHECK_THROW(lMetric.getSnapshot().getValue<IntegerMetricDataType_t>(), MetricValueNotKnown);
    BOOST_CHECK_THROW(lMetric.getSnapshot().getValue<bool>(), MetricValueNotKnown);
  }
}


BOOST_FIXTURE_TEST_CASE(UpdateMetrics_MetricForgottenNoThrow, MonitorableObjectTestSetup)
{
  checkUpdateMetricsResetsForgottenMetricsToUnknown(false);
}


BOOST_FIXTURE_TEST_CASE(UpdateMetrics_MetricForgottenWithThrow, MonitorableObjectTestSetup)
{
  checkUpdateMetricsResetsForgottenMetricsToUnknown(true);
}


/*   MetricUpdateGuard & monitorable object's status   */

//BOOST_AUTO_TEST_CASE(MetricWriteGuardRequiresStatus)
//{
//  // 1) DummyMonitorableObject: Doesn't have a status member variable (inheriting from AbstractMonitorableObject);
//  // ... hence: MetricWriteGuard CTOR should throw; updateMetrics should throw before calling retrieveMetricValues
//  DummyMonitorableObject obj;
//  obj.registerMetric<DummyMonitorableObject::IntegerMetricDataType_t>("aMetric");
//  // 1a) Require that assumptions are correct (to prevent false positives later)
//  BOOST_REQUIRE_EQUAL(timerisset(&obj.getMetric("aMetric").getUpdateTimestamp()), false);
//  //TODO: same check for each metric
//  // 1b) Try to create guard or update metrics
//  BOOST_CHECK_THROW( MetricUpdateGuard lMetricWriteGuard(obj), std::runtime_error);
//  BOOST_CHECK_THROW( obj.updateMetrics(), std::runtime_error);
//  // 1c) Check that retriveMetricValues() hasn't been called
//  BOOST_CHECK_EQUAL(timerisset(&obj.getMetric("aMetric").getUpdateTimestamp()), false);
//
//
//  // 2) DummyMasterMonitorableObject does have a status member variable (inheriting from AbstractMonitorableObject);
//  // ... hence, all should be good
//  DummyMasterMonitorableObject masterObj;
//  masterObj.registerMetric<DummyMonitorableObject::IntegerMetricDataType_t>("aMetric");
//  // 2a) Require that assumptions are correct (to prevent false positives later)
//  BOOST_REQUIRE_EQUAL(timerisset(&masterObj.getMetric("aMetric").getUpdateTimestamp()), false);
//  // 2b) Try to create guard or update metrics - should succeed without throwing
//  BOOST_CHECK_NO_THROW( MetricUpdateGuard lMetricWriteGuard2(masterObj) );
//  BOOST_CHECK_NO_THROW( masterObj.updateMetrics() );
//  // 2c) Check that retriveMetricValues() has been called
//  BOOST_CHECK_EQUAL(timerisset(&masterObj.getMetric("aMetric").getUpdateTimestamp()), true);
//}
//
//
//BOOST_AUTO_TEST_CASE(IncorrectMetricWriteGuardCaseA)
//{
//  // UpdateMetrics should throw if monitorable object is given a MetricWriteGuard for a different object
//  // Case A: Using write guard from one master with another master
//
//  DummyMasterMonitorableObject lMasterObj1, lMasterObj2;
//  lMasterObj1.registerMetric<DummyMonitorableObject::IntegerMetricDataType_t>("aMetric");
//  lMasterObj2.registerMetric<DummyMonitorableObject::IntegerMetricDataType_t>("aMetric");
//  MetricUpdateGuard lWriteGuard(lMasterObj1);
//
//  // 1) Guard should work fine with lMasterObj1
//  BOOST_REQUIRE_EQUAL(timerisset(&lMasterObj1.getMetric("aMetric").getUpdateTimestamp()), false);
//  BOOST_CHECK_NO_THROW( lMasterObj1.updateMetrics(lWriteGuard) );
//  BOOST_CHECK_EQUAL(timerisset(&lMasterObj1.getMetric("aMetric").getUpdateTimestamp()), true);
//
//  // 2) Using object 1's write guard with object 2: updateMetrics should throw before calling retrieveMetricValues
//  BOOST_REQUIRE_EQUAL(timerisset(&lMasterObj2.getMetric("aMetric").getUpdateTimestamp()), false);
//  BOOST_CHECK_THROW( lMasterObj2.updateMetrics(lWriteGuard), std::runtime_error );
//  BOOST_CHECK_EQUAL(timerisset(&lMasterObj2.getMetric("aMetric").getUpdateTimestamp()), false);
//}
//
//
//BOOST_AUTO_TEST_CASE(IncorrectMetricWriteGuardCaseB)
//{
//  // UpdateMetrics should throw if monitorable object is given a MetricWriteGuard for a different object
//  // Case B: Using write guard with a DummyMonitorableObject whose status pointer hasn't been set yet
//
//  DummyMasterMonitorableObject lMasterObj;
//  lMasterObj.registerMetric<DummyMonitorableObject::IntegerMetricDataType_t>("aMetric");
//  MetricUpdateGuard lWriteGuard(lMasterObj);
//
//  // 1) Guard should work fine with lMasterObj
//  BOOST_REQUIRE_EQUAL(timerisset(&lMasterObj.getMetric("aMetric").getUpdateTimestamp()), false);
//  BOOST_CHECK_NO_THROW( lMasterObj.updateMetrics(lWriteGuard) );
//  BOOST_CHECK_EQUAL(timerisset(&lMasterObj.getMetric("aMetric").getUpdateTimestamp()), true);
//
//  // 2) Using master object's write guard with disconnected object : updateMetrics should throw before calling retrieveMetricValues
//  DummyMonitorableObject lObj;
//  lObj.registerMetric<DummyMonitorableObject::IntegerMetricDataType_t>("aMetric");
//  BOOST_REQUIRE_EQUAL(timerisset(&lObj.getMetric("aMetric").getUpdateTimestamp()), false);
//  BOOST_CHECK_THROW( lObj.updateMetrics(lWriteGuard), std::runtime_error );
//  BOOST_CHECK_EQUAL(timerisset(&lObj.getMetric("aMetric").getUpdateTimestamp()), false);
//}
//
//
//BOOST_AUTO_TEST_CASE(SettingStatusPointer_AddChildFirst)
//{
//  // Check that the addMonitorable method sets the child and grandchild's status pointers correctly
//  //  - Master
//  //    - Child
//  //      - Grandchild
//  // Case A: Add child to master; then add grandchild to child
//
//  DummyMasterMonitorableObject lMaster;
//  MetricUpdateGuard lWriteGuard(lMaster);
//  DummyMonitorableObject& lChild = dynamic_cast<DummyMonitorableObject&>(lMaster.addMonitorable( new DummyMonitorableObject() ));
//  DummyMonitorableObject& lGrandChild = dynamic_cast<DummyMonitorableObject&>(lChild.addMonitorable( new DummyMonitorableObject() ));
//
//  const std::vector<DummyMonitorableObject*> lObjVec = {&lMaster, &lChild, &lGrandChild};
//  std::vector<DummyMonitorableObject*>::const_iterator lObjIt;
//
//  for(lObjIt=lObjVec.begin(); lObjIt!=lObjVec.end(); lObjIt++)
//  {
//    DummyMonitorableObject& lObj = **lObjIt;
//    lObj.registerMetric<DummyMonitorableObject::IntegerMetricDataType_t>("aMetric");
//    BOOST_TEST_MESSAGE("Object:" << lObj.getPath());
//    BOOST_REQUIRE_EQUAL(timerisset(&lObj.getMetric("aMetric").getUpdateTimestamp()), false);
//    BOOST_CHECK_NO_THROW( lObj.updateMetrics(lWriteGuard) );
//    BOOST_CHECK_EQUAL(timerisset(&lObj.getMetric("aMetric").getUpdateTimestamp()), true);
//  }
//}
//
//
//BOOST_AUTO_TEST_CASE(SettingStatusPointer_AddGrandChildFirst)
//{
//  // Check that the addMonitorable method sets the child and grandchild's status pointers correctly
//  //  - Master
//  //    - Child
//  //      - Grandchild
//  // Case B: Add grandchild to child; then add child to master
//
//  DummyMasterMonitorableObject lMaster;
//  MetricUpdateGuard lWriteGuard(lMaster);
//  DummyMonitorableObject* lChild = new DummyMonitorableObject();
//  DummyMonitorableObject& lGrandChild = dynamic_cast<DummyMonitorableObject&>(lChild->addMonitorable( new DummyMonitorableObject() ));
//  lMaster.addMonitorable(lChild);
//
//  const std::vector<DummyMonitorableObject*> lObjVec = {&lMaster, lChild, &lGrandChild};
//  std::vector<DummyMonitorableObject*>::const_iterator lObjIt;
//
//  for(lObjIt=lObjVec.begin(); lObjIt!=lObjVec.end(); lObjIt++)
//  {
//    DummyMonitorableObject& lObj = **lObjIt;
//    lObj.registerMetric<DummyMonitorableObject::IntegerMetricDataType_t>("aMetric");
//    BOOST_TEST_MESSAGE("Object:" << lObj.getPath());
//    BOOST_REQUIRE_EQUAL(timerisset(&lObj.getMetric("aMetric").getUpdateTimestamp()), false);
//    BOOST_CHECK_NO_THROW( lObj.updateMetrics(lWriteGuard) );
//    BOOST_CHECK_EQUAL(timerisset(&lObj.getMetric("aMetric").getUpdateTimestamp()), true);
//  }
//}


/*

BOOST_AUTO_TEST_CASE(GetMetric)
{
  DummyMasterMonitorableObject m;

  // Check that getters throw if metric of ID doesn't exist
  BOOST_CHECK_THROW(m.getMetric("non_existant_metric"), MetricNotFoundInMonitorableObject);
  BOOST_CHECK_THROW(m.getMetric<uint32_t>("non_existant_metric"), MetricNotFoundInMonitorableObject);

  // Access metric that actually exists
  BOOST_CHECK_EQUAL(&m.getMetric("DummyNonCriticalInteger"), (swatch::core::AbstractMetric*) &m.getMetric<int>("DummyNonCriticalInteger"));

  // Check that templated getters throws if cast to incorrect data type
  BOOST_CHECK_THROW(m.getMetric<bool>("DummyNonCriticalInteger"), ObjectFailedCast);
}


BOOST_AUTO_TEST_CASE(GoodMonitorableObject)
{
  DummyMasterMonitorableObject m;
  m.updateMetrics();
  MetricSnapshot ms = m.getMetric("DummyCriticalInteger").getSnapshot();
  MetricSnapshot ms2 = m.getMetric("DummyNonCriticalInteger").getSnapshot();
  BOOST_CHECK_EQUAL(ms.getValue<DummyMonitorableObject::IntegerMetricDataType_t>(), 1);
  BOOST_CHECK_EQUAL(ms2.getValue<DummyMonitorableObject::IntegerMetricDataType_t>(), 2);
  BOOST_CHECK_EQUAL(ms.getStatusFlag(), swatch::core::StatusFlag::kGood);
  BOOST_CHECK_EQUAL(ms2.getStatusFlag(), swatch::core::StatusFlag::kGood);
  BOOST_CHECK_EQUAL(m.getStatusFlag(), swatch::core::StatusFlag::kGood);

  // After change monitorable object to 'disabled', getStatusFlag() should return kNoLimit, regardless of metric status
  m.setMonitoringStatus(swatch::core::monitoring::kDisabled);
  BOOST_REQUIRE_EQUAL(m.getMonitoringStatus(), swatch::core::monitoring::kDisabled);
  BOOST_CHECK_EQUAL(m.getStatusFlag(), swatch::core::kNoLimit);
}



BOOST_AUTO_TEST_CASE(CriticalFailure)
{
  DummyMasterMonitorableObject m;
  m.updateMetrics();
  // set a value outside the range
  m.setCriticalMetric(-232);
  MetricSnapshot ms = m.getMetric("DummyCriticalInteger").getSnapshot();
  MetricSnapshot ms2 = m.getMetric("DummyNonCriticalInteger").getSnapshot();
  BOOST_CHECK_EQUAL(ms.getValue<DummyMonitorableObject::IntegerMetricDataType_t>(), -232);
  BOOST_CHECK_EQUAL(ms2.getValue<DummyMonitorableObject::IntegerMetricDataType_t>(), 2);
  BOOST_CHECK_EQUAL(ms.getStatusFlag(), swatch::core::StatusFlag::kError);
  BOOST_CHECK_EQUAL(ms2.getStatusFlag(), swatch::core::StatusFlag::kGood);
  BOOST_CHECK_EQUAL(m.getStatusFlag(), swatch::core::StatusFlag::kError);

  // After change monitorable object to 'disabled', getStatusFlag() should return kNoLimit, regardless of metric status
  m.setMonitoringStatus(swatch::core::monitoring::kDisabled);
  BOOST_REQUIRE_EQUAL(m.getMonitoringStatus(), swatch::core::monitoring::kDisabled);
  BOOST_CHECK_EQUAL(m.getStatusFlag(), swatch::core::kNoLimit);
}

BOOST_AUTO_TEST_CASE(NonCriticalFailure)
{
  DummyMasterMonitorableObject m;
  m.updateMetrics();
  // set a value outside the range
  m.setNonCriticalMetric(-232);
  MetricSnapshot ms = m.getMetric("DummyCriticalInteger").getSnapshot();
  MetricSnapshot ms2 = m.getMetric("DummyNonCriticalInteger").getSnapshot();
  BOOST_CHECK_EQUAL(ms2.getMonitoringStatus(), monitoring::Status::kNonCritical);
  BOOST_CHECK_EQUAL(ms.getValue<DummyMonitorableObject::IntegerMetricDataType_t>(), 1);
  BOOST_CHECK_EQUAL(ms2.getValue<DummyMonitorableObject::IntegerMetricDataType_t>(), -232);
  BOOST_CHECK_EQUAL(ms.getStatusFlag(), swatch::core::StatusFlag::kGood);
  // non-critical metric fails
  BOOST_CHECK_EQUAL(ms2.getStatusFlag(), swatch::core::StatusFlag::kError);
  // but the MonitoringObject status should be still good
  BOOST_CHECK_EQUAL(m.getStatusFlag(), swatch::core::StatusFlag::kGood);
}

BOOST_AUTO_TEST_CASE(DisabledMetricFailure)
{
  DummyMasterMonitorableObject m;
  m.updateMetrics();
  // set a value outside the range
  m.setDisabledMetric(-232);
  MetricSnapshot ms = m.getMetric("DummyDisabledInteger").getSnapshot();
  BOOST_CHECK_EQUAL(ms.getMonitoringStatus(), monitoring::Status::kDisabled);
  BOOST_CHECK_EQUAL(ms.getValue<DummyMonitorableObject::IntegerMetricDataType_t>(), -232);
  // nothing changes for disabled Metric
  BOOST_CHECK_EQUAL(ms.getStatusFlag(), swatch::core::StatusFlag::kNoLimit);
  // and no effect on MonitoringObject either
  BOOST_CHECK_EQUAL(m.getStatusFlag(), swatch::core::StatusFlag::kGood);
}


BOOST_AUTO_TEST_CASE(DisabledMetricFailureNoUnknown)
{
  DummyMasterMonitorableObject m;
  m.updateMetrics();
  // at this point the time of last update should be 0
  // normally MonitoringObject will attempt to set the value
  // to unknown, but that should not be the case for disabled metrics
  MetricSnapshot ms = m.getMetric("DummyDisabledInteger").getSnapshot();
  BOOST_CHECK_EQUAL(ms.getMonitoringStatus(), monitoring::Status::kDisabled);
  BOOST_CHECK_EQUAL(ms.getStatusFlag(), swatch::core::StatusFlag::kNoLimit);
  BOOST_CHECK_EQUAL(m.getStatusFlag(), swatch::core::StatusFlag::kGood);
}

 */

BOOST_AUTO_TEST_SUITE_END() // MonitorableObjectTestSuite
} /* namespace test */
} /* namespace core */
} /* namespace swatch */

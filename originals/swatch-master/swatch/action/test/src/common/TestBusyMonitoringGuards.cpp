#include <boost/test/unit_test.hpp>


// Standard headers
#include <stdexcept>

// boost headers
#include "boost/chrono/chrono_io.hpp"
#include "boost/scoped_ptr.hpp"

// SWATCH headers
#include "swatch/action/test/DummyActionableObject.hpp"
#include "swatch/action/test/DummyCommand.hpp"



namespace swatch {
namespace action {
namespace test {


struct BusyMonitoringGuardTestSetup {
  BusyMonitoringGuardTestSetup();
  ~BusyMonitoringGuardTestSetup();

  boost::shared_ptr<DummyActionableObject> obj;
  Command& waitingCmd;
  Command& anotherCmd;
  DummyActionableObject::MonChild& child;
};


BusyMonitoringGuardTestSetup::BusyMonitoringGuardTestSetup() :
  obj(new DummyActionableObject("dummyObj"), ActionableObject::Deleter()),
  waitingCmd(obj->registerCommand<DummyActionableObject::WaitCommand>("waitCommand")),
  anotherCmd(obj->registerCommand<DummyCommand>("anotherCommand")),
  child(obj->addMonitorable(new DummyActionableObject::MonChild("monitorableChild", *obj)))
{
}

BusyMonitoringGuardTestSetup::~BusyMonitoringGuardTestSetup()
{
}


class UpdateMetrics {
public:
  UpdateMetrics(core::MonitorableObject& aObj);
  ~UpdateMetrics();

  void operator()();

private:
  core::MonitorableObject& mObj;
};


UpdateMetrics::UpdateMetrics(core::MonitorableObject& aObj) :
  mObj(aObj)
{
}

UpdateMetrics::~UpdateMetrics() {}

void UpdateMetrics::operator()()
{
  core::MetricUpdateGuard lGuard(mObj);
  mObj.updateMetrics(lGuard);
}


class ReadMetrics {
public:
  ReadMetrics(DummyActionableObject& aObj);
  ~ReadMetrics();

  void operator()();

  boost::scoped_ptr<const core::MetricSnapshot> dummyMetricSnapshot;

private:
  DummyActionableObject& mObj;
};

ReadMetrics::ReadMetrics(DummyActionableObject& aObj) :
  mObj(aObj)
{
}

ReadMetrics::~ReadMetrics() {}

void ReadMetrics::operator()()
{
  std::cout << "ReadMetrics::operator()() -- ENTERING" << std::endl;
  core::MetricReadGuard lGuard(mObj);
  std::cout << "  ... mid-way ..." << std::endl;
  dummyMetricSnapshot.reset(new core::MetricSnapshot(mObj.getDummyMetric().getSnapshot()));
  std::cout << "ReadMetrics::operator()() -- EXITING" << std::endl;
}


class RunCommand {
public:
  RunCommand(Command& aCmd, const core::ReadOnlyXParameterSet& aParams);
  ~RunCommand();

  void operator()();

private:
  Command& mCmd;
  core::ReadOnlyXParameterSet mParams;
};


RunCommand::RunCommand(Command& aCmd, const core::ReadOnlyXParameterSet& aParams) :
  mCmd(aCmd),
  mParams(aParams)
{
}

RunCommand::~RunCommand()
{
}

void RunCommand::operator ()()
{
  mCmd.exec(mParams, false);
}



BOOST_AUTO_TEST_SUITE( BusyMonitoringGuardsTestSuite )


BOOST_FIXTURE_TEST_CASE(TestBusyThenMetricWriteGuard, BusyMonitoringGuardTestSetup)
{
  // 0) Check that metrics are "unknown" and command status "initialised" before proceed with tests.
  core::MetricSnapshot lMetricSnapshot = obj->getDummyMetric().getSnapshot();
  BOOST_REQUIRE_EQUAL(lMetricSnapshot.getStatusFlag(), swatch::core::kUnknown);
  BOOST_REQUIRE_EQUAL(lMetricSnapshot.getUpdateTimestamp().steady, core::SteadyTimePoint_t());
  BOOST_REQUIRE_EQUAL(lMetricSnapshot.getUpdateTimestamp().system, core::SystemTimePoint_t());
  BOOST_REQUIRE_EQUAL(waitingCmd.getState(), Functionoid::kInitial);

  // 1a) Run command in another thread (via thread pool)
  obj->pleaseWait();
  waitingCmd.exec(core::ReadOnlyXParameterSet());
  // 1b) Spawn another thread that starts updating metrics
  UpdateMetrics lUpdateMetricsFunctor(*obj);
  boost::thread lUpdateMetricsThread(lUpdateMetricsFunctor);

  // 2) Check that busy guard has updated the actionable object's status
  ActionableObject::Status_t lObjStatus = obj->getStatus();
  BOOST_REQUIRE_EQUAL(lObjStatus.isRunning(), true);
  BOOST_REQUIRE_NE(lObjStatus.getRunningActions().size(), size_t(0));
  BOOST_REQUIRE_EQUAL(lObjStatus.isUpdatingMetrics(), false);
  BOOST_REQUIRE_NE(waitingCmd.getState(), Functionoid::kInitial);

  // 3) Sleep until command is waiting; then check that metric still hasn't been updated
  while ( obj->getWaitingThread() == boost::thread::id() ) {
    boost::this_thread::sleep_for(boost::chrono::milliseconds(2));
  }
  boost::this_thread::sleep_for(boost::chrono::milliseconds(10));
  BOOST_REQUIRE_EQUAL(waitingCmd.getState(), Functionoid::kRunning);
  lMetricSnapshot = obj->getDummyMetric().getSnapshot();
  BOOST_REQUIRE_EQUAL(lMetricSnapshot.getStatusFlag(), swatch::core::kUnknown);
  BOOST_REQUIRE_EQUAL(lMetricSnapshot.getUpdateTimestamp().steady, core::SteadyTimePoint_t());
  BOOST_REQUIRE_EQUAL(lMetricSnapshot.getUpdateTimestamp().system, core::SystemTimePoint_t());

  // 4) Let threads continue from command into monitoring update; check that metric's value has been updated
  obj->pleaseContinue();
  while ( obj->getWaitingThread() != lUpdateMetricsThread.get_id() ) {
    boost::this_thread::sleep_for(boost::chrono::milliseconds(2));
  }
  BOOST_REQUIRE_EQUAL(waitingCmd.getState(), Functionoid::kDone);
  lMetricSnapshot = obj->getDummyMetric().getSnapshot();
  BOOST_REQUIRE_EQUAL(lMetricSnapshot.getStatusFlag(), swatch::core::kNoLimit);
  BOOST_REQUIRE_NE(lMetricSnapshot.getUpdateTimestamp().steady, core::SteadyTimePoint_t());
  BOOST_REQUIRE_NE(lMetricSnapshot.getUpdateTimestamp().system, core::SystemTimePoint_t());
  lObjStatus = obj->getStatus();
  BOOST_REQUIRE_EQUAL(lObjStatus.isRunning(), false);
  BOOST_REQUIRE_EQUAL(lObjStatus.getRunningActions().size(), size_t(0));
  BOOST_REQUIRE_EQUAL(lObjStatus.isUpdatingMetrics(), true);

  // 5) Let threads continue out of monitoring update; check that object's status has been updated
  obj->pleaseContinue();
  lUpdateMetricsThread.join();
  BOOST_CHECK_EQUAL(lObjStatus.isRunning(), false);
  BOOST_CHECK_EQUAL(lObjStatus.getRunningActions().size(), size_t(0));
  BOOST_CHECK_EQUAL(lObjStatus.isUpdatingMetrics(), true);
}


BOOST_FIXTURE_TEST_CASE(TestMetricWriteThenBusyGuard, BusyMonitoringGuardTestSetup)
{
  // 0) Check that metrics are "unknown" and command status "initialised" before proceed with tests.
  core::MetricSnapshot lMetricSnapshot = obj->getDummyMetric().getSnapshot();
  BOOST_REQUIRE_EQUAL(lMetricSnapshot.getStatusFlag(), swatch::core::kUnknown);
  BOOST_REQUIRE_EQUAL(lMetricSnapshot.getUpdateTimestamp().steady, core::SteadyTimePoint_t());
  BOOST_REQUIRE_EQUAL(lMetricSnapshot.getUpdateTimestamp().system, core::SystemTimePoint_t());
  BOOST_REQUIRE_EQUAL(waitingCmd.getState(), Functionoid::kInitial);

  // 1) Spawn a thread that will update metrics
  obj->pleaseWait();
  UpdateMetrics lUpdateMetricsFunctor(*obj);
  boost::thread lUpdateMetricsThread(lUpdateMetricsFunctor);

  // 2) Sleep until updateMetrics is waiting; then spawn a thread that runs a command
  while ( obj->getWaitingThread() != lUpdateMetricsThread.get_id() ) {
    boost::this_thread::sleep_for( boost::chrono::milliseconds(2) );
  }
  lMetricSnapshot = obj->getDummyMetric().getSnapshot();
  BOOST_CHECK_NE(lMetricSnapshot.getStatusFlag(), swatch::core::kUnknown);
  BOOST_CHECK_NE(lMetricSnapshot.getUpdateTimestamp().steady, core::SteadyTimePoint_t());
  BOOST_CHECK_NE(lMetricSnapshot.getUpdateTimestamp().system, core::SystemTimePoint_t());
  BOOST_CHECK_EQUAL(waitingCmd.getState(), Functionoid::kInitial);
  ActionableObject::Status_t lObjStatus = obj->getStatus();
  BOOST_CHECK( lObjStatus.isUpdatingMetrics() );
  BOOST_REQUIRE( ! lObjStatus.isRunning() );
  BOOST_REQUIRE( lObjStatus.getRunningActions().empty() );

  RunCommand lRunCommandFunctor(waitingCmd, core::ReadOnlyXParameterSet());
  boost::thread lRunCommandThread(lRunCommandFunctor);

  // 3) Sleep until other thread is in BusyGuard CTOR; then check that can't have two commands waiting at same time
  while ( ! obj->getStatus().isActionWaitingToRun()  ) {
    boost::this_thread::sleep_for( boost::chrono::milliseconds(2) );
  }
  // BOOST_CHECK_THROW( anotherCmd.exec(core::ReadOnlyXParameterSet()), std::runtime_error );
  BOOST_CHECK_THROW( anotherCmd.exec(core::ReadOnlyXParameterSet()), swatch::core::RuntimeError );

  // 4) Let metric update thread continue, and sleep until command is in dummy's "wait" method;
  //    ... then check that actionable object's status is correct
  obj->pleaseContinue();
  lUpdateMetricsThread.join();
  while ( obj->getWaitingThread() != lRunCommandThread.get_id() ) {
    boost::this_thread::sleep_for(boost::chrono::milliseconds(2));
  }

  lObjStatus = obj->getStatus();
  BOOST_CHECK( ! lObjStatus.isUpdatingMetrics() );
  BOOST_REQUIRE( lObjStatus.isRunning() );
  BOOST_REQUIRE( ! lObjStatus.isActionWaitingToRun() );
  BOOST_REQUIRE_EQUAL( lObjStatus.getRunningActions().size(), size_t(1) );
  BOOST_REQUIRE_EQUAL( lObjStatus.getRunningActions().at(0), &waitingCmd );
  BOOST_REQUIRE_EQUAL( waitingCmd.getState(), Functionoid::kRunning );

  // 5) Let "run command" thread continue; once it's finished, check that actionable object's status is correct
  obj->pleaseContinue();
  lRunCommandThread.join();
  lObjStatus = obj->getStatus();
  BOOST_CHECK( ! lObjStatus.isUpdatingMetrics() );
  BOOST_CHECK( ! lObjStatus.isRunning() );
  BOOST_CHECK( ! lObjStatus.isActionWaitingToRun() );
  BOOST_CHECK( lObjStatus.getRunningActions().empty() );

  // 5) Finally, check that the other command has never been run
  BOOST_CHECK_EQUAL( anotherCmd.getState(), Functionoid::kInitial );
}


BOOST_FIXTURE_TEST_CASE(TestMetricUpdateThenMetricRead, BusyMonitoringGuardTestSetup)
{
  // 0) Check that metric's value is "unknown" .
  core::MetricSnapshot lMetricSnapshot = obj->getDummyMetric().getSnapshot();
  BOOST_REQUIRE_EQUAL(lMetricSnapshot.getStatusFlag(), swatch::core::kUnknown);
  BOOST_REQUIRE_EQUAL(lMetricSnapshot.getUpdateTimestamp().steady, core::SteadyTimePoint_t());
  BOOST_REQUIRE_EQUAL(lMetricSnapshot.getUpdateTimestamp().system, core::SystemTimePoint_t());

  // 1) Spawn a thread that will update metrics
  obj->pleaseWaitBeforeSettingMetricValue();
  UpdateMetrics lUpdateMetricsFunctor(*obj);
  boost::thread lUpdateMetricsThread(lUpdateMetricsFunctor);

  // 2) Sleep until updateMetrics is waiting; then spawn a thread that reads the dummy metric
  while ( obj->getWaitingThread() != lUpdateMetricsThread.get_id() ) {
    boost::this_thread::sleep_for( boost::chrono::milliseconds(2) );
  }
  lMetricSnapshot = obj->getDummyMetric().getSnapshot();
  BOOST_REQUIRE_EQUAL(lMetricSnapshot.getStatusFlag(), swatch::core::kUnknown);
  BOOST_REQUIRE_EQUAL(lMetricSnapshot.getUpdateTimestamp().steady, core::SteadyTimePoint_t());
  BOOST_REQUIRE_EQUAL(lMetricSnapshot.getUpdateTimestamp().system, core::SystemTimePoint_t());
  ActionableObject::Status_t lObjStatus = obj->getStatus();
  BOOST_REQUIRE( lObjStatus.isUpdatingMetrics() );

  ReadMetrics lMetricReader(*obj);
  boost::thread lReadMetricsThread(boost::ref(lMetricReader));

  // 3) Sleep for many milli-seconds & check that metric reader thread hasn't been allowed to read the metrics yet
  //   (temp solution to make sure that "metric reader" thread is in MetricReadGuard's CTOR)
  boost::this_thread::sleep_for( boost::chrono::milliseconds(200) );
  BOOST_CHECK( lMetricReader.dummyMetricSnapshot.get() == (const swatch::core::MetricSnapshot*) NULL );


  // 4) Let "metric update" thread continue, and join both "metric update" & "metric read" sthreads
  //    ... then check that metric values read by the "metric read" thread are not "kUnknown" - i.e. check that
  //    ... MetricReadGuard's CTOR didn't return until after updateMetrics returned.
  obj->pleaseContinue();
  lUpdateMetricsThread.join();
  lReadMetricsThread.join();

  lObjStatus = obj->getStatus();
  BOOST_CHECK( ! lObjStatus.isUpdatingMetrics() );

  BOOST_REQUIRE( lMetricReader.dummyMetricSnapshot.get() != (const swatch::core::MetricSnapshot*) NULL );
  BOOST_CHECK_NE(lMetricReader.dummyMetricSnapshot->getStatusFlag(), swatch::core::kUnknown);
  BOOST_CHECK_NE(lMetricReader.dummyMetricSnapshot->getUpdateTimestamp().steady, core::SteadyTimePoint_t());
  BOOST_CHECK_NE(lMetricReader.dummyMetricSnapshot->getUpdateTimestamp().system, core::SystemTimePoint_t());
}


BOOST_FIXTURE_TEST_CASE(TestMetricReadThenMetricUpdate, BusyMonitoringGuardTestSetup)
{
  // 0) Check that metric's value is "unknown".
  core::MetricSnapshot lMetricSnapshot = obj->getDummyMetric().getSnapshot();
  BOOST_REQUIRE_EQUAL(lMetricSnapshot.getStatusFlag(), swatch::core::kUnknown);
  BOOST_REQUIRE_EQUAL(lMetricSnapshot.getUpdateTimestamp().steady, core::SteadyTimePoint_t());
  BOOST_REQUIRE_EQUAL(lMetricSnapshot.getUpdateTimestamp().system, core::SystemTimePoint_t());

  // 1) Create a MetricReadGuard instance, and then spawn a "update metrics" thread
  UpdateMetrics lUpdateMetricsFunctor(*obj);
  boost::thread lUpdateMetricsThread;
  {
    core::MetricReadGuard lMetricReadGuard(*obj);
    obj->pleaseWait();
    lUpdateMetricsThread = boost::thread(lUpdateMetricsFunctor);

    // 2) Check that MetricUpdateGuard CTOR is in a blocking wait (i.e. retrieveMetricValues hasn't been called yet,
    //    even after a short sleep)
    boost::this_thread::sleep_for(boost::chrono::milliseconds(500));
    BOOST_CHECK( obj->getWaitingThread() == boost::thread::id() );
    BOOST_CHECK( ! obj->getStatus().isUpdatingMetrics() );
  }

  // 3) Now that MetricReadGuard has been destroyed, sleep until dummy object's updateMetrics is waiting
  //    ... Check that metric have been updated at this point, and actionable object's status is "updating metrics"
  while ( obj->getWaitingThread() != lUpdateMetricsThread.get_id() ) {
    boost::this_thread::sleep_for( boost::chrono::milliseconds(2) );
  }
  lMetricSnapshot = obj->getDummyMetric().getSnapshot();
  BOOST_CHECK_NE(lMetricSnapshot.getStatusFlag(), swatch::core::kUnknown);
  BOOST_CHECK_NE(lMetricSnapshot.getUpdateTimestamp().steady, core::SteadyTimePoint_t());
  BOOST_CHECK_NE(lMetricSnapshot.getUpdateTimestamp().system, core::SystemTimePoint_t());
  BOOST_CHECK( obj->getStatus().isUpdatingMetrics() );

  // 4) Let "metric update" thread continue, join that thread, and ...
  //    ... check that actionable object's status is no longer "updating metrics"
  obj->pleaseContinue();
  lUpdateMetricsThread.join();

  BOOST_CHECK( ! obj->getStatus().isUpdatingMetrics() );
}


BOOST_AUTO_TEST_SUITE_END() // BusyMonitoringGuardsTestSuite


} // end namespace test
} // end namespace action
} // end namespace swatch

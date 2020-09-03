/*
 * TestThreadPool.cpp
 *
 *  Created on: 28 May 2015
 *      Author: kreczko
 */
#include <boost/test/unit_test.hpp>

// boost headers
#include "boost/chrono/chrono_io.hpp"

// SWATCH headers
#include "swatch/core/ReadOnlyXParameterSet.hpp"
#include "swatch/action/ThreadPool.hpp"


using namespace swatch::core;

namespace swatch {
namespace action {
namespace test {


//! Class with an execution method that performs a blocking wait, until released by another thread calling the "stop waiting" method - useful for forcing
class WaitingActor {
public:
  struct DummyGuard {};

  WaitingActor();
  ~WaitingActor();

  //! Run exec method in thread pool
  void execInPool(const XParameterSet&);

  //! Blocks until the "stopWaiting" method is called (from some other thread); then returns immediately
  void exec(boost::shared_ptr<DummyGuard>, const XParameterSet&);

  //! Blocks until this actor is waiting (middle of exec method) in another thread, then returns; the exec method will stop waiting after the specified duration.
  template <class DurationType>
  void stopWaiting(const DurationType& ) const;

  bool isWaiting() const;

  size_t getExecutionCount() const;


private:
  typedef boost::chrono::steady_clock SteadyClock_t;
  mutable SteadyClock_t::time_point mTimeToStopWaiting;
  mutable boost::thread::id mWaitingThread;
  size_t mExecutionCount;
  mutable boost::mutex mMutex;
  mutable boost::condition_variable mConditionVar;
};


WaitingActor::WaitingActor() :
  mTimeToStopWaiting( SteadyClock_t::time_point::max() ),
  mExecutionCount(0)
{
}


WaitingActor::~WaitingActor()
{
}


void WaitingActor::execInPool(const XParameterSet& aParams)
{
  ThreadPool& pool = ThreadPool::getInstance();
  boost::shared_ptr<DummyGuard> lGuard(new DummyGuard);
  pool.addTask<WaitingActor,DummyGuard>(this, &WaitingActor::exec, lGuard, aParams);
}


void WaitingActor::exec(boost::shared_ptr<DummyGuard>, const XParameterSet&)
{
  boost::unique_lock<boost::mutex> lGuard(mMutex);

  // Wait until no other threads are waiting ...
  while (mWaitingThread != boost::thread::id()) {
    mConditionVar.wait(lGuard);
  }

  mExecutionCount++;
  mWaitingThread = boost::this_thread::get_id();
  mConditionVar.notify_all();

  // Now wait until we're told to stop waiting ...
  while (SteadyClock_t::now() < mTimeToStopWaiting) {
    if (mTimeToStopWaiting == SteadyClock_t::time_point::max())
      mConditionVar.wait(lGuard);
    else
      mConditionVar.wait_until(lGuard, mTimeToStopWaiting);
  }

  // Finally, reset mWaitingThread, and let other threads have their turn to wait ...
  mWaitingThread = boost::thread::id();
  mTimeToStopWaiting = SteadyClock_t::time_point::max();
  mConditionVar.notify_all();
}


template <class DurationType>
void WaitingActor::stopWaiting(const DurationType& aDuration) const
{
  boost::unique_lock<boost::mutex> lGuard(mMutex);

  // Wait until a thread is in middle of "exec" method ...
  while (mWaitingThread == boost::thread::id()) {
    mConditionVar.wait(lGuard);
  }

  // Then tell the thread to continue
  mTimeToStopWaiting = SteadyClock_t::now() + aDuration;
  mConditionVar.notify_all();
}


bool WaitingActor::isWaiting() const
{
  boost::unique_lock<boost::mutex> lGuard(mMutex);
  return (mWaitingThread != boost::thread::id());
}


size_t WaitingActor::getExecutionCount() const
{
  boost::unique_lock<boost::mutex> lGuard(mMutex);
  return mExecutionCount;
}



struct ThreadPoolSetup {
public:
  ThreadPoolSetup() {}
  ~ThreadPoolSetup() {}

  WaitingActor action1, action2, action3;
  ReadOnlyXParameterSet params;
};



BOOST_AUTO_TEST_SUITE( ThreadPoolTestSuite )
BOOST_FIXTURE_TEST_CASE(TolerantPool1, ThreadPoolSetup)
{
  // create thread pool with 2 threads
  // does not finish queue
  // does not forcefully terminate running commands
  ThreadPool::getInstance(2, false, false);
  action1.execInPool(params);
  action2.execInPool(params);
  action3.execInPool(params);

  // Short sleeps to ensure that actions have started execution in the pool ...
  while (!(action1.isWaiting() && action2.isWaiting())) {
    boost::this_thread::sleep_for(boost::chrono::milliseconds(1));
  }

  // Tell actions 1 and 2 to stop waiting in 5millisec, then immediately reset thread pool
  // Actions 1 and 2 should be allowed to finish, but pool should discard action 3 since it hasn't started execution
  BOOST_REQUIRE_EQUAL(action1.isWaiting(), true);
  BOOST_REQUIRE_EQUAL(action2.isWaiting(), true);
  BOOST_REQUIRE_EQUAL(action3.isWaiting(), false);
  action1.stopWaiting(boost::chrono::milliseconds(5));
  action2.stopWaiting(boost::chrono::milliseconds(5));
  ThreadPool::reset();

  // Confirm that actions 1 & 2 have been executed once, but action 3 has never been executed.
  BOOST_CHECK_EQUAL(action1.isWaiting(), false);
  BOOST_CHECK_EQUAL(action2.isWaiting(), false);
  BOOST_CHECK_EQUAL(action3.isWaiting(), false);
  BOOST_CHECK_EQUAL(action1.getExecutionCount(), size_t(1));
  BOOST_CHECK_EQUAL(action2.getExecutionCount(), size_t(1));
  BOOST_CHECK_EQUAL(action3.getExecutionCount(), size_t(0));
}



BOOST_FIXTURE_TEST_CASE(TolerantPool2, ThreadPoolSetup)
{
  // create thread pool with 2 threads
  // does not finish queue
  // does not forcefully terminate running commands
  ThreadPool::getInstance(2, false, false);
  action1.execInPool(params);
  action2.execInPool(params);
  action3.execInPool(params);

  // Tell actions 1 and 2 not to wait forever (but don't continue until they have started waiting)
  action1.stopWaiting(boost::chrono::milliseconds(2));
  action2.stopWaiting(boost::chrono::milliseconds(20));

  // Short sleeps to until action 3 has started execution in the pool ...
  while ( !action3.isWaiting() ) {
    boost::this_thread::sleep_for(boost::chrono::milliseconds(1));
  }

  // Tell action 3 to stop waiting in 5millisec, then immediately reset thread pool
  // All currently running actions should be allowed to finish
  BOOST_REQUIRE_EQUAL((!action1.isWaiting()) || (!action2.isWaiting()), true);
  BOOST_REQUIRE_EQUAL(action3.isWaiting(), true);
  action3.stopWaiting(boost::chrono::milliseconds(5));
  ThreadPool::reset();

  // Confirm that all actions 1 & 2 have been executed once
  BOOST_CHECK_EQUAL(action1.isWaiting(), false);
  BOOST_CHECK_EQUAL(action2.isWaiting(), false);
  BOOST_CHECK_EQUAL(action3.isWaiting(), false);
  BOOST_CHECK_EQUAL(action1.getExecutionCount(), size_t(1));
  BOOST_CHECK_EQUAL(action2.getExecutionCount(), size_t(1));
  BOOST_CHECK_EQUAL(action3.getExecutionCount(), size_t(1));
}

BOOST_FIXTURE_TEST_CASE(GoodGuyPool, ThreadPoolSetup)
{
  // create thread pool with 2 threads
  // this pool will wait until the queue is complete
  ThreadPool::getInstance(2, true, false);
  action1.execInPool(params);
  action2.execInPool(params);
  action3.execInPool(params);

  // Short sleeps to ensure that actions have started execution in the pool ...
  typedef boost::chrono::milliseconds MilliSec_t;
  while (!(action1.isWaiting() && action2.isWaiting())) {
    boost::this_thread::sleep_for(MilliSec_t(1));
  }

  // Tell actions 1 and 2 to stop waiting in 5millisec, then immediately reset thread pool
  // Since our pool is nice and should wait until the queue is empty, all actions should be allowed to finish
  BOOST_REQUIRE_EQUAL(action1.isWaiting(), true);
  BOOST_REQUIRE_EQUAL(action2.isWaiting(), true);
  BOOST_REQUIRE_EQUAL(action3.isWaiting(), false);
  action1.stopWaiting(MilliSec_t(5));
  action2.stopWaiting(MilliSec_t(5));
  // N.B. We stop action 3 from waiting forever via another thread (since "stopWaiting" method blocks until actor is waiting)
  boost::thread lAsyncStopAction3(boost::bind(&WaitingActor::stopWaiting<MilliSec_t>, &action3, MilliSec_t(5)));
  ThreadPool::reset();

  // Confirm that all actions have been executed once
  BOOST_CHECK_EQUAL(action1.isWaiting(), false);
  BOOST_CHECK_EQUAL(action2.isWaiting(), false);
  BOOST_CHECK_EQUAL(action3.isWaiting(), false);
  BOOST_CHECK_EQUAL(action1.getExecutionCount(), size_t(1));
  BOOST_CHECK_EQUAL(action2.getExecutionCount(), size_t(1));
  BOOST_CHECK_EQUAL(action3.getExecutionCount(), size_t(1));
}


/*
 * This is causing
 * terminate called after throwing an instance of 'boost::exception_detail::clone_impl<boost::exception_detail::error_info_injector<boost::lock_error> >'
terminate called recursively
  what():  boost: mutex lock failed in pthread_mutex_lock: Invalid argument
Aborted
 *
 */
//BOOST_FIXTURE_TEST_CASE(AngryPool, ThreadPoolSetup) {
//  // create thread pool with 2 threads
//  // does not finish queue
//  // does forcefully terminate running commands
//  ThreadPool::getInstance(2, false, true);
//  cmd1->exec(params);
//  cmd2->exec(params);
//  cmd3->exec(params);
//  LOG(kInfo) << "cmd1 " << cmd1->getProgress();
//  // now wait for a bit
//  // in 5ms cmd1 and cmd2 should start
//  // but not cmd3
//  boost::this_thread::sleep_for(boost::chrono::milliseconds(ThreadPoolCommand::kDefaultWaitTimeInMs/2));
//  // delete pool, forces all running commands (cmd1 & cmd2) to cancel
//  try {
//    ThreadPool::reset();
//  } catch (...) {
//
//  }
//
//  BOOST_CHECK_EQUAL(cmd1->getStatus(), Command::kRunning);
//  BOOST_CHECK_EQUAL(cmd2->getStatus(), Command::kRunning);
//  // cmd3 should be in running state
//  BOOST_CHECK_EQUAL(cmd3->getStatus(), Command::kInitial);
//}

BOOST_AUTO_TEST_SUITE_END() // ThreadPoolTestSuite

} /* namespace test */
} /* namespace action */
} /* namespace swatch */

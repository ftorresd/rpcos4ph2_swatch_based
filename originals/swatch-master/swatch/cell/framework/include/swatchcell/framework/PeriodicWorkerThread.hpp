/* 
 * File:   PeriodicWorkerThread.hpp
 * Author: Tom Williams
 * Date:   April 2017
 */

#ifndef __SWATCHCELL_FRAMEWORK_PERIODICWORKERTHREAD_HPP__
#define __SWATCHCELL_FRAMEWORK_PERIODICWORKERTHREAD_HPP__


// boost headers
#include "boost/noncopyable.hpp"
#include "boost/chrono.hpp"
#include "boost/thread/condition_variable.hpp"
#include "boost/thread/mutex.hpp"
#include "boost/thread/thread.hpp"

// log4cplus headers
#include "log4cplus/logger.h"


namespace swatchcellframework {


class PeriodicWorkerThread : public boost::noncopyable {
public:

  typedef boost::chrono::steady_clock Clock_t;

  class Stats {
  public:
    Stats();
  
    void add(const Clock_t::duration& aExecTime);

    void reset();

    const boost::chrono::system_clock::time_point& getTimeOfLastReset() const;
    
    size_t getNumberOfIterations() const;

    boost::chrono::duration<float> getAverageDuration() const;

    const Clock_t::duration& getMinDuration() const;

    const Clock_t::duration& getMaxDuration() const;

  private:
    //! Time that reset method was last called
    boost::chrono::system_clock::time_point mTimeOfLastReset;

    //! Number of times executed
    size_t mNumIterations;

    //! Total time spent executing, excluding sleeps
    Clock_t::duration mTotalDuration;
  
    //! Min time spent executing (excluding sleep time)
    Clock_t::duration mMinDuration;

    //! Max time spent executing (excluding sleep time)
    Clock_t::duration mMaxDuration;
  };


  PeriodicWorkerThread(const std::string& aLoggerName, size_t aPeriodInSeconds);
  virtual ~PeriodicWorkerThread();

  Stats getStats() const;
  
  size_t getPeriod() const;
  
  //! Sets the new period, which takes effect immediately
  void setPeriod(size_t aPeriod);
  
  bool isActive() const;

  //! Starts the worker; does nothing if it is already started
  void start();
  
  //! Stops the worker; current iteration of worker will continue, but thread will sleep before next iteration
  void stop();

  //! Triggers the worker to wake up, and run now (if currently asleep); returns immediately
  void runNow();

protected:
  virtual void doWork() = 0;

  log4cplus::Logger& getLogger();
  
private:
  void operator() ();

  //! Mutex, locked to prevent corruption of member data that's read/written from multiple threads
  mutable boost::mutex mMutex;
  
  log4cplus::Logger mLogger;

  //! Minimum time (in seconds) between monitoring update cycles
  size_t mPeriod;

  //! Indicates if worker loop has been stopped / started
  bool mActive;

  //! Indicates if the worker should run immediately upon waking; set via runNow method
  bool mRunNow;

  //! Indicates if destructor has been called
  bool mSelfDestruct;

  //! Time at which the next update will be performed
  Clock_t::time_point mNextUpdateTime;

  // //! Statistics data; reset upon start
  Stats mStats;

  //! Thread in which the work is done
  boost::thread mThread;

  //! Condition variable for notifying thread of changes to member data
  boost::condition_variable mConditionVar;
};
  
} // end ns: swatchcellframework

#endif /* __SWATCHCELL_FRAMEWORK_PERIODICWORKERTHREAD_HPP__ */


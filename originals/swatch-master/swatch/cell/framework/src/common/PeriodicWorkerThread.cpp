/* 
 * File:   PeriodicWorkerThread.cpp
 * Author: Tom Williams
 * Date:   April 2017
 */

#include "swatchcell/framework/PeriodicWorkerThread.hpp"


// boost headers
#include "boost/function.hpp"

// log4cplus headers
#include "log4cplus/loggingmacros.h"



namespace swatchcellframework {


PeriodicWorkerThread::Stats::Stats() :
  mNumIterations(0),
  mTotalDuration(0),
  mMinDuration(0),
  mMaxDuration(0)
{
}

void PeriodicWorkerThread::Stats::add(const Clock_t::duration& aExecTime)
{
  mNumIterations++;
  mTotalDuration += aExecTime;
  if ((aExecTime < mMinDuration) || (mNumIterations == 1))
    mMinDuration = aExecTime;
  if (aExecTime > mMaxDuration)
    mMaxDuration = aExecTime;
}


void PeriodicWorkerThread::Stats::reset()
{
  mTimeOfLastReset = boost::chrono::system_clock::now();
  mNumIterations = 0;
  mTotalDuration = Clock_t::duration(0);
  mMinDuration = Clock_t::duration(0);
  mMaxDuration = Clock_t::duration(0);
//  mErrorCount = 0;
//  mWarnCount = 0;
//  mGoodCount = 0;
//  mUnknownCount = 0;
//  mNoLimitCount = 0;
}


size_t PeriodicWorkerThread::Stats::getNumberOfIterations() const
{
  return mNumIterations;
}

const boost::chrono::system_clock::time_point& PeriodicWorkerThread::Stats::getTimeOfLastReset() const
{
  return mTimeOfLastReset;
}


boost::chrono::duration<float> PeriodicWorkerThread::Stats::getAverageDuration() const
{
  return boost::chrono::duration<float>(mTotalDuration) / float(mNumIterations);
}


const PeriodicWorkerThread::Clock_t::duration& PeriodicWorkerThread::Stats::getMinDuration() const
{
  return mMinDuration;
}


const PeriodicWorkerThread::Clock_t::duration& PeriodicWorkerThread::Stats::getMaxDuration() const
{
  return mMaxDuration;
}




PeriodicWorkerThread::PeriodicWorkerThread(const std::string& aLoggerName, size_t aPeriodInSeconds) : 
  mLogger(log4cplus::Logger::getInstance(aLoggerName)),
  mPeriod(aPeriodInSeconds),
  mActive(false),
  mRunNow(false),
  mSelfDestruct(false),
  mNextUpdateTime()
{
  // To avoid undefined behaviour, should only start thread after all member data has been constructed
  // N.B. Long-term solution (breaks ABI): Place mThread as last member variable
  mThread = boost::thread(boost::bind( &PeriodicWorkerThread::operator (), this));

  LOG4CPLUS_INFO(mLogger, "Created periodic worker thread (period = " << aPeriodInSeconds << "s)");
}

PeriodicWorkerThread::~PeriodicWorkerThread()
{
  LOG4CPLUS_INFO(mLogger, "Destroying periodic worker thread");

  boost::lock_guard<boost::mutex> lGuard(mMutex);
  mSelfDestruct = true;
  mConditionVar.notify_one();
}


PeriodicWorkerThread::Stats PeriodicWorkerThread::getStats() const
{
  boost::lock_guard<boost::mutex> lGuard(mMutex);
  return mStats;
}


size_t PeriodicWorkerThread::getPeriod() const
{
  boost::lock_guard<boost::mutex> lGuard(mMutex);
  return mPeriod;
}


void PeriodicWorkerThread::setPeriod(size_t aPeriod)
{
  LOG4CPLUS_INFO(mLogger, "Worker thread period set to " << aPeriod << " seconds");
  boost::lock_guard<boost::mutex> lGuard(mMutex);
  mPeriod = aPeriod;
  mNextUpdateTime = Clock_t::now() + boost::chrono::seconds(mPeriod);
  mConditionVar.notify_one();
}


bool PeriodicWorkerThread::isActive() const
{
  boost::lock_guard<boost::mutex> lGuard(mMutex);
  return mActive;
}


void PeriodicWorkerThread::start()
{
  boost::lock_guard<boost::mutex> lGuard(mMutex);

  if (!mActive) {
    LOG4CPLUS_INFO(mLogger, "Starting periodic worker thread (period = " << mPeriod << "s)");

    mStats.reset();
    mActive = true;
    mNextUpdateTime = Clock_t::now();
    mConditionVar.notify_one();
  }
}
  

void PeriodicWorkerThread::stop()
{
  boost::lock_guard<boost::mutex> lGuard(mMutex);

  std::time_t resetTime = boost::chrono::system_clock::to_time_t(mStats.getTimeOfLastReset());
  char resetTimeText[20];
  strftime(resetTimeText, 20, "%Y-%m-%d %H:%M:%S", std::gmtime(&resetTime));

  LOG4CPLUS_INFO(mLogger, "Stopping periodic worker thread. " << mStats.getNumberOfIterations() << " iterations since "
          << /*mStats.getTimeOfLastReset()*/ resetTimeText << "." << boost::chrono::duration_cast<boost::chrono::milliseconds>(mStats.getTimeOfLastReset().time_since_epoch()).count() % 1000 
          << ", min/average/max durations = " << boost::chrono::duration<float>(mStats.getMinDuration()) 
          << " / " << mStats.getAverageDuration() << " / " << boost::chrono::duration<float>(mStats.getMaxDuration())
//          << mStats.getAverageDuration() << ". Numbers of iterations leading to each monitoring state ..."
//          << std::endl << "      error:    " << mStats.getNumberIterations(swatch::core::kError)
//          << std::endl << "      warning:  " << mStats.getNumberIterations(swatch::core::kWarning)
//          << std::endl << "      good:     " << mStats.getNumberIterations(swatch::core::kGood)
//          << std::endl << "      unknown:  " << mStats.getNumberIterations(swatch::core::kUnknown)
//          << std::endl << "      no limit: " << mStats.getNumberIterations(swatch::core::kNoLimit)
          );
  mActive = false;
}


void PeriodicWorkerThread::runNow()
{
  LOG4CPLUS_INFO(mLogger, "Periodic worker thread will run again ASAP");
  boost::lock_guard<boost::mutex> lGuard(mMutex);
  mRunNow = true;
  mConditionVar.notify_one();
}


log4cplus::Logger& PeriodicWorkerThread::getLogger()
{
  return mLogger;
}
  

void PeriodicWorkerThread::operator() ()
{
  while (true) {
    LOG4CPLUS_DEBUG(mLogger, "Periodic worker thread: operator()()  >>  Top of while(true)");
    {
      boost::unique_lock<boost::mutex> lGuard(mMutex);

      while (!((mActive && (mNextUpdateTime <= Clock_t::now())) || mRunNow || mSelfDestruct)) {
        if (mActive) {
          mConditionVar.wait_until(lGuard, mNextUpdateTime);
        } else
          mConditionVar.wait(lGuard);
      }
      mRunNow = false;

      // In case this instance is being destroyed, then return so that the thread can stop
      if (mSelfDestruct) {
        LOG4CPLUS_INFO(mLogger, "Periodic worker thread: Main loop returning now");
        return;
      }
      // Otherwise, proceed with our main purpose (updating monitoring data) after unlocking mMutex
    }

    LOG4CPLUS_DEBUG(mLogger, "Periodic worker thread: Executing action now");

    Clock_t::time_point lWorkStartTime = Clock_t::now();

    doWork();

    LOG4CPLUS_DEBUG(mLogger, "Periodic worker thread: Finished current iteration");

    // Set time for next update
    boost::unique_lock<boost::mutex> lGuard(mMutex);
    mNextUpdateTime = lWorkStartTime + boost::chrono::seconds(mPeriod);

    // Update the stats counters
    Clock_t::duration lWorkDuration = Clock_t::now() - lWorkStartTime;
//    CellContext::SharedGuard_t lContextGuard(mContext);
    mStats.add(lWorkDuration /*, mContext.getSystem(lContextGuard).getStatusFlag()*/);
  }
}


}

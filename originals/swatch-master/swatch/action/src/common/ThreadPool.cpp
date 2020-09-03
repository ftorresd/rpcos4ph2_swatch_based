#include "swatch/action/ThreadPool.hpp"


#include <exception>                    // for exception
#include <ostream>                      // for operator<<, char_traits

#include "boost/thread/lock_types.hpp"  // for unique_lock
#include "boost/thread/pthread/condition_variable.hpp"

#include "log4cplus/loggingmacros.h"

#include "swatch/logger/Logger.hpp"
#include "swatch/core/utilities.hpp"



namespace swatch {
namespace action {


boost::mutex ThreadPool::sInstanceMutex;
boost::scoped_ptr<ThreadPool> ThreadPool::sInstance(NULL);


ThreadPool& ThreadPool::getInstance(size_t aNumThreads,
                                    bool aRunUntilQueueEmpty, bool aForceThreadCancellation)
{
  boost::unique_lock<boost::mutex> lLock(ThreadPool::sInstanceMutex);
  if (!sInstance) {
    sInstance.reset(new ThreadPool(aNumThreads, aRunUntilQueueEmpty,
                                   aForceThreadCancellation));
  }
  return *ThreadPool::sInstance;
}


void ThreadPool::reset()
{
  boost::unique_lock<boost::mutex> lLock(ThreadPool::sInstanceMutex);
  ThreadPool::sInstance.reset(NULL);
}


ThreadPool::ThreadPool(size_t aNumThreads, bool aRunUntilQueueEmpty,
                       bool aForceThreadCancellation) :
  mLogger(swatch::logger::Logger::getInstance("swatch.action.ThreadPool")),
  mNumThreads(aNumThreads),
  mWorkers(),
  mTasks(),
  mQueueMutex(),
  mCondition(),
  mStop(false),
  mForceThreadCancellation(aForceThreadCancellation)
{
  for (size_t i = 0; i < mNumThreads; ++i)
    mWorkers.create_thread(Worker(*this, aRunUntilQueueEmpty));
}


ThreadPool::~ThreadPool()
{
  {
    boost::unique_lock<boost::mutex> lLock(mQueueMutex);
    // set the pool to stop
    mStop = true;
  }
  // notify all waiters
  mCondition.notify_all();

  // join them
  if (mForceThreadCancellation) {
    // bad things happen if you interrupt a thread
    try {
      mWorkers.interrupt_all();
    }
    catch (...) {
    }
  }
  else
    mWorkers.join_all();
}


size_t ThreadPool::size() const
{
  return mNumThreads;
}


Worker::Worker(ThreadPool& aPool, bool aRunUntilQueueEmpty) :
  mPool(aPool),
  mRunUntilQueueEmpty(aRunUntilQueueEmpty)
{
}


void Worker::operator()()
{
  while (true) {
    // N.B: Declare task object within the loop, so that it's destroyed after task has completed ...
    // ... otherwise if task object isn't destroyed after task has completed execution, then ...
    // ... the object instances that form arguments of the task (e.g. shared_ptr<ActionableObject::BusyGuard>) ...
    // ... will still exist until the worker is given its next
    boost::packaged_task<void> lTask;

    {
      // get lock
      boost::unique_lock<boost::mutex> lLock(mPool.mQueueMutex);
      // look for work
      while (!mPool.mStop && mPool.mTasks.empty()) {
        //nothing to do, please wait
        mPool.mCondition.wait(lLock);
      }
      if (mPool.mStop && (!mRunUntilQueueEmpty || mPool.mTasks.empty()))
        //pool stopped and so shall I
        return;

      //otherwise get a task from the queue
      lTask = boost::move(mPool.mTasks.front());
      mPool.mTasks.pop_front();
    } // release lock

    //execute the task
    try {
      lTask();
    }
    catch (std::exception& lExc) {
      LOG4CPLUS_ERROR(mPool.mLogger, "Exception of type '" << swatch::core::demangleName(typeid(lExc).name())
                      << "' caught by worker. Message: " << lExc.what());
    }
  }
}


} // namespace action
} // namespace swatch

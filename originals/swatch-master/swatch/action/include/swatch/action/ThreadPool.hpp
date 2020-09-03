/**
 * @file    ThreadPool.hpp
 * @author  Luke Kreczko
 * @brief   ThreadPool class to manage threads
 * @date    May 2015
 */

#ifndef __SWATCH_ACTION_THREADPOOL_HPP__
#define __SWATCH_ACTION_THREADPOOL_HPP__


// Standard headers
#include <deque>                        // for deque
#include <stddef.h>                     // for size_t

// boost headers
#include "boost/scoped_ptr.hpp"
#include "boost/thread/future.hpp"
#include "boost/thread/pthread/condition_variable_fwd.hpp"
#include "boost/thread/pthread/mutex.hpp"  // for mutex
#include "boost/thread/thread.hpp"      // for thread_group
// need boost 1.57 for this
//#include <boost/move/unique_ptr.hpp>

// log4cplus headers
#include "log4cplus/logger.h"

// SWATCH headers
#include "swatch/core/exception.hpp"


namespace boost {
template <typename Signature> class function;
template <class T> class shared_ptr;
}

namespace swatch {

namespace core {
class XParameterSet;
}

namespace action {

class ThreadPool;


// wrapper for thread objects
class Worker {

public:
  Worker(ThreadPool& aPool, bool aRunUntilQueueEmpty);
  void operator()();

private:
  ThreadPool& mPool;
  bool mRunUntilQueueEmpty;
};


/*
 * @brief The ThreadPool class provides the functionality to create
 * a fixed number of threads and deliver work to them.
 *
 * The thread pool can operate in three different ways:
 * 1. on 'stop' request wait for the queue of tasks to be depleted
 * 2. on 'stop' request wait for running tasks to complete
 * 3. on 'stop' request terminate immediatly (still in development)
 * For more information, see constructor documentation
*/
class ThreadPool {
public:
  /**
   * Returns an instance of a ThreadPool. Only one instance is allowed per program (SingleTon).
   * For information about the parameters, please see constructor documentation.
   */
  static ThreadPool& getInstance(size_t aNumThreads = 1, bool aRunUntilQueueEmpty = false, bool aForceThreadCancellation = false);

  /**
   * Terminates the current instance of the ThreadPool thus allowing
   * for a new one to be created.
   */
  static void reset();

  virtual ~ThreadPool();

  /**
   * Adds a task to the pool's queue.
   * @param aCmd  pointer to the object
   * @param aFunction  reference to the object function
   * @param aResourceGuard  shared pointer to RAII-style guard object corresponding for resource's "busy" state resource
   * @param aParamSet  parameter set to be passed to the object function
   */
  template<class OBJECT, class ResourceGuardType>
  void addTask( OBJECT* aCmd , boost::function<void(OBJECT*, boost::shared_ptr<ResourceGuardType>, const core::XParameterSet&)> aFunction , const boost::shared_ptr<ResourceGuardType>& aResourceGuard, const core::XParameterSet& aParamSet );


  /**
   * Add a task to the pool's queue.
   * @param aCmd  pointer to the object
   * @param aFunction  reference to the object function
   */
  template<class OBJECT, class ResourceGuardType>
  void addTask( OBJECT* aCmd , boost::function<void(OBJECT*, boost::shared_ptr<ResourceGuardType>)> aFunction, const boost::shared_ptr<ResourceGuardType>& aResourceGuard );

  size_t size() const;

private:
  /**
   * ThreadPool constructor
   * @param aNumThreads number of simultaneous threads
   * @param aRunUntilQueueEmpty if true the pool will wait until the task queue is empty. Default is false
   * @param aForceThreadCancellation if true the pool will cancel all running threads when stopped. Default is false
   */
  ThreadPool(size_t aNumThreads = 1, bool aRunUntilQueueEmpty = false, bool aForceThreadCancellation = false);

  ThreadPool(ThreadPool const&); // not implemented
  ThreadPool& operator=(ThreadPool const&); // not implemented

private:
  // allow workers to access private variables
  friend class Worker;
  // pointer to the only ThreadPool instance
  static boost::scoped_ptr<ThreadPool> sInstance;
  log4cplus::Logger mLogger;
  // number of simultaneous threads
  size_t mNumThreads;
  // thread group for the workers
  boost::thread_group mWorkers;
  // task queue
  std::deque<boost::packaged_task<void> > mTasks;

  // synchronisation
  static boost::mutex sInstanceMutex;
  boost::mutex mQueueMutex;
  boost::condition_variable mCondition;
  bool mStop, mForceThreadCancellation;

};

} // namespace core
} // namespace swatch

SWATCH_DEFINE_EXCEPTION(OperationOnStoppedThreadPool)

#include "swatch/action/ThreadPool.hxx"

#endif /* __SWATCH_ACTION_THREADPOOL_HPP__ */

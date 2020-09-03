#include "swatchcell/framework/LogMessageQueue.h"


// Log4Cplus Headers
#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"
#include "log4cplus/spi/loggingevent.h"

namespace swatchcellframework {


// --------------------------------------------------------
LogMessageQueue::LogMessageQueue(std::string aLoggerId, size_t aSize) :
  mLoggerId(aLoggerId),
  mSize(aSize)
{
  setName(mLoggerId);
}


// --------------------------------------------------------
LogMessageQueue::~LogMessageQueue()
{
  destructorImpl();

}


// --------------------------------------------------------
size_t LogMessageQueue::getSize() const
{
  return mQueue.size();
}


// --------------------------------------------------------
std::deque<LogMessageQueue::LogMessage> LogMessageQueue::getMessages()
{
 boost::shared_lock<boost::shared_mutex> lLock(mMutex);
 return mQueue;
}


// --------------------------------------------------------
void LogMessageQueue::clear() {
  // Lock me first
  boost::unique_lock<boost::shared_mutex> lLock(mMutex);

  mQueue.clear();
}


// --------------------------------------------------------
void LogMessageQueue::close()
{

  clear();

  closed = true;
}

// --------------------------------------------------------
void LogMessageQueue::append(const log4cplus::spi::InternalLoggingEvent& event)
{
  // Lock me first
  boost::unique_lock< boost::shared_mutex > lLock(mMutex);

  // Pop one element from the front of the queue if the queue is full
  if (mQueue.size() >= mSize) {
    mQueue.pop_front();
  }

  std::string shortName = ( event.getLoggerName() == mLoggerId ? "" : event.getLoggerName().substr(mLoggerId.size()+1));

  LogMessage lTmpMsg = { event.getLogLevel(), shortName, event.getMessage(), log4cplus::helpers::to_time_t(event.getTimestamp()) };
  mQueue.push_back( lTmpMsg );

}

} // namespace swatchcellframework

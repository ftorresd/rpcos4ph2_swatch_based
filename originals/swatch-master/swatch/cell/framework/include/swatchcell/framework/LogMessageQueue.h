
#ifndef __SWATCHCELL_FRAMEWORK_LOGMESSAGEQUEUE_H__
#define __SWATCHCELL_FRAMEWORK_LOGMESSAGEQUEUE_H__


// C++ Headers
#include <deque>
  
// Boost Headers
#include <boost/thread/shared_mutex.hpp>

// Log4Cplus Headers
#include "log4cplus/appender.h"
#include "log4cplus/loglevel.h"


namespace swatchcellframework {


class LogMessageQueue : public log4cplus::Appender, public boost::noncopyable {
public:
  LogMessageQueue(std::string aLoggerId, size_t aSize = 50 );
  virtual ~LogMessageQueue();

  struct LogMessage {
    log4cplus::LogLevel level;
    const std::string logger;
    const std::string message;
    time_t time;
  };

  size_t getSize() const;
  std::deque<LogMessage> getMessages();
  void clear();
protected:
  void append(log4cplus::spi::InternalLoggingEvent const & event);
  
  virtual void close();
private:
  
  mutable boost::shared_mutex mMutex;
  std::string mLoggerId;
  size_t mSize;
  std::deque<LogMessage> mQueue;

};

} // namespace swatchcellframework


#endif  /* __SWATCHCELL_FRAMEWORK_LOGMESSAGEQUEUE_H__ */


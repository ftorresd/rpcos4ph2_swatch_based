/*
 * File:   LogWidget.h
 * Author: ale
 *
 * Created on November 30, 2015, 10:51 AM
 */

#ifndef __SWATCHCELL_FRAMEWORK_LOGWIDGET_LOGWIDGET_H__
#define __SWATCHCELL_FRAMEWORK_LOGWIDGET_LOGWIDGET_H__


#include "swatchcell/framework/LogMessageQueue.h"

// TS Headers
#include "ajax/AutoHandledWidget.h"


namespace ajax {
class Div;
class PlainHtml;
}

namespace tsframework {
class CellAbstractContext;
} // namespace tsframework


namespace swatchcellframework {

class LogWidget : public ajax::AutoHandledWidget {
public:
  LogWidget(tsframework::CellAbstractContext & aContext, log4cplus::SharedAppenderPtr aQueue, size_t aHeight=220 );
  virtual ~LogWidget();

  static const uint8_t kOk;
  static const uint8_t kNotSet;
  static const uint8_t kInfo;
  static const uint8_t kWarn;
  static const uint8_t kError;
  static const uint8_t kUnknown;
  static const uint8_t kMax;
  static const uint8_t kDefault;

  static const char * kNames[];
  static const char * kShorts[];
  static const char * kColors[];
  static const char * kBgColors[];
  static const char * kImages[];

  static std::string getStateName(unsigned char level);
  static std::string getStateShort(unsigned char level);
  static std::string getStateColor(unsigned char level);
  static std::string getStateBgColor(unsigned char level);
  static std::string getStateImage(unsigned char level);

  static const log4cplus::LogLevel loglevel_[];

  static log4cplus::LogLevel getStateLogLevel(unsigned char _level);

  void html(cgicc::Cgicc & cgi, std::ostream & out);

private:
  static uint8_t localLogLevel(log4cplus::LogLevel aLevel );


  ajax::Div * logMessage( const LogMessageQueue::LogMessage& m);

  log4cplus::SharedAppenderPtr mQueue;

  ajax::Div* mBox;
};

} //namespace swatchcellframework


#endif  /* __SWATCHCELL_FRAMEWORK_LOGWIDGET_LOGWIDGET_H__ */


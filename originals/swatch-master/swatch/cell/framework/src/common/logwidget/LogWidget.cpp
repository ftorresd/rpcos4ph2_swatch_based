/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   NewLogWidget.cpp
 * Author: ale
 *
 * Created on November 30, 2015, 10:51 AM
 */

#include "swatchcell/framework/logwidget/LogWidget.h"
#include "swatchcell/framework/tools/panelFunctions.h"
#include "ts/framework/CellAbstractContext.h"
#include "ajax/Div.h"
#include "ajax/ResultBox.h"
#include "ajax/PlainHtml.h"
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

namespace swatchcellframework {

const uint8_t LogWidget::kOk = 0;
const uint8_t LogWidget::kNotSet = 1;
const uint8_t LogWidget::kInfo = 2;
const uint8_t LogWidget::kWarn = 3;
const uint8_t LogWidget::kError = 4;
const uint8_t LogWidget::kUnknown = 5;
const uint8_t LogWidget::kDefault = LogWidget::kNotSet;
const uint8_t LogWidget::kMax = 5;

const char * LogWidget::kNames[] = {"ok", "notset", "info", "warn", "error", "unknown"};
const char * LogWidget::kShorts[] = {"o", "n", "i", "w", "e", "u"};
const char * LogWidget::kColors[] = {"#67e667", "#eee", "#6C8Ce5", "#ffd073", "#ff7373", "#ffd073"};
const char * LogWidget::kBgColors[] = {"#dfd", "#efefef", "#ddf", "#ffb", "#fdd", "#ffb"};
const char * LogWidget::kImages[] = {
  "/extern/icons/accept.png",
  "/extern/icons/control_stop.png",
  "/extern/icons/information.png",
  "/extern/icons/error.png",
  "/extern/icons/exclamation.png",
  "/extern/icons/lightning.png"
};


const log4cplus::LogLevel LogWidget::loglevel_[] = {
  log4cplus::INFO_LOG_LEVEL,
  log4cplus::INFO_LOG_LEVEL,
  log4cplus::INFO_LOG_LEVEL,
  log4cplus::WARN_LOG_LEVEL,
  log4cplus::ERROR_LOG_LEVEL,
  log4cplus::INFO_LOG_LEVEL
};


// --------------------------------------------------------
LogWidget::LogWidget(tsframework::CellAbstractContext& aContext, log4cplus::SharedAppenderPtr aQueue, size_t aHeight):
  ajax::AutoHandledWidget("/" + aContext.getLocalUrn() + "/Default"),
  mQueue(aQueue)
{
  mBox = new ajax::Div();
  mBox->setIsOwned(false);
  mBox->set("style", "font:8pt monospace;overflow:auto;border:1px solid #888;margin:2px;background-color:#efefef;height:"+boost::lexical_cast<std::string>(aHeight)+"px;");
  mBox->set("test","test");
  add(mBox);
}


// --------------------------------------------------------
LogWidget::~LogWidget()
{
}



// --------------------------------------------------------
void LogWidget::html(cgicc::Cgicc& cgi, std::ostream& out)
{

  mBox->remove();

  // Get the list of messages
  std::deque<LogMessageQueue::LogMessage> lMessages = dynamic_cast<LogMessageQueue*>(mQueue.get())->getMessages();

  BOOST_FOREACH(LogMessageQueue::LogMessage& m, lMessages) {
    mBox->add(logMessage(m));
  }

  addPlainHtml(*mBox) << "<script type=\"text/javascript\">"
      << "var lmc = document.getElementById(\"" << mBox->getId() << "\");"
      << "lmc.scrollTop = lmc.scrollHeight;"
      << "</script>";

  mBox->html(cgi,out);

}

// --------------------------------------------------------
uint8_t LogWidget::localLogLevel(log4cplus::LogLevel mLevel ) {
  switch ( mLevel ) {
    case log4cplus::NOT_SET_LOG_LEVEL:
//    case log4cplus::ALL_LOG_LEVEL:
    case log4cplus::TRACE_LOG_LEVEL:
    case log4cplus::DEBUG_LOG_LEVEL:
    case log4cplus::INFO_LOG_LEVEL:
      return kInfo;
    case log4cplus::WARN_LOG_LEVEL:
      return kWarn;
    case log4cplus::ERROR_LOG_LEVEL:
    case log4cplus::FATAL_LOG_LEVEL:
    case log4cplus::OFF_LOG_LEVEL:
      return kError;
    default:
      return kUnknown;
  }
}



// --------------------------------------------------------
ajax::Div* LogWidget::logMessage(const LogMessageQueue::LogMessage& aMsg)
{

  uint8_t level = localLogLevel(aMsg.level);

  ajax::Div* lLine = new ajax::Div();
  lLine->set("style","background:"+getStateBgColor(level)+" "+" url('" + getStateImage(level)+"') no-repeat 2px 2px;padding:2px 28px;");
  addPlainHtml(*lLine) << "<span style=\"font-weight:bold;\">" << ctime(&aMsg.time) << ": </span>" << (aMsg.logger.empty() ? "" : aMsg.logger+" > ") << aMsg.message;

  return lLine;
}


// --------------------------------------------------------
std::string LogWidget::getStateName(unsigned char level)
{
  if (level < kMax)
    return kNames[level];
  else
    return kNames[kUnknown];
}


// --------------------------------------------------------
std::string LogWidget::getStateShort(unsigned char level)
{
  if (level < kMax)
    return kShorts[level];
  else
    return kShorts[kUnknown];
}


// --------------------------------------------------------
std::string LogWidget::getStateColor(unsigned char level)
{
  if (level < kMax)
    return kColors[level];
  else
    return kColors[kUnknown];
}


// --------------------------------------------------------
std::string LogWidget::getStateBgColor(unsigned char level)
{
  if (level < kMax)
    return kBgColors[level];
  else
    return kBgColors[kUnknown];
}


// --------------------------------------------------------
std::string LogWidget::getStateImage(unsigned char level)
{
  if (level < kMax)
    return kImages[level];
  else
    return kImages[kUnknown];
}


// --------------------------------------------------------
log4cplus::LogLevel LogWidget::getStateLogLevel(unsigned char _level)
{
  if (_level < kMax)
    return loglevel_[_level];
  else
    return loglevel_[kUnknown];
}

} //namespace swatchcellframework

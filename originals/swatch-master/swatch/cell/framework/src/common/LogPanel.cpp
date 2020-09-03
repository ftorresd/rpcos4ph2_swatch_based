#include "swatchcell/framework/LogPanel.h"

#include "boost/foreach.hpp"
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "ajax/PolymerElement.h"
#include "ajax/toolbox.h"

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"
#include "swatchcell/framework/LogMessageQueue.h"


#include "jsoncpp/json/json.h"

namespace swatchcellframework
{

  LogPanel::LogPanel(tsframework::CellAbstractContext* context, log4cplus::Logger& logger)
  :
    CellPanel(context, logger),
    mLogger(logger),
    mCellContext(dynamic_cast<swatchcellframework::CellContext&>(*context))
  {


  }

  LogPanel::~LogPanel()
  {

  }

  void LogPanel::layout(cgicc::Cgicc& cgi)
  {
    this->remove();

    setEvent("LogPanel::getLoggers", ajax::Eventable::OnClick, this, &LogPanel::getLoggers);
    setEvent("LogPanel::getLog", ajax::Eventable::OnClick, this, &LogPanel::getLog);

    ajax::PolymerElement* logPanel = new ajax::PolymerElement("log-panel");

    this -> add(logPanel);

    return;

  }
  
  void LogPanel::getLoggers(cgicc::Cgicc& cgi, std::ostream& out) {
    
    std::set<std::string> logIds = this -> mCellContext.getLogAppenderIDs();
    
    Json::Value lLoggersJson(Json::arrayValue);
    
    BOOST_FOREACH( const std::string& lId, logIds) {
      lLoggersJson.append(lId);
    }
    
    out << lLoggersJson;
    
    return;
  }
  
  void LogPanel::getLog(cgicc::Cgicc& cgi, std::ostream& out) {
    
    std::string lSelectedLogger = ajax::toolbox::getSubmittedValue(cgi, "logger");
    std::string lCommand = ajax::toolbox::getSubmittedValue(cgi, "command");
    
    Json::Value lLogJson(Json::objectValue);
    
    lLogJson["logger"] = lSelectedLogger;
    
    log4cplus::SharedAppenderPtr lQueue;
    
    try {
      lQueue = mCellContext.getLogAppender(lSelectedLogger);
      
      if(lCommand == "clear") dynamic_cast<LogMessageQueue*>(lQueue.get())->clear();
      
      std::deque<LogMessageQueue::LogMessage> lMessages = dynamic_cast<LogMessageQueue*>(lQueue.get())->getMessages();
      Json::Value lMessagesJson(Json::arrayValue);
      BOOST_FOREACH(LogMessageQueue::LogMessage& lMessage, lMessages) {
        Json::Value lMessageJson(Json::objectValue);
        lMessageJson["logLevel"] = this -> localLogLevel(lMessage.level);
        lMessageJson["time"] = ctime(&lMessage.time);
        lMessageJson["logger"] = (lMessage.logger.empty() ? "" : lMessage.logger);
        lMessageJson["message"] = lMessage.message;        
        lMessagesJson.append(lMessageJson);
      }
      
      lLogJson["messages"] = lMessagesJson;
      
      lLogJson["found"] = true;
    } catch ( std::out_of_range ) {
      lLogJson["found"] = false;
    }
    
    out << lLogJson;
    
    return;

  }

  std::string LogPanel::localLogLevel(log4cplus::LogLevel aLevel ) {
    switch ( aLevel ) {
      case log4cplus::NOT_SET_LOG_LEVEL:
  //    case log4cplus::ALL_LOG_LEVEL:
      case log4cplus::TRACE_LOG_LEVEL:
      case log4cplus::DEBUG_LOG_LEVEL:
      case log4cplus::INFO_LOG_LEVEL:
        return "Info";
      case log4cplus::WARN_LOG_LEVEL:
        return "Warning";
      case log4cplus::ERROR_LOG_LEVEL:
      case log4cplus::FATAL_LOG_LEVEL:
      case log4cplus::OFF_LOG_LEVEL:
        return "Error";
      default:
        return "Unknown";
    }
  }
}


 // end ms swatchframework

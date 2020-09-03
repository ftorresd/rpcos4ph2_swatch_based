/*
 * @file    Logger.hpp
 * @author  Luke Kreczko
 * @brief   Swatch wrapper for log4cplus
 * @date    November 2015
 */

#ifndef __SWATCH_LOGGER_LOGGER_HPP__
#define __SWATCH_LOGGER_LOGGER_HPP__


#include <string>

#include <log4cplus/logger.h>
#include <log4cplus/tstring.h>
// #include "swatch/logger/Log.hpp"

namespace swatch {
namespace logger {

class Logger {
public:
  /**
   * Retrieves an instace of the log4cplus logger. Instances are structured in a hierarchy:
   *                                root
   *                                  |
   *                                swatch
   *                              /   |    \
   *                             /    |     \
   *                            /     |      \
   *                         core  processor  system etc.
   *                         /
   *                   component
   *
   *  e.g. for accessing the logger for a component in core one would
   *  log4cplus::Logger logger = Logger::getInstance("swatch.core.componentA");
   */
  static log4cplus::Logger getInstance(const log4cplus::tstring& aName);
  // static void log(LogLevel aLevel, const std::string& aMessage);

private:
  static bool sInitialised;
  static std::string sConfigFile;

  /**
   * Registration of all custom log4cplus filters, layouts and appenders
   */
  static void registerCustomLoggingClasses();

};
}
}



#endif /* _SWATCH_LOGGER_LOGGER_HPP__ */

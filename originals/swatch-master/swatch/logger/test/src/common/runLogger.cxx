#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>

#include <iostream>
#include <sys/types.h>
#include <unistd.h> 

#include "swatch/logger/Logger.hpp"
namespace swlog = swatch::logger;

using namespace std;

void printMessages(log4cplus::Logger logger) {
  using namespace log4cplus;
  LOG4CPLUS_TRACE(logger, "printMessages() " << getpid());
  LOG4CPLUS_DEBUG(logger, "This is a DEBUG message");
  LOG4CPLUS_INFO(logger, "This is a INFO message");
  LOG4CPLUS_WARN(logger, "This is a WARN message");
  LOG4CPLUS_ERROR(logger, "This is a ERROR message");
  LOG4CPLUS_FATAL(logger, "This is a FATAL message");
}

//void printMessagesSwatch() {
//  LOG(swlog::kDebug4) << "printMessagesSwatch() " << getpid();
//  LOG(swlog::kDebug) << "This is a DEBUG message";
//  LOG(swlog::kInfo) << "This is a INFO message";
//  LOG(swlog::kWarning) << "This is a WARN message";
//  LOG(swlog::kError) << "This is a ERROR message";
//  LOG(swlog::kFatal) << "This is a FATAL message";
//}

int main() {
  using namespace log4cplus;
//  ::log4cplus::initialize();
//  ::log4cplus::PropertyConfigurator::doConfigure("log4cplus.properties");
//  ::log4cplus::Logger logger2 = log4cplus::Logger::getInstance("swatch.logger.test.runLogger");
  log4cplus::Logger logger = swlog::Logger::getInstance("swatch.logger.testLogger");
  log4cplus::Logger logger2 = swlog::Logger::getInstance("swatch.logger.test.runLogger");
//  LOG4CPLUS_WARN(logger2,
//      "This is" << " a reall" << "y long message." << endl << "Just testing it out" << endl << "What do you think?");
//  LOG4CPLUS_WARN(logger2, "This is a bool: " << true);
//  LOG4CPLUS_WARN(logger2, "This is a char: " << 'x');
//  LOG4CPLUS_WARN(logger2, "This is a short: " << (short) -100);
//  LOG4CPLUS_WARN(logger2, "This is a unsigned short: " << (unsigned short) 100);
//  LOG4CPLUS_WARN(logger2, "This is a int: " << (int) 1000);
//  LOG4CPLUS_WARN(logger2, "This is a unsigned int: " << (unsigned int) 1000);
//  LOG4CPLUS_WARN(logger2, "This is a long(hex): " << hex << (long) 100000000);
//  LOG4CPLUS_WARN(logger2, "This is a unsigned long: " << (unsigned long) 100000000);
//  LOG4CPLUS_WARN(logger2, "This is a float: " << 1.2345f);
//  LOG4CPLUS_WARN(logger2, "This is a double: " << setprecision(15) << 1.2345234234);
//  LOG4CPLUS_WARN(logger2, "This is a long double: " << setprecision(15) << (long double) 123452342342.342);

  LOG4CPLUS_TRACE(logger2, "This is a TRACE message");
  LOG4CPLUS_DEBUG(logger2, "This is a DEBUG message");
  LOG4CPLUS_INFO(logger2, "This is a INFO message");
  LOG4CPLUS_WARN(logger2, "This is a WARN message");
  LOG4CPLUS_ERROR(logger2, "This is a ERROR message");
  LOG4CPLUS_FATAL(logger2, "This is a FATAL message");

  logger.setLogLevel(TRACE_LOG_LEVEL);
  cout << "*** calling printMessages() with TRACE set: ***" << endl;
  printMessages(logger);

  logger.setLogLevel(DEBUG_LOG_LEVEL);
  cout << "\n*** calling printMessages() with DEBUG set: ***" << endl;
  printMessages(logger);

  logger.setLogLevel(INFO_LOG_LEVEL);
  cout << "\n*** calling printMessages() with INFO set: ***" << endl;
  printMessages(logger);

  logger.setLogLevel(WARN_LOG_LEVEL);
  cout << "\n*** calling printMessages() with WARN set: ***" << endl;
  printMessages(logger);

  logger.setLogLevel(ERROR_LOG_LEVEL);
  cout << "\n*** calling printMessages() with ERROR set: ***" << endl;
  printMessages(logger);

  logger.setLogLevel(FATAL_LOG_LEVEL);
  cout << "\n*** calling printMessages() with FATAL set: ***" << endl;
  printMessages(logger);

//  cout << "\n*** And now the same with swatch::logger ***" << endl;
//  swlog::Log::setLogThreshold(swlog::kDebug4);
//  cout << "*** calling printMessagesSwatch() with TRACE set: ***" << endl;
//  printMessagesSwatch();
//
//  swlog::Log::setLogThreshold(swlog::kDebug);
//  cout << "\n*** calling printMessagesSwatch() with DEBUG set: ***" << endl;
//  printMessagesSwatch();
//
//  swlog::Log::setLogThreshold(swlog::kInfo);
//  cout << "\n*** calling printMessagesSwatch() with INFO set: ***" << endl;
//  printMessagesSwatch();
//
//  swlog::Log::setLogThreshold(swlog::kWarning);
//  cout << "\n*** calling printMessagesSwatch() with WARN set: ***" << endl;
//  printMessagesSwatch();
//
//  swlog::Log::setLogThreshold(swlog::kError);
//  cout << "\n*** calling printMessagesSwatch() with ERROR set: ***" << endl;
//  printMessagesSwatch();
//
//  swlog::Log::setLogThreshold(swlog::kFatal);
//  cout << "\n*** calling printMessagesSwatch() with FATAL set: ***" << endl;
//  printMessagesSwatch();

  return 0;
}

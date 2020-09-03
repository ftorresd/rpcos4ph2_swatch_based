/* 
 * File:   boostTest.cpp
 * Author: ale
 *
 * Created on August 13, 2014, 6:49 PM
 */



#include <iostream>
#include <map>

#include <log4cplus/logger.h>


#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE SwatchTest

// BOOST_TEST_NO_MAIN: Disable auto-generation of main function, in order to define our own, which sets the log threshold
#define BOOST_TEST_NO_MAIN

#include <boost/test/unit_test.hpp>


int BOOST_TEST_CALL_DECL
main( int argc, char* argv[] )
{
  const char* lLogLevelVar = getenv("SWATCH_TEST_LOG_LEVEL");
  if (lLogLevelVar != NULL) {
    
    const std::map<std::string, log4cplus::LogLevel> lLogLevelMap = {
      {"FATAL", log4cplus::FATAL_LOG_LEVEL},
      {"ERROR", log4cplus::ERROR_LOG_LEVEL},
      {"WARNING", log4cplus::WARN_LOG_LEVEL},
      {"WARN", log4cplus::WARN_LOG_LEVEL},
      {"INFO", log4cplus::INFO_LOG_LEVEL},
      {"DEBUG", log4cplus::DEBUG_LOG_LEVEL}
    };

    auto lIt = lLogLevelMap.find(lLogLevelVar);
    if (lIt != lLogLevelMap.end())
      log4cplus::Logger::getRoot().setLogLevel(lIt->second);
    else {
      std::cout << "Invalid value '" << lLogLevelVar << "' given for SWATCH_TEST_LOG_LEVEL env var. Valid values: " << std::endl;
      for(auto lIt=lLogLevelMap.begin(); lIt != lLogLevelMap.end(); lIt++)
        std::cout << "     " << lIt->first << std::endl;
      return 1;
    }
  }
  else
    log4cplus::Logger::getRoot().setLogLevel(log4cplus::FATAL_LOG_LEVEL);

  return ::boost::unit_test::unit_test_main( &init_unit_test, argc, argv );
}

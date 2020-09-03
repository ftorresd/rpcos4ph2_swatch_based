#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/configurator.h>
#include <log4cplus/consoleappender.h>

#include <iomanip>
#include <iostream>

#include <boost/thread/thread.hpp>
#include <boost/chrono.hpp>

#include "swatch/logger/Logger.hpp"
namespace swlog = swatch::logger;

#include "swatch/logger/SwatchPatternLayout.hpp"
using namespace std;


namespace swatch {
namespace logger {
namespace test {

struct MyTestStruct {
};

std::ostream& operator<<(std::ostream& aOut, const MyTestStruct& aStruct) {
  boost::this_thread::sleep_for(boost::chrono::milliseconds(50));
  return (aOut << "test message");
}

void test() {
  log4cplus::Logger logger = swlog::Logger::getInstance("swatch.logger.test.testThreadedLogger");
  LOG4CPLUS_INFO(logger, "LOGGER: I'm logging from thread " << boost::this_thread::get_id());
  LOG4CPLUS_INFO(logger, "LOGGER: I'" << MyTestStruct() << "' from thread " << boost::this_thread::get_id());
}

} // end ns: test 
} // end ns: logger
} // end ns: swatch


int main() {
  std::vector<boost::thread *> threads;

  for (size_t i = 0; i < 100; i++) {
    threads.push_back(new boost::thread(swatch::logger::test::test));
  }

  for (size_t i = 0; i < 100; i++) {
    threads[i]->join();
    delete threads[i];
  }

  return 0;
}

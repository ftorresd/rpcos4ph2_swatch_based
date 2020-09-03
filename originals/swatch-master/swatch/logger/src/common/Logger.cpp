#include "swatch/logger/Logger.hpp"

// Standard headers
#include <stdlib.h>

// log4cplus headers
#include <log4cplus/configurator.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/spi/factory.h>

// SWATCH headers
#include "swatch/logger/SwatchPatternLayout.hpp"


namespace swatch {
namespace logger {

bool Logger::sInitialised = false;
// needs absolute path
std::string Logger::sConfigFile = (getenv("SWATCH_LOG4CPLUS_CONFIG") ? getenv("SWATCH_LOG4CPLUS_CONFIG") : "log4cplus.properties");

log4cplus::Logger Logger::getInstance(const log4cplus::tstring& aName)
{
  using namespace log4cplus;
  if (!sInitialised) {
    {
      // this shoud probably have a lock
      ::log4cplus::initialize();
      // register our stuff
      Logger::registerCustomLoggingClasses();
      PropertyConfigurator::doConfigure(Logger::sConfigFile);
      Logger::sInitialised = true;
    }
  }
  return log4cplus::Logger::getInstance(aName);

}

void Logger::registerCustomLoggingClasses()
{
  using namespace log4cplus;
  // register all custom layouts
  spi::LayoutFactoryRegistry& lReg = spi::getLayoutFactoryRegistry();
  LOG4CPLUS_REG_PRODUCT (lReg, "swatch::logger::", SwatchPatternLayout, swatch::logger::, spi::LayoutFactory);
}

}
}

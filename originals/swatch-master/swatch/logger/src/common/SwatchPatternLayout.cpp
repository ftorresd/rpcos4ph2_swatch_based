#include "swatch/logger/SwatchPatternLayout.hpp"
#include "swatch/logger/ansi.hpp"

#include <log4cplus/helpers/loglog.h>
#include <iostream>

namespace swatch {
namespace logger {

SwatchPatternLayout::SwatchPatternLayout(const log4cplus::tstring& aPattern) :
  log4cplus::PatternLayout(aPattern)
{
  PatternLayout::init(aPattern, 0);
}

SwatchPatternLayout::SwatchPatternLayout(const log4cplus::helpers::Properties& aProperties) :
  log4cplus::PatternLayout(aProperties)
{
}

SwatchPatternLayout::~SwatchPatternLayout()
{

}

void SwatchPatternLayout::formatAndAppend(log4cplus::tostream& aOutput,
    const log4cplus::spi::InternalLoggingEvent& aEvent)
{
  log4cplus::LogLevel lLoglevel = aEvent.getLogLevel();
  const char* lColour = getColourForLogLevel(lLoglevel);
  aOutput << lColour;
  PatternLayout::formatAndAppend(aOutput, aEvent);
  aOutput << ansi::kReset;
}

void SwatchPatternLayout::init(const log4cplus::tstring& aPattern, unsigned aNdcMaxDepth)
{
  PatternLayout::init(aPattern, aNdcMaxDepth);
}

const char* SwatchPatternLayout::getColourForLogLevel(log4cplus::LogLevel aLogLevel) const
{
  using namespace log4cplus;
  switch (aLogLevel) {
    case FATAL_LOG_LEVEL:
      return ansi::kRed;
    case ERROR_LOG_LEVEL:
      return ansi::kLightRed;
    case WARN_LOG_LEVEL:
      return ansi::kYellow;
    case INFO_LOG_LEVEL:
      return ansi::kBlue;
    case DEBUG_LOG_LEVEL:
      return ansi::kCyan;
    case TRACE_LOG_LEVEL:
      return ansi::kMagenta;
    default:
      return "";
  }
}

}
}


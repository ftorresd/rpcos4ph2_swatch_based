#ifndef __SWATCH_LOGGER_LOGLEVEL_HPP__
#define __SWATCH_LOGGER_LOGLEVEL_HPP__

#include <log4cplus/loglevel.h>

namespace swatch {
namespace logger {

const log4cplus::LogLevel NOTICE_LOG_LEVEL = log4cplus::INFO_LOG_LEVEL + 1;

const log4cplus::LogLevel DEBUG1_LOG_LEVEL = log4cplus::DEBUG_LOG_LEVEL - 1;
const log4cplus::LogLevel DEBUG2_LOG_LEVEL = log4cplus::DEBUG_LOG_LEVEL - 2;
const log4cplus::LogLevel DEBUG3_LOG_LEVEL = log4cplus::DEBUG_LOG_LEVEL - 3;
const log4cplus::LogLevel DEBUG4_LOG_LEVEL = log4cplus::DEBUG_LOG_LEVEL - 4;


} // swatch
} // logger

#endif /* __SWATCH_LOGGER_LOGLEVEL_HPP__ */

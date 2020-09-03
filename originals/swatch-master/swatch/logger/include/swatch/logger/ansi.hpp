/**
 * @file    Logger.hpp
 * @author  Alessandro Thea
 * @brief   MP7 board processor implementation
 * @date    June 2014
 */

#ifndef __SWATCH_LOGGER_ANSI_HPP__
#define __SWATCH_LOGGER_ANSI_HPP__

namespace swatch {
namespace logger {

namespace ansi {
const char* const kEsc       = "\033[";
const char* const kReset     = "\033[0m";
const char* const kBlack     = "\033[30m";
const char* const kRed       = "\033[31m";
const char* const kLightRed  = "\033[1;31m";
const char* const kGreen     = "\033[32m";
const char* const kYellow    = "\033[33m";
const char* const kBlue      = "\033[34m";

const char* const kMagenta   = "\033[35m";
const char* const kCyan      = "\033[36m";
const char* const kWhite     = "\033[37m";

const char* const kBckBlack  = "\033[40m";
const char* const kBckRed    = "\033[41m";
const char* const kBckGreen  = "\033[42m";
const char* const kBckYellow = "\033[43m";
const char* const kBckBlue   = "\033[44m";
const char* const kBckMagenta= "\033[45m";
const char* const kBckCyan   = "\033[46m";
const char* const kBckWhite  = "\033[47m";

const char* const kUnderline = "\033[4m";
const char* const kBlink     = "\033[5m";
const char* const kBright    = "\033[1m";
const char* const kDark      = "\033[2m";
}

} // swatch
} // logger

#endif /* __SWATCH_LOGGER_ANSI_HPP__ */

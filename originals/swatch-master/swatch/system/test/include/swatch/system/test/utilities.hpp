/**
 * @file    utilities.hpp
 * @author  Tom Williams
 * @date    May 2015
 */

#ifndef __SWATCH_SYSTEM_TEST_UTILITIES_HPP__
#define __SWATCH_SYSTEM_TEST_UTILITIES_HPP__


// Swatch Headers
#include "swatch/processor/ProcessorStub.hpp"


namespace swatch {
namespace system {
namespace test {

//! Add a dummy set of input and output port stubs to a processor stub, for system construction unit tests
void addRxTxPortStubs(swatch::processor::ProcessorStub& aProcStub);

} // namespace test
} // namespace processor
} // namespace swatch


#endif  /* SWATCH_SYSTEM_TEST_UTILITIES_HPP */


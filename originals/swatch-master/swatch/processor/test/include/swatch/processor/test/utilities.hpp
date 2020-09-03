
#ifndef __SWATCH_PROCESSOR_TEST_UTILITIES_HPP__
#define __SWATCH_PROCESSOR_TEST_UTILITIES_HPP__


#include <string>                       // for string
#include <vector>                       // for vector

// SWATCH headers
#include "swatch/processor/ProcessorStub.hpp"


namespace swatch {
namespace processor {
namespace test {

void pushBackPortStubs(std::vector<ProcessorPortStub>& aPortStubs, const std::string& aName, const std::string& aIndex);

} // namespace test
} // namespace processor
} // namespace swatch


#endif

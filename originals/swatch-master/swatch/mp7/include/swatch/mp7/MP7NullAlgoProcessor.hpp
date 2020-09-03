
#ifndef __SWATCH_MP7_MP7NULLALGOPROCESSOR_HPP__
#define __SWATCH_MP7_MP7NULLALGOPROCESSOR_HPP__


#include "swatch/mp7/MP7Processor.hpp"


namespace swatch {

namespace core {
class AbstractStub;
}

namespace mp7 {

class MP7NullAlgoProcessor: public MP7Processor {
public:
  MP7NullAlgoProcessor(const swatch::core::AbstractStub& aStub);

  ~MP7NullAlgoProcessor();
};

} // namespace mp7
} // namespace swatch


#endif /* __SWATCH_MP7_MP7NULLALGOPROCESSOR_HPP__ */


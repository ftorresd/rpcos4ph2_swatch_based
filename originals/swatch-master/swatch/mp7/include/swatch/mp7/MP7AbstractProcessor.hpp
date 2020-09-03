/*
 * File:   MP7AbstractProcessor.hpp
 * Author: ale
 *
 * Created on December 1, 2015, 8:05 PM
 */

#ifndef __SWATCH_MP7_MP7ABSTRACTPROCESSOR_HPP__
#define __SWATCH_MP7_MP7ABSTRACTPROCESSOR_HPP__


#include "swatch/processor/Processor.hpp"
#include "swatch/mp7/ChannelDescriptorCollection.hpp"


namespace mp7 {
class MP7MiniController;
}

namespace swatch {

namespace core {
class AbstractStub;
}

namespace mp7 {

class MP7AbstractProcessor : public swatch::processor::Processor {
public:
  MP7AbstractProcessor(const swatch::core::AbstractStub& aStub) : swatch::processor::Processor(aStub)
  {
  }
  virtual ~MP7AbstractProcessor() {}

  virtual ::mp7::MP7MiniController& driver() = 0;

  template< class T>
  T& driver();


  const ChannelDescriptorCollection& getRxDescriptors() const;

  const ChannelDescriptorCollection& getTxDescriptors() const;

protected:
  ChannelDescriptorCollection mRxDescriptors;

  ChannelDescriptorCollection mTxDescriptors;
};

}
}

#endif  /* __SWATCH_MP7_MP7ABSTRACTPROCESSOR_HPP__ */


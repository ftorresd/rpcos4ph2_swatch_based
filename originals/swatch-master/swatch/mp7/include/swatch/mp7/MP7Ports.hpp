/**
 * @file    MP7Ports.hpp
 * @author  Alessandro Thea
 * @brief   MP7Processor Rx and Tx ports implementations
 * @date    February 2015
 */

#ifndef __SWATCH_MP7_MP7PORTS_HPP__
#define __SWATCH_MP7_MP7PORTS_HPP__


// Standard headers
#include <stdint.h>                     // for uint32_t
#include <string>                       // for string

// SWATCH headers
#include "swatch/processor/Port.hpp"


namespace  mp7 {
class MP7MiniController;
class MGTRegionNode;
class DatapathNode;
class AlignMonNode;
}

namespace swatch {

namespace core {
template <typename DataType> class SimpleMetric;
}

namespace mp7 {

class MP7RxPort : public processor::InputPort {

public:
  MP7RxPort( const std::string& aId, uint32_t aChannelID, ::mp7::MP7MiniController& aController );
  virtual ~MP7RxPort();

protected:
  virtual void retrieveMetricValues();

//private:
  //! MP7 channel number
  const uint32_t mChannelID;

  //! Reference to the MP7Controller
  ::mp7::MP7MiniController& mDriver;

  //! Reference to Datapath node
  const ::mp7::DatapathNode& mDatapath;

  //! Reference to MGT Node
  const ::mp7::MGTRegionNode& mMgt;

  //! Reference to Alignment Node
  const ::mp7::AlignMonNode& mAlign;

private:
  //! Metric containing packet counter
  core::SimpleMetric<uint32_t>& mMetricPacketCounter;

  //! Metric containing alignment bx value
  core::SimpleMetric<uint32_t>& mMetricAlignBx;

  //! Metric containing alignment cycle
  core::SimpleMetric<uint32_t>& mMetricAlignCycle;

  //! Metric containing alignment cycle
  core::SimpleMetric<uint32_t>& mMetricAlignErrors;
};


class MP7TxPort : public processor::OutputPort {

public:
  MP7TxPort( const std::string& aId, uint32_t aChannelID, ::mp7::MP7MiniController& aController );
  virtual ~MP7TxPort();

protected:
  virtual void retrieveMetricValues();

  ::mp7::MP7MiniController& driver()
  {
    return mDriver;
  }

//private:
  //! Pointer to the main resource
  const uint32_t mChannelID;
  ::mp7::MP7MiniController& mDriver;
  const ::mp7::DatapathNode& mDatapath;
  const ::mp7::MGTRegionNode& mMgt;

};

}
}


#endif /* __SWATCH_MP7_MP7PORTS_HPP__ */

/*
 * File:   MP7NullAlgo.hpp
 * Author: Tom Williams
 *
 */

#ifndef __SWATCH_MP7_MP7NULLALGO_HPP__
#define __SWATCH_MP7_MP7NULLALGO_HPP__


// Swatch Headers
#include "swatch/processor/AlgoInterface.hpp"


namespace mp7 {
class MP7Controller;
}


namespace swatch {
namespace mp7 {

class MP7NullAlgo : public swatch::processor::AlgoInterface {
public:
  MP7NullAlgo( ::mp7::MP7Controller& );
  virtual ~MP7NullAlgo();

  //! Implemented as a no-op for the moment
  virtual void reset();

protected:
  virtual void retrieveMetricValues();
};

} // namespace mp7
} // namespace swatch

#endif

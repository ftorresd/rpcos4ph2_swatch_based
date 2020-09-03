/*
 * File:   TTCInterface.hpp
 * Author: ale
 *
 * Created on September 21, 2015, 12:20 PM
 */

#ifndef __SWATCH_AMC13_TTCINTERFACE_HPP__
#define __SWATCH_AMC13_TTCINTERFACE_HPP__


#include <stdint.h>                     // for uint32_t

#include "swatch/dtm/TTCInterface.hpp"


namespace amc13 {
class AMC13;
} // namespace amc13


namespace swatch {

namespace core {
template <typename DataType> class SimpleMetric;
}

namespace amc13 {

class TTCInterface : public dtm::TTCInterface {
public:
  TTCInterface( ::amc13::AMC13& aDriver );
  virtual ~TTCInterface( );

protected:
  virtual void retrieveMetricValues();

private:
  ::amc13::AMC13& mDriver;

  //! T2: measured TTC clock frequency [Hz]
  core::SimpleMetric<double>& mClockFreq;
  //! T2: TTC BC0 counter
  core::SimpleMetric<uint32_t>& mBC0Counter;
  //! T2: for TTC BC error counter
  core::SimpleMetric<uint32_t>& mBC0Errors;
  //! T2: TTC single-bit error counter
  core::SimpleMetric<uint32_t>& mSingleBitErrors;
  //! T2: TTC double-bit error counter
  core::SimpleMetric<uint32_t>& mDoubleBitErrors;

  //! T1 Resync count
  core::SimpleMetric<uint32_t>& mResyncCount;

};


} // namespace amc13
} // namespace swatch



#endif  /* __SWATCH_AMC13_TTCINTERFACE_HPP__ */


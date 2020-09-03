/*
 * File:   SLinkExpress.hpp
 * Author: ale
 *
 * Created on October 1, 2015, 3:12 PM
 */

#ifndef __SWATCH_AMC13_SLINKEXPRESS_HPP__
#define __SWATCH_AMC13_SLINKEXPRESS_HPP__


#include <stdint.h>                     // for uint32_t

#include "swatch/dtm/SLinkExpress.hpp"


namespace amc13 {
class AMC13;
} // namespace amc13

namespace swatch {

namespace core {
template <typename DataType> class SimpleMetric;
}

namespace amc13 {

class SLinkExpress : public dtm::SLinkExpress {
public:
  SLinkExpress( uint32_t aSfpID, ::amc13::AMC13& aDriver );
  virtual ~SLinkExpress();

protected:
  virtual void retrieveMetricValues();

private:
  ::amc13::AMC13& mDriver;

  core::SimpleMetric<uint32_t>& mRevision;
  core::SimpleMetric<bool>& mCoreInitialised;
  core::SimpleMetric<bool>& mLinkUp;
  core::SimpleMetric<bool>& mBackPressure;
  core::SimpleMetric<uint64_t>& mBackPressureTime;
  core::SimpleMetric<uint32_t>& mEvents;
  core::SimpleMetric<uint32_t>& mEventsSent;
  core::SimpleMetric<uint32_t>& mWords;
  core::SimpleMetric<uint32_t>& mWordsSent;
  core::SimpleMetric<uint32_t>& mPacketsSent;
  core::SimpleMetric<uint32_t>& mPacketsReceived;
};

} // namespace amc13
} // namespace swatch

#endif  /* __SWATCH_AMC13_SLINK_HPP__ */


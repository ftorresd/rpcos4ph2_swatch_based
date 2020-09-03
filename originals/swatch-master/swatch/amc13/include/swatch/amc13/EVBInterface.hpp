/*
 * File:   EVBInterface.hpp
 * Author: ale
 *
 * Created on September 21, 2015, 12:21 PM
 */

#ifndef __SWATCH_AMC13_EVBINTERFACE_HPP__
#define __SWATCH_AMC13_EVBINTERFACE_HPP__


#include <stdint.h>                     // for uint64_t, uint32_t

#include "swatch/core/TTSUtils.hpp"
#include "swatch/dtm/EVBInterface.hpp"


namespace amc13 {
class AMC13;
} // namespace amc13


namespace swatch {

namespace core {
template <typename DataType> class SimpleMetric;
}

namespace amc13 {

class EVBInterface : public dtm::EVBInterface {
public:
  EVBInterface(::amc13::AMC13& aDriver);
  virtual ~EVBInterface();


  virtual void retrieveMetricValues();

private:
  static core::tts::State ttsInternalStatusDecoder( uint32_t aTTSInternalState );

  ::amc13::AMC13& mDriver;

  // T1: event data ready in event buffer of event builders [STATUS.EVB.DATA_READY_MASK]
  // T1: event size in event buffer of event builders [STATUS.EVB.EVENT_SIZE_MASK]

  // T1: L1A overflow warning [STATUS.EVB.OVERFLOW_WARNING]
  core::SimpleMetric<bool>& mOverflowWarning;

  // T1: TTC sync lost (L1A buffer overflow) [STATUS.EVB.SYNC_LOST]
  core::SimpleMetric<bool>& mSyncLost;

  // T1: STATUS.AMC_TTS_STATE
  core::SimpleMetric<core::tts::State>& mAMCsTTSState;

  // T1: STATUS.T1_TTS_STATE
  core::SimpleMetric<core::tts::State>& mTTSState;

  // T1: STATUS.GENERAL.L1A_COUNT
  core::SimpleMetric<uint64_t>& mL1ACount;

  // T1: STATUS.GENERAL.RUN_TIME
  core::SimpleMetric<uint64_t>& mRunTime;

  // T1: STATUS.GENERAL.READY_TIME
  core::SimpleMetric<uint64_t>& mReadyTime;

  // T1: STATUS.GENERAL.BUSY_TIME
  core::SimpleMetric<uint64_t>& mBusyTime;

  // T1: STATUS.GENERAL.SYNC_LOST_TIME
  core::SimpleMetric<uint64_t>& mSyncLostTime;

  // T1: STATUS.GENERAL.OF_WARN_TIME
  core::SimpleMetric<uint64_t>& mOverflowWarningTime;

  // T1:

};

} // namespace amc13
} // namespace swatch


#endif  /* __SWATCH_AMC13_DAQINTERFACE_HPP__ */


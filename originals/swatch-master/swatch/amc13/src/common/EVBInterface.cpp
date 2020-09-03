/*
 * File:   DAQInterface.cpp
 * Author: ale
 *
 * Created on September 21, 2015, 12:21 PM
 */


#include <stdint.h>                     // for uint64_t, uint32_t
#include <string>                       // for operator+, string

#include "swatch/core/MetricConditions.hpp"
#include "swatch/core/TTSUtils.hpp"

#include "amc13/AMC13.hh"
#include "amc13/AMC13Simple.hh"         // for AMC13Simple::Board::T1
#include "swatch/amc13/EVBInterface.hpp"
#include "swatch/amc13/utilities.hpp"


namespace swatch {
namespace amc13 {

EVBInterface::EVBInterface( ::amc13::AMC13& aDriver ) :
  dtm::EVBInterface(),
  mDriver(aDriver),
  mOverflowWarning(registerMetric<bool>("warningOverflow") ),
  mSyncLost(registerMetric<bool>("outOfSync") ),
  mAMCsTTSState(registerMetric<core::tts::State>("amcsTts") ),
  mTTSState(registerMetric<core::tts::State>("tts") ),
  mL1ACount(registerMetric<uint64_t>("l1aCount")),
  mRunTime(registerMetric<uint64_t>("runTime")),
  mReadyTime(registerMetric<uint64_t>("readyTime")),
  mBusyTime(registerMetric<uint64_t>("busyTime")),
  mSyncLostTime(registerMetric<uint64_t>("oosTime")),
  mOverflowWarningTime(registerMetric<uint64_t>("warnTime"))
{

  // Assign Error and Warning conditions
  setErrorCondition(mOverflowWarning,core::EqualCondition<bool>(true));
  setErrorCondition(mSyncLost,core::EqualCondition<bool>(true));

  setConditions(mAMCsTTSState,
                core::EqualCondition<core::tts::State>(core::tts::kError),
                core::NotEqualCondition<core::tts::State>(core::tts::kReady)
               );

  setConditions(mTTSState,
                core::EqualCondition<core::tts::State>(core::tts::kError),
                core::NotEqualCondition<core::tts::State>(core::tts::kReady)
               );
}

EVBInterface::~EVBInterface()
{
}


core::tts::State EVBInterface::ttsInternalStatusDecoder(uint32_t aTTSInternalState)
{
  /*
   * From: http://bucms.bu.edu/twiki/bin/view/BUCMSPublic/AMC13UserManual
   *
   * Bit 4 Disconnected
   * Bit 3 Error
   * Bit 2 Sync Lost
   * Bit 1 Busy
   * Bit 0 Overflow Warning
   *
   * TCDS TTS States: https://twiki.cern.ch/twiki/bin/view/CMS/TcdsTtsStates
  */

  const uint32_t kDisconnected = (1<<4);
  const uint32_t kError = (1<<3);
  const uint32_t kSyncLost = (1<<2);
  const uint32_t kBusy = (1<<1);
  const uint32_t kOverflowWarning = 1;
  const uint32_t kReady = 0;
  switch (aTTSInternalState) {
    case kReady:
      return core::tts::kReady;
    case kBusy:
      return core::tts::kBusy;
    case kOverflowWarning:
      return core::tts::kWarning;
    case kSyncLost:
      return core::tts::kOutOfSync;
    case kError:
      return core::tts::kError;
    case kDisconnected:
      return core::tts::kDisconnected;
    default:
      return core::tts::kUnknown;
  }
}

void
EVBInterface::retrieveMetricValues()
{
  using ::amc13::AMC13;

  const std::string lPrefixStat = "STATUS.";
  const std::string lPrefixEvb = "STATUS.EVB.";
  const std::string lPrefixGeneral = "STATUS.GENERAL.";
  setMetricValue<>(mOverflowWarning, (bool)mDriver.read(AMC13::T1,lPrefixEvb+"OVERFLOW_WARNING"));
  setMetricValue<>(mSyncLost, (bool)mDriver.read(AMC13::T1,lPrefixEvb+"SYNC_LOST"));
  setMetricValue<>(mAMCsTTSState, ttsInternalStatusDecoder(mDriver.read(AMC13::T1,lPrefixStat+"AMC_TTS_STATE")));
  setMetricValue<>(mTTSState, static_cast<core::tts::State>(mDriver.read(AMC13::T1,lPrefixStat+"T1_TTS_STATE")));
  setMetricValue<>(mL1ACount, read64bCounter(mDriver, AMC13::T1,lPrefixGeneral+"L1A_COUNT"));
  setMetricValue<>(mRunTime, read64bCounter(mDriver, AMC13::T1,lPrefixGeneral+"RUN_TIME"));
  setMetricValue<>(mReadyTime, read64bCounter(mDriver, AMC13::T1,lPrefixGeneral+"READY_TIME"));
  setMetricValue<>(mBusyTime, read64bCounter(mDriver, AMC13::T1,lPrefixGeneral+"BUSY_TIME"));
  setMetricValue<>(mSyncLostTime, read64bCounter(mDriver, AMC13::T1,lPrefixGeneral+"SYNC_LOST_TIME"));
  setMetricValue<>(mOverflowWarningTime, read64bCounter(mDriver, AMC13::T1,lPrefixGeneral+"OF_WARN_TIME"));


}


} // namespace amc13
} // namespace swatch



/*
 * File:   AMCLink.cpp
 * Author: ale
 *
 * Created on October 5, 2015, 5:00 PM
 */

#include "swatch/amc13/AMCPort.hpp"


// Standard headers
#include <string>                       // for operator+, string

// SWATCH headers
#include "swatch/core/SimpleMetric.hpp"
#include "swatch/core/MetricConditions.hpp"
#include "swatch/action/MonitoringSetting.hpp"
#include "swatch/core/utilities.hpp"
#include "swatch/core/TTSUtils.hpp"

// AMC13 Headers
#include "amc13/AMC13.hh"
#include "amc13/AMC13Simple.hh"         // for AMC13Simple::Board::T1
#include "swatch/amc13/utilities.hpp"


//          AMC_Links|       AMC08|
//                 --|------------|
// AMC13_BCN_MISMATCH| 0x 112E8916|
//       AMC13_EVENTS| 0x 112E8916|
//     AMC13_LINK_VER| 0x       11|
// AMC13_ORN_MISMATCH| 0x 112E8916|
//        AMC13_WORDS| 0xAABE724AA|
//   AMC_BCN_MISMATCH| 0x 112E8911|
//         AMC_EVENTS| 0x 112E8911|
//        AMC_HEADERS| 0x 112E8907|
//     AMC_LINK_READY|           1|
//       AMC_LINK_VER| 0x       10|
//   AMC_ORN_MISMATCH| 0x 112E8911|
//       AMC_TRAILERS| 0x 112E8907|
//            AMC_TTS|   RDY (0x8)|
//          AMC_WORDS| 0xAABE71B59|
//           BC0_LOCK|           0|
//         BC0_LOCKED|           0|
//         EVB_EVENTS| 0x 112E8916|
//            LINK_OK|           1|
//    LINK_VERS_WRONG|           1|
//       MISMATCH_BCN|           0|
//       MISMATCH_ORN|           0|
//       MISMATCH_TTS|           8|
//           TTC_LOCK|           0|
//            TTS_RAW|   DIS (0x0)|
//         TTS_UPDATE| 0x12B9B24C9|
// AMC_EVN_ERRORS
// CRC_ERR
// BP_ERR
namespace swatch {
namespace amc13 {

AMCPort::AMCPort(uint32_t aSlot, ::amc13::AMC13& aDriver) :
  dtm::AMCPort(aSlot),
  mDriver(aDriver),
  mAMCLinkRevision(registerMetric<uint32_t>("amcRevision") ),
  mLinkRevisionWrong(registerMetric<bool>("revisionWrong") ),
  mLinkReady(registerMetric<bool>("ready") ),
  mLinkOK(registerMetric<bool>("ok") ),
  mAMC13LinkRevision(registerMetric<uint32_t>("amc13Revision") ),
  mTTS(registerMetric<core::tts::State>("tts") ) ,
  mAMCEvents(registerMetric<uint64_t>("amcEvents") ) ,
  mAMCHeaders(registerMetric<uint64_t>("amcHeaders") ) ,
  mAMCTrailers(registerMetric<uint64_t>("amcTrailers") ) ,
  mAMCWords(registerMetric<uint64_t>("amcWords") ) ,

  mAMC13Events(registerMetric<uint64_t>("amc13Events") ) ,
  mAMC13Words(registerMetric<uint64_t>("amc13Words") ) ,

  mAMCBcnMismatch(registerMetric<uint64_t>("amcBcnMismatch") ) ,
  mAMCOrnMismatch(registerMetric<uint64_t>("amcOrnMismatch") ) ,
  mAMC13BcnMismatch(registerMetric<uint64_t>("amc13BcnMismatch") ) ,
  mAMC13OrnMismatch(registerMetric<uint64_t>("amc13OrnMismatch") ) ,

  mEventCounterJumps(registerMetric<uint64_t>("eventCounterJumps") ) ,
  mCrcErrors(registerMetric<uint32_t>("crcErrors") )
{

  // Assign Error and Warning conditions
  setWarningCondition(mLinkRevisionWrong,core::EqualCondition<bool>(true) );
  setErrorCondition(mLinkReady,core::EqualCondition<bool>(false) );
  setErrorCondition(mLinkOK,core::EqualCondition<bool>(false));

  // Error if OOS, Warning if not Ready
  setConditions(mTTS,
                core::EqualCondition<core::tts::State>(core::tts::kError),
                core::NotEqualCondition<core::tts::State>(core::tts::kReady));


  setErrorCondition(mCrcErrors,core::NotEqualCondition<uint32_t>(0x0));

// See ticket #1820
//  setErrorCondition(mAMCBcnMismatch,core::NotEqualCondition<uint64_t>(0x0));
//  setErrorCondition(mAMC13BcnMismatch,core::NotEqualCondition<uint64_t>(0x0));

  //  setWarningCondition(mAMCOrnMismatch,core::NotEqualCondition<uint64_t>(0x0));
//  setWarningCondition(mAMC13OrnMismatch,core::NotEqualCondition<uint64_t>(0x0));

  setWarningCondition(mEventCounterJumps,core::NotEqualCondition<uint64_t>(0x0));

//  mEventErrors.setMonitoringStatus(core::monitoring::kNonCritical);
  mAMCOrnMismatch.setMonitoringStatus(core::monitoring::kNonCritical);
  mAMC13OrnMismatch.setMonitoringStatus(core::monitoring::kNonCritical);
}

AMCPort::~AMCPort()
{
}


// uint64_t
// AMCPort::read64bCounter(::amc13::AMC13::Board aBoard, const std::string& aName) {
//   return
//       ((uint64_t)mDriver.read(aBoard,aName+"_HI") << 32) +
//       mDriver.read(aBoard,aName+"_LO");
// }

void AMCPort::retrieveMetricValues()
{

  using ::amc13::AMC13;

  const std::string lPrefixAmc = swatch::core::strPrintf("STATUS.AMC%02d.", getSlot());
  const std::string lPrefixLink = swatch::core::strPrintf("STATUS.AMC%02d.LINK.", getSlot());
  const std::string lPrefixCtrs = swatch::core::strPrintf("STATUS.AMC%02d.COUNTERS.", getSlot());

  setMetricValue<>(mAMCLinkRevision, mDriver.read(AMC13::T1,lPrefixLink+"AMC_LINK_VER"));

  setMetricValue<>(mLinkRevisionWrong, (bool)mDriver.read(AMC13::T1,lPrefixAmc+"LINK_VERS_WRONG_MASK"));

  setMetricValue<>(mLinkReady, (bool)mDriver.read(AMC13::T1,lPrefixAmc+"AMC_LINK_READY_MASK"));

  setMetricValue<>(mLinkOK, (bool)mDriver.read(AMC13::T1,lPrefixAmc+"LOSS_OF_SYNC_MASK"));

  setMetricValue<>(mAMC13LinkRevision, mDriver.read(AMC13::T1,lPrefixLink+"AMC13_LINK_VER"));

  setMetricValue<>(mTTS, static_cast<core::tts::State>(mDriver.read(AMC13::T1,lPrefixLink+"AMC_TTS")));

  // Counters
  setMetricValue<>(mAMCEvents, read64bCounter(mDriver, AMC13::T1,lPrefixCtrs+"RECEIVED_EVENT_COUNTER"));

  setMetricValue<>(mAMCHeaders, read64bCounter(mDriver, AMC13::T1,lPrefixCtrs+"HEADERS_AT_LINK_INPUT"));

  setMetricValue<>(mAMCTrailers, read64bCounter(mDriver, AMC13::T1,lPrefixCtrs+"TRAILERS_AT_LINK_INPUT"));

  setMetricValue<>(mAMCWords, read64bCounter(mDriver, AMC13::T1,lPrefixCtrs+"WORDS_AT LINK_INPUT"));

  setMetricValue<>(mAMC13Events, read64bCounter(mDriver, AMC13::T1,lPrefixCtrs+"AMC13_AMC_EVENTS"));

  setMetricValue<>(mAMC13Words, read64bCounter(mDriver, AMC13::T1,lPrefixCtrs+"WORD_COUNTER"));

  setMetricValue<>(mAMCBcnMismatch, read64bCounter(mDriver, AMC13::T1,lPrefixCtrs+"BCN_MISMATCH_COUNTER"));

  setMetricValue<>(mAMCOrnMismatch, read64bCounter(mDriver, AMC13::T1,lPrefixCtrs+"ORN_MISMATCH_COUNTER"));

  setMetricValue<>(mAMC13BcnMismatch, read64bCounter(mDriver, AMC13::T1,lPrefixCtrs+"AMC13_BCN_MISMATCH"));

  setMetricValue<>(mAMC13OrnMismatch, read64bCounter(mDriver, AMC13::T1,lPrefixCtrs+"AMC13_ORN_MISMATCH"));

  setMetricValue<>(mEventCounterJumps, read64bCounter(mDriver, AMC13::T1,lPrefixCtrs+"EVN_ERRORS_AT_LINK_INPUT"));

  setMetricValue<>(mCrcErrors, mDriver.read(AMC13::T1,lPrefixAmc+"BP_CRC_ERR"));


}




} // namespace amc13
} // namespace swatch

/*
 * File:   SLink.cpp
 * Author: ale
 *
 * Created on October 1, 2015, 3:13 PM
 */


//     Slink_Express|       SFP0|
//                --|-----------|
// BACKPRESSURE_TIME| 0x 170BC7E|
//            BLOCKS| 0x   32F80|
//       BLOCKS_FREE|          1|
//       BLOCKS_SENT| 0x   33EBE|
//  BLOCK_READY_MASK|          0|
//   BLOCK_USED_MASK|          0|
//       CORE_STATUS| 0x80000000|
//            EVENTS| 0x   32F80|
//       EVENTS_SENT| 0x   32F80|
//    EVENT_LGTH_ERR| 0x    A799|
//    EVENT_LGTH_SUM| 0x 3A25B78|
//          EXPERT_1|          0|
//          EXPERT_2| 0x   2F4BD|
//          FSM_IDLE|          1|
//       INITIALIZED|          1|
//       LINKNOTFULL|          0|
//           LINK_UP|          1|
//   NO_BACKPRESSURE|          1|
//      PACKETS_RCVD| 0x   3373B|
//      PACKETS_SENT| 0x 7724E55|
//          REVISION| 0x5E000008|
//      SFP_LSC_DOWN|          0|
//          STATUS_D| 0x70000001|
//         SYNC_LOSS| 0x2EF05FC7|
//         TEST_MODE|          0|
//             WORDS| 0x 3A25B78|
//        WORDS_SENT| 0x 3A25B78|

#include "swatch/amc13/SLinkExpress.hpp"


#include <string>                       // for operator+, string

#include "swatch/core/MetricConditions.hpp"
#include "swatch/core/utilities.hpp"

// AMC13 Headers
#include "amc13/AMC13.hh"
#include "amc13/AMC13Simple.hh"
#include "swatch/amc13/utilities.hpp"         // for AMC13Simple::Board::T1


namespace swatch {
namespace amc13 {

// --------------------------------------------------------
SLinkExpress::SLinkExpress( uint32_t aSfpId, ::amc13::AMC13& aDriver ) :
  dtm::SLinkExpress(aSfpId),
  mDriver(aDriver),
  mRevision(registerMetric<uint32_t>("revision") ),
  mCoreInitialised(registerMetric<bool>("initialized") ),
  mLinkUp(registerMetric<bool>("linkUp") ),
  mBackPressure(registerMetric<bool>("backPressure") ),
  mBackPressureTime(registerMetric<uint64_t>("backPressureTime") ) ,
  mEvents(registerMetric<uint32_t>("events") ),
  mEventsSent(registerMetric<uint32_t>("eventsSent") ),
  mWords(registerMetric<uint32_t>("words") ),
  mWordsSent(registerMetric<uint32_t>("wordsSent") ),
  mPacketsSent(registerMetric<uint32_t>("packetsSent") ),
  mPacketsReceived(registerMetric<uint32_t>("packetsReceived") )
{

  // Error if the core is not initialized
  setErrorCondition(mCoreInitialised,core::EqualCondition<bool>(false));

  // Warning if link up is not declared
  setWarningCondition(mLinkUp, core::EqualCondition<bool>(false));

  // Warning if there is backpressure
  setWarningCondition(mBackPressure,core::EqualCondition<bool>(true));
}


// --------------------------------------------------------
SLinkExpress::~SLinkExpress()
{
}


// --------------------------------------------------------
void SLinkExpress::retrieveMetricValues()
{
  using ::amc13::AMC13;

  const std::string lPrefixA = swatch::core::strPrintf("STATUS.SFP.SFP%d.LSC.", mSfpID);
  const std::string lPrefixB = swatch::core::strPrintf("STATUS.LSC.SFP%d.", mSfpID);
  setMetricValue<>(mRevision, mDriver.read(AMC13::T1,lPrefixA+"VERSION"));
  setMetricValue<>(mCoreInitialised, (bool)mDriver.read(AMC13::T1,lPrefixA+"INITIALIZED"));
  setMetricValue<>(mLinkUp, (bool)mDriver.read(AMC13::T1,lPrefixA+"LINK_UP"));
  setMetricValue<>(mBackPressure, !(bool)mDriver.read(AMC13::T1,lPrefixA+"NO_BACKPRESSURE"));
  setMetricValue<>(mBackPressureTime, read64bCounter(mDriver,AMC13::T1,lPrefixA+"BACKP_COUNTER"));
  setMetricValue<>(mEvents, mDriver.read(AMC13::T1,lPrefixB+"BUILT_EVENT_COUNT"));
  setMetricValue<>(mEventsSent, mDriver.read(AMC13::T1,lPrefixA+"EVENTS_SENT"));
  setMetricValue<>(mWords, mDriver.read(AMC13::T1,lPrefixB+"WORD_COUNT"));
  setMetricValue<>(mWordsSent, mDriver.read(AMC13::T1,lPrefixA+"WORDS_SENT"));
  setMetricValue<>(mPacketsSent, mDriver.read(AMC13::T1,lPrefixA+"PACKETS_SENT"));
  setMetricValue<>(mPacketsReceived, mDriver.read(AMC13::T1,lPrefixA+"PACKETS_RCVD"));
  // "PACKETS_SENT";
  // "PACKETS_RCVD";
  // "EVENT_LGTH_ERR";
  // "EVENT_LGTH_SUM";

}


} // namespace amc13
} // namespace swatch

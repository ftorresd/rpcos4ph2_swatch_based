/*
 * File:   AMCPort.hpp
 * Author: ale
 *
 * Created on October 5, 2015, 5:00 PM
 */

#ifndef __SWATCH_AMC13_AMCPORT_HPP__
#define __SWATCH_AMC13_AMCPORT_HPP__


#include <stdint.h>                     // for uint64_t, uint32_t

#include "swatch/dtm/AMCPort.hpp"
#include "swatch/core/TTSUtils.hpp"


namespace amc13 {
class AMC13;
} // namespace amc13


namespace swatch {

namespace core {
template <typename DataType> class SimpleMetric;
}

namespace amc13 {

class AMCPort : public dtm::AMCPort {
public:
  AMCPort( uint32_t aSlot, ::amc13::AMC13& aDriver );
  virtual ~AMCPort();


protected:
  virtual void retrieveMetricValues();

  ::amc13::AMC13& mDriver;

  //! AMC_Links: AMC_LINK_VER
  core::SimpleMetric<uint32_t>& mAMCLinkRevision;

  //! AMC_Links: LINK_VERS_WRONG
  core::SimpleMetric<bool>& mLinkRevisionWrong;

  //! AMC_Links: AMC_LINK_READY
  core::SimpleMetric<bool>& mLinkReady;

  //! AMC_Links: LINK_OK
  core::SimpleMetric<bool>& mLinkOK;

  //! AMC_Links: AMC13_LINK_VER
  core::SimpleMetric<uint32_t>& mAMC13LinkRevision;

  //! AMC_Links: AMC_TTS
  core::SimpleMetric<core::tts::State>& mTTS;

  //! AMC_Links: AMC_EVENTS
  core::SimpleMetric<uint64_t>& mAMCEvents;

  //! AMC_Links: AMC_HEADERS
  core::SimpleMetric<uint64_t>& mAMCHeaders;

  //! AMC_Links: AMC_TRAILERS
  core::SimpleMetric<uint64_t>& mAMCTrailers;

  //! AMC_Links: AMC_WORDS
  core::SimpleMetric<uint64_t>& mAMCWords;

  //! AMC_Links: AMC13_EVENTS
  core::SimpleMetric<uint64_t>& mAMC13Events;

  //! AMC_Links: AMC13_WORDS
  core::SimpleMetric<uint64_t>& mAMC13Words;

  //! AMC_Links: AMC_BCN_MISMATCH
  core::SimpleMetric<uint64_t>& mAMCBcnMismatch;

  //! AMC_Links: AMC_ORN_MISMATCH
  core::SimpleMetric<uint64_t>& mAMCOrnMismatch;

  //! AMC_Links: AMC13_BCN_MISMATCH
  core::SimpleMetric<uint64_t>& mAMC13BcnMismatch;

  //! AMC_Links: AMC13_ORN_MISMATCH
  core::SimpleMetric<uint64_t>& mAMC13OrnMismatch;

  //! AMC_Links: AMC_EVN_ERRORS
  core::SimpleMetric<uint64_t>& mEventCounterJumps;

  //! AMC_Links: CRC_ERR
  core::SimpleMetric<uint32_t>& mCrcErrors;

};

} // namespace amc13
} // namespace swatch

#endif  /* __SWATCH_AMC13_AMCPORT_HPP__ */


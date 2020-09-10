
#include "rpcos4ph2/dummy/DummyAMC13Driver.hpp"

// SWATCH Headers
#include "swatch/core/exception.hpp"

// Boost Headers
#include "boost/lexical_cast.hpp"


namespace rpcos4ph2 {
namespace dummy {


DummyAMC13Driver::DummyAMC13Driver() :
  mVec(2 * 2 * (1024 + 256) * 1024, 0x0),
  mClkTtcState(kError),
  mEvbState(kError),
  mSLinkState(kError),
  mAMCPortState(kError),
  mRunning(false),
  mFedId (0xFFFF)
{
  reboot();
}


DummyAMC13Driver::~DummyAMC13Driver()
{
}


DummyAMC13Driver::TTCStatus DummyAMC13Driver::readTTCStatus() const
{
  TTCStatus lStatus;
  lStatus.bc0Counter = mRunning ? 42 : 0;

  switch (mClkTtcState) {
    // Good & Warning : Almost all metric values are the same
    case ComponentState::kGood :
    case ComponentState::kWarning :
      lStatus.clockFreq = 40.0e6;
      lStatus.errCountBC0 = 0;
      lStatus.errCountSingleBit = 0;
      lStatus.errCountDoubleBit = 0;
      lStatus.warningSign = (mClkTtcState == ComponentState::kWarning);
      break;
    // Error : Incorrect clock freq; error counters non-zero
    case ComponentState::kError :
      lStatus.clockFreq = 15.0e6;
      lStatus.errCountBC0 = 5;
      lStatus.errCountSingleBit = 100;
      lStatus.errCountDoubleBit = 10;
      lStatus.warningSign = true;
      break;
    // Hardware unreachable : Driver usually throws
    case ComponentState::kNotReachable :
      XCEPT_RAISE(swatch::core::RuntimeError,"Problem communicating with AMC13 (TTC block).");
  }

  return lStatus;
}


uint16_t DummyAMC13Driver::readFedId() const
{
  return mFedId;
}


DummyAMC13Driver::EventBuilderStatus DummyAMC13Driver::readEvbStatus() const
{
  EventBuilderStatus lStatus;
  lStatus.outOfSync = (mEvbState == ComponentState::kError);
  lStatus.ttsWarning = (mEvbState != ComponentState::kGood);
  lStatus.l1aCount = mRunning ? 42 : 0;

  if (mEvbState == ComponentState::kNotReachable)
    XCEPT_RAISE(swatch::core::RuntimeError,"Problem communicating with AMC13 (event builder).");

  return lStatus;
}


DummyAMC13Driver::SLinkStatus DummyAMC13Driver::readSLinkStatus() const
{
  SLinkStatus lStatus;
  lStatus.coreInitialised = (mSLinkState != ComponentState::kError);
  lStatus.backPressure = (mSLinkState != ComponentState::kGood);
  lStatus.wordsSent = mRunning ? 42000 : 0;
  lStatus.packetsSent = mRunning ? 42 : 0;

  if (mSLinkState == ComponentState::kNotReachable)
    XCEPT_RAISE(swatch::core::RuntimeError,"Problem communicating with AMC13 (event builder).");

  return lStatus;
}


DummyAMC13Driver::AMCPortStatus DummyAMC13Driver::readAMCPortStatus(uint32_t aSlotId) const
{
  AMCPortStatus lStatus;
  lStatus.outOfSync = (mAMCPortState == ComponentState::kError);
  lStatus.ttsWarning = (mAMCPortState != ComponentState::kGood);
  lStatus.amcEventCount = mRunning ? 42 : 0;

  if (mAMCPortState == ComponentState::kNotReachable)
    XCEPT_RAISE(swatch::core::RuntimeError,"Problem communicating with AMC13 (AMC backplane port " + boost::lexical_cast<std::string>(aSlotId) + ").");

  return lStatus;
}


void DummyAMC13Driver::reboot()
{
  mClkTtcState = kError;
  mEvbState = kError;
  mSLinkState = kError;
  mAMCPortState = kError;
  mRunning = false;
  mFedId = 0;
}


void DummyAMC13Driver::reset()
{
  mClkTtcState = kGood;
  mEvbState = kError;
  mSLinkState = kError;
  mAMCPortState = kError;
  mRunning = false;
  mFedId = 0;
}


void DummyAMC13Driver::forceClkTtcState(ComponentState aNewState)
{
  mClkTtcState = aNewState;
}


void DummyAMC13Driver::configureEvb(uint16_t aFedId)
{
  if (mClkTtcState == kError)
    XCEPT_RAISE(swatch::core::RuntimeError,"Couldn't configure event builder - no clock!");
  else {
    mEvbState = kGood;
    mFedId = aFedId;
  }
}


void DummyAMC13Driver::forceEvbState(ComponentState aNewState)
{
  mEvbState = aNewState;
}


void DummyAMC13Driver::configureSLink(uint16_t aFedId)
{
  if (mClkTtcState == kError)
    XCEPT_RAISE(swatch::core::RuntimeError,"Couldn't configure event builder - no clock!");
  else {
    mSLinkState = kGood;
    mFedId = aFedId;
  }
}


void DummyAMC13Driver::forceSLinkState(ComponentState aNewState)
{
  mSLinkState = aNewState;
}


void DummyAMC13Driver::configureAMCPorts()
{
  if (mClkTtcState == kError)
    XCEPT_RAISE(swatch::core::RuntimeError,"Couldn't configure AMC port - no clock!");
  else
    mAMCPortState = kGood;
}


void DummyAMC13Driver::forceAMCPortState(ComponentState aNewState)
{
  mAMCPortState = aNewState;
}


void DummyAMC13Driver::startDaq()
{
  if (mClkTtcState == kError)
    XCEPT_RAISE(swatch::core::RuntimeError,"Couldn't start run - no clock!");
  else if (mEvbState == kError)
    XCEPT_RAISE(swatch::core::RuntimeError,"Couldn't start run - my event builder isn't configured!");
  else if (mSLinkState == kError)
    XCEPT_RAISE(swatch::core::RuntimeError,"Couldn't start run - my SLink express block isn't configured!");
  else
    mRunning = true;
}


void DummyAMC13Driver::stopDaq()
{
  if (!mRunning)
    XCEPT_RAISE(swatch::core::RuntimeError,"Couldn't stop run - not currently in run!");
  else
    mRunning = false;
}


} // namespace dummy
} // namespace rpcos4ph2

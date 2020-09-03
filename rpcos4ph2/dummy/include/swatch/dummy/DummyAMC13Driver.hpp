
#ifndef __SWATCH_DUMMY_DUMMYAMC13DRIVER_HPP__
#define __SWATCH_DUMMY_DUMMYAMC13DRIVER_HPP__


#include <stdint.h>
#include <vector>

#include "swatch/dummy/ComponentState.hpp"


namespace swatch {
namespace dummy {


class DummyAMC13Driver {
public:

  // Forward declarations
  struct TTCStatus;
  struct EventBuilderStatus;
  struct SLinkStatus;
  struct AMCPortStatus;

  DummyAMC13Driver();

  ~DummyAMC13Driver();

  TTCStatus readTTCStatus() const;

  uint16_t readFedId() const;

  EventBuilderStatus readEvbStatus() const;

  SLinkStatus readSLinkStatus() const;

  AMCPortStatus readAMCPortStatus(uint32_t aSlotId) const;

  void reboot();

  void reset();

  void forceClkTtcState(ComponentState aNewState);

  void configureEvb(uint16_t aFedId);

  void forceEvbState(ComponentState aNewState);

  void configureSLink(uint16_t aFedId);

  void forceSLinkState(ComponentState aNewState);

  void configureAMCPorts();

  void forceAMCPortState(ComponentState aNewState);

  void startDaq();

  void stopDaq();

private:

  std::vector<uint8_t> mVec;

  ComponentState mClkTtcState;
  ComponentState mEvbState;
  ComponentState mSLinkState;
  ComponentState mAMCPortState;
  bool mRunning;

  uint16_t mFedId;

public:
  struct TTCStatus {
    double clockFreq;
    uint32_t bc0Counter;
    uint32_t errCountBC0;
    uint32_t errCountSingleBit;
    uint32_t errCountDoubleBit;
    bool warningSign;
  };

  struct EventBuilderStatus {
    bool outOfSync;
    bool ttsWarning;
    uint64_t l1aCount;
  };

  struct SLinkStatus {
    bool coreInitialised;
    bool backPressure;
    uint32_t wordsSent;
    uint32_t packetsSent;
  };

  struct AMCPortStatus {
    bool outOfSync;
    bool ttsWarning;
    uint64_t amcEventCount;
  };

};


} // namespace dummy
} // namespace swatch

#endif  /* SWATCH_DUMMY_DUMMYAMC13DRIVER_HPP */


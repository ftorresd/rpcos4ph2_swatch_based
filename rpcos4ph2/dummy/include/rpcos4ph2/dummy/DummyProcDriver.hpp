
#ifndef _RPCOS4PH2_DUMMY_DUMMYPROCDRIVER_HPP__
#define _RPCOS4PH2_DUMMY_DUMMYPROCDRIVER_HPP__


#include <stdint.h>
#include <string>
#include <vector>

#include "rpcos4ph2/dummy/ComponentState.hpp"
#include "swatch/core/TTSUtils.hpp"


namespace rpcos4ph2 {
namespace dummy {


class DummyProcDriver {
public:
  // Forward declarations
  struct TTCStatus;
  struct ReadoutStatus;
  struct RxPortStatus;
  struct TxPortStatus;
  struct AlgoStatus;

  DummyProcDriver();

  virtual ~DummyProcDriver();

  uint64_t getFirmwareVersion() const;

  TTCStatus getTTCStatus() const;

  ReadoutStatus getReadoutStatus() const;

  RxPortStatus getRxPortStatus(uint32_t channelId) const;

  TxPortStatus getTxPortStatus(uint32_t channelId) const;

  AlgoStatus getAlgoStatus() const;

  void reboot();

  void reset();

  void forceClkTtcState(ComponentState aNewState);

  void configureRxPorts();

  void forceRxPortsState(ComponentState aNewState);

  void configureTxPorts();

  void forceTxPortsState(ComponentState aNewState);

  void configureReadout();

  void forceReadoutState(ComponentState aNewState);

  void configureAlgo();

  void forceAlgoState(ComponentState aNewState);

private:

  std::vector<uint8_t> mVec;

  ComponentState mClkState;
  ComponentState mTxState;
  ComponentState mRxState;
  ComponentState mReadoutState;
  ComponentState mAlgoState;

public:
  struct TTCStatus {
    uint32_t bunchCounter;
    uint32_t eventCounter;
    uint32_t orbitCounter;
    bool clk40Locked;
    bool clk40Stopped;
    bool bc0Locked;
    uint32_t errSingleBit;
    uint32_t errDoubleBit;
    bool warningSign;
  };

  struct ReadoutStatus {
    ReadoutStatus(bool aAMCCoreReady, swatch::core::tts::State aTTSState, uint32_t aEventCounter) :
      amcCoreReady(aAMCCoreReady),
      ttsState(aTTSState),
      eventCounter(aEventCounter)
    {}
    bool amcCoreReady;
    swatch::core::tts::State ttsState;
    uint32_t eventCounter;
  };

  struct RxPortStatus {
    RxPortStatus(bool aIsLocked, bool aIsAligned, uint32_t aCrcErrCount, bool aWarningSign) :
      isLocked(aIsLocked),
      isAligned(aIsAligned),
      crcErrCount(aCrcErrCount),
      warningSign(aWarningSign)
    {}
    bool isLocked;
    bool isAligned;
    uint32_t crcErrCount;
    bool warningSign;
  };

  struct TxPortStatus {
    TxPortStatus(bool aIsOperating, bool aWarningSign) :
      isOperating(aIsOperating),
      warningSign(aWarningSign)
    {}
    bool isOperating;
    bool warningSign;
  };

  struct AlgoStatus {
    AlgoStatus(float aRateCounterA, float aRateCounterB) :
      rateCounterA(aRateCounterA),
      rateCounterB(aRateCounterB)
    {}
    float rateCounterA;
    float rateCounterB;
  };
};


} // namespace dummy
} // namespace rpcos4ph2

#endif  /* SWATCH_DUMMY_DUMMYPROCESSOR_HPP */


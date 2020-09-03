#ifndef __SWATCH_MP7_MP7PROCESSOR_HPP__
#define __SWATCH_MP7_MP7PROCESSOR_HPP__


#include <stdint.h>                     // for uint64_t
#include <string>                       // for string

#include "swatch/processor/ProcessorStub.hpp"  // for ProcessorStub

#include "mp7/MP7Controller.hpp"

#include "swatch/mp7/MP7AbstractProcessor.hpp"


namespace swatch {

namespace core {
class AbstractStub;
}

namespace mp7 {

// Temporary class
struct MP7ClockMode {
  std::string clkCfg;
  std::string clk40Src;
  std::string ttcCfg;
};

class MP7Processor : public MP7AbstractProcessor {
public:
  struct FirmwareVersion {
    uint8_t design;
    uint8_t infraMajor;
    uint8_t infraMinor;
    uint8_t infraPatch;
    uint32_t algo;
  };

  MP7Processor(const swatch::core::AbstractStub& aStub);
  virtual ~MP7Processor();

  virtual FirmwareVersion retrieveFirmwareVersion() const;

  virtual std::string firmwareInfo() const;

  virtual ::mp7::MP7Controller& driver();


private:
  ::mp7::MP7Controller* mDriver;

  //! Algo firmware version metric
  core::SimpleMetric<uint32_t>& mMetricAlgoFirmwareVersion;

  void buildPorts( const processor::ProcessorStub& aStub );


protected:
  virtual void retrieveMetricValues();

  struct CmdIds {
    static const std::string kUploadFw;
    static const std::string kDeleteFw;
    static const std::string kChkFwRevision;
    static const std::string kReboot;
    static const std::string kHardReset;
    static const std::string kScanSD;
    static const std::string kReset;
    static const std::string kClearCounters;
    static const std::string kZeroInputs;
    static const std::string kSetId;
    static const std::string kCfgRxMGTs;
    static const std::string kCfgTxMGTs;
    static const std::string kAlignMGTs;
    static const std::string kAutoAlignMGTs;
    static const std::string kCfgRxBuffers;
    static const std::string kCfgTxBuffers;
    static const std::string kCaptureBuffers;
    static const std::string kSaveRxBuffers;
    static const std::string kSaveTxBuffers;
    static const std::string kCfgLatencyRxBuffers;
    static const std::string kCfgLatencyTxBuffers;
    static const std::string kCfgEasyRxLatency;
    static const std::string kCfgEasyTxLatency;
    static const std::string kCfgHdrFormatter;
    static const std::string kCfgDVFormatter;
    static const std::string kSetupReadout;
    static const std::string kLoadReadoutMenu;
    static const std::string kSetupZS;
    static const std::string kLoadZSMenu;
  };




private:
  //  static action::Command* createLoadReadoutMenuCommand(const std::string& aId, const ::mp7::MP7Controller& aController);
};

} // namespace mp7
} // namespace swatch


#endif


/**
 * @file    MP7Processor.cpp
 * @author  Alessandro Thea
 * @brief   MP7 board processor implementation
 * @date    06/11/14
 */

#include "swatch/mp7/MP7Processor.hpp"


// Standard headers
#include <ios>
#include <ostream>                      // for basic_ostream, operator<<, etc
#include <vector>                       // for vector

// log4cplus headers
#include "log4cplus/loggingmacros.h"

// SWATCH headers
#include "swatch/core/Factory.hpp"
#include "swatch/processor/PortCollection.hpp"
#include "swatch/processor/ProcessorStub.hpp"

// uHAL headers
#include "uhal/ConnectionManager.hpp"
#include "uhal/HwInterface.hpp"
#include "uhal/Node.hpp"                // for Node
#include "uhal/ValMem.hpp"              // for ValWord

// MP7 headers
#include "mp7/ChannelManager.hpp"       // for ChannelManager
#include "mp7/CtrlNode.hpp"             // for CtrlNode
#include "mp7/DatapathDescriptor.hpp"   // for DatapathDescriptor
#include "mp7/definitions.hpp"          // for RegionInfo, etc
#include "mp7/MP7Controller.hpp"

// SWATCH MP7 headers
#include "swatch/mp7/ChannelDescriptor.hpp"  // for ChannelDescriptor
#include "swatch/mp7/ChannelDescriptorCollection.hpp"
#include "swatch/mp7/MP7MiniPODPorts.hpp"
#include "swatch/mp7/MP7ReadoutInterface.hpp"
#include "swatch/mp7/MP7TTCInterface.hpp"
#include "swatch/mp7/MP7MiniPODPorts.hpp"

// SWATCH MP7 Buffers commands
#include "swatch/mp7/cmds/ZeroInputs.hpp"
#include "swatch/mp7/cmds/CaptureBuffers.hpp"
#include "swatch/mp7/cmds/ConfigureBuffers.hpp"
#include "swatch/mp7/cmds/EasyLatency.hpp"
#include "swatch/mp7/cmds/LatencyBuffers.hpp"
#include "swatch/mp7/cmds/SaveBuffersToFile.hpp"

// SWATCH MP7 Formatter commands
#include "swatch/mp7/cmds/HeaderFormatter.hpp"
#include "swatch/mp7/cmds/DatavalidFormatter.hpp"

// SWATCH MP7 Generic commands
#include "swatch/mp7/cmds/CheckFirmwareRevision.hpp"
#include "swatch/mp7/cmds/Reset.hpp"
#include "swatch/mp7/cmds/ClearCounters.hpp"
#include "swatch/mp7/cmds/SetID.hpp"
 
#include "swatch/mp7/cmds/ConfigureRxMGTs.hpp"
#include "swatch/mp7/cmds/ConfigureTxMGTs.hpp"
#include "swatch/mp7/cmds/AlignRxsTo.hpp"
#include "swatch/mp7/cmds/AutoAlign.hpp"

#include "swatch/mp7/cmds/SetupReadout.hpp"
#include "swatch/mp7/cmds/LoadReadoutMenu.hpp"
#include "swatch/mp7/cmds/SetupZeroSuppression.hpp"
#include "swatch/mp7/cmds/LoadZeroSuppressionMenu.hpp"

#include "swatch/mp7/cmds/UploadFirmware.hpp"
#include "swatch/mp7/cmds/DeleteFirmware.hpp"
#include "swatch/mp7/cmds/RebootFPGA.hpp"
#include "swatch/mp7/cmds/HardReset.hpp"
#include "swatch/mp7/cmds/ScanSD.hpp"

namespace swpro = swatch::processor;


SWATCH_REGISTER_CLASS(swatch::mp7::MP7Processor);


namespace swatch {
namespace mp7 {

// Static Members Initialization
const std::string MP7Processor::CmdIds::kUploadFw = "uploadFw";
const std::string MP7Processor::CmdIds::kDeleteFw = "deleteFw";
const std::string MP7Processor::CmdIds::kChkFwRevision = "chkFwRevision";
const std::string MP7Processor::CmdIds::kReboot = "reboot";
const std::string MP7Processor::CmdIds::kHardReset = "hardReset";
const std::string MP7Processor::CmdIds::kScanSD = "scanSD";
const std::string MP7Processor::CmdIds::kReset = "reset";
const std::string MP7Processor::CmdIds::kZeroInputs = "zeroInputs";
const std::string MP7Processor::CmdIds::kClearCounters = "clearCounters";
const std::string MP7Processor::CmdIds::kSetId = "setId";
const std::string MP7Processor::CmdIds::kCfgRxMGTs = "cfgRxMGTs";
const std::string MP7Processor::CmdIds::kCfgTxMGTs = "cfgTxMGTs";
const std::string MP7Processor::CmdIds::kAlignMGTs = "alignMGTs";
const std::string MP7Processor::CmdIds::kAutoAlignMGTs = "autoAlignMGTs";
const std::string MP7Processor::CmdIds::kCfgRxBuffers = "cfgRxBuffers";
const std::string MP7Processor::CmdIds::kCfgTxBuffers = "cfgTxBuffers";
const std::string MP7Processor::CmdIds::kCaptureBuffers = "capture";
const std::string MP7Processor::CmdIds::kSaveRxBuffers = "saveRxBuffers";
const std::string MP7Processor::CmdIds::kSaveTxBuffers = "saveTxBuffers";
const std::string MP7Processor::CmdIds::kCfgLatencyRxBuffers = "latencyRxBuffers";
const std::string MP7Processor::CmdIds::kCfgLatencyTxBuffers = "latencyTxBuffers";
const std::string MP7Processor::CmdIds::kCfgEasyRxLatency = "easyRxLatency";
const std::string MP7Processor::CmdIds::kCfgEasyTxLatency = "easyTxLatency";
const std::string MP7Processor::CmdIds::kCfgHdrFormatter = "cfgHdrFormatter";
const std::string MP7Processor::CmdIds::kCfgDVFormatter = "cfgDVFormatter";
const std::string MP7Processor::CmdIds::kSetupReadout = "roSetup";
const std::string MP7Processor::CmdIds::kLoadReadoutMenu = "roLoadMenu";
const std::string MP7Processor::CmdIds::kSetupZS = "zsSetup";
const std::string MP7Processor::CmdIds::kLoadZSMenu = "zsLoadMenu";

// ----------------------------------------------------------------------------
MP7Processor::MP7Processor(const swatch::core::AbstractStub& aStub) :
  MP7AbstractProcessor(aStub),
  mMetricAlgoFirmwareVersion( registerMetric<uint32_t>("algoVersion") )
{
  // Extract stub, and create driver
  const processor::ProcessorStub& stub = getStub();

  mDriver = new ::mp7::MP7Controller(uhal::ConnectionManager::getDevice(stub.id, stub.uri, stub.addressTable));

  // Build subcomponents
  registerInterface(new MP7TTCInterface(*mDriver));
  registerInterface(new MP7ReadoutInterface(*mDriver));
  registerInterface(new swpro::InputPortCollection());
  registerInterface(new swpro::OutputPortCollection());

  buildPorts(stub);

  // Register default MP7 commands
  registerCommand<cmds::Reset>(CmdIds::kReset);
  registerCommand<cmds::ClearCounters>(CmdIds::kClearCounters);
  registerCommand<cmds::SetID>(CmdIds::kSetId);
  registerCommand<cmds::CheckFirmwareRevision>(CmdIds::kChkFwRevision);

  registerCommand<cmds::ZeroInputs>(CmdIds::kZeroInputs);
  registerCommand<cmds::ConfigureRxBuffers>(CmdIds::kCfgRxBuffers);
  registerCommand<cmds::ConfigureTxBuffers>(CmdIds::kCfgTxBuffers);
  registerCommand<cmds::CaptureBuffers>(CmdIds::kCaptureBuffers);
  registerCommand<cmds::SaveRxBuffersToFile>(CmdIds::kSaveRxBuffers);
  registerCommand<cmds::SaveTxBuffersToFile>(CmdIds::kSaveTxBuffers);
  
  registerCommand<cmds::LatencyRxBuffers>(CmdIds::kCfgLatencyRxBuffers);
  registerCommand<cmds::LatencyTxBuffers>(CmdIds::kCfgLatencyTxBuffers);
  registerCommand<cmds::EasyRxLatency>(CmdIds::kCfgEasyRxLatency);
  registerCommand<cmds::EasyTxLatency>(CmdIds::kCfgEasyTxLatency);

  registerCommand<cmds::HeaderFormatter>(CmdIds::kCfgHdrFormatter);
  registerCommand<cmds::DatavalidFormatter>(CmdIds::kCfgDVFormatter);

  registerCommand<cmds::UploadFirmware>(CmdIds::kUploadFw);
  registerCommand<cmds::DeleteFirmware>(CmdIds::kDeleteFw);
  registerCommand<cmds::RebootFPGA>(CmdIds::kReboot);
  registerCommand<cmds::HardReset>(CmdIds::kHardReset);
  registerCommand<cmds::ScanSD>(CmdIds::kScanSD);

  registerCommand<cmds::ConfigureRxMGTs>(CmdIds::kCfgRxMGTs);
  registerCommand<cmds::ConfigureTxMGTs>(CmdIds::kCfgTxMGTs);
  registerCommand<cmds::AlignRxsTo>(CmdIds::kAlignMGTs);
  registerCommand<cmds::AutoAlign>(CmdIds::kAutoAlignMGTs);

  registerCommand<cmds::SetupReadout>(CmdIds::kSetupReadout);
  registerCommand<cmds::LoadReadoutMenu>(CmdIds::kLoadReadoutMenu);
  registerCommand<cmds::SetupZeroSuppression>(CmdIds::kSetupZS);
  registerCommand<cmds::LoadZeroSuppressionMenu>(CmdIds::kLoadZSMenu);

  const auto lFwVsn = retrieveFirmwareVersion();
  LOG4CPLUS_INFO(getLogger(), "MP7 processor '" << getId() << "' built: firmware design " << uint16_t(lFwVsn.design)
          << ", infra " << uint16_t(lFwVsn.infraMajor) << "." << uint16_t(lFwVsn.infraMinor) << "." << uint16_t(lFwVsn.infraPatch)
          << ", algo " << lFwVsn.algo);
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
MP7Processor::~MP7Processor()
{
  delete mDriver;
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
void MP7Processor::buildPorts(const processor::ProcessorStub& aStub)
{

  ::mp7::DatapathDescriptor lDescriptor = mDriver->channelMgr().getDescriptor();

  // Add input ports
  for (auto it = aStub.rxPorts.begin(); it != aStub.rxPorts.end(); it++) {

    MP7RxPort* rxPort = new MP7RxPort(it->id, it->number, *mDriver);
//    MP7RxMiniPODPort* rxPort = new MP7RxMiniPODPort(it->id, it->number, *mDriver);
    getInputPorts().addPort(rxPort);
    const ::mp7::RegionInfo& lRegInfo = lDescriptor.getRegionInfoByChannel(it->number);

    mRxDescriptors.insert(
      ChannelDescriptor(
        it->id,
        it->number,
        lRegInfo.mgtIn != ::mp7::kNoMGT,
        lRegInfo.bufIn != ::mp7::kNoBuffer,
        lRegInfo.fmt,
        rxPort
      )
    );

  }


  for (auto it = aStub.txPorts.begin(); it != aStub.txPorts.end(); it++) {

    getOutputPorts().addPort(new MP7TxPort(it->id, it->number, *mDriver));
//    getOutputPorts().addPort(new MP7TxMiniPODPort(it->id, it->number, *mDriver));

    const ::mp7::RegionInfo& lRegInfo = lDescriptor.getRegionInfoByChannel(it->number);

    mTxDescriptors.insert(
      ChannelDescriptor(
        it->id,
        it->number,
        lRegInfo.mgtOut != ::mp7::kNoMGT,
        lRegInfo.bufOut != ::mp7::kNoBuffer,
        lRegInfo.fmt
      )
    );

  }
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
::mp7::MP7Controller& MP7Processor::driver()
{
  return *mDriver;
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
MP7Processor::FirmwareVersion MP7Processor::retrieveFirmwareVersion() const
{
  uhal::ValWord<uint32_t> infraDesign = mDriver->getCtrl().getNode("id.fwrev.design").read();
  uhal::ValWord<uint32_t> infraMajorVsn = mDriver->getCtrl().getNode("id.fwrev.a").read();
  uhal::ValWord<uint32_t> infraMinorVsn = mDriver->getCtrl().getNode("id.fwrev.b").read();
  uhal::ValWord<uint32_t> infraPatchVsn = mDriver->getCtrl().getNode("id.fwrev.c").read();
  uhal::ValWord<uint32_t> algoVsn = mDriver->getCtrl().getNode("id.algorev").read();
  mDriver->hw().dispatch();

  FirmwareVersion lResult;
  lResult.design = infraDesign.value();
  lResult.infraMajor = infraMajorVsn.value();
  lResult.infraMinor = infraMinorVsn.value();
  lResult.infraPatch = infraPatchVsn.value();
  lResult.algo = algoVsn.value();
  
  return lResult;
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
std::string
MP7Processor::firmwareInfo() const
{
  return "";
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
void MP7Processor::retrieveMetricValues()
{
  const auto lFirmwareVersion = retrieveFirmwareVersion();
  uint64_t lCombinedVersion = uint64_t(lFirmwareVersion.design) << 56;
  lCombinedVersion += uint64_t(lFirmwareVersion.infraMajor) << 48;
  lCombinedVersion += uint64_t(lFirmwareVersion.infraMinor) << 40;
  lCombinedVersion += uint64_t(lFirmwareVersion.infraPatch) << 32;
  lCombinedVersion += lFirmwareVersion.algo;
  setMetricValue<>(mMetricFirmwareVersion, lCombinedVersion);
  setMetricValue<>(mMetricAlgoFirmwareVersion, lFirmwareVersion.algo);
}
// ----------------------------------------------------------------------------

} // namespace mp7
} // namespace swatch

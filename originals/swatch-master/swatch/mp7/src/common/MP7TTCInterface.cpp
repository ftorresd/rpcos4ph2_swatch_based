/*
 * File:   MP7TCCInterface.hpp
 * Author: Alessandro Thea
 *
 * Created on November 6, 2014, 4:50 PM
 */

#include "swatch/mp7/MP7TTCInterface.hpp"


// uHAL headers
#include "uhal/ClientInterface.hpp"     // for ClientInterface
#include "uhal/Node.hpp"                // for Node
#include "uhal/ValMem.hpp"              // for ValWord

// MP7 Headers
#include "mp7/MP7MiniController.hpp"
#include "mp7/CtrlNode.hpp"
#include "mp7/TTCNode.hpp"


namespace swatch {
namespace mp7 {


MP7TTCInterface::MP7TTCInterface(::mp7::MP7MiniController& aController) :
  mDriver(aController),
  mMetricBC0Counter(registerMetric<uint32_t>("bc0Counter")),
  mMetricEC0Counter(registerMetric<uint32_t>("ec0Counter")),
  mMetricOC0Counter(registerMetric<uint32_t>("oc0Counter")),
  mMetricResyncCounter(registerMetric<uint32_t>("resyncCounter")),
  mMetricStartCounter(registerMetric<uint32_t>("startCounter")),
  mMetricStopCounter(registerMetric<uint32_t>("stopCounter")),
  mMetricTestCounter(registerMetric<uint32_t>("testCounter")),
  mMetricTestEnableCounter(registerMetric<uint32_t>("testEnableCounter")),
  mMetricHardResetCounter(registerMetric<uint32_t>("hardResetCounter"))
{
}


MP7TTCInterface::~MP7TTCInterface()
{
}


void
MP7TTCInterface::clearCounters()
{
  mDriver.getTTC().clear();
}


void MP7TTCInterface::clearErrors()
{
  mDriver.getTTC().clearErrors();
}


void MP7TTCInterface::retrieveMetricValues()
{

  const ::mp7::TTCNode& ttc = mDriver.getTTC();
  const ::mp7::CtrlNode& ctrl = mDriver.getCtrl();

  // L1A Counter = Event Counter (next event)-1
  setMetricValue<>(mMetricL1ACounter, ttc.readEventCounter()-1);

  setMetricValue<>(mMetricBunchCounter, ttc.readBunchCounter());
  setMetricValue<>(mMetricOrbitCounter, ttc.readOrbitCounter());

  setMetricValue<>(mMetricSingleBitErrors, ttc.readSingleBitErrorCounter());
  setMetricValue<>(mMetricDoubleBitErrors, ttc.readDoubleBitErrorCounter());
  setMetricValue<>(mMetricIsClock40Locked, ctrl.clock40Locked());
  setMetricValue<>(mMetricHasClock40Stopped, !ctrl.clock40Locked());
  setMetricValue<>(mMetricIsBC0Locked, ttc.readBC0Locked());

  //TTC_BCMD_HARD_RESET
  //TTC_BCMD_TEST_ENABLE
  //TTC_BCMD_BC0
  //TTC_BCMD_EC0
  //TTC_BCMD_RESYNC
  //TTC_BCMD_OC0
  //TTC_BCMD_TEST_SYNC
  //TTC_BCMD_START
  //TTC_BCMD_STOP

  // The order here is everything.
  // Taken from ttc_cmd_ctrs.hdl
//  enum {
//    TTC_BCMD_HARD_RESET,
//    TTC_BCMD_TEST_ENABLE,
//    TTC_BCMD_BC0,
//    TTC_BCMD_EC0,
//    TTC_BCMD_RESYNC,
//    TTC_BCMD_OC0,
//    TTC_BCMD_TEST_SYNC,
//    TTC_BCMD_START,
//    TTC_BCMD_STOP
//  };
//
//  uhal::ValWord<uint32_t> bc0Counter, ec0Counter, oc0Counter, resyncCounter, startCounter, stopCounter, testCounter, testEnableCounter, hardResetCounter;
//  const uhal::Node& lSelCtr(ttc.getNode("cmd_ctrs.ctr_sel"));
//  const uhal::Node& lCtr(ttc.getNode("cmd_ctrs.ctr"));
//
//  // BC0
//  lSelCtr.write(TTC_BCMD_BC0);
//  bc0Counter = lCtr.read();
//
//  // OC0
//  lSelCtr.write(TTC_BCMD_OC0);
//  oc0Counter = lCtr.read();
//
//  // EC0
//  lSelCtr.write(TTC_BCMD_EC0);
//  ec0Counter = lCtr.read();
//
//  // Resync
//  lSelCtr.write(TTC_BCMD_RESYNC);
//  resyncCounter = lCtr.read();
//
//  // Start
//  lSelCtr.write(TTC_BCMD_START);
//  startCounter = lCtr.read();
//
//  // Stop
//  lSelCtr.write(TTC_BCMD_STOP);
//  stopCounter = lCtr.read();
//
//  // TestSync (buffer capture))
//  lSelCtr.write(TTC_BCMD_TEST_SYNC);
//  testCounter = lCtr.read();
//
//  // TestEnable
//  lSelCtr.write(TTC_BCMD_TEST_ENABLE);
//  testEnableCounter = lCtr.read();
//
//  // HardReset
//  lSelCtr.write(TTC_BCMD_HARD_RESET);
//  hardResetCounter = lCtr.read();
//
//  ttc.getClient().dispatch();
//
//  setMetricValue<>(mMetricBC0Counter, (uint32_t)bc0Counter);
//  setMetricValue<>(mMetricEC0Counter, (uint32_t)ec0Counter);
//  setMetricValue<>(mMetricOC0Counter, (uint32_t)oc0Counter);
//  setMetricValue<>(mMetricResyncCounter, (uint32_t)resyncCounter);
//  setMetricValue<>(mMetricStartCounter, (uint32_t)startCounter);
//  setMetricValue<>(mMetricStopCounter, (uint32_t)stopCounter);
//  setMetricValue<>(mMetricTestCounter, (uint32_t)testCounter
  
  setMetricValue<>(mMetricBC0Counter, ttc.readBCmdCounter(::mp7::TTCBCommand::kBC0));
  setMetricValue<>(mMetricEC0Counter, ttc.readBCmdCounter(::mp7::TTCBCommand::kEC0));
  setMetricValue<>(mMetricOC0Counter, ttc.readBCmdCounter(::mp7::TTCBCommand::kOC0));
  setMetricValue<>(mMetricResyncCounter, ttc.readBCmdCounter(::mp7::TTCBCommand::kResync));
  setMetricValue<>(mMetricStartCounter, ttc.readBCmdCounter(::mp7::TTCBCommand::kStart));
  setMetricValue<>(mMetricStopCounter, ttc.readBCmdCounter(::mp7::TTCBCommand::kStop));
  setMetricValue<>(mMetricTestCounter, ttc.readBCmdCounter(::mp7::TTCBCommand::kTest));
  setMetricValue<>(mMetricTestEnableCounter, ttc.readBCmdCounter(::mp7::TTCBCommand::kTestEnable));
  setMetricValue<>(mMetricHardResetCounter, ttc.readBCmdCounter(::mp7::TTCBCommand::kHardReset));

}

} // namespace mp7
} // namespace swatch

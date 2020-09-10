
#include "rpcos4ph2/dummy/DummyProcessor.hpp"

// SWATCH headers
#include "swatch/action/CommandSequence.hpp"
#include "swatch/core/Factory.hpp"
#include "swatch/core/MetricSnapshot.hpp"
#include "swatch/action/StateMachine.hpp"
#include "swatch/processor/PortCollection.hpp"
#include "swatch/processor/ProcessorStub.hpp"
#include "rpcos4ph2/dummy/DummyAlgo.hpp"
#include "rpcos4ph2/dummy/DummyProcDriver.hpp"
#include "rpcos4ph2/dummy/DummyProcessorCommands.hpp"
#include "rpcos4ph2/dummy/DummyReadout.hpp"
#include "rpcos4ph2/dummy/DummyRxPort.hpp"
#include "rpcos4ph2/dummy/DummyTxPort.hpp"
#include "rpcos4ph2/dummy/DummyTTC.hpp"
#include "rpcos4ph2/dummy/utilities.hpp"

// XDAQ Headers
#include "xdata/String.h"
#include "xdata/Vector.h"

// Boost Headers
#include <boost/assign.hpp>
#include <boost/foreach.hpp>

// C++ Headers
#include <iomanip>


SWATCH_REGISTER_CLASS(rpcos4ph2::dummy::DummyProcessor)


namespace rpcos4ph2 {
namespace dummy {


DummyProcessor::DummyProcessor(const swatch::core::AbstractStub& aStub) :
  Processor(aStub),
  mDriver(new DummyProcDriver())
{
  // 1) Interfaces
  registerInterface( new DummyTTC(*mDriver) );
  registerInterface( new DummyReadoutInterface(*mDriver) );
  registerInterface( new DummyAlgo(*mDriver) );
  registerInterface( new swatch::processor::InputPortCollection() );
  registerInterface( new swatch::processor::OutputPortCollection() );

  const swatch::processor::ProcessorStub& stub = getStub();

  for (auto it = stub.rxPorts.begin(); it != stub.rxPorts.end(); it++)
    getInputPorts().addPort(new DummyRxPort(it->id, it->number, *mDriver));
  for (auto it = stub.txPorts.begin(); it != stub.txPorts.end(); it++)
    getOutputPorts().addPort(new DummyTxPort(it->id, it->number, *mDriver));

  // 2) Monitoring
  std::vector<swatch::core::MonitorableObject*> lInputPorts;
  std::vector<swatch::core::AbstractMetric*> lCRCErrorMetrics;
  for (auto lIt=getInputPorts().getPorts().begin(); lIt != getInputPorts().getPorts().end(); lIt++) {
    lCRCErrorMetrics.push_back(&(*lIt)->getMetric(swatch::processor::InputPort::kMetricIdCRCErrors));
    lInputPorts.push_back(*lIt);
  }
  registerComplexMetric<uint32_t>("totalCRCErrors", lCRCErrorMetrics.begin(), lCRCErrorMetrics.end(), swatch::core::ComplexMetric<uint32_t>::CalculateFunction_t(&sumUpCRCErrors), &filterOutMaskedPorts);
  registerComplexMetric<uint32_t>("portsInError", lInputPorts.begin(), lInputPorts.end(), swatch::core::ComplexMetric<uint32_t>::CalculateFunction2_t(&countObjectsInError), &filterOutMaskedPorts);

  // 3) Commands
  swatch::action::Command& reboot = registerCommand<DummyResetCommand>("reboot");
  swatch::action::Command& reset = registerCommand<DummyResetCommand>("reset");
  swatch::action::Command& cfgTx = registerCommand<DummyConfigureTxCommand>("configureTx");
  swatch::action::Command& cfgRx = registerCommand<DummyConfigureRxCommand>("configureRx");
  swatch::action::Command& cfgDaq = registerCommand<DummyConfigureDaqCommand>("configureDaq");
  swatch::action::Command& cfgAlgo = registerCommand<DummyConfigureAlgoCommand>("configureAlgo");

  registerCommand<DummyProcessorForceClkTtcStateCommand>("forceClkTtcState");
  registerCommand<DummyProcessorForceRxPortsStateCommand>("forceRxPortsState");
  registerCommand<DummyProcessorForceTxPortsStateCommand>("forceTxPortsState");
  registerCommand<DummyProcessorForceReadoutStateCommand>("forceReadoutState");
  registerCommand<DummyProcessorForceAlgoStateCommand>("forceAlgoState");

  // 4) Command sequences
  swatch::action::CommandSequence& cfgSeq = registerSequence("configPartA", reset).then(cfgDaq).then(cfgTx);
  registerSequence("fullReconfigure", reset).then(cfgDaq).then(cfgAlgo).then(cfgRx).then(cfgTx);

  // 5) State machines
  swatch::processor::RunControlFSM& lFSM = getRunControlFSM();
  lFSM.coldReset.add(reboot);
  lFSM.setup.add(cfgSeq);
  lFSM.configure.add(cfgAlgo);
  lFSM.align.add(cfgRx);
  lFSM.fsm.addTransition("dummyNoOp", swatch::processor::RunControlFSM::kStateAligned, swatch::processor::RunControlFSM::kStateInitial);
}


DummyProcessor::~DummyProcessor()
{
}


std::string DummyProcessor::firmwareInfo() const
{
  return "none";
}


void DummyProcessor::retrieveMetricValues()
{
  setMetricValue<uint64_t>(mMetricFirmwareVersion, mDriver->getFirmwareVersion());
}


}
}


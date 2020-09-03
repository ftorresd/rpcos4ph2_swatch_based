
#include "swatch/dummy/DummyProcessor.hpp"

// SWATCH headers
#include "swatch/action/CommandSequence.hpp"
#include "swatch/core/Factory.hpp"
#include "swatch/core/MetricSnapshot.hpp"
#include "swatch/action/StateMachine.hpp"
#include "swatch/processor/PortCollection.hpp"
#include "swatch/processor/ProcessorStub.hpp"
#include "swatch/dummy/DummyAlgo.hpp"
#include "swatch/dummy/DummyProcDriver.hpp"
#include "swatch/dummy/DummyProcessorCommands.hpp"
#include "swatch/dummy/DummyReadout.hpp"
#include "swatch/dummy/DummyRxPort.hpp"
#include "swatch/dummy/DummyTxPort.hpp"
#include "swatch/dummy/DummyTTC.hpp"
#include "swatch/dummy/utilities.hpp"

// XDAQ Headers
#include "xdata/String.h"
#include "xdata/Vector.h"

// Boost Headers
#include <boost/assign.hpp>
#include <boost/foreach.hpp>

// C++ Headers
#include <iomanip>


SWATCH_REGISTER_CLASS(swatch::dummy::DummyProcessor)


namespace swatch {
namespace dummy {


DummyProcessor::DummyProcessor(const swatch::core::AbstractStub& aStub) :
  Processor(aStub),
  mDriver(new DummyProcDriver())
{
  // 1) Interfaces
  registerInterface( new DummyTTC(*mDriver) );
  registerInterface( new DummyReadoutInterface(*mDriver) );
  registerInterface( new DummyAlgo(*mDriver) );
  registerInterface( new processor::InputPortCollection() );
  registerInterface( new processor::OutputPortCollection() );

  const processor::ProcessorStub& stub = getStub();

  for (auto it = stub.rxPorts.begin(); it != stub.rxPorts.end(); it++)
    getInputPorts().addPort(new DummyRxPort(it->id, it->number, *mDriver));
  for (auto it = stub.txPorts.begin(); it != stub.txPorts.end(); it++)
    getOutputPorts().addPort(new DummyTxPort(it->id, it->number, *mDriver));

  // 2) Monitoring
  std::vector<core::MonitorableObject*> lInputPorts;
  std::vector<core::AbstractMetric*> lCRCErrorMetrics;
  for (auto lIt=getInputPorts().getPorts().begin(); lIt != getInputPorts().getPorts().end(); lIt++) {
    lCRCErrorMetrics.push_back(&(*lIt)->getMetric(processor::InputPort::kMetricIdCRCErrors));
    lInputPorts.push_back(*lIt);
  }
  registerComplexMetric<uint32_t>("totalCRCErrors", lCRCErrorMetrics.begin(), lCRCErrorMetrics.end(), core::ComplexMetric<uint32_t>::CalculateFunction_t(&sumUpCRCErrors), &filterOutMaskedPorts);
  registerComplexMetric<uint32_t>("portsInError", lInputPorts.begin(), lInputPorts.end(), core::ComplexMetric<uint32_t>::CalculateFunction2_t(&countObjectsInError), &filterOutMaskedPorts);

  // 3) Commands
  action::Command& reboot = registerCommand<DummyResetCommand>("reboot");
  action::Command& reset = registerCommand<DummyResetCommand>("reset");
  action::Command& cfgTx = registerCommand<DummyConfigureTxCommand>("configureTx");
  action::Command& cfgRx = registerCommand<DummyConfigureRxCommand>("configureRx");
  action::Command& cfgDaq = registerCommand<DummyConfigureDaqCommand>("configureDaq");
  action::Command& cfgAlgo = registerCommand<DummyConfigureAlgoCommand>("configureAlgo");

  registerCommand<DummyProcessorForceClkTtcStateCommand>("forceClkTtcState");
  registerCommand<DummyProcessorForceRxPortsStateCommand>("forceRxPortsState");
  registerCommand<DummyProcessorForceTxPortsStateCommand>("forceTxPortsState");
  registerCommand<DummyProcessorForceReadoutStateCommand>("forceReadoutState");
  registerCommand<DummyProcessorForceAlgoStateCommand>("forceAlgoState");

  // 4) Command sequences
  action::CommandSequence& cfgSeq = registerSequence("configPartA", reset).then(cfgDaq).then(cfgTx);
  registerSequence("fullReconfigure", reset).then(cfgDaq).then(cfgAlgo).then(cfgRx).then(cfgTx);

  // 5) State machines
  processor::RunControlFSM& lFSM = getRunControlFSM();
  lFSM.coldReset.add(reboot);
  lFSM.setup.add(cfgSeq);
  lFSM.configure.add(cfgAlgo);
  lFSM.align.add(cfgRx);
  lFSM.fsm.addTransition("dummyNoOp", processor::RunControlFSM::kStateAligned, processor::RunControlFSM::kStateInitial);
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


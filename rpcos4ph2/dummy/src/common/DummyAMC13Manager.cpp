
#include "swatch/dummy/DummyAMC13Manager.hpp"


// boost headers
#include "boost/foreach.hpp"

// SWATCH headers
#include "swatch/core/Factory.hpp"
#include "swatch/action/StateMachine.hpp"
#include "swatch/dtm/DaqTTCStub.hpp"
#include "swatch/dummy/DummyAMC13Driver.hpp"
#include "swatch/dummy/DummyAMC13Interfaces.hpp"
#include "swatch/dummy/DummyAMC13ManagerCommands.hpp"
#include "swatch/dtm/AMCPortCollection.hpp"
#include "swatch/action/CommandSequence.hpp"


SWATCH_REGISTER_CLASS(swatch::dummy::DummyAMC13Manager)


namespace swatch {
namespace dummy {


DummyAMC13Manager::DummyAMC13Manager( const swatch::core::AbstractStub& aStub ) :
  dtm::DaqTTCManager(aStub),
  mDriver(new DummyAMC13Driver())
{
  // 0) Monitoring interfaces
  registerInterface( new AMC13TTC(*mDriver) );
  registerInterface( new AMC13SLinkExpress(0, *mDriver) );
  registerInterface( new dtm::AMCPortCollection() );
  for ( uint32_t s(1); s<=kNumAMCPorts; ++s)
    getAMCPorts().addPort(new AMC13BackplaneDaqPort(s, *mDriver));
  registerInterface( new AMC13EventBuilder(*mDriver));

  // 1) Commands
  action::Command& reboot = registerCommand<DummyAMC13RebootCommand>("reboot");
  action::Command& reset = registerCommand<DummyAMC13ResetCommand>("reset");
  action::Command& cfgEvb = registerCommand<DummyAMC13ConfigureEvbCommand>("configureEvb");
  action::Command& cfgSLink = registerCommand<DummyAMC13ConfigureSLinkCommand>("configureSLink");
  action::Command& cfgAMCPorts = registerCommand<DummyAMC13ConfigureAMCPortsCommand>("configureAMCPorts");
  action::Command& startDaq = registerCommand<DummyAMC13StartDaqCommand>("startDaq");
  action::Command& stopDaq = registerCommand<DummyAMC13StopDaqCommand>("stopDaq");

  registerCommand<DummyAMC13ForceClkTtcStateCommand>("forceClkTtcState");
  registerCommand<DummyAMC13ForceEvbStateCommand>("forceEventBuilderState");
  registerCommand<DummyAMC13ForceSLinkStateCommand>("forceSLinkState");
  registerCommand<DummyAMC13ForceAMCPortStateCommand>("forceAMCPortState");

  // 2) Command sequences
  //registerFunctionoid<DaqTTCMgrCommandSequence>("resetAndConfigure").run(reset).then(configureDaq);
  registerSequence("fullReconfigure", reboot).then(reset).then(cfgEvb).then(cfgSLink).then(cfgAMCPorts).then(startDaq);

  // 3) State machines
  dtm::RunControlFSM& lFSM = getRunControlFSM();
  lFSM.coldReset.add(reboot);
  lFSM.clockSetup.add(reset);
  lFSM.cfgDaq.add(cfgEvb).add(cfgSLink).add(cfgAMCPorts);
  lFSM.start.add(startDaq);
  //lFSM.pause;
  //lFSM.resume;
  lFSM.stopFromPaused.add(stopDaq);
  lFSM.stopFromRunning.add(stopDaq);
}


DummyAMC13Manager::~DummyAMC13Manager()
{
}


void DummyAMC13Manager::retrieveMetricValues()
{
  DummyAMC13Driver::TTCStatus s = mDriver->readTTCStatus();

  setMetricValue<uint16_t>(mDaqMetricFedId, mDriver->readFedId());
}


} // namespace dummy
} // namespace swatch

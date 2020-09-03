/**
 * @file    AMC13Service.cpp
 * @author  Alessandro Thea
 * @brief   Brief description
 * @date    07/11/14
 */

#include "swatch/amc13/AMC13Manager.hpp"


#include <ostream>                      // for operator<<, basic_ostream, etc
#include <string>                       // for char_traits, operator<<

// log4cplus headers
#include "log4cplus/loggingmacros.h"

// SWATCH headers
#include "swatch/action/CommandSequence.hpp"
#include "swatch/core/Factory.hpp"
#include "swatch/action/StateMachine.hpp"  // for StateMachine::Transition
#include "swatch/dtm/AMCPortCollection.hpp"
#include "swatch/dtm/DaqTTCStub.hpp"
#include "swatch/amc13/cmds/Reboot.hpp"
#include "swatch/amc13/cmds/Reset.hpp"
#include "swatch/amc13/cmds/ConfigureTTC.hpp"
#include "swatch/amc13/cmds/ConfigureDAQ.hpp"
#include "swatch/amc13/cmds/Start.hpp"
#include "swatch/amc13/cmds/Stop.hpp"
#include "swatch/amc13/cmds/ResetCounters.hpp"

// uHAL headers
#include "uhal/ConnectionManager.hpp"   // for ConnectionManager
#include "uhal/HwInterface.hpp"         // for HwInterface

// AMC13 headers
#include "amc13/AMC13.hh"
#include "amc13/AMC13Simple.hh"         // for AMC13Simple::Board::T1, etc
#include "swatch/amc13/TTCInterface.hpp"
#include "swatch/amc13/SLinkExpress.hpp"
#include "swatch/amc13/AMCPort.hpp"
#include "swatch/amc13/EVBInterface.hpp"


SWATCH_REGISTER_CLASS(swatch::amc13::AMC13Manager)


namespace swatch {
namespace amc13 {


// --------------------------------------------------------
AMC13Manager::AMC13Manager(const swatch::core::AbstractStub& aStub) :
  swatch::dtm::DaqTTCManager(aStub),
  mDriver(0x0),
  mFwVersionT1(registerMetric<uint32_t>("fwVersionT1")),
  mFwVersionT2(registerMetric<uint32_t>("fwVersionT2"))
{

  // Create driver first
  using ::amc13::AMC13;
  const dtm::DaqTTCStub& desc = getStub();

  uhal::HwInterface t1 = uhal::ConnectionManager::getDevice("T1", desc.uriT1, desc.addressTableT1);
  uhal::HwInterface t2 = uhal::ConnectionManager::getDevice("T2", desc.uriT2, desc.addressTableT2);

  mDriver = new AMC13(t1, t2);

  // Then Monitoring interfaces
  registerInterface(new TTCInterface(*mDriver));
  // Hard-coded id for the moment
  registerInterface(new SLinkExpress(0,*mDriver));

  registerInterface(new dtm::AMCPortCollection());

  for ( uint32_t s(1); s<= kNumAMCPorts; ++s) {
    getAMCPorts().addPort(new AMCPort(s, *mDriver));
  }

  registerInterface(new EVBInterface(*mDriver));

  // // Commands
  action::Command& coldResetCmd = registerCommand<cmds::Reboot>("reboot");
  action::Command& resetCmd = registerCommand<cmds::Reset>("reset");
  action::Command& cfgTTCCmd = registerCommand<cmds::ConfigureTTC>("configTCC");
  action::Command& cfgDAQCmd = registerCommand<cmds::ConfigureDAQ>("configDAQ");
  action::Command& startCmd = registerCommand<cmds::Start>("start");
  action::Command& stopCmd = registerCommand<cmds::Stop>("stop");
  action::Command& resetCtrsCmd = registerCommand<cmds::Stop>("resetCounters");

  // // Sequences
  registerSequence("configSeq", resetCmd).then(cfgTTCCmd).then(cfgDAQCmd);

  // // State machine
  dtm::RunControlFSM& lFSM = getRunControlFSM();
  lFSM.coldReset.add(coldResetCmd);
  lFSM.clockSetup.add(resetCmd).add(cfgTTCCmd);
  lFSM.cfgDaq.add(cfgDAQCmd);
  lFSM.start.add(resetCtrsCmd).add(startCmd);
  lFSM.stopFromPaused.add(stopCmd);
  lFSM.stopFromRunning.add(stopCmd);


  uint32_t vT1 = mDriver->read(AMC13::T1, "STATUS.FIRMWARE_VERS");
  uint32_t vT2 = mDriver->read(AMC13::T2, "STATUS.FIRMWARE_VERS");
  LOG4CPLUS_INFO(getLogger(), "AMC13 manager '" << getId() << "' built. T1 ver: 0x" << std::hex << vT1 << " T2 ver: 0x" << std::hex << vT2);
}


// --------------------------------------------------------
AMC13Manager::~AMC13Manager()
{
  delete mDriver;
}

// --------------------------------------------------------
void AMC13Manager::retrieveMetricValues()
{
  using ::amc13::AMC13;

  setMetricValue<>(mFwVersionT1, mDriver->read(::amc13::AMC13::T1, "STATUS.FIRMWARE_VERS"));
  setMetricValue<>(mFwVersionT2, mDriver->read(::amc13::AMC13::T2, "STATUS.FIRMWARE_VERS"));

  setMetricValue<>(mDaqMetricFedId, (uint16_t) mDriver->read(AMC13::T1, "CONF.ID.SOURCE_ID"));
}


} // namespace amc13
} // namespace swatch

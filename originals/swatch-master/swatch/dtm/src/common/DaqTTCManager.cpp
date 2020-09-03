/*
 * File:   DaqTTCManager.cpp
 * Author: ale
 * Date:   July 2014
 */

#include "swatch/dtm/DaqTTCManager.hpp"


// SWATCH headers
#include "swatch/core/AbstractStub.hpp"
#include "swatch/core/MetricConditions.hpp"
#include "swatch/action/StateMachine.hpp"
#include "swatch/dtm/AMCPortCollection.hpp"
#include "swatch/dtm/EVBInterface.hpp"
#include "swatch/dtm/TTCInterface.hpp"
#include "swatch/dtm/SLinkExpress.hpp"



namespace swatch {
namespace dtm {

const std::string RunControlFSM::kId = "runControl";
const std::string RunControlFSM::kStateInitial = "Halted";
const std::string RunControlFSM::kStateError = "Error";
const std::string RunControlFSM::kStateClockOK = "ClockOK";
const std::string RunControlFSM::kStateConfigured = "Configured";
const std::string RunControlFSM::kStateRunning = "Running";
const std::string RunControlFSM::kStatePaused = "Paused";

const std::string RunControlFSM::kTrColdReset = "coldReset";
const std::string RunControlFSM::kTrClockSetup = "clockSetup";
const std::string RunControlFSM::kTrCfgDaq = "cfgDaq";
const std::string RunControlFSM::kTrStart = "start";
const std::string RunControlFSM::kTrPause = "pause";
const std::string RunControlFSM::kTrResume = "resume";
const std::string RunControlFSM::kTrStop = "stop";


RunControlFSM::RunControlFSM(action::StateMachine& aFSM) :
  fsm( addStates(aFSM) ),
  coldReset( fsm.addTransition(kTrColdReset, kStateInitial, kStateInitial) ),
  clockSetup( fsm.addTransition(kTrClockSetup, kStateInitial, kStateClockOK) ),
  cfgDaq( fsm.addTransition(kTrCfgDaq, kStateClockOK, kStateConfigured)),
  start( fsm.addTransition(kTrStart, kStateConfigured, kStateRunning) ),
  pause( fsm.addTransition(kTrPause, kStateRunning, kStatePaused) ),
  resume( fsm.addTransition(kTrResume, kStatePaused, kStateRunning) ),
  stopFromPaused( fsm.addTransition(kTrStop, kStatePaused, kStateConfigured) ),
  stopFromRunning( fsm.addTransition(kTrStop, kStateRunning, kStateConfigured) )
{
}


action::StateMachine& RunControlFSM::addStates(action::StateMachine& aFSM)
{
  aFSM.addState(kStateClockOK);
  aFSM.addState(kStateConfigured);
  aFSM.addState(kStateRunning);
  aFSM.addState(kStatePaused);
  return aFSM;
}

const uint32_t DaqTTCManager::kNumAMCPorts = 12;
const std::vector<std::string> DaqTTCManager::kDefaultMetrics = {"fedId"};
const std::vector<std::string> DaqTTCManager::kDefaultMonitorableObjects = {"ttc","evb","amcports"};

DaqTTCManager::DaqTTCManager(const swatch::core::AbstractStub& aStub ) :
  swatch::action::ActionableObject(aStub.id, aStub.alias, aStub.loggerName),
  mStub(dynamic_cast<const DaqTTCStub&>(aStub)),
  mTTC(0x0),
  mSLink(0x0),
  mAMCPorts(0x0),
  mEvb(0x0),
  mRunControlFSM( registerStateMachine(RunControlFSM::kId, RunControlFSM::kStateInitial, RunControlFSM::kStateError) ),
  mDaqMetricFedId( registerMetric<uint16_t>("fedId", core::NotEqualCondition<uint16_t>(mStub.fedId)) )
{
}


DaqTTCManager::~DaqTTCManager()
{
}


const DaqTTCStub& DaqTTCManager::getStub() const
{
  return mStub;
}


uint32_t
DaqTTCManager::getSlot() const
{
  return mStub.slot;
}


const
std::string& DaqTTCManager::getCrateId() const
{
  return mStub.crate;
}


uint16_t
DaqTTCManager::getFedId() const
{
  return mStub.fedId;
}

const std::vector<std::string>&
DaqTTCManager::getGateKeeperContexts() const
{
  // Can't set the table names in constructor, since don't know parent at that time ...
  // ... instead, have to set tables names first time this method is called
  if ( mGateKeeperTables.empty() ) {
    mGateKeeperTables.push_back(getPath());

    std::string basePath = getPath();
    basePath.resize(basePath.size() - getId().size());
    mGateKeeperTables.push_back(basePath + getStub().role);
    mGateKeeperTables.push_back(basePath + "daqttcs");
  }
  return mGateKeeperTables;
}


RunControlFSM& DaqTTCManager::getRunControlFSM()
{
  return mRunControlFSM;
}


// --------------------------------------------------------
TTCInterface&
DaqTTCManager::getTTC()
{
  return *mTTC;
}


// --------------------------------------------------------
dtm::AMCPortCollection&
DaqTTCManager::getAMCPorts()
{
  return *mAMCPorts;
}


// --------------------------------------------------------
const dtm::AMCPortCollection&
DaqTTCManager::getAMCPorts() const
{
  return *mAMCPorts;
}

const dtm::SLinkExpress& DaqTTCManager::getSLinkExpress() const
{
  return *(this -> mSLink);
}


dtm::SLinkExpress& DaqTTCManager::getSLinkExpress()
{
  return *(this -> mSLink);
}

// --------------------------------------------------------
TTCInterface&
DaqTTCManager::registerInterface(TTCInterface* aTTCInterface)
{
  if (mTTC) {
    delete aTTCInterface;
    XCEPT_RAISE(DaqTTCManagerInterfaceAlreadyDefined,"TTCInterface already defined for amc13 '" + getPath() + "'");
  }
  this->addMonitorable(aTTCInterface);
  mTTC = aTTCInterface;
  return *mTTC;
}


// --------------------------------------------------------
SLinkExpress&
DaqTTCManager::registerInterface(SLinkExpress* aSLink)
{
  if (mSLink) {
    delete aSLink;
    XCEPT_RAISE(DaqTTCManagerInterfaceAlreadyDefined,"SLink already defined for amc13 '" + getPath() + "'");
  }
  this->addMonitorable(aSLink);
  mSLink = aSLink;
  return *mSLink;
}

// --------------------------------------------------------
dtm::AMCPortCollection&
DaqTTCManager::registerInterface( dtm::AMCPortCollection* aAMCPortCollection )
{
  if ( mAMCPorts ) {
    delete aAMCPortCollection;
    XCEPT_RAISE(DaqTTCManagerInterfaceAlreadyDefined, "PortCollection already defined for amc13 '" + getPath() + "'" );
  }
  this->addMonitorable(aAMCPortCollection);
  mAMCPorts = aAMCPortCollection;
  return *mAMCPorts;
}

// --------------------------------------------------------
EVBInterface&
DaqTTCManager::registerInterface(EVBInterface* aEventBuilder)
{
  if (mEvb) {
    delete aEventBuilder;
    XCEPT_RAISE(DaqTTCManagerInterfaceAlreadyDefined,"TTCInterface already defined for amc13 '" + getPath() + "'");
  }
  this->addMonitorable(aEventBuilder);
  mEvb = aEventBuilder;
  return *mEvb;
}


} // namespace dtm
} // namespace swatch


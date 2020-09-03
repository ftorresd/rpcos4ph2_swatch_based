#include "swatchcell/framework/RunControl.h"

#include <malloc.h>

#include "swatchcell/framework/CellAbstract.h"
#include "swatchcell/framework/tools/utilities.h"

#include "ts/toolbox/Tools.h"

#include "swatch/core/Factory.hpp"
#include "swatch/action/MaskableObject.hpp"
#include "swatch/action/StateMachine.hpp"
#include "swatch/action/SystemStateMachine.hpp"
#include "swatch/action/ThreadPool.hpp"
#include "swatch/core/utilities.hpp"
#include "swatch/system/System.hpp"
#include "swatch/xml/XmlGateKeeper.hpp"
#include "swatch/dtm/DaqTTCManager.hpp"
#include "swatch/processor/Processor.hpp"
#include "swatch/system/MaskingConfigurator.hpp"

#include "log4cplus/loggingmacros.h"
#include "swatchcell/framework/tools/utilities.h"

#include <boost/range/algorithm/copy.hpp>
#include <boost/algorithm/string.hpp>
#include <exception>
#include <numeric>


using namespace std;

namespace swatchcellframework {

std::ostream& operator<<(std::ostream& aStream, const struct mallinfo& aInfo)
{
  aStream << "arena=" << aInfo.arena
          << ", ordblks=" << aInfo.ordblks
          << ", smblks=" << aInfo.smblks
          << ", hblks=" << aInfo.hblks
          << ", hblkhd=" << aInfo.hblkhd
          << ", usmblks=" << aInfo.usmblks
          << ", fsmblks=" << aInfo.fsmblks
          << ", uordblks=" << aInfo.uordblks
          << ", fordblks=" << aInfo.fordblks
          << ", keepcost=" << aInfo.keepcost;
  return aStream;
}


typedef swatch::system::RunControlFSM RunControlFSM_t;

const string RunControl::kStateHalted       = "halted";
const string RunControl::kStateEngaged      = "engaged";
const string RunControl::kStateSynchronized = "synchronized";
const string RunControl::kStateConfigured   = "configured";
const string RunControl::kStateAligned      = "aligned";
const string RunControl::kStateRunning      = "running";
const string RunControl::kStatePaused       = "paused";

const string RunControl::kTrEngage    = "engage";
const string RunControl::kTrColdReset = "coldReset";
const string RunControl::kTrSetup     = "setup";
const string RunControl::kTrConfigure = "configure";
const string RunControl::kTrAlign     = "align";
const string RunControl::kTrStart     = "start";
const string RunControl::kTrStop      = "stop";
const string RunControl::kTrPause     = "pause";
const string RunControl::kTrResume    = "resume";

const string RunControl::kConfigKey    = "Configuration Key";
const string RunControl::kRSKey        = "Run Settings Key";
const string RunControl::kRunNumber    = "Run Number";
const string RunControl::kTtcMap       = "TTC Map";
const string RunControl::kFedMap       = "FED Map";
const string RunControl::kTestModeFlag = "Test mode";

const string RunControl::kConfigKeyDefaultFileValue = "file://";

const boost::chrono::seconds RunControl::kExecTransitionMaxWaitTime(20);
const boost::chrono::milliseconds RunControl::kExecTransitionSleepDuration(20);



// --------------------------------------------------------
RunControl::RunControl(log4cplus::Logger& aLogger, tsframework::CellAbstractContext* aContext) :
  tsframework::CellOperation(aLogger, aContext),
  mContext(dynamic_cast<swatchcellframework::CellContext&>(*aContext))
{
  LOG4CPLUS_DEBUG( getLogger(), "#RunControl CTOR");

  try
  {
    getFSM().clear();
    getFSM().addState ( kStateHalted );
    getFSM().addState ( kStateEngaged );
    getFSM().addState ( kStateSynchronized );
    getFSM().addState ( kStateConfigured );
    getFSM().addState ( kStateAligned );
    getFSM().addState ( kStateRunning );
    getFSM().addState ( kStatePaused );
    getFSM().addTransition ( kStateHalted,        kStateEngaged,       kTrEngage,       this, &RunControl::checkEngage,       &RunControl::execEngage );
    getFSM().addTransition ( kStateEngaged,       kStateEngaged,       kTrColdReset,    this, &RunControl::checkColdReset,    &RunControl::execColdReset );
    getFSM().addTransition ( kStateEngaged,       kStateSynchronized,  kTrSetup,        this, &RunControl::checkSetup,        &RunControl::execSetup );
    getFSM().addTransition ( kStateSynchronized,  kStateConfigured,    kTrConfigure,    this, &RunControl::checkConfigure,    &RunControl::execConfigure );
    getFSM().addTransition ( kStateConfigured,    kStateAligned,       kTrAlign,        this, &RunControl::checkAlign,        &RunControl::execAlign );
    getFSM().addTransition ( kStateAligned,       kStateConfigured,    kTrStop,         this, &RunControl::checkStop,         &RunControl::execStop );
    getFSM().addTransition ( kStateAligned,       kStateRunning,       kTrStart,        this, &RunControl::checkStart,        &RunControl::execStart );
    getFSM().addTransition ( kStateRunning,       kStateConfigured,    kTrStop,         this, &RunControl::checkStop,         &RunControl::execStop );
    getFSM().addTransition ( kStateRunning,       kStatePaused,        kTrPause,        this, &RunControl::checkPause,        &RunControl::execPause );
    getFSM().addTransition ( kStatePaused,        kStateRunning,       kTrResume,       this, &RunControl::checkResume,       &RunControl::execResume );
    getFSM().addTransition ( kStatePaused,        kStateConfigured,    kTrStop,         this, &RunControl::checkStop,         &RunControl::execStop );
    getFSM().setInitialState ( kStateHalted );
    getFSM().reset();
  }
  catch ( xcept::Exception& e )
  {
    XCEPT_RETHROW ( tsexception::CellException,"Failed to initialize the FSM!", e );
  }

  // Add/initialize parameters
  //
  //! Subsystem configuration key
  getParamList() [kConfigKey] = new xdata::String ( "" );
  getParamList() [kRSKey]     = new xdata::String ( "" );
  //! Run number
  getParamList() [kRunNumber] = new xdata::UnsignedLong ( 0 );
  //! TTC partition map
  getParamList() [kTtcMap]    = new xdata::String( "{dummy=0}" );
  //! FED map
  getParamList() [kFedMap]    = new xdata::String ( "0&0%" );
  //! Test mode flag
  getParamList() [kTestModeFlag] = new xdata::Boolean (false);
}


// --------------------------------------------------------
RunControl::~RunControl()
{
}


// --------------------------------------------------------
xdata::UnsignedLong RunControl::getRunNumber()
{
  return getParam<xdata::UnsignedLong>(kRunNumber);
}


// --------------------------------------------------------
xdata::String RunControl::getConfigurationKey()
{
  return getParam<xdata::String>(kConfigKey);
}


// --------------------------------------------------------
xdata::String RunControl::getRunSettingsKey()
{
  return getParam<xdata::String>(kRSKey);
}

  
// --------------------------------------------------------
xdata::String RunControl::getTtcMap()
{
  return getParam<xdata::String>(kTtcMap);
}


// --------------------------------------------------------
xdata::String RunControl::getFedMap()
{
  return getParam<xdata::String>(kFedMap);
}


// --------------------------------------------------------
xdata::Boolean RunControl::getTestModeFlag()
{
  return getParam<xdata::Boolean>(kTestModeFlag);
}


// --------------------------------------------------------
swatch::system::System& RunControl::getSystem(const CellContext::SharedGuard_t& aGuard)
{
  return mContext.getSystem(aGuard);
}


// --------------------------------------------------------
void RunControl::execPostStart()
{
}


// --------------------------------------------------------
void RunControl::execPreStop()
{
}


// --------------------------------------------------------
void RunControl::publishError ( const string& message )
{
  getWarning().append ( message + "\n", tsframework::CellWarning::ERROR );
  LOG4CPLUS_ERROR ( getLogger(), message );
}


// --------------------------------------------------------
void RunControl::publishWarning ( const string& message )
{
  getWarning().append ( message + "\n", tsframework::CellWarning::WARNING );
  LOG4CPLUS_WARN ( getLogger(), message );
}


// --------------------------------------------------------
string RunControl::getParam ( const string& name, bool mustBeAvailable )
{
  if ( getParamList().find ( name ) != getParamList().end() && getParamList() [name] )
  {
    return boost::trim_copy ( getParamList() [name]->toString() );
  }

  iAssert ( !mustBeAvailable && "getParam() was called for a non-existing parameter!" );
  return "";
}


// --------------------------------------------------------
bool RunControl::getBoolParam ( const string& name )
{
  return ( dynamic_cast<xdata::Boolean*> ( getParamList() [name] ) )->toString () == "true";
}


// --------------------------------------------------------
bool RunControl::memCheck(size_t errorLimit, size_t warnLimit )
{
  if (!tstoolbox::Tools::checkAvailableMemory(errorLimit)) {
    publishError("Available memory in critical level! Please restart!");

    return false;
  }
  else if (!tstoolbox::Tools::checkAvailableMemory(warnLimit)) {
    publishWarning("Available memory low! Please restart at the next opportunity!");

    return true;
  }

  return true;
}


// --------------------------------------------------------
bool RunControl::checkEngage()
{
  mTimer.reset();
  bool lResult = memCheck();

  LOG4CPLUS_INFO(getLogger(), "Preliminary check for 'engage' transition " << (lResult ? "passed" : "failed") << "; took " << mTimer.ms() << " ms");
  return lResult;
}


// --------------------------------------------------------
bool RunControl::checkColdReset()
{
  mTimer.reset();
  bool lResult = memCheck() && checkTransition(RunControlFSM_t::kStateInitial, RunControlFSM_t::kTrColdReset);

  LOG4CPLUS_INFO(getLogger(), "Preliminary check for 'cold reset' transition " << (lResult ? "passed" : "failed") << "; took " << mTimer.ms() << " ms");
  return lResult;
}


// --------------------------------------------------------
bool RunControl::checkSetup()
{
  mTimer.reset();
  bool lResult = memCheck() && checkTransition(RunControlFSM_t::kStateInitial, RunControlFSM_t::kTrSetup);
  
  LOG4CPLUS_INFO(getLogger(), "Preliminary check for 'setup' transition " << (lResult ? "passed" : "failed") << "; took " << mTimer.ms() << " ms");
  return lResult;
}


// --------------------------------------------------------
bool RunControl::checkConfigure()
{
  mTimer.reset();
  bool lResult = memCheck() && checkTransition(RunControlFSM_t::kStateSync, RunControlFSM_t::kTrConfigure);

  LOG4CPLUS_INFO(getLogger(), "Preliminary check for 'configure' transition " << (lResult ? "passed" : "failed") << "; took " << mTimer.ms() << " ms");
  return lResult;
}


// --------------------------------------------------------
bool RunControl::checkAlign()
{
  mTimer.reset();
  bool lResult = memCheck() && checkTransition(RunControlFSM_t::kStateConfigured, RunControlFSM_t::kTrAlign);
  
  LOG4CPLUS_INFO(getLogger(), "Preliminary check for 'align' transition " << (lResult ? "passed" : "failed") << "; took " << mTimer.ms() << " ms");
  return lResult;
}


// --------------------------------------------------------
bool RunControl::checkStart()
{
  mTimer.reset();
  bool lResult = memCheck() && checkTransition(RunControlFSM_t::kStateAligned, RunControlFSM_t::kTrStart);

  LOG4CPLUS_INFO(getLogger(), "Preliminary check for 'start' transition " << (lResult ? "passed" : "failed") << "; took " << mTimer.ms() << " ms");
  return lResult;
}


// --------------------------------------------------------
bool RunControl::checkStop()
{
  mTimer.reset();

  bool lResult = memCheck();
  
  if ( getFSM().getState() == (kStateAligned + "_" + kTrStop + "_" + kStateConfigured) )
    lResult = lResult && checkTransition(RunControlFSM_t::kStateAligned, RunControlFSM_t::kTrStop);
  else if ( getFSM().getState() == (kStateRunning + "_" + kTrStop + "_" + kStateConfigured) )
    lResult = lResult && checkTransition(RunControlFSM_t::kStateRunning, RunControlFSM_t::kTrStop);
  else if ( getFSM().getState() == (kStatePaused + "_" + kTrStop + "_" + kStateConfigured) )
    lResult = lResult && checkTransition(RunControlFSM_t::kStatePaused, RunControlFSM_t::kTrStop);
  else // should not happen but you never know...
  {
    publishError("Unexpected response from getState(): " + getFSM().getState());
    return false;
  }

  LOG4CPLUS_INFO(getLogger(), "Preliminary check for 'stop' transition " << (lResult ? "passed" : "failed") << "; took " << mTimer.ms() << " ms");
  return lResult;
}


// --------------------------------------------------------
bool RunControl::checkPause()
{
  mTimer.reset();
  bool lResult = memCheck() && checkTransition(RunControlFSM_t::kStateRunning, RunControlFSM_t::kTrPause);

  LOG4CPLUS_INFO(getLogger(), "Preliminary check for 'pause' transition " << (lResult ? "passed" : "failed") << "; took " << mTimer.ms() << " ms");
  return lResult;
}


// --------------------------------------------------------
bool RunControl::checkResume()
{
  mTimer.reset();
  bool lResult = memCheck() && checkTransition(RunControlFSM_t::kStatePaused, RunControlFSM_t::kTrResume);

  LOG4CPLUS_INFO(getLogger(), "Preliminary check for 'resume' transition " << (lResult ? "passed" : "failed") << "; took " << mTimer.ms() << " ms");
  return lResult;
}


// --------------------------------------------------------
void RunControl::execEngage()
{
  mTimer.reset();
  LOG4CPLUS_INFO(getLogger(), "Run control, executing 'engage' transition. Config key = '" << getConfigurationKey().value_ << "', RS key = '" << getRunSettingsKey().value_ << "', test mode = " << (getTestModeFlag() ? "true" : "false"));

  const std::string lConfigKey = getConfigurationKey();
  const std::string lRSKey = getRunSettingsKey();
  
  // Part 0: Stop straight away if the config key is empty
  if ( lConfigKey.empty() ) {
    publishError("Invalid config key: config key is empty!");
    return;
  }

  // Part 1: Construct the system from supplied initialisation key
  const std::string& lDefaultSystemFilePath = mContext.getGuiContext().getDefaultSystemDescriptionFilePath();

  CellContext::WriteGuard_t lGuard(mContext);

  // Lock access to system
  try {
    LOG4CPLUS_INFO(getLogger(), "Building new System");

    // and replace it
    if (lConfigKey == kConfigKeyDefaultFileValue)
      mContext.replaceSystemFromFile(lDefaultSystemFilePath, lGuard);
    else
      mContext.replaceSystemFromDb(mContext.getSubsystemId(), lConfigKey, lGuard);
  }
  catch(const std::exception& lExc) {
    std::ostringstream lOss;
    lOss << "Caught exception when constructing system ... " << std::endl;
    if (lConfigKey == kConfigKeyDefaultFileValue)
      lOss << "   File: " << lDefaultSystemFilePath << std::endl;
    else
      lOss << "   Config key: " << lConfigKey << std::endl;

    lOss << "   Exception type: " << swatch::core::demangleName(typeid(lExc).name()) << std::endl;
    lOss << "   Message:  " << lExc.what() << std::endl;
    publishError(lOss.str());
    return;
  }

  LOG4CPLUS_INFO(getLogger(), "Memory allocation info ('run control' engage, after sys created, before GK): " << mallinfo());

  const swatch::action::GateKeeper* lGK = NULL;
  CellContext::GuiContext& lGuiContext = mContext.getGuiContext();
  const std::string lGateKeeperFilename(lGuiContext.getDefaultGateKeeperXmlFilePath());
  const std::string lGateKeeperKey(lGuiContext.getDefaultGateKeeperKey());

  // Create the gatekeeper, and push it to cell's context
  if (lConfigKey == kConfigKeyDefaultFileValue) {
    try {
      lGK = &mContext.setGateKeeperFromFile(lGateKeeperFilename, lGateKeeperKey, lGuard);
    }
    catch(const std::exception& lExc) {
      std::ostringstream lOss;
      lOss << "Caught exception when creating gatekeeper ..." << std::endl;
      lOss << "   File: " << lGateKeeperFilename << std::endl;
      lOss << "   Key: " << lGateKeeperKey << std::endl;
      lOss << "   Exception type: " << swatch::core::demangleName(typeid(lExc).name()) << std::endl;
      lOss << "   Message: " << lExc.what() << std::endl;
      publishError(lOss.str());
      return;
    }
  } else {
    try {
      lGK = &mContext.setGateKeeperFromDb(mContext.getSubsystemId(), lConfigKey, lRSKey, lGuard);
    }
    catch(const std::exception& lExc) {
      std::ostringstream lOss;
      lOss << "Caught exception when creating gatekeeper ..." << std::endl;
      lOss << "   Configuration key: " << lConfigKey << std::endl;
      lOss << "   RS Key: " << lRSKey << std::endl;
      lOss << "   Exception type: " << swatch::core::demangleName(typeid(lExc).name()) << std::endl;
      lOss << "   Message: " << lExc.what() << std::endl;
      publishError(lOss.str());
      return;
    }
  }
  
  // Check that contexts in gatekeeper all appear in system stub (skip if in test mode)
  if ( ! getTestModeFlag() ) {
    if ( ! checkGateKeeperContextIds(lGuard) )
      return;
  }

  swatch::system::MaskingConfigurator maskCfg(mContext.getSystem(lGuard));

  // Retrieve FED Map parameter
  std::string lFEDMapString = getFedMap();

  swatch::system::MaskingConfigurator::FEDEnableMap_t lFEDEnableMap;

  // Turn it into a map
  try {
    lFEDEnableMap = CellContext::parseFEDEnableMap(lFEDMapString);
  } catch ( xcept::Exception& e) {
    XCEPT_RETHROW(tsexception::CellException, "Failed to parse FEDMap", e);
  }
  // Disable crates according to FEDMap
  maskCfg.disableCrates(lFEDEnableMap);

  // Engage the system's run control FSM
  mContext.getSystem(lGuard).getStateMachine(swatch::system::RunControlFSM::kId).engage(*lGK);

  // Apply static masks
  // Currently disabled because if interferes with enabled/disabled flags set by enable.
  // TO BE DISCUSSED
  // maskCfg.applyStatic(*mContext.getGateKeeper().get());

  // Apply dynamic masks
  maskCfg.applyDynamicMasks(lFEDEnableMap);

  std::ostringstream lResult;
  lResult << "System '" << mContext.getSystem(lGuard).getId() << "' Engaged.<br>";

  if ( !maskCfg.getAutoDisabledDTMs().empty() ) {
    lResult << "<b>Auto-disabled DTMs</b><br>";
    boost::copy(maskCfg.getAutoDisabledDTMs(), std::ostream_iterator<std::string>(lResult,"<br>"));
    lResult << "<br>";
  }
  if ( !maskCfg.getAutoDisabledAMCs().empty() ){
    lResult << "<b>Auto-disabled AMCs</b><br>";
    boost::copy(maskCfg.getAutoDisabledAMCs(), std::ostream_iterator<std::string>(lResult,"<br>"));
    lResult << "<br>";
  }
  if ( !maskCfg.getAutoDisabledAMCPorts().empty() ) {
    lResult << "<b>Auto-disabled AMC Ports</b><br>";
    boost::copy(maskCfg.getAutoDisabledAMCPorts(), std::ostream_iterator<std::string>(lResult,"<br>"));
    lResult << "<br>";
  }
  if ( !maskCfg.getAutoMaskedLinks().empty() ) {
    lResult << "<b>Auto-masked Links</b><br>";
    boost::copy(maskCfg.getAutoMaskedLinks(), std::ostream_iterator<std::string>(lResult,"<br>"));
    lResult << "<br>";
  }
  if ( !maskCfg.getAutoMaskedInputs().empty() ) {
    lResult << "<b>Auto-masked Inputs</b><br>";
    boost::copy(maskCfg.getAutoMaskedInputs(), std::ostream_iterator<std::string>(lResult,"<br>"));
    lResult << "<br>";
  }
  setResult(lResult.str());

  disableMonitoringOnMaskedObjects(mContext.getSystem(lGuard), getLogger());

  const std::vector<std::string> lContexts = {mContext.getSystem(lGuard).getId()};
  swatch::action::GateKeeper::Parameter_t lParam = lGK->get("", "", "runcontrol_engage_invoke_malloc_trim", lContexts);
  if ( (lParam != NULL) && (dynamic_cast<const xdata::Boolean*>(&*lParam) != NULL) && bool(*dynamic_cast<const xdata::Boolean*>(&*lParam))) {
    LOG4CPLUS_INFO(getLogger(), "Invoking malloc_trim within 'run control' engage transition; memory allocation info: " << mallinfo());
    malloc_trim(0);
  }

  LOG4CPLUS_INFO(getLogger(), "Memory allocation info (end of 'run control' engage): " << mallinfo());
  LOG4CPLUS_INFO ( getLogger(), "Executed 'engage' transition, took " << mTimer.ms() << " ms" );
}


// --------------------------------------------------------
bool RunControl::checkGateKeeperContextIds(const CellContext::WriteGuard_t& aGuard)
{
  const swatch::system::SystemStub& lSystemStub = mContext.getSystem(aGuard).getStub();
  std::set<std::string> lValidContextIds;
  lValidContextIds.insert(lSystemStub.id);
  for (const auto lProcStub : lSystemStub.processors) {
    lValidContextIds.insert(lSystemStub.id + "." + lProcStub.id);
    lValidContextIds.insert(lSystemStub.id + "." + lProcStub.role);
  }
  lValidContextIds.insert(lSystemStub.id + ".processors");
  for (const auto lDaqTtcStub : lSystemStub.daqttcs) {
    lValidContextIds.insert(lSystemStub.id + "." + lDaqTtcStub.id);
    lValidContextIds.insert(lSystemStub.id + "." + lDaqTtcStub.role);
  }
  lValidContextIds.insert(lSystemStub.id + ".daqttcs");
 
  const swatch::action::GateKeeper& lGK = *mContext.getGateKeeper(aGuard);
  std::set<std::string> lInvalidContextIds;
  for (auto lIt = lGK.parametersBegin(); lIt != lGK.parametersEnd(); lIt++) {
    if (lValidContextIds.count(lIt->first) == 0)
      lInvalidContextIds.insert(lIt->first);
  }

  for (auto lIt = lGK.masksBegin(); lIt != lGK.masksEnd(); lIt++) {
    if (lValidContextIds.count(lIt->first) == 0)
      lInvalidContextIds.insert(lIt->first);
  }

  for (auto lIt = lGK.monitoringSettingsBegin(); lIt != lGK.monitoringSettingsEnd(); lIt++) {
    if (lValidContextIds.count(lIt->first) == 0)
      lInvalidContextIds.insert(lIt->first);
  }

  if ( ! lInvalidContextIds.empty() ) {
    std::ostringstream lErrorMsg;
    lErrorMsg << "The following " << lInvalidContextIds.size() << " contexts do not match the ID or role of any object listed in the system description: ";
    for (auto lIt = lInvalidContextIds.begin(); lIt != lInvalidContextIds.end(); lIt++) {
      lErrorMsg << "'" << *lIt << "'";
      if (lIt != --lInvalidContextIds.end())
        lErrorMsg << ", ";
    }
    publishError(lErrorMsg.str());
    return false;
  }
  else
    return true;
}


// --------------------------------------------------------
void RunControl::execColdReset()
{
  mTimer.reset();
  LOG4CPLUS_INFO(getLogger(), "Run control, executing 'cold reset' transition. Config key = '" << getConfigurationKey().value_ << "', RS key = '" << getRunSettingsKey().value_ << "'");

  execTransition(RunControlFSM_t::kStateInitial, RunControlFSM_t::kTrColdReset);

  LOG4CPLUS_INFO ( getLogger(), "Executed 'cold reset' transition, took " << mTimer.ms() << " ms" );
}


// --------------------------------------------------------
void RunControl::execSetup()
{
  mTimer.reset();
  LOG4CPLUS_INFO(getLogger(), "Run control, executing 'setup' transition. Config key = '" << getConfigurationKey().value_ << "', RS key = '" << getRunSettingsKey().value_ << "'");

  // Run the SWATCH system FSM's transition
  execTransition(RunControlFSM_t::kStateInitial, RunControlFSM_t::kTrSetup);

  // Start the monitoring thread
  mContext.getMonitoringThread().start();

  LOG4CPLUS_INFO(getLogger(), "Executed 'setup' transition, took " << mTimer.ms() << " ms" );
}


// --------------------------------------------------------
void RunControl::execConfigure()
{
  mTimer.reset();
  LOG4CPLUS_INFO(getLogger(), "Run control, executing 'configure' transition. Config key = '" << getConfigurationKey().value_ << "', RS key = '" << getRunSettingsKey().value_ << "'");
  
  execTransition(RunControlFSM_t::kStateSync, RunControlFSM_t::kTrConfigure);

  LOG4CPLUS_INFO(getLogger(), "Executed 'configure' transition, took " << mTimer.ms() << " ms" );
}


// --------------------------------------------------------
void RunControl::execAlign()
{
  mTimer.reset();
  LOG4CPLUS_INFO(getLogger(), "Run control, executing 'align' transition. Config key = '" << getConfigurationKey().value_ << "', RS key = '" << getRunSettingsKey().value_ << "'");
  
  execTransition(RunControlFSM_t::kStateConfigured, RunControlFSM_t::kTrAlign);

  LOG4CPLUS_INFO(getLogger(), "Executed 'align' transition, took " << mTimer.ms() << " ms" );
}


// --------------------------------------------------------
void RunControl::execStart()
{
  mTimer.reset();
  LOG4CPLUS_INFO(getLogger(), "Run control, executing 'start' transition. Run number = " << getRunNumber().value_);

  execTransition(RunControlFSM_t::kStateAligned, RunControlFSM_t::kTrStart);

  execPostStart();

  LOG4CPLUS_INFO(getLogger(), "Executed 'start' transition, took " << mTimer.ms() << " ms" );
}


// --------------------------------------------------------
void RunControl::execStop()
{
  mTimer.reset();
  LOG4CPLUS_INFO(getLogger(), "Run control, executing 'stop' transition. Run number = " << getRunNumber().value_);

  try {
    execPreStop();
  }
  catch (const std::exception& aExc) {
    publishError("Exception of type '" + swatch::core::demangleName(typeid(aExc).name()) + "'thrown by run control pre-stop callback, message is: " + aExc.what());
  }

  if ( getFSM().getState() == (kStateAligned + "_" + kTrStop + "_" + kStateConfigured) )
    execTransition(RunControlFSM_t::kStateAligned, RunControlFSM_t::kTrStop);
  else if ( getFSM().getState() == (kStateRunning + "_" + kTrStop + "_" + kStateConfigured) )
    execTransition(RunControlFSM_t::kStateRunning, RunControlFSM_t::kTrStop);
  else if ( getFSM().getState() == (kStatePaused + "_" + kTrStop + "_" + kStateConfigured) )
    execTransition(RunControlFSM_t::kStatePaused, RunControlFSM_t::kTrStop);
  else // should not happen but you never know...
    publishError("Unexpected response from getState(): '" + getFSM().getState() + "'. Not executing any system FSM transition.");

  LOG4CPLUS_INFO(getLogger(), "Executed 'stop' transition, took " << mTimer.ms() << " ms" );
}


// --------------------------------------------------------
void RunControl::execPause()
{
  mTimer.reset();
  LOG4CPLUS_INFO(getLogger(), "Run control, executing 'pause' transition. Run number = " << getRunNumber().value_);

  execTransition(RunControlFSM_t::kStateRunning, RunControlFSM_t::kTrPause);

  LOG4CPLUS_INFO(getLogger(), "Executed 'pause' transition, took " << mTimer.ms() << " ms" );
}


// --------------------------------------------------------
void RunControl::execResume()
{
  mTimer.reset();
  LOG4CPLUS_INFO(getLogger(), "Run control, executing 'resume' transition. Run number = " << getRunNumber().value_);

  execTransition(RunControlFSM_t::kStatePaused, RunControlFSM_t::kTrResume);

  LOG4CPLUS_INFO(getLogger(), "Executed 'resume' transition, took " << mTimer.ms() << " ms" );
}


// --------------------------------------------------------
void RunControl::resetting()
{
  mTimer.reset();

  LOG4CPLUS_INFO(getLogger(), "Resetting run control operation.");

  LOG4CPLUS_INFO(getLogger(), "Memory allocation info ('run control' reset): " << mallinfo());

  CellContext::SharedGuard_t lGuard(mContext);
  const swatch::action::GateKeeper* lGK = mContext.getGateKeeper(lGuard);
  if (lGK != NULL) {
    const std::vector<std::string> lContexts = {mContext.getSystem(lGuard).getId()};
    swatch::action::GateKeeper::Parameter_t lParam = lGK->get("", "", "runcontrol_reset_invoke_malloc_trim", lContexts);
    if ( (lParam != NULL) && (dynamic_cast<const xdata::Boolean*>(&*lParam) != NULL) && bool(*dynamic_cast<const xdata::Boolean*>(&*lParam))) {
      LOG4CPLUS_INFO(getLogger(), "Invoking malloc_trim within 'run control' reset transition");
      malloc_trim(0);
      LOG4CPLUS_INFO(getLogger(), "Memory allocation info ('run control' reset): " << mallinfo());
    }
  }

  // Stop the monitoring thread
  mContext.getMonitoringThread().stop();

  LOG4CPLUS_INFO ( getLogger(), "Run control operation has been reset, took " << mTimer.ms() << " ms" );
}


// --------------------------------------------------------
bool RunControl::checkTransition(const std::string& aFromState, const std::string& aTransitionId)
{
  // Grab system handler and lock access to system
  CellContext::SharedGuard_t lGuard(mContext);
  swatch::system::System& lSys = getSystem(lGuard);

  const swatch::action::SystemTransition& lTransition = *lSys.getStateMachine(RunControlFSM_t::kId)
  .getTransitions(aFromState).at(aTransitionId);

  // 1) Check that system is in the correct state machine, and in correct state
  swatch::system::System::Status_t lStatus = lSys.getStatus();
  if ( !lStatus.isEngaged() ) {
    publishError("System is not engaged in any state machine");
    return false;
  }
  else if (lStatus.getStateMachineId() != RunControlFSM_t::kId) {
    publishError("System is in incorrect state machine ' " + lStatus.getStateMachineId() + "'");
    return false;
  }
  else if (lStatus.getState() != aFromState) {
    publishError("System is in wrong state '" + lStatus.getState() + "' for transition '" + aTransitionId + "'");
    return false;
  }

  // 2) Check that each enabled processor/AMC13 is in the correct state machine, and in correct state
  std::vector<std::string> lChildIds = lSys.getChildren();
  std::vector<std::string> lErrors;
  for (std::vector<std::string>::const_iterator lIt=lChildIds.begin(); lIt != lChildIds.end(); lIt++) {
    if(swatch::action::ActionableObject* lChild = lSys.getObjPtr<swatch::action::ActionableObject>(*lIt)) {
      swatch::action::ActionableObject::Status_t lStatus = lChild->getStatus();

      if ( lStatus.isEnabled() ) {
        if ( !lStatus.isEngaged() )
          lErrors.push_back(lChild->getPath() + " is not engaged in any state machine");
        // FIXME: must use FSM ID / state ID for the child, not for the system
//        else if (lStatus.getStateMachineId() != RunControlFSM_t::kId)
//          lErrors.push_back(lChild->getPath() + " is in incorrect state machine ' " + lStatus.getStateMachineId() + "'");
//        else if (lStatus.getState() != aFromState)
//          lErrors.push_back(lChild->getPath() + " is in wrong state '" + lStatus.getState() + "' for transition '" + aTransitionId + "'");
      }
    }
  }
  if (!lErrors.empty()) {
    std::ostringstream lOss;
    lOss << lErrors.size() << " children not in correct state:";
    for(std::vector<std::string>::const_iterator lIt=lErrors.begin(); lIt != lErrors.end(); lIt++)
      lOss << std::endl << "   " << *lIt;
    publishError(lOss.str());
    return false;
  }

  // 3) Check that there aren't any missing parameters
  std::map< const swatch::action::Transition*, std::vector<swatch::action::CommandVec::MissingParam> > lMissingParamMap;
  lTransition.checkForMissingParameters(*mContext.getGateKeeper(lGuard), lMissingParamMap);

  // Communicate missing parameters to users
  if ( !lMissingParamMap.empty() ) {
    ostringstream oss;
    oss << "Required parameters for requested operation are missing!\n";

    for (const auto& lItem : lMissingParamMap)
    {
      const auto& lTransition = *lItem.first;
      const auto& lMissingParams = lItem.second;

      oss << "Transition '" << lTransition.getActionable().getId() << "." << lTransition.getId() << "'\n";

      for (const auto& lMissingParam : lMissingParams)
        oss <<  " - '" << lMissingParam.command << "' in namespace '" << lMissingParam.nspace
            << "': missing parameter '" << lMissingParam.parameter << "'" << endl;
    }

    publishError(oss.str());
    return false;
  }

  // 4) Check that all parameters pass rules & constraints
  using swatch::action::Transition;
  std::map< const Transition*, std::vector<Transition::ParamRuleViolationList> > lRuleViolationMap;
  lTransition.checkForInvalidParameters(*mContext.getGateKeeper(lGuard), lRuleViolationMap);

  // Communicate rule violations to users
  if ( !lRuleViolationMap.empty() ) {
    ostringstream oss;
    oss << "Parameter values in requested operation fail rules and/or constraints!\n";

    for (const auto& lItem : lRuleViolationMap) {
      const auto& lTransition = *lItem.first;
      const auto& lRuleViolationLists = lItem.second;

      oss << "Transition '" << lTransition.getActionable().getId() << "." << lTransition.getId() << "'\n";


      for ( const auto& lRuleViolationList : lRuleViolationLists) {
        oss << " - [" << lRuleViolationList.commandIdx << "] '" << lRuleViolationList.command << "': ";
        if (lRuleViolationList.violations.size() > 1)
           oss << std::endl;
        for ( const auto& lRuleViolation : lRuleViolationList.violations) {
          // Write each violation on a separate line, unless there is only one violation for this command
          if (lRuleViolationList.violations.size() > 1)
            oss << "      * ";
          oss << "parameter" << ((lRuleViolation.parameters.size() > 1) ? "s " : " ");
          
          for (auto lIt = lRuleViolation.parameters.begin(); lIt != lRuleViolation.parameters.end(); lIt++) {
            if (lIt != lRuleViolation.parameters.begin())
              oss << ", ";
            oss << *lIt;
            
            const auto& lSerializable = lRuleViolationList.paramSet[*lIt];
            const auto lType = lSerializable.type();
            if ( (lType != "vector") && (lType != "table") )
              oss << "='" << lSerializable.toString() << "'";
          }
          
          oss << ((lRuleViolation.parameters.size() > 1) ? " fail '"  : " fails '" ) << lRuleViolation.ruleDescription << "'. ";
          if (!lRuleViolation.details.empty())
            oss << "Details: " << lRuleViolation.details << std::endl;
          else
            oss << std::endl;
        }
      }
    }

    publishError(oss.str());
    return false;
  }

  // If we reached this point, then all is fine, so return true
  return true;
}


// --------------------------------------------------------
void RunControl::execTransition(const std::string& aFromState, const std::string& aTransitionId)
{
  // Lock access to system
  CellContext::SharedGuard_t lGuard(mContext);

  execTransition(aFromState, aTransitionId, lGuard);
}


// --------------------------------------------------------
void RunControl::execTransition(const std::string& aFromState, const std::string& aTransitionId, const CellContext::SharedGuard_t& aGuard)
{
  swatch::system::System& lSys = getSystem(aGuard);

  using namespace swatch::action;
  SystemTransition& lTransition = *lSys.getStateMachine(RunControlFSM_t::kId).getTransitions(aFromState).at(aTransitionId);

  // Run transition; in case any of the actionable objects are currently busy, retry every X milliseconds until we are successful or until Y seconds after the first attempt
  typedef boost::chrono::steady_clock SteadyClock_t;
  const SteadyClock_t::time_point lStartTime = SteadyClock_t::now();
  size_t lAttemptCount = 0;
  while (true) {
    try {
      lAttemptCount++;
      lTransition.exec(*mContext.getGateKeeper(aGuard), false);
      break;
    }
    catch (const ActionableObjectIsBusy& e)
    {
      const SteadyClock_t::duration lDuration = SteadyClock_t::now() - lStartTime;
      if (lDuration > kExecTransitionMaxWaitTime) {
        LOG4CPLUS_WARN(getLogger(), "Could not execute FSM transition '" << aTransitionId << "' within " << kExecTransitionMaxWaitTime << " (" << lAttemptCount << " attempts) due to processors/DAQ-TTC managers being busy");
        throw;
      }
    }
    if ((lAttemptCount % 25) == 0) {
      const boost::chrono::milliseconds lDuration = boost::chrono::duration_cast<boost::chrono::milliseconds>(SteadyClock_t::now() - lStartTime);
      LOG4CPLUS_WARN(getLogger(), lAttemptCount << " attempts (over " << lDuration << ") to execute FSM transition '" << aTransitionId << "' have failed due to processors/DAQ-TTC managers being busy");
    }

    boost::this_thread::sleep_for(kExecTransitionSleepDuration);
  } 

  SystemTransitionSnapshot lStatus = lTransition.getStatus();

  if (lStatus.getState() == Functionoid::kError) {
    size_t lStepIdx = lStatus.getNumberOfCompletedSteps()-1;

    boost::unordered_map<std::string, std::vector<std::string> > lCmdErrorSummaries;
    for(auto lObjTransitionSnapshotPtr : lStatus.at(lStepIdx)) {
      if (lObjTransitionSnapshotPtr != NULL) {
        const TransitionSnapshot& lObjTransitionSnapshot = *lObjTransitionSnapshotPtr;
        if (lObjTransitionSnapshot.getState() == Functionoid::kError) {
          const Transition& lObjTransition = lSys.getObjByFullPath<Transition>(lObjTransitionSnapshot.getActionPath());
          std::ostringstream lErrorSummary;
          lErrorSummary << "Transition '" << lObjTransitionSnapshot.getActionId() << "' ('"
              << lObjTransition.getStartState() << "' -> '" << lObjTransition.getEndState() << "')" << std::endl;
          if (lObjTransitionSnapshot.begin() == lObjTransitionSnapshot.end()) // (Only handling this situation in case framework screws up; in principle, this command status vec should never be empty if transition is in error)
            lErrorSummary << " (no commands executed)";
          else {
            const CommandSnapshot& lLastCommand = *(lObjTransitionSnapshot.end()-1);
            lErrorSummary << "   Command: '" << lLastCommand.getActionId() << "' (" << lObjTransitionSnapshot.size()
                    << " of " << lObjTransitionSnapshot.getTotalNumberOfCommands() << ")" << std::endl;
            lErrorSummary << "   Message: '" << lLastCommand.getStatusMsg() << "'";
          }

          auto lMapIt = lCmdErrorSummaries.insert( std::make_pair(lErrorSummary.str(), std::vector<std::string> {"'" + lObjTransitionSnapshot.getActionableId() + "'"}) );
          if ( ! lMapIt.second )
            lMapIt.first->second.push_back("'" + lObjTransitionSnapshot.getActionableId() + "'");
        }
      }
    }

    const size_t lNrObjErrors = std::accumulate(lCmdErrorSummaries.cbegin(), lCmdErrorSummaries.cend(), 0, 
            [](size_t aSum, const pair<string, vector<string> >& aItem) {return aSum + aItem.second.size();});
    std::ostringstream lOss;
    lOss << "An error occurred in system transition '" << aTransitionId << "' (attempt " << lAttemptCount << "), step "
         << lStatus.getNumberOfCompletedSteps() << " of " << lStatus.getTotalNumberOfSteps() << ", for " 
            << lNrObjErrors << " objects. ";
    if (!lStatus.getMessage().empty())
      lOss << "Summary message: " << std::endl << "   " << lStatus.getMessage() << std::endl << std::endl << "Details:";

    for (auto lItem : lCmdErrorSummaries)
      lOss << std::endl << " * " << swatch::core::join(lItem.second, ", ") << ": " << lItem.first;

    publishError(lOss.str());
  }
  else if (lStatus.getState() == Functionoid::kWarning) {
    boost::unordered_map<std::string, std::vector<std::string> > lCmdWarnSummaries;
    for(auto lStep : lStatus) {
      for(auto lStepItem : lStep)  {
        if (lStepItem != NULL) {
          const TransitionSnapshot& lObjTransitionSnapshot = *lStepItem;
          if (lObjTransitionSnapshot.getState() == Functionoid::kWarning) {
            for(auto lCmdSnapshot : lObjTransitionSnapshot) {
              if (lCmdSnapshot.getState() == Functionoid::kWarning) {
                const Transition& lObjTransition = lSys.getObjByFullPath<Transition>(lObjTransitionSnapshot.getActionPath());
                std::ostringstream lWarnSummary;
                lWarnSummary << "Transition '" << lObjTransitionSnapshot.getActionId() << "' ('"
                    << lObjTransition.getStartState() << "' -> '" << lObjTransition.getEndState() << "')" << std::endl;;
                if (lObjTransitionSnapshot.begin() == lObjTransitionSnapshot.end()) // (Only handling this situation in case framework screws up; in principle, this command status vec should never be empty if transition is in error)
                  lWarnSummary << " (no commands executed)";
                else {
                  const CommandSnapshot& lLastCommand = *(lObjTransitionSnapshot.end()-1);
                  lWarnSummary << "   Command: '" << lLastCommand.getActionId() << "' (" << lObjTransitionSnapshot.size()
                          << " of " << lObjTransitionSnapshot.getTotalNumberOfCommands() << ")" << std::endl;
                  lWarnSummary << "   Message: '" << lLastCommand.getStatusMsg() << "'";
                }

                auto lMapIt = lCmdWarnSummaries.insert( std::make_pair(lWarnSummary.str(), std::vector<std::string> {"'" + lObjTransitionSnapshot.getActionableId() + "'"}) );
                if ( ! lMapIt.second )
                  lMapIt.first->second.push_back("'" + lObjTransitionSnapshot.getActionableId() + "'");
              }
            }
          }
        }
      }
    }

    
    const size_t lNrCmdWarnings = std::accumulate(lCmdWarnSummaries.cbegin(), lCmdWarnSummaries.cend(), 0, 
            [](size_t aSum, const pair<string, vector<string> >& aItem) {return aSum + aItem.second.size();});
    std::ostringstream lOss;
    lOss << "Warning in system transition '" << aTransitionId << "' (attempt " << lAttemptCount << "), for " << lNrCmdWarnings << " commands. ";
    if (!lStatus.getMessage().empty())
      lOss << "Summary message: " << std::endl << "   " << lStatus.getMessage() << std::endl << std::endl << "Details:";

    for (auto lItem : lCmdWarnSummaries)
      lOss << std::endl << " * " << swatch::core::join(lItem.second, ", ") << ": " << lItem.first;

    publishWarning(lOss.str());
  }
  else
    setResult("Successfully completed system FSM transition '" + aTransitionId + "' (attempt " + boost::lexical_cast<std::string>(lAttemptCount) + ")!");

  disableMonitoringOnMaskedObjects(lSys, getLogger());
}


void RunControl::disableMonitoringOnMaskedObjects(swatch::system::System& aSystem, log4cplus::Logger& aLogger)
{
  std::vector<const swatch::action::MaskableObject*> lNewlyDisabledObjects;
  for(swatch::core::Object::iterator lIt=aSystem.begin(); lIt != aSystem.end(); lIt++) {
    if(swatch::action::MaskableObject* lMaskableObj = dynamic_cast<swatch::action::MaskableObject*>(&*lIt)) {
      if (lMaskableObj->isMasked() && (lMaskableObj->getMonitoringStatus() != swatch::core::monitoring::kDisabled)) {
        lNewlyDisabledObjects.push_back(lMaskableObj);
        lMaskableObj->setMonitoringStatus(swatch::core::monitoring::kDisabled);
      }
    }
  }

  if (! lNewlyDisabledObjects.empty()) {
    std::ostringstream lOss;
    lOss << "The monitoring was not disabled for " << lNewlyDisabledObjects.size() << " of the masked objects which seems like a mistake. ";
    lOss << "So, the framework has now disabled the monitoring on these masked objects. Full list:";
    typedef std::vector<const swatch::action::MaskableObject*>::const_iterator Iterator_t;
    for(Iterator_t lIt = lNewlyDisabledObjects.begin(); lIt != lNewlyDisabledObjects.end(); lIt++)
      lOss << std::endl << "  * '" << (*lIt)->getPath() << "'";
    LOG4CPLUS_INFO(aLogger, lOss.str());
  }
}


} // end ns swatchframework

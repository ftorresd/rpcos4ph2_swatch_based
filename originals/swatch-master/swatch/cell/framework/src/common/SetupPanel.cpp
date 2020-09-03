#include "swatchcell/framework/SetupPanel.h"

#include "swatchcell/framework/CellAbstract.h"

#include "ajax/PolymerElement.h"
#include "ajax/toolbox.h"

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"

// Swatch Headers
#include "swatch/processor/Processor.hpp"
#include "swatch/dtm/DaqTTCManager.hpp"
#include "swatch/processor/PortCollection.hpp"
#include "swatch/processor/Port.hpp"
#include "swatch/system/Crate.hpp"
#include "swatch/system/Link.hpp"
#include "swatch/system/System.hpp"
#include "swatch/action/ThreadPool.hpp"
#include "swatch/xml/XmlGateKeeper.hpp"

#include "jsoncpp/json/json.h"


// Boost Headers
#include <boost/foreach.hpp>
//#include <boost/algorithm/string/replace.hpp>
//#include <boost/range/algorithm/copy.hpp>

namespace swatchcellframework
{

  SetupPanel::SetupPanel(tsframework::CellAbstractContext* aContext, log4cplus::Logger& aLogger)
  :
    CellPanel(aContext, aLogger),
    mLogger(aLogger),
    mCellContext(dynamic_cast<swatchcellframework::CellContext&>(*aContext)),
    mResultBox(new ajax::ResultBox())
  {
    mResultBox->setIsOwned(false);
  }


  SetupPanel::~SetupPanel()
  {
    delete mResultBox;
  }


  /*
  ██       █████  ██    ██  ██████  ██    ██ ████████
  ██      ██   ██  ██  ██  ██    ██ ██    ██    ██
  ██      ███████   ████   ██    ██ ██    ██    ██
  ██      ██   ██    ██    ██    ██ ██    ██    ██
  ███████ ██   ██    ██     ██████   ██████     ██
  */


  void SetupPanel::layout(cgicc::Cgicc& aCgi)
  {
    this->remove();

    //Events go here!
    setEvent("tabbed-page-change-setup", ajax::Eventable::OnSubmit, this, &swatchcellframework::SetupPanel::switchView);
    setEvent("SetupPanel::loadSystemFromFile", ajax::Eventable::OnClick, this, &swatchcellframework::SetupPanel::loadSystemFromFile);
    setEvent("SetupPanel::loadSystemFromDB", ajax::Eventable::OnClick, this, &swatchcellframework::SetupPanel::loadSystemFromDB);
    setEvent("SetupPanel::getSystemTreeJSON", ajax::Eventable::OnClick, this, &swatchcellframework::SetupPanel::getSystemTreeJSON);
    setEvent("SetupPanel::getObjectDetails", ajax::Eventable::OnClick, this, &swatchcellframework::SetupPanel::getObjectDetails);
    setEvent("SetupPanel::getMonitoringThreadStatus", ajax::Eventable::OnClick, this, &swatchcellframework::SetupPanel::getMonitoringThreadStatus);
    setEvent("SetupPanel::monitoringThreadRunCommand", ajax::Eventable::OnClick, this, &swatchcellframework::SetupPanel::monitoringThreadRunCommand);
    setEvent("SetupPanel::loadGateKeeperFromXML", ajax::Eventable::OnClick, this, &swatchcellframework::SetupPanel::loadGateKeeperFromXML);
    setEvent("SetupPanel::loadGateKeeperFromDB", ajax::Eventable::OnClick, this, &swatchcellframework::SetupPanel::loadGateKeeperFromDB);

    ajax::PolymerElement* lTabPage = new ajax::PolymerElement("generic-tabbed-page");
    lTabPage->set("pageid", "setup");
    lTabPage->set("tabnames", ajax::toolbox::escapeHTML("[\"System\", \"GateKeeper\", \"Monitoring Thread\"]"));
    mResultBox->add(lTabPage);

    this->add(mResultBox);


  }

  void SetupPanel::switchView(cgicc::Cgicc& aCgi, std::ostream& aOut)
  {
    mResultBox->remove();
    std::map<std::string, std::string> lValues(ajax::toolbox::getSubmittedValues(aCgi));
    mMode = static_cast<SetupPanel::Mode_t>(boost::lexical_cast<int>(lValues["selected"]));

    switch(mMode) {
    case SetupPanel::kSystemSetup:
      respondSystemSetupPage();
      break;
    case SetupPanel::kGateKeeper:
      respondGateKeeperPage();
      break;
    case SetupPanel::kMonitoringThread:
      respondMonitoringThreadPage();
      break;
    }

    mResultBox->innerHtml(aCgi, aOut);
  }

  void SetupPanel::respondSystemSetupPage()
  {
    //I create a new system view and I pass the default file path
    ajax::PolymerElement* lSystemSetupView = new ajax::PolymerElement("system-setup-view");
    std::string lSystemFilePath = mCellContext.getGuiContext().getLatestSystemDescriptionFilePath();
    lSystemSetupView -> set ("file-path", lSystemFilePath);
    lSystemSetupView -> set("subsystem-id", getenv("SUBSYSTEM_ID") ? getenv("SUBSYSTEM_ID") : "unknownSubsystem");
    lSystemSetupView -> set("configuration-key", "aTopLevelConfigKey");

    mResultBox -> add(lSystemSetupView);
    return;
  }

  void SetupPanel::respondGateKeeperPage()
  {
    ajax::PolymerElement* lGatekeeperSetupView = new ajax::PolymerElement("gatekeeper-setup-view");
    std::string lXmlFilePath = mCellContext.getGuiContext().getLatestGateKeeperXmlFilePath();
    std::string lKey = mCellContext.getGuiContext().getLatestGateKeeperKey();
    lGatekeeperSetupView -> set ("xml-file-path", lXmlFilePath);
    lGatekeeperSetupView -> set ("xml-key", lKey);
    mResultBox -> add(lGatekeeperSetupView);
    return;
  }

  void SetupPanel::respondMonitoringThreadPage()
  {
    ajax::PolymerElement* lMonitoringThreadSetupView = new ajax::PolymerElement("monitoring-thread-setup-view");
    mResultBox -> add(lMonitoringThreadSetupView);
    return;
  }



  /*
  ███████ ██    ██ ███████ ████████ ███████ ███    ███     ███████ ███████ ████████ ██    ██ ██████
  ██       ██  ██  ██         ██    ██      ████  ████     ██      ██         ██    ██    ██ ██   ██
  ███████   ████   ███████    ██    █████   ██ ████ ██     ███████ █████      ██    ██    ██ ██████
       ██    ██         ██    ██    ██      ██  ██  ██          ██ ██         ██    ██    ██ ██
  ███████    ██    ███████    ██    ███████ ██      ██     ███████ ███████    ██     ██████  ██
  */



  /*
  ███████ ██    ██ ███████ ███    ██ ████████ ███████
  ██      ██    ██ ██      ████   ██    ██    ██
  █████   ██    ██ █████   ██ ██  ██    ██    ███████
  ██       ██  ██  ██      ██  ██ ██    ██         ██
  ███████   ████   ███████ ██   ████    ██    ███████
  */


  /*
██       ██████   █████  ██████      ███████ ██    ██ ███████     ███████ ██ ██      ███████
██      ██    ██ ██   ██ ██   ██     ██       ██  ██  ██          ██      ██ ██      ██
██      ██    ██ ███████ ██   ██     ███████   ████   ███████     █████   ██ ██      █████
██      ██    ██ ██   ██ ██   ██          ██    ██         ██     ██      ██ ██      ██
███████  ██████  ██   ██ ██████      ███████    ██    ███████     ██      ██ ███████ ███████
*/


  void SetupPanel::loadSystemFromFile(cgicc::Cgicc& aCgi, std::ostream& aOut)
  {

    std::map<std::string, std::string> lValues(ajax::toolbox::getSubmittedValues(aCgi));

    std::string lFilePath = lValues["filepath"];

    if ((lFilePath == "undefined")||(lFilePath == "")) {
      aOut << "{}";
      return;
    }

    try{
      { // Lock system locally
        CellContext::WriteGuard_t lGuard(mCellContext);

        LOG4CPLUS_INFO(getLogger(), "geting new System");

        // Create the new system only then, and replace it
        swatch::system::System& lNewSys = mCellContext.replaceSystemFromFile(lFilePath, lGuard);
        mCellContext.getGuiContext().setSystemDescriptionFilePath(lFilePath);

        Json::Value lResult;
        lResult["status"] = "success";
        lResult["errorMessage"] = "";
        lResult["systemID"] = lNewSys.getId();
        aOut << lResult;
      }
    }
    catch (const std::exception& e) {
      std::ostringstream lOss;
      lOss << "Failed to construct system from file '" << lFilePath << "'. Details below. \n";
      lOss << "Exception '" << swatch::core::demangleName(typeid(e).name()) << "': ";
      if ( e.what() == NULL or e.what()[0] == '\0')
        lOss << "(no exception message)";
      else
        lOss << e.what();
      const std::string& lErrorMsg = lOss.str();
      LOG4CPLUS_ERROR(getLogger(), lErrorMsg);

      Json::Value lResult;
      lResult["status"] = "error";
      lResult["errorMessage"] = lErrorMsg;
      aOut << lResult;
    }
  }


  /*
  ██       ██████   █████  ██████      ███████ ██    ██ ███████     ██████  ██████
  ██      ██    ██ ██   ██ ██   ██     ██       ██  ██  ██          ██   ██ ██   ██
  ██      ██    ██ ███████ ██   ██     ███████   ████   ███████     ██   ██ ██████
  ██      ██    ██ ██   ██ ██   ██          ██    ██         ██     ██   ██ ██   ██
  ███████  ██████  ██   ██ ██████      ███████    ██    ███████     ██████  ██████
  */

  void SetupPanel::loadSystemFromDB(cgicc::Cgicc& aCgi, std::ostream& aOut)
  {

    std::map<std::string, std::string> lValues(ajax::toolbox::getSubmittedValues(aCgi));

    const std::string lSubsystemId = lValues["subsystemid"];
    const std::string lConfigKey = lValues["configurationkey"];


    try{
      { // Lock system locally
        CellContext::WriteGuard_t lGuard(mCellContext);

        LOG4CPLUS_INFO(getLogger(), "Building new System");

        // Create the new system only then, and replace it
        swatch::system::System& lNewSys = mCellContext.replaceSystemFromDb(lSubsystemId, lConfigKey, lGuard);

        Json::Value lResult;
        lResult["status"] = "success";
        lResult["errorMessage"] = "";
        lResult["systemID"] = lNewSys.getId();
        aOut << lResult;
      }
    }
    catch (const std::exception& e) {
      std::ostringstream lOss;
      lOss << "Failed to construct system from DB key '" << lConfigKey << "'. Details below. \n";
      lOss << "Exception '" << swatch::core::demangleName(typeid(e).name()) << "': ";
      if ( e.what() == NULL or e.what()[0] == '\0')
        lOss << "(no exception message)";
      else
        lOss << e.what();
      const std::string lErrorMsg = lOss.str();
      LOG4CPLUS_ERROR(getLogger(), lErrorMsg);

      Json::Value lResult;
      lResult["status"] = "error";
      lResult["errorMessage"] = lErrorMsg;
      aOut << lResult;
    }

    return;
  }



  /*
███████ ██    ██ ███████     ████████ ██████  ███████ ███████
██       ██  ██  ██             ██    ██   ██ ██      ██
███████   ████   ███████        ██    ██████  █████   █████
     ██    ██         ██        ██    ██   ██ ██      ██
███████    ██    ███████        ██    ██   ██ ███████ ███████
*/

  void SetupPanel::getSystemTreeJSON(cgicc::Cgicc& aCgi, std::ostream& aOut)
  {
    CellContext::SharedGuard_t lGuard(mCellContext);
    swatch::system::System& lSystem = mCellContext.getSystem(lGuard);


    Json::Value lProcessorsArray(Json::arrayValue);
    BOOST_FOREACH( const swatch::processor::Processor* lProcessor, lSystem.getProcessors()) {
      Json::Value lProcessorObject(Json::objectValue);
      lProcessorObject["name"] = lProcessor -> getId();
      lProcessorObject["type"] = "processor";
      lProcessorsArray.append(lProcessorObject);
    }

    Json::Value lDaqttcsArray(Json::arrayValue);
    BOOST_FOREACH( const swatch::dtm::DaqTTCManager* lDtm, lSystem.getDaqTTCs()) {
      Json::Value lDaqttcObject(Json::objectValue);
      lDaqttcObject["name"] = lDtm -> getId();
      lDaqttcObject["type"] = "daqttc";
      lDaqttcsArray.append(lDaqttcObject);
    }

    Json::Value lCratesArray(Json::arrayValue);
    BOOST_FOREACH( const swatch::system::System::CratesMap_t::value_type& lCratePair, lSystem.getCrates()) {
      swatch::system::Crate* lCrate = lCratePair.second;
      Json::Value lCrateObject(Json::objectValue);
      lCrateObject["name"] = lCrate -> getId();
      lCrateObject["type"] = "crate";
      lCratesArray.append(lCrateObject);
    }

    Json::Value lProcessorsNode(Json::objectValue);
    lProcessorsNode["name"] = "processors";
    lProcessorsNode["type"] = "processors";
    lProcessorsNode["tree"] = lProcessorsArray;

    Json::Value lDaqttcsNode(Json::objectValue);
    lDaqttcsNode["name"] = "daqttcs";
    lDaqttcsNode["type"] = "daqttcs";
    lDaqttcsNode["tree"] = lDaqttcsArray;

    Json::Value lCratesNode(Json::objectValue);
    lCratesNode["name"] = "crates";
    lCratesNode["type"] = "crates";
    lCratesNode["tree"] = lCratesArray;

    Json::Value lLinksNode(Json::objectValue);
    lLinksNode["type"] = "links";
    lLinksNode["name"] = "links";

    Json::Value lConnectedFEDNode(Json::objectValue);
    lConnectedFEDNode["name"] = "connected FEDs";
    lConnectedFEDNode["type"] = "connectedFEDs";

    Json::Value lComponentsArray(Json::arrayValue);
    lComponentsArray.append(lProcessorsNode);
    lComponentsArray.append(lDaqttcsNode);
    lComponentsArray.append(lCratesNode);
    lComponentsArray.append(lLinksNode);
    lComponentsArray.append(lConnectedFEDNode);

    Json::Value lSystemNode(Json::objectValue);

    lSystemNode["name"] = lSystem.getId();
    lSystemNode["type"] = "system";
    lSystemNode["tree"] = lComponentsArray;

    Json::Value lSystemTreeArray(Json::arrayValue);
    lSystemTreeArray.append(lSystemNode);

    aOut << lSystemTreeArray;

    return;

  }

/*
   ██████  ██████       ██     ██████  ███████ ████████  █████  ██ ██      ███████
  ██    ██ ██   ██      ██     ██   ██ ██         ██    ██   ██ ██ ██      ██
  ██    ██ ██████       ██     ██   ██ █████      ██    ███████ ██ ██      ███████
  ██    ██ ██   ██ ██   ██     ██   ██ ██         ██    ██   ██ ██ ██           ██
   ██████  ██████   █████      ██████  ███████    ██    ██   ██ ██ ███████ ███████
  */

  void SetupPanel::getObjectDetails(cgicc::Cgicc& aCgi, std::ostream& aOut)
  {

    std::map<std::string, std::string> lValues(ajax::toolbox::getSubmittedValues(aCgi));
    std::string lObjectType = lValues["type"];
    std::string lObjectName = lValues["objectname"];

    if ((lObjectType == "undefined")||(lObjectType == "")) {
      aOut << "[]";
      return;
    };

    if ((lObjectName == "undefined")||(lObjectName == "")) {
      aOut << "[]";
      return;
    };

    if (lObjectType == "system") {
      this -> returnSystemDetails(aOut);
      return;
    }

    if (lObjectType == "processor") {
      this -> returnProcessorDetails(lObjectName, aOut);
      return;
    }

    if (lObjectType == "daqttc") {
      this -> returnDAQTTCDetails(lObjectName, aOut);
      return;
    }

    if (lObjectType == "crate") {
      this -> returnCrateDetails(lObjectName, aOut);
      return;
    }

    if (lObjectType == "processors") {
      this -> returnProcessorsDetails(aOut);
      return;
    }

    if (lObjectType == "daqttcs") {
      this -> returnDAQTTCsDetails(aOut);
      return;
    }

    if (lObjectType == "crates") {
      this -> returnCratesDetails(aOut);
      return;
    }

    if (lObjectType == "links") {
      this -> returnLinksDetails(aOut);
      return;
    }

    if (lObjectType == "connectedFEDs") {
      this -> returnConnectedFEDsDetails(aOut);
      return;
    }

    return;
  }

  /*
  ██████  ██████   ██████   ██████     ██████  ███████ ████████  █████  ██ ██      ███████
  ██   ██ ██   ██ ██    ██ ██          ██   ██ ██         ██    ██   ██ ██ ██      ██
  ██████  ██████  ██    ██ ██          ██   ██ █████      ██    ███████ ██ ██      ███████
  ██      ██   ██ ██    ██ ██          ██   ██ ██         ██    ██   ██ ██ ██           ██
  ██      ██   ██  ██████   ██████     ██████  ███████    ██    ██   ██ ██ ███████ ███████
  */


  void SetupPanel::returnProcessorDetails(const std::string& aProcId, std::ostream& aOut)
  {
    Json::Value lDetails(Json::arrayValue);

    CellContext::SharedGuard_t lGuard(mCellContext);
    swatch::system::System& lSys = mCellContext.getSystem(lGuard);

    swatch::processor::Processor& p = lSys.getObj<swatch::processor::Processor>(aProcId);

    // std::cout << "Processor found " << p << std::endl;

    const swatch::processor::ProcessorStub& stub = p.getStub();

    Json::Value lJsonID(Json::objectValue);
    lJsonID["name"] = "ID";
    lJsonID["value"] = p.getId();
    lDetails.append(lJsonID);

    Json::Value lJsonPath(Json::objectValue);
    lJsonPath["name"] = "Path";
    lJsonPath["value"] = p.getPath();
    lDetails.append(lJsonPath);

    Json::Value lJsonHWType(Json::objectValue);
    lJsonHWType["name"] = "Hardware type";
    lJsonHWType["value"] = stub.hwtype;
    lDetails.append(lJsonHWType);

    Json::Value lJsonRole(Json::objectValue);
    lJsonRole["name"] = "Role";
    lJsonRole["value"] = stub.role;
    lDetails.append(lJsonRole);

    Json::Value lJsonCreator(Json::objectValue);
    lJsonCreator["name"] = "Creator";
    lJsonCreator["value"] = stub.creator;
    lDetails.append(lJsonCreator);

    Json::Value lJsonURI(Json::objectValue);
    lJsonURI["name"] = "URI";
    lJsonURI["value"] = stub.uri;
    lDetails.append(lJsonURI);

    Json::Value lJsonAddressTable(Json::objectValue);
    lJsonAddressTable["name"] = "Address table";
    lJsonAddressTable["value"] = stub.addressTable;
    lDetails.append(lJsonAddressTable);

    Json::Value lJsonCrate(Json::objectValue);
    lJsonCrate["name"] = "Crate";
    lJsonCrate["value"] = stub.crate;
    lDetails.append(lJsonCrate);

    Json::Value lJsonSlot(Json::objectValue);
    lJsonSlot["name"] = "Slot";
    lJsonSlot["value"] = boost::lexical_cast<std::string>(stub.slot);
    lDetails.append(lJsonSlot);

    // Rx ports
    std::ostringstream lOss;
    if (!stub.rxPorts.empty()) {
      std::vector<swatch::processor::ProcessorPortStub>::const_iterator itPort;
      itPort = stub.rxPorts.begin();
      lOss << (itPort++)->id;
      for ( ; itPort != stub.rxPorts.end(); ++itPort)
        lOss << ", " << itPort->id;
    }
    else
      lOss << "None";

    Json::Value rxPorts(Json::objectValue);
    rxPorts["name"] = "Rx ports";
    rxPorts["value"] = lOss.str();
    lDetails.append(rxPorts);

    // Tx ports
    lOss.str("");
    if (!stub.txPorts.empty()) {
      std::vector<swatch::processor::ProcessorPortStub>::const_iterator itPort;
      itPort = stub.txPorts.begin();
      lOss << (itPort++)->id;
      for ( ; itPort != stub.txPorts.end(); ++itPort)
        lOss << ", " << itPort->id;
    }
    else
      lOss << "None";

    Json::Value txPorts(Json::objectValue);
    txPorts["name"] = "Tx ports";
    txPorts["value"] = lOss.str();
    lDetails.append(txPorts);

    aOut << lDetails;

  }

  /*
  ██████   █████   ██████  ████████ ████████  ██████ ███████     ██████  ███████ ████████  █████  ██ ██      ███████
  ██   ██ ██   ██ ██    ██    ██       ██    ██      ██          ██   ██ ██         ██    ██   ██ ██ ██      ██
  ██   ██ ███████ ██    ██    ██       ██    ██      ███████     ██   ██ █████      ██    ███████ ██ ██      ███████
  ██   ██ ██   ██ ██ ▄▄ ██    ██       ██    ██           ██     ██   ██ ██         ██    ██   ██ ██ ██           ██
  ██████  ██   ██  ██████     ██       ██     ██████ ███████     ██████  ███████    ██    ██   ██ ██ ███████ ███████
                      ▀▀
  */

  void SetupPanel::returnDAQTTCDetails(const std::string& aDtmId, std::ostream& aOut)
  {
    Json::Value lDetails(Json::arrayValue);

    CellContext::SharedGuard_t lGuard(mCellContext);
    swatch::system::System& lSys = mCellContext.getSystem(lGuard);

    swatch::dtm::DaqTTCManager& dtm = lSys.getObj<swatch::dtm::DaqTTCManager>(aDtmId);

    const swatch::dtm::DaqTTCStub& stub = dtm.getStub();

    Json::Value lJsonID(Json::objectValue);
    lJsonID["name"] = "ID";
    lJsonID["value"] = dtm.getId();
    lDetails.append(lJsonID);

    Json::Value lJsonPath(Json::objectValue);
    lJsonPath["name"] = "Path";
    lJsonPath["value"] = dtm.getPath();
    lDetails.append(lJsonPath);

    Json::Value lJsonRole(Json::objectValue);
    lJsonRole["name"] = "Role";
    lJsonRole["value"] = stub.role;
    lDetails.append(lJsonRole);

    Json::Value lJsonCreator(Json::objectValue);
    lJsonCreator["name"] = "Creator";
    lJsonCreator["value"] = stub.creator;
    lDetails.append(lJsonCreator);

    Json::Value lJsonURIT1(Json::objectValue);
    lJsonURIT1["name"] = "T1 URI";
    lJsonURIT1["value"] = stub.uriT1;
    lDetails.append(lJsonURIT1);

    Json::Value lJsonURIT2(Json::objectValue);
    lJsonURIT2["name"] = "T2 URI";
    lJsonURIT2["value"] = stub.uriT2;
    lDetails.append(lJsonURIT2);

    Json::Value lJsonAddressTableT1(Json::objectValue);
    lJsonAddressTableT1["name"] = "T1 Address table";
    lJsonAddressTableT1["value"] = stub.addressTableT1;
    lDetails.append(lJsonAddressTableT1);

    Json::Value lJsonAddressTableT2(Json::objectValue);
    lJsonAddressTableT2["name"] = "T2 Address table";
    lJsonAddressTableT2["value"] = stub.addressTableT2;
    lDetails.append(lJsonAddressTableT2);

    Json::Value lJsonCrate(Json::objectValue);
    lJsonCrate["name"] = "Crate";
    lJsonCrate["value"] = stub.crate;
    lDetails.append(lJsonCrate);

    Json::Value lJsonSlot(Json::objectValue);
    lJsonSlot["name"] = "Slot";
    lJsonSlot["value"] = boost::lexical_cast<std::string>(stub.slot);
    lDetails.append(lJsonSlot);

    Json::Value lJsonFEDID(Json::objectValue);
    lJsonFEDID["name"] = "FED ID";
    lJsonFEDID["value"] = boost::lexical_cast<std::string>(stub.fedId);
    lDetails.append(lJsonFEDID);

    aOut << lDetails;

  }

  /*
   ██████ ██████   █████  ████████ ███████     ██████  ███████ ████████  █████  ██ ██      ███████
  ██      ██   ██ ██   ██    ██    ██          ██   ██ ██         ██    ██   ██ ██ ██      ██
  ██      ██████  ███████    ██    █████       ██   ██ █████      ██    ███████ ██ ██      ███████
  ██      ██   ██ ██   ██    ██    ██          ██   ██ ██         ██    ██   ██ ██ ██           ██
   ██████ ██   ██ ██   ██    ██    ███████     ██████  ███████    ██    ██   ██ ██ ███████ ███████
  */


  void SetupPanel::returnCrateDetails(const std::string& aCrateId, std::ostream& aOut){

    Json::Value lDetails(Json::arrayValue);

    CellContext::SharedGuard_t lGuard(mCellContext);
    swatch::system::System& lSys = mCellContext.getSystem(lGuard);

    swatch::system::Crate& c = lSys.getObj<swatch::system::Crate>(aCrateId);

    const swatch::system::CrateStub& stub = c.getStub();

    Json::Value lJsonID(Json::objectValue);
    lJsonID["name"] = "ID";
    lJsonID["value"] = c.getId();
    lDetails.append(lJsonID);

    Json::Value lJsonPath(Json::objectValue);
    lJsonPath["name"] = "Path";
    lJsonPath["value"] = c.getPath();
    lDetails.append(lJsonPath);

    Json::Value lJsonLocation(Json::objectValue);
    lJsonLocation["name"] = "Location";
    lJsonLocation["value"] = stub.location;
    lDetails.append(lJsonLocation);

    Json::Value lJsonDescription(Json::objectValue);
    lJsonDescription["name"] = "Creator";
    lJsonDescription["value"] = stub.description;
    lDetails.append(lJsonDescription);

    std::vector<uint32_t> slots = c.getAMCSlots();
    std::ostringstream lOss;
    if (!slots.empty()) {
      // Convert all but the last element to avoid a trailing ","
      std::copy(slots.begin(), slots.end()-1,
      std::ostream_iterator<int>(lOss, ", "));

      // Now add the last element with no delimiter
      lOss << slots.back();
    }

    Json::Value lJsonSlots(Json::objectValue);
    lJsonSlots["name"] = "Populated slots";
    lJsonSlots["value"] = lOss.str();
    lDetails.append(lJsonSlots);

    aOut << lDetails;

  }

  /*
██    ██ ███    ██ ██    ██ ███████ ███████ ██████      ██████  ███████ ████████  █████  ██ ██      ███████
██    ██ ████   ██ ██    ██ ██      ██      ██   ██     ██   ██ ██         ██    ██   ██ ██ ██      ██
██    ██ ██ ██  ██ ██    ██ ███████ █████   ██   ██     ██   ██ █████      ██    ███████ ██ ██      ███████
██    ██ ██  ██ ██ ██    ██      ██ ██      ██   ██     ██   ██ ██         ██    ██   ██ ██ ██           ██
 ██████  ██   ████  ██████  ███████ ███████ ██████      ██████  ███████    ██    ██   ██ ██ ███████ ███████
*/


  void SetupPanel::returnProcessorsDetails(std::ostream& aOut)
  {
  }

  void SetupPanel::returnDAQTTCsDetails(std::ostream& aOut)
  {
  }

  void SetupPanel::returnCratesDetails(std::ostream& aOut)
  {
  }

  /*
  ██      ██ ███    ██ ██   ██ ███████     ██████  ███████ ████████  █████  ██ ██      ███████
  ██      ██ ████   ██ ██  ██  ██          ██   ██ ██         ██    ██   ██ ██ ██      ██
  ██      ██ ██ ██  ██ █████   ███████     ██   ██ █████      ██    ███████ ██ ██      ███████
  ██      ██ ██  ██ ██ ██  ██       ██     ██   ██ ██         ██    ██   ██ ██ ██           ██
  ███████ ██ ██   ████ ██   ██ ███████     ██████  ███████    ██    ██   ██ ██ ███████ ███████
  */


  void SetupPanel::returnLinksDetails(std::ostream& aOut)
  {

    Json::Value lDetails(Json::arrayValue);

    { // Lock system in this scope
      CellContext::SharedGuard_t lGuard(mCellContext);
      swatch::system::System& lSystem = mCellContext.getSystem(lGuard);

      std::deque<swatch::system::Link*> lLinks = lSystem.getLinks();
      for( size_t i(0); i<lLinks.size(); ++i ) {

        swatch::system::Link* lLink = lLinks[i];

        Json::Value lJsonLink(Json::objectValue);

        lJsonLink["id"] = lLink->getId();

        lJsonLink["srcPort"] = lLink->getSrcProcessor()->getId()+"."+
        lLink->getSrcProcessor()->getOutputPorts().getId()+"."+
        lLink->getSrcPort()->getId();

        lJsonLink["dstPort"] = lLink->getDstProcessor()->getId()+"."+
        lLink->getDstProcessor()->getInputPorts().getId()+"."+
        lLink->getDstPort()->getId();

        lDetails.append(lJsonLink);
      }
    }

    aOut << lDetails;
  }

  /*
  ███████ ███████ ██████  ███████     ██████  ███████ ████████  █████  ██ ██      ███████
  ██      ██      ██   ██ ██          ██   ██ ██         ██    ██   ██ ██ ██      ██
  █████   █████   ██   ██ ███████     ██   ██ █████      ██    ███████ ██ ██      ███████
  ██      ██      ██   ██      ██     ██   ██ ██         ██    ██   ██ ██ ██           ██
  ██      ███████ ██████  ███████     ██████  ███████    ██    ██   ██ ██ ███████ ███████
  */

  void SetupPanel::returnConnectedFEDsDetails(std::ostream& aOut)
  {

    CellContext::SharedGuard_t lGuard(mCellContext);
    swatch::system::System& lSystem = mCellContext.getSystem(lGuard);

    Json::Value lDetails(Json::arrayValue);

    BOOST_FOREACH( swatch::system::SystemStub::FEDInputPortsMap_t::value_type p, lSystem.getStub().connectedFEDs ) {
      Json::Value lJsonFED(Json::objectValue);

      lJsonFED["FED"] = "FED "+boost::lexical_cast<std::string>(p.first);

      Json::Value lJsonPorts(Json::arrayValue);

      for ( std::vector<std::string>::iterator it=p.second.begin(); it!=p.second.end(); ++it){
        lJsonPorts.append(*it);
      }

      lJsonFED["inputs"] = lJsonPorts;

      lDetails.append(lJsonFED);
    }

    aOut << lDetails;

    return;

  }

  /*
  ███████ ██    ██ ███████     ██████  ███████ ████████  █████  ██ ██      ███████
  ██       ██  ██  ██          ██   ██ ██         ██    ██   ██ ██ ██      ██
  ███████   ████   ███████     ██   ██ █████      ██    ███████ ██ ██      ███████
       ██    ██         ██     ██   ██ ██         ██    ██   ██ ██ ██           ██
  ███████    ██    ███████     ██████  ███████    ██    ██   ██ ██ ███████ ███████
  */

  void SetupPanel::returnSystemDetails(std::ostream& aOut)
  {
    CellContext::SharedGuard_t lGuard(mCellContext);
    swatch::system::System& lSys = mCellContext.getSystem(lGuard);
    const swatch::system::SystemStub & stub = lSys.getStub();

    Json::Value lDetails(Json::arrayValue);

    Json::Value lJsonID(Json::objectValue);
    lJsonID["name"] = "ID";
    lJsonID["value"] = stub.id;
    lDetails.append(lJsonID);

    Json::Value lJsonPath(Json::objectValue);
    lJsonPath["name"] = "Path";
    lJsonPath["value"] = lSys.getPath();
    lDetails.append(lJsonPath);

    Json::Value lJsonCreator(Json::objectValue);
    lJsonCreator["name"] = "Creator";
    lJsonCreator["value"] = stub.creator;
    lDetails.append(lJsonCreator);

    Json::Value lJsonNProcessors(Json::objectValue);
    lJsonNProcessors["name"] = "N. processors";
    lJsonNProcessors["value"] = boost::lexical_cast<std::string>(stub.processors.size());
    lDetails.append(lJsonNProcessors);

    Json::Value lJsonNDAQTTCS(Json::objectValue);
    lJsonNDAQTTCS["name"] = "N. daqttcs";
    lJsonNDAQTTCS["value"] = boost::lexical_cast<std::string>(stub.daqttcs.size());
    lDetails.append(lJsonNDAQTTCS);

    Json::Value lJsonNCrates(Json::objectValue);
    lJsonNCrates["name"] = "N. crates";
    lJsonNCrates["value"] = boost::lexical_cast<std::string>(stub.crates.size());
    lDetails.append(lJsonNCrates);

    aOut << lDetails;

  }


  /*
  ███    ███  ██████  ███    ██     ████████ ██   ██ ██████  ███████  █████  ██████
  ████  ████ ██    ██ ████   ██        ██    ██   ██ ██   ██ ██      ██   ██ ██   ██
  ██ ████ ██ ██    ██ ██ ██  ██        ██    ███████ ██████  █████   ███████ ██   ██
  ██  ██  ██ ██    ██ ██  ██ ██        ██    ██   ██ ██   ██ ██      ██   ██ ██   ██
  ██      ██  ██████  ██   ████        ██    ██   ██ ██   ██ ███████ ██   ██ ██████
  */

  /*
   ██████  ███████ ████████     ███████ ████████  █████  ████████ ██    ██ ███████
  ██       ██         ██        ██         ██    ██   ██    ██    ██    ██ ██
  ██   ███ █████      ██        ███████    ██    ███████    ██    ██    ██ ███████
  ██    ██ ██         ██             ██    ██    ██   ██    ██    ██    ██      ██
   ██████  ███████    ██        ███████    ██    ██   ██    ██     ██████  ███████
  */

  void SetupPanel::getMonitoringThreadStatus(cgicc::Cgicc& aCgi, std::ostream& aOut)
  {
    MonitoringThread& lMonThread = mCellContext.getMonitoringThread();
    Json::Value lJsonMonitoringThreadStatus(Json::objectValue);
    lJsonMonitoringThreadStatus["status"] = lMonThread.isActive() ? "Running" : "Stopped";
    lJsonMonitoringThreadStatus["period"] = boost::lexical_cast<std::string>(lMonThread.getPeriod());
    aOut << lJsonMonitoringThreadStatus;
    return;
  }

  /*
  ██████  ██    ██ ███    ██      ██████ ███    ███ ██████
  ██   ██ ██    ██ ████   ██     ██      ████  ████ ██   ██
  ██████  ██    ██ ██ ██  ██     ██      ██ ████ ██ ██   ██
  ██   ██ ██    ██ ██  ██ ██     ██      ██  ██  ██ ██   ██
  ██   ██  ██████  ██   ████      ██████ ██      ██ ██████
  */

  void SetupPanel::monitoringThreadRunCommand(cgicc::Cgicc& aCgi, std::ostream& aOut)
  {
    std::map<std::string, std::string> lValues(ajax::toolbox::getSubmittedValues(aCgi));
    std::string lCommand = lValues["command"];
    if ((lCommand == "undefined")||(lCommand == "")) {
      aOut << "{}";
      return;
    }

    MonitoringThread& lMonThread = mCellContext.getMonitoringThread();

    if (lCommand == "start"){
      lMonThread.start();
    }

    if (lCommand == "stop"){
      lMonThread.stop();
    }

    if (lCommand == "execute"){
      lMonThread.runNow();
    }

    if (lCommand == "setperiod"){
      std::string lNewPeriodStr = lValues["period"];
      if ((lNewPeriodStr == "undefined")||(lNewPeriodStr == "")) return;
      size_t lNewPeriod = boost::lexical_cast<size_t>(lNewPeriodStr);
      mCellContext.getMonitoringThread().setPeriod(lNewPeriod);
    }


    Json::Value lJsonMonitoringThreadStatus(Json::objectValue);
    lJsonMonitoringThreadStatus["status"] = lMonThread.isActive() ? "Running" : "Stopped";
    lJsonMonitoringThreadStatus["period"] = boost::lexical_cast<std::string>(lMonThread.getPeriod());
    aOut << lJsonMonitoringThreadStatus;
    return;
  }

  /*
 ██████  ██   ██     ███████ ███████ ████████ ██    ██ ██████
██       ██  ██      ██      ██         ██    ██    ██ ██   ██
██   ███ █████       ███████ █████      ██    ██    ██ ██████
██    ██ ██  ██           ██ ██         ██    ██    ██ ██
 ██████  ██   ██     ███████ ███████    ██     ██████  ██
*/

/*
██       ██████   █████  ██████       ██████  ██   ██     ██   ██ ███    ███ ██
██      ██    ██ ██   ██ ██   ██     ██       ██  ██       ██ ██  ████  ████ ██
██      ██    ██ ███████ ██   ██     ██   ███ █████         ███   ██ ████ ██ ██
██      ██    ██ ██   ██ ██   ██     ██    ██ ██  ██       ██ ██  ██  ██  ██ ██
███████  ██████  ██   ██ ██████       ██████  ██   ██     ██   ██ ██      ██ ███████
*/


  void SetupPanel::loadGateKeeperFromXML(cgicc::Cgicc& aCgi, std::ostream& aOut)
  {
    std::map<std::string, std::string> lValues(ajax::toolbox::getSubmittedValues(aCgi));
    std::string lXmlFilePath = lValues["xmlfilepath"];
    std::string lKey = lValues["key"];
    std::string load = lValues["load"];

    CellContext::WriteGuard_t lGuard(mCellContext);

    if (load == "true") {
      try {
        mCellContext.setGateKeeperFromFile(lXmlFilePath, lKey, lGuard);
        Json::Value lGkReturn(Json::objectValue);
        lGkReturn["status"] = "Loaded";
        lGkReturn["output"] = "";
        aOut << lGkReturn;
      } catch ( std::exception& e ) {
        std::ostringstream lOss;
        lOss << "Failed to load gateKeeper from file '" << lXmlFilePath << "', key '" << lKey << "'. Details below. \n ";
        lOss << "Exception '" << swatch::core::demangleName(typeid(e).name()) << "': " << e.what();
        std::string lErrorMessage = lOss.str();
        LOG4CPLUS_ERROR(getLogger(), lErrorMessage);

        Json::Value lGkReturn(Json::objectValue);
        lGkReturn["status"] = "Error";
        lGkReturn["error"] = lErrorMessage;
        lGkReturn["output"] = "";
        aOut << lGkReturn;
        return;
      }
    }

    if ((load == "undefined") || (load == "false")) {
      if (getGateKeeper(lGuard)){
        std::ostringstream lOss;
        lOss << *getGateKeeper(lGuard);
        Json::Value lGkReturn(Json::objectValue);
        lGkReturn["status"] = "Loaded";
        lGkReturn["output"] = lOss.str();
        aOut << lGkReturn;
      } else {
        Json::Value lGkReturn(Json::objectValue);
        lGkReturn["status"] = "Undefined";
        lGkReturn["output"] = "";
        aOut << lGkReturn;
      }
    }

  }

  /*
  ██       ██████   █████  ██████       ██████  ██   ██     ██████  ██████
  ██      ██    ██ ██   ██ ██   ██     ██       ██  ██      ██   ██ ██   ██
  ██      ██    ██ ███████ ██   ██     ██   ███ █████       ██   ██ ██████
  ██      ██    ██ ██   ██ ██   ██     ██    ██ ██  ██      ██   ██ ██   ██
  ███████  ██████  ██   ██ ██████       ██████  ██   ██     ██████  ██████
  */


  void SetupPanel::loadGateKeeperFromDB(cgicc::Cgicc& aCgi, std::ostream& aOut)
  {
    std::map<std::string, std::string> lValues(ajax::toolbox::getSubmittedValues(aCgi));
    // Extract DB paths
    const std::string lSubsystemId = lValues["subsystemid"];
    const std::string lConfigKey = lValues["configurationkey"];
    const std::string lRunSettingsKey = lValues["runsettingskey"];

    CellContext::WriteGuard_t lGuard(mCellContext);

    try {
      mCellContext.setGateKeeperFromDb(lSubsystemId, lConfigKey, lRunSettingsKey, lGuard);

      if (getGateKeeper(lGuard)){
        Json::Value lGkReturn(Json::objectValue);
        lGkReturn["status"] = "Loaded";
        lGkReturn["output"] = "";
        aOut << lGkReturn;
      }

    } catch ( std::exception& e ) {
      std::ostringstream lOss;
      lOss << "Failed to load gateKeeper from DB for subsystem '" << lSubsystemId << "', with config key '";
      lOss << lConfigKey << "', RS key '" << lRunSettingsKey << "'. Details below. \n";
      lOss << "Exception '" << swatch::core::demangleName(typeid(e).name()) << "': " << e.what();
      std::string lErrorMessage = lOss.str();
      LOG4CPLUS_ERROR(getLogger(), lErrorMessage);

      Json::Value lGkReturn(Json::objectValue);
      lGkReturn["status"] = "Error";
      lGkReturn["error"] = lErrorMessage;
      lGkReturn["output"] = "";
      aOut << lGkReturn;

      return;
    }

  }

} // end ms swatchframework

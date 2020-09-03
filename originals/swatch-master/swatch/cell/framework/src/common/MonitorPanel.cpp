#include "swatchcell/framework/MonitorPanel.h"
#include "swatchcell/framework/tools/panelFunctions.h"
#include "swatchcell/framework/tools/polymerFunctions.h"

#include <vector>

#include "boost/foreach.hpp"
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "ajax/PlainHtml.h"
#include "ajax/Table.h"
#include "ajax/toolbox.h"
#include "ajax/Div.h"
#include "ajax/PolymerElement.h"

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"

#include "swatch/core/MetricView.hpp"
#include "swatch/core/StatusFlag.hpp"
#include "swatch/core/MonitorableObject.hpp"
#include "swatch/dtm/DaqTTCManager.hpp"
#include "swatch/dtm/SLinkExpress.hpp"
#include "swatch/system/Crate.hpp"
#include "swatch/system/Link.hpp"
#include "swatch/system/System.hpp"
#include "swatch/processor/Processor.hpp"
#include "swatch/processor/TTCInterface.hpp"
#include "swatch/processor/PortCollection.hpp"
#include "swatch/processor/AlgoInterface.hpp"
#include "swatch/processor/Port.hpp"


namespace swatchcellframework
{

  MonitorPanel::MonitorPanel(tsframework::CellAbstractContext* context, log4cplus::Logger& logger)
  :
    CellPanel(context, logger),
    logger_(logger),
    cellContext_(dynamic_cast<swatchcellframework::CellContext&>(*context)),
    resultBox_(new ajax::ResultBox())
    //objectResultBox_(new ajax::ResultBox())
  {
    resultBox_->setIsOwned(false);
    // objectResultBox_->setIsOwned(false);
    // objectResultBox_->set("style", "padding-top: 15px;");
  }


  MonitorPanel::~MonitorPanel()
  {
    delete resultBox_;
    // delete objectResultBox_;
  }

  /*
  ██       █████  ██    ██  ██████  ██    ██ ████████
  ██      ██   ██  ██  ██  ██    ██ ██    ██    ██
  ██      ███████   ████   ██    ██ ██    ██    ██
  ██      ██   ██    ██    ██    ██ ██    ██    ██
  ███████ ██   ██    ██     ██████   ██████     ██
  */

  void MonitorPanel::layout(cgicc::Cgicc& cgi)
  {
    this->remove();

    ajax::PolymerElement* title = new ajax::PolymerElement("monitoring-title");
    title -> setId("monitoringtitle");
    {
      CellContext::SharedGuard_t lGuard(cellContext_);
      swatch::system::System& lSystem = cellContext_.getSystem(lGuard);
      title->set("title", lSystem.getId());
      std::ostringstream oss;
      oss << lSystem.getStatusFlag();
      title->set("status", oss.str());
    }





    ajax::PolymerElement* tabPage = new ajax::PolymerElement("generic-tabbed-page");
    tabPage->set("pageid", "monitoring");
    tabPage->set("tabnames", ajax::toolbox::escapeHTML("[\"System\", \"Processors\", \"Object Details\", \"Ports\"]"));

    setEvent("tabbed-page-change-monitoring", ajax::Eventable::OnSubmit, this, &swatchcellframework::MonitorPanel::switchView);

    setEvent("MonitorPanel::respondProcessorJSON", ajax::Eventable::OnTime, this, &MonitorPanel::respondProcessorJSON);
    setEvent("MonitorPanel::respondSystemJSON", ajax::Eventable::OnTime, this, &MonitorPanel::respondSystemJSON);
    setEvent("MonitorPanel::respondMonitoringTitleJSON", ajax::Eventable::OnTime, this, &MonitorPanel::respondMonitoringTitleJSON);
    setEvent("MonitorPanel::switchComponent", ajax::Eventable::OnTime, this, &swatchcellframework::MonitorPanel::switchComponent);
    setEvent("MonitorPanel::respondPortsJSON", ajax::Eventable::OnTime, this, &MonitorPanel::respondPortsJSON);

    this->add(title);
    this->add(tabPage);
    this->add(resultBox_);
    // this->add(objectResultBox_);
  }

  void MonitorPanel::respondObjectPage()
  {
//    std::cout << " > MonitorPanel::respondObjectPage" << std::endl;

    CellContext::SharedGuard_t lGuard(cellContext_);
    swatch::system::System& lSystem = cellContext_.getSystem(lGuard);

    ajax::PolymerElement* componentView = new ajax::PolymerElement("component-view");

    std::ostringstream objects;

    // this should be moved to polymerFunctions
    objects << "[";
    for(std::deque<swatch::processor::Processor*>::const_iterator it=lSystem.getProcessors().begin(); it != lSystem.getProcessors().end(); ++it)
      objects << "\"" << (*it)->getId() << "\",";

    for(std::deque<swatch::dtm::DaqTTCManager*>::const_iterator it=lSystem.getDaqTTCs().begin(); it != lSystem.getDaqTTCs().end(); ++it)
      {
        objects << "\"" << (*it)->getId() << "\"";
        if (it != --lSystem.getDaqTTCs().end()) {
          objects << ",";
        }
      }
    objects << "]";
//    std::cout << objects.str() << std::endl;
    componentView->set("components", ajax::toolbox::escapeHTML(objects.str()));
    resultBox_->add(componentView);
  }

  void MonitorPanel::respondProcessorsPage()
  {
//    std::cout << " > MonitorPanel::respondProcessorsPage" << std::endl;
    ajax::PolymerElement* processorsView = new ajax::PolymerElement("processors-view");
    resultBox_->add(processorsView);
  }

  void MonitorPanel::respondPortsPage()
  {
//    std::cout << " > MonitorPanel::respondPortsPage" << std::endl;
    ajax::PolymerElement* portsView = new ajax::PolymerElement("ports-view");
    resultBox_->add(portsView);
  }


  void MonitorPanel::respondSystemPage()
  {
//    std::cout << " > MonitorPanel::respondSystemPage" << std::endl;
    ajax::PolymerElement* systempage = new ajax::PolymerElement("system-page");
    resultBox_->add(systempage);
  }



  void MonitorPanel::switchView(cgicc::Cgicc& cgi, std::ostream& out)
  {
    resultBox_->remove();
    std::map<std::string, std::string> values(ajax::toolbox::getSubmittedValues(cgi));
    mode_ = static_cast<MonitorPanel::Mode>(boost::lexical_cast<int>(values["selected"]));
//    std::cout << " > MonitorPanel::switchView (" << mode_ << ")" << std::endl;

    switch(mode_) {
    case MonitorPanel::kSystem:
      respondSystemPage();
      break;
    case MonitorPanel::kProcessors:
      respondProcessorsPage();
      break;
    case MonitorPanel::kObjectDetails:
      respondObjectPage();
      break;
    case MonitorPanel::kPorts:
      respondPortsPage();
      break;
    }

    resultBox_->innerHtml(cgi, out);
  }


    /*
         ██ ███████  ██████  ███    ██
         ██ ██      ██    ██ ████   ██
         ██ ███████ ██    ██ ██ ██  ██
    ██   ██      ██ ██    ██ ██  ██ ██
     █████  ███████  ██████  ██   ████
    */


    /*
     ██████  ██████  ███    ███ ██████      ██    ██ ██ ███████ ██     ██
    ██      ██    ██ ████  ████ ██   ██     ██    ██ ██ ██      ██     ██
    ██      ██    ██ ██ ████ ██ ██████      ██    ██ ██ █████   ██  █  ██
    ██      ██    ██ ██  ██  ██ ██           ██  ██  ██ ██      ██ ███ ██
     ██████  ██████  ██      ██ ██            ████   ██ ███████  ███ ███
    */



  void MonitorPanel::switchComponent(cgicc::Cgicc& cgi,std::ostream& out)
  {
    std::map<std::string,std::string> lValues(ajax::toolbox::getSubmittedValues(cgi));
    std::string lMonitorablesJsonString(lValues["monitorables"]);
    if ((lMonitorablesJsonString == "undefined")||(lMonitorablesJsonString == "")) return;

    std::stringstream  lSs;
    lSs << lMonitorablesJsonString;
    Json::Value lMonitorablesJson;
    lSs >> lMonitorablesJson;

    Json::ValueIterator lComponentJson = lMonitorablesJson.begin();

    if (lComponentJson == lMonitorablesJson.end()) return;

    std::string lObjId = lComponentJson -> get("id", "").asString();
    Json::Value lSubComponentArray = lComponentJson -> get("openedSubmonitorables", "");

//    std::cout << " > MonitorPanel::switchComponent (" << objId << ")" << std::endl;
    CellContext::SharedGuard_t lGuard(cellContext_);
    swatch::system::System& lSystem = cellContext_.getSystem(lGuard);

    try {
      if( swatch::processor::Processor* lProcessor = lSystem.getObjPtr<swatch::processor::Processor>(lObjId) )
      {
        Json::Value lJsonOutput(Json::objectValue);
        serializeProcessorForComponentView(lJsonOutput, lSubComponentArray ,*lProcessor);
        out << lJsonOutput;
      }
      else if( swatch::dtm::DaqTTCManager* lDaqTTC = lSystem.getObjPtr<swatch::dtm::DaqTTCManager>(lObjId) )
      {
        Json::Value lJsonOutput(Json::objectValue);
        serializeAMC13ForComponentView(lJsonOutput, lSubComponentArray ,*lDaqTTC);
        out << lJsonOutput;
      }
    }
    catch (const std::exception& lException)
      {
        out << "{}";
	// addPlainHtml(*objectResultBox_) << "ERROR encountered trying to access processor/daqTTCManager of ID '" << objId << "'<br>Exception (type: " << swatch::core::demangleName(typeid(e).name()) << ") thrown: " << e.what();
      }
  }


  std::ostream& operator<<(std::ostream& aOut, MonitorPanel::Mode aMode){
    if (aMode == MonitorPanel::kSystem)
      return (aOut << "System");
    else if (aMode == MonitorPanel::kProcessors)
      return (aOut << "Processors");
    else if (aMode == MonitorPanel::kObjectDetails)
      return (aOut << "ObjectDetails");
    else if (aMode == MonitorPanel::kPorts)
      return (aOut << "Ports");
    XCEPT_RAISE(swatch::core::RuntimeError,"Invalid MonitorPanel::Mode enum value in ostream operator<<");
  }


  std::istream& operator>>(std::istream& aIn, MonitorPanel::Mode& aMode){
    std::string str;
    aIn >> str;

    if (str == "System")
      aMode = MonitorPanel::kSystem;
    else if (str == "Processors")
      aMode = MonitorPanel::kProcessors;
    else if (str == "ObjectDetails")
      aMode = MonitorPanel::kObjectDetails;
    else if (str == "Ports")
      aMode = MonitorPanel::kPorts;
    else
      XCEPT_RAISE(swatch::core::RuntimeError,"Invalid string value \"" + str + "\" for MonitorPanel::Mode enum");

    return aIn;
  }

  /*
  ██████  ██████   ██████   ██████     ██    ██ ██ ███████ ██     ██
  ██   ██ ██   ██ ██    ██ ██          ██    ██ ██ ██      ██     ██
  ██████  ██████  ██    ██ ██          ██    ██ ██ █████   ██  █  ██
  ██      ██   ██ ██    ██ ██           ██  ██  ██ ██      ██ ███ ██
  ██      ██   ██  ██████   ██████       ████   ██ ███████  ███ ███
  */

  void MonitorPanel::respondProcessorJSON(cgicc::Cgicc& cgi,std::ostream& out)
  {


    std::map<std::string,std::string> lValues(ajax::toolbox::getSubmittedValues(cgi));
    std::string lMonitorablesJsonString(lValues["monitorables"]);
    if ((lMonitorablesJsonString == "undefined")||(lMonitorablesJsonString == "")){
      lMonitorablesJsonString = "[]";
    }
    //Parsion the processor list
    std::stringstream  lSs;
    lSs << lMonitorablesJsonString;
    Json::Value lMonitorablesJson;
    lSs >> lMonitorablesJson;

    //Preparing the processors JSON
    CellContext::SharedGuard_t lGuard(cellContext_);
    swatch::system::System& system = cellContext_.getSystem(lGuard);

    Json::Value jsonOutput (Json::objectValue);
    Json::Value systemJson (Json::objectValue);
    Json::Value processorsArray (Json::arrayValue);
    for(std::deque<swatch::processor::Processor*>::const_iterator it=system.getProcessors().begin(); it != system.getProcessors().end(); ++it) {

      //I look for submonitorables list if present
      Json::Value lSubComponentArray;

      //Looking if that processor entry exists, if it exists I save the subcomp array
      for (Json::ValueIterator lComponentJson = lMonitorablesJson.begin(); lComponentJson != lMonitorablesJson.end(); lComponentJson++){
        std::string lObjId = lComponentJson -> get("id", "").asString();
        //If exists I save the subcomp array
        if (lObjId == (*it) -> getId()) {
          lSubComponentArray = lComponentJson -> get("openedSubmonitorables", Json::nullValue);
        }

      }


      Json::Value processorJson(Json::objectValue);
      serializeProcessorForProcessorsView(processorJson, lSubComponentArray, **it);
      processorsArray.append(processorJson);
    }
    systemJson["processors"] = processorsArray;
    jsonOutput["system"] = systemJson;

    out << jsonOutput;

    return;
  }


  /*
  ██████   ██████  ██████  ████████ ███████     ██    ██ ██ ███████ ██     ██
  ██   ██ ██    ██ ██   ██    ██    ██          ██    ██ ██ ██      ██     ██
  ██████  ██    ██ ██████     ██    ███████     ██    ██ ██ █████   ██  █  ██
  ██      ██    ██ ██   ██    ██         ██      ██  ██  ██ ██      ██ ███ ██
  ██       ██████  ██   ██    ██    ███████       ████   ██ ███████  ███ ███
  */

  void MonitorPanel::respondPortsJSON(cgicc::Cgicc& cgi,std::ostream& out){
    CellContext::SharedGuard_t lGuard(cellContext_);
    swatch::system::System& system = cellContext_.getSystem(lGuard);

    Json::Value lProcessorsArray(Json::arrayValue);
    const Json::Value nullValue(Json::nullValue);

    for(std::deque<swatch::processor::Processor*>::const_iterator it=system.getProcessors().begin(); it != system.getProcessors().end(); ++it) {

      Json::Value lProcessorJson(Json::objectValue);
      Json::Value lProcessorComponentsArrayJson(Json::arrayValue);
      Json::Value lInputPortsJson(Json::objectValue);
      Json::Value lOutputPortsJson(Json::objectValue);

      serializePortCollectionForPortsTable(lInputPortsJson, (*it) -> getInputPorts());

      serializePortCollectionForPortsTable(lOutputPortsJson, (*it) -> getOutputPorts());

      lProcessorComponentsArrayJson.append(lInputPortsJson);
      lProcessorComponentsArrayJson.append(lOutputPortsJson);
      lProcessorJson["components"] = lProcessorComponentsArrayJson;
      lProcessorJson["id"] = (*it) -> getId();
      lProcessorsArray.append(lProcessorJson);
    }

    out << lProcessorsArray;

    return;
  }

  /*
  ███████ ██    ██ ███████     ██    ██ ██ ███████ ██     ██
  ██       ██  ██  ██          ██    ██ ██ ██      ██     ██
  ███████   ████   ███████     ██    ██ ██ █████   ██  █  ██
       ██    ██         ██      ██  ██  ██ ██      ██ ███ ██
  ███████    ██    ███████       ████   ██ ███████  ███ ███
  */


  void MonitorPanel::respondSystemJSON(cgicc::Cgicc& cgi,std::ostream& out)
  {
    CellContext::SharedGuard_t lGuard(cellContext_);
    swatch::system::System& lSystem = this -> cellContext_.getSystem(lGuard);

    Json::Value lJsonOutput(Json::objectValue);

    const swatch::system::System::CratesMap_t& lCrates = lSystem.getCrates();

    Json::Value lCratesArray(Json::arrayValue);
    Json::Value lSystemJson(Json::objectValue);

    serializeMonitorableCompleteInfo(lSystemJson, lSystem);

    BOOST_FOREACH(const swatch::system::System::CratesMap_t::value_type& lCrateDesc, lCrates) {
      Json::Value lCrate(Json::objectValue);
      serializeCrateForSystemView(lCrate, *lCrateDesc.second);
      swatch::core::StatusFlag lProcStatus = swatch::core::kNoLimit;
      for (uint32_t i = 1; i <= 12; ++i) {
        if (lCrateDesc.second -> isAMCSlotTaken(i)) lProcStatus = lProcStatus & lCrateDesc.second -> amc(i) -> getStatusFlag();
      }
      std::ostringstream lOss;
	    lOss << lProcStatus;
	    lCrate["processor_status"] = lOss.str();
      lCratesArray.append(lCrate);
    }
    lSystemJson["crates"] = lCratesArray;
    if (!lCrates.empty()) {
      Json::Value lDaqttcinfo(Json::objectValue);
      Json::Value lAmc13Array(Json::arrayValue);
      swatch::core::StatusFlag amc13Status = swatch::core::kNoLimit;
      BOOST_FOREACH(const swatch::system::System::CratesMap_t::value_type& crateDesc, lCrates) {
        Json::Value amc13Json(Json::objectValue);
        if (const swatch::dtm::DaqTTCManager * amc13 = crateDesc.second->amc13()) {
          serializeAMC13ForSystemView(amc13Json, *crateDesc.second);
          amc13Status = amc13Status & amc13->getStatusFlag();
        } else {
          amc13Json["status"] = "Empty";
        }
        lAmc13Array.append(amc13Json);
      }
      std::ostringstream ossDaq;
      ossDaq << amc13Status;
      lDaqttcinfo["ttc_daq_status"] = ossDaq.str();
      lDaqttcinfo["ttc_daqs"] = lAmc13Array;
      lJsonOutput["daqttcinfo"] = lDaqttcinfo;
    }

    std::deque<swatch::system::Link*> lLinks( lSystem.getLinks() );
    if ( ! lLinks.empty() ) {
      Json::Value lLinksJson(Json::objectValue);
      serializeLinks(lLinksJson, lLinks, lSystem.getId().size());
      lJsonOutput["links"] = lLinksJson;
    }

    lJsonOutput["system"] = lSystemJson;
    out << lJsonOutput;

    return;
  }

  /*
  ███    ███  ██████  ███    ██     ████████ ██ ████████ ██      ███████
  ████  ████ ██    ██ ████   ██        ██    ██    ██    ██      ██
  ██ ████ ██ ██    ██ ██ ██  ██        ██    ██    ██    ██      █████
  ██  ██  ██ ██    ██ ██  ██ ██        ██    ██    ██    ██      ██
  ██      ██  ██████  ██   ████        ██    ██    ██    ███████ ███████
  */


  void MonitorPanel::respondMonitoringTitleJSON(cgicc::Cgicc& cgi,std::ostream& out){
    CellContext::SharedGuard_t lGuard(cellContext_);
    swatch::system::System& lSystem = cellContext_.getSystem(lGuard);
    Json::Value lJsonOutput (Json::objectValue);
    std::ostringstream lOss;
    lOss << lSystem.getStatusFlag();
    lJsonOutput["status"] = lOss.str();
    serializeStateMachineStatus(lJsonOutput, lSystem.getStatus());
    out << lJsonOutput;
    return;
  }


} // end ms swatchframework

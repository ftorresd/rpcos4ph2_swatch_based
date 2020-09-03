#include "swatchcell/framework/CommandPanel.h"


#include "boost/foreach.hpp"
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "ajax/PolymerElement.h"
#include "ajax/toolbox.h"

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"

#include "xdata/Serializable.h"

#include "swatch/action/Command.hpp"
#include "swatch/core/ReadWriteXParameterSet.hpp"
#include "swatch/core/XParameterSet.hpp"
#include "swatch/system/Crate.hpp"
#include "swatch/dtm/DaqTTCManager.hpp"
#include "swatch/system/System.hpp"
#include "swatch/processor/Processor.hpp"
#include "swatch/action/ActionableSystem.hpp"
#include "swatch/action/GateKeeper.hpp"

#include "jsoncpp/json/json.h"


namespace swatchcellframework
{

  CommandPanel::CommandPanel(tsframework::CellAbstractContext* context, log4cplus::Logger& logger)
  :
    CellPanel(context, logger),
    logger_(logger),
    cellContext_(dynamic_cast<swatchcellframework::CellContext&>(*context)),
    resultBox_(new ajax::ResultBox())
  {
    resultBox_->setIsOwned(false);

    // Record all of the object types present in the Processor/DaqTTCManager parts of the system
    CellContext::SharedGuard_t lGuard(cellContext_);
    swatch::system::System& lSystem = cellContext_.getSystem(lGuard);

    BOOST_FOREACH(swatch::dtm::DaqTTCManager* daqttcMgr, lSystem.getDaqTTCs())
    {
      objTypeActionableMap_[daqttcMgr->getTypeName()][daqttcMgr->getId()] = daqttcMgr->getId();
    }

    BOOST_FOREACH(swatch::processor::Processor* proc, lSystem.getProcessors())
    {
      objTypeActionableMap_[proc->getTypeName()][proc->getId()] = proc->getId();
    }

  }


  CommandPanel::~CommandPanel()
  {
    delete resultBox_;
  }

  void CommandPanel::layout(cgicc::Cgicc& cgi)
  {
    this->remove();

    setEvent("CommandPanel::getObjectTypeArray", ajax::Eventable::OnClick, this, &CommandPanel::getObjectTypeArray);
    setEvent("CommandPanel::getDeviceArray", ajax::Eventable::OnTime, this, &CommandPanel::getDeviceArray);
    setEvent("CommandPanel::getCommandArray", ajax::Eventable::OnClick, this, &CommandPanel::getCommandArray);
    setEvent("CommandPanel::getParameterArray", ajax::Eventable::OnClick, this, &CommandPanel::getParameterArray);
    setEvent("CommandPanel::runCommand", ajax::Eventable::OnClick, this, &CommandPanel::runCommand);
    setEvent("CommandPanel::getCommandStatus", ajax::Eventable::OnTime, this, &CommandPanel::getCommandStatus);
    setEvent("CommandPanel::isGateKeeperReady", ajax::Eventable::OnClick, this, &CommandPanel::isGateKeeperReady);

    ajax::PolymerElement* commandsPanel = new ajax::PolymerElement("commands-panel");
    resultBox_->add(commandsPanel);

    this->add(resultBox_);


  }

  void CommandPanel::getObjectTypeArray(cgicc::Cgicc& cgi,std::ostream& out){

    //std::cout << "> Replying with the Object Type array" << std::endl;

    int x = 0;
    out << "[ ";
    BOOST_FOREACH(ActionableMap::value_type& i, objTypeActionableMap_)
    {
      // We don't need the comma before the first one
      if (i.first != "undefined") {
        if (x > 0) out << " , ";
        out << "\"" << i.first << "\"";
        x++;
      }
    }

    out << " ]";
  }

  void CommandPanel::getDeviceArray(cgicc::Cgicc& cgi,std::ostream& out){

    //std::cout << "> Replying with the device array" << std::endl;

    std::map<std::string,std::string> values(ajax::toolbox::getSubmittedValues(cgi));
    std::string selectedObjType = values["objecttype"];


    if (selectedObjType == "undefined") {
      out << "[]";
      return;
    }


    int x = 0;
    out << "[ ";

    std::map<std::string, std::string>& actionableObjectMap = objTypeActionableMap_[selectedObjType];

    CellContext::SharedGuard_t lGuard(cellContext_);
    swatch::system::System& lSystem = cellContext_.getSystem(lGuard);
    swatch::system::System::ActionableChildMap_t childMap = lSystem.getActionableChildren();

    for (std::map<std::string, std::string>::iterator it=actionableObjectMap.begin(); it!=actionableObjectMap.end(); ++it)
    {
      // We don't need the comma before the first one
      if (x > 0) out << " , ";
      swatch::action::ActionableObject* actionableObject =  childMap.at(it -> first);
      boost::property_tree::ptree deviceTree;
      deviceTree.put("id", it -> first);
      swatch::action::ActionableSnapshot lSnapshot = actionableObject->getStatus();
      if (lSnapshot.getRunningActions().empty()) {
        deviceTree.put("status", "NotBusy");
      } else {
        deviceTree.put("status", "Busy");
        deviceTree.put("lockedby", lSnapshot.getRunningActions().back()->getId());
      }
      boost::property_tree::write_json(out, deviceTree);
      x++;
    }

    out << " ]";

  }

  void CommandPanel::getCommandArray(cgicc::Cgicc& cgi,std::ostream& out){

    //std::cout << "> Replying with the command array" << std::endl;

    std::map<std::string,std::string> values(ajax::toolbox::getSubmittedValues(cgi));
    std::string selectedObjType = values["objecttype"];

    if (selectedObjType == "undefined") {
      out << "[]";
      return;
    }

    int x = 0;
    out << "[ ";

    CellContext::SharedGuard_t lGuard(cellContext_);
    swatch::system::System& lSystem = cellContext_.getSystem(lGuard);

    const swatch::action::ActionableSystem::ActionableChildMap_t&  map = lSystem.getActionableChildren();
    std::string objID = objTypeActionableMap_[selectedObjType].begin()->first;
    const std::set<std::string>& cmds = map.at(objID)->getCommands();
    BOOST_FOREACH(const std::string& cmd, cmds)
    {
      if (x > 0) out << " , ";
      out << "\"" << cmd << "\"";
      x++;
    }

    out << " ]";
  }

  void CommandPanel::getParameterArray(cgicc::Cgicc& cgi,std::ostream& out){

    std::map<std::string,std::string> values(ajax::toolbox::getSubmittedValues(cgi));
    std::string selectedCmdName = values["command"];
    std::string selectedObjType = values["objecttype"];
    std::string selectedObjIds = values["devices"];
    std::stringstream selectedObjIdsSS;
    selectedObjIdsSS.str(values["devices"]);
    std::string selectedNamespace = values["namespace"];
    std::string loadFromGK = values["loadfromgk"];


    if ((selectedObjIds == "undefined")||(selectedObjIds == "")||(selectedObjIds == "[]")) {
      out << "[]";
      return;
    }

    Json::Value selectedObjIdsJson(Json::arrayValue);
    selectedObjIdsSS >> selectedObjIdsJson;

    //std::cout << "> Replying with the parameter array of " << selectedObjId << "::" << selectedCmdName << std::endl;

    if ((selectedCmdName == "undefined")||(selectedCmdName == "")) {
      out << "[]";
      return;
    }
    if ((selectedObjType == "undefined")||(selectedObjType == "")) {
      out << "[]";
      return;
    }

    CellContext::SharedGuard_t lGuard(cellContext_);
    swatch::system::System& lSystem = cellContext_.getSystem(lGuard);


    swatch::action::ActionableObject* actionableObject =  lSystem.getActionableChildren().at(selectedObjIdsJson[0].asString());


    swatch::action::Command& cmd = actionableObject->getCommand(selectedCmdName);

    const swatch::core::XParameterSet& defaultParams = cmd.getDefaultParams();


    out << "[ " ;

    int x = 0;

    BOOST_FOREACH(const std::string& key, defaultParams.keys())
    {
      if (x > 0) out << " , ";
      boost::property_tree::ptree parameterTree;
      parameterTree.put("name", key);
      parameterTree.put("type", defaultParams.get(key).type());
      if ((loadFromGK != "true"))
        parameterTree.put("value", defaultParams.get(key).toString());
      else {
        const swatch::action::GateKeeper* gk = this -> getGateKeeper(lGuard);
        swatch::action::GateKeeper::Parameter_t GKParameter = gk -> get(selectedNamespace, selectedCmdName, key, actionableObject -> getGateKeeperContexts());
        if (GKParameter != NULL)
          parameterTree.put("value", GKParameter -> toString());
        else
          parameterTree.put("value", defaultParams.get(key).toString());
      }
      boost::property_tree::write_json(out, parameterTree);
      x++;
    }

    out << " ]";

    return;
  }

  void CommandPanel::runCommand(cgicc::Cgicc& cgi,std::ostream& out){

    std::map<std::string,std::string> values(ajax::toolbox::getSubmittedValues(cgi));
    std::string run = values["run"];
    std::string selectedCmdName = values["command"];
    std::string selectedObjType = values["objecttype"];
    std::string selectedObjIds = values["devices"];
    std::stringstream selectedObjIdsSS;
    selectedObjIdsSS.str(selectedObjIds);

    if (run != "true") return;

    if ((selectedObjIds == "undefined")||(selectedObjIds == "")||(selectedObjIds == "[]")) {
      out << "{}";
      return;
    }

    Json::Value selectedObjIdsJson(Json::arrayValue);
    selectedObjIdsSS >> selectedObjIdsJson;

    //std::cout << "> Replying with the parameter array of " << selectedObjId << "::" << selectedCmdName << std::endl;

    if ((selectedCmdName == "undefined")||(selectedCmdName == ""))  {
      out << "{}";
      return;
    }
    if ((selectedObjType == "undefined")||(selectedObjType == "")) {
      out << "{}";
      return;
    }


    CellContext::SharedGuard_t lGuard(cellContext_);
    swatch::system::System& lSystem = cellContext_.getSystem(lGuard);

    for (unsigned int x = 0; x < selectedObjIdsJson.size(); x++) {

      const std::string& selectedObjId = selectedObjIdsJson[x].asString();
      swatch::action::Command& cmd = lSystem.getActionableChildren().at(selectedObjId)->getCommand(selectedCmdName);

      swatch::core::ReadWriteXParameterSet params;
      params.deepCopyFrom(cmd.getDefaultParams());
      BOOST_FOREACH(const std::string& key, params.keys())
      {
        params.get(key).fromString(ajax::toolbox::getSubmittedValue(cgi, key));
      }

      // 2) Execute the command
      LOG4CPLUS_INFO(this->getLogger(), "CommandPanel is now executing command '" << cmd.getPath() << "'");
      cmd.exec(params);
    }
    out << "{}";
    return;

  }


  void CommandPanel::getCommandStatus(cgicc::Cgicc& cgi,std::ostream& out){

    std::map<std::string,std::string> values(ajax::toolbox::getSubmittedValues(cgi));
    std::string selectedObjIds = values["devices"];
    std::string selectedCommand = values["command"];
    std::stringstream selectedObjIdsSS;
    selectedObjIdsSS.str(selectedObjIds);

    if ((selectedObjIds == "undefined")||(selectedObjIds == "")||(selectedObjIds == "[]")) {
      out << "[]";
      return;
    }

    Json::Value selectedObjIdsJson(Json::arrayValue);
    selectedObjIdsSS >> selectedObjIdsJson;

    if ((selectedCommand == "undefined")||(selectedCommand == "")) {
      out << "[]";
      return;
    }

    CellContext::SharedGuard_t lGuard(cellContext_);
    swatch::system::System& lSystem = cellContext_.getSystem(lGuard);

    Json::Value jsonOutput (Json::arrayValue);

    for (unsigned int x = 0; x < selectedObjIdsJson.size(); x++) {

      const std::string& selectedObjId = selectedObjIdsJson[x].asString();
      swatch::action::ActionableObject *actionableObject = lSystem.getActionableChildren().at(selectedObjId);

      swatch::action::Command& cmd =  actionableObject->getCommand(selectedCommand);

      //FIXME: Update to better way of retrieving time of day as string
      timeval now_tv;
      gettimeofday(&now_tv, NULL);
      time_t now = now_tv.tv_sec;
      char timeText[21];
      strftime(timeText, sizeof timeText, "%H:%M:%S", gmtime(&now));



      //timeText = current time

      swatch::action::CommandSnapshot lStatus = cmd.getStatus();

      Json::Value cmdStatus(Json::objectValue);

      cmdStatus["device"] = selectedObjId;

      std::ostringstream lOss;

      lOss << lStatus.getState();

      cmdStatus["status"] = lOss.str();

      if ( (lStatus.getState() != swatch::action::Functionoid::kInitial) && (lStatus.getState() != swatch::action::Functionoid::kScheduled) ) {


        cmdStatus["time"] = timeText;

        std::ostringstream oss;

        oss << std::setprecision(3) << lStatus.getRunningTime() << " s";
        cmdStatus["runningtime"] = oss.str();

        oss.str("");

        oss << std::setprecision(5) << (lStatus.getProgress() * 100.0);
        cmdStatus["progress"] = oss.str();

        cmdStatus["message"] = lStatus.getStatusMsg();
        
        const swatch::core::XParameterSet& lXParameterSet = lStatus.getExecutionDetails();
        
        const std::set< std::string >& lParameterKeys = lXParameterSet.keys();
        Json::Value lExecutionDetailsJson(Json::arrayValue);
        
        for(auto lParameterKeysIterator = lParameterKeys.begin(); lParameterKeysIterator != lParameterKeys.end(); lParameterKeysIterator++){
          Json::Value lParameterJson(Json::objectValue);
          lParameterJson["key"] = *lParameterKeysIterator;
          lParameterJson["value"] = lXParameterSet.get(*lParameterKeysIterator).toString();
          lExecutionDetailsJson.append(lParameterJson);
        }
        
        cmdStatus["detailedInfo"] = lExecutionDetailsJson;

        if ( lStatus.getState() != swatch::action::Functionoid::kRunning )
        {
          cmdStatus["result"] = (lStatus.getResult() != NULL) ? lStatus.getResult()->toString() : "";
          cmdStatus["running"] = "false";
        }
        else
        {
          cmdStatus["result"] = "";
          cmdStatus["running"] = "true";
        }
        jsonOutput.append(cmdStatus);
      }
    }


    out << jsonOutput;
  }

  void CommandPanel::isGateKeeperReady(cgicc::Cgicc& cgi,std::ostream& out)
  {
    CellContext::SharedGuard_t lGuard(cellContext_);
    out << ((bool) getGateKeeper(lGuard));
  }


} // end ms swatchframework

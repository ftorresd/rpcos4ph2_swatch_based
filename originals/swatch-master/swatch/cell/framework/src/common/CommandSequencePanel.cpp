#include "swatchcell/framework/CommandSequencePanel.h"

#include "boost/foreach.hpp"
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "ajax/PolymerElement.h"
#include "ajax/toolbox.h"

#include "ts/toolbox/Tools.h"

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"

#include "xdata/Serializable.h"

#include "swatch/action/Command.hpp"
#include "swatch/action/CommandSequence.hpp"
#include "swatch/core/ReadWriteXParameterSet.hpp"
#include "swatch/system/Crate.hpp"
#include "swatch/dtm/DaqTTCManager.hpp"
#include "swatch/system/System.hpp"
#include "swatch/processor/Processor.hpp"
#include <sstream>
#include <iostream>


namespace swatchcellframework
{

  CommandSequencePanel::CommandSequencePanel(tsframework::CellAbstractContext* context, log4cplus::Logger& logger)
  :
    CellPanel(context, logger),
    logger_(logger),
    cellContext_(dynamic_cast<swatchcellframework::CellContext&>(*context)),
    resultBox_(new ajax::ResultBox())
  {
    resultBox_->setIsOwned(false);

    CellContext::SharedGuard_t lGuard(cellContext_);
    swatch::system::System& lSystem = cellContext_.getSystem(lGuard);

    BOOST_FOREACH(swatch::dtm::DaqTTCManager* daqttcMgr, lSystem.getDaqTTCs())
    {
      objTypeActionableMap_[daqttcMgr->getTypeName()][daqttcMgr->getId()] = daqttcMgr->getId();
      std::set<std::string> seq = daqttcMgr->getSequences();
      objTypeSequenceMap_[daqttcMgr->getTypeName()].insert(seq.begin(), seq.end());
    }

    BOOST_FOREACH(swatch::processor::Processor* proc, lSystem.getProcessors())
    {
      objTypeActionableMap_[proc->getTypeName()][proc->getId()] = proc->getId();
      std::set<std::string> seq = proc->getSequences();
      objTypeSequenceMap_[proc->getTypeName()].insert(seq.begin(), seq.end());
    }


  }


  CommandSequencePanel::~CommandSequencePanel()
  {
    delete resultBox_;
  }

  void CommandSequencePanel::layout(cgicc::Cgicc& cgi)
  {
    this->remove();

    ajax::PolymerElement* CommandSequencePanel = new ajax::PolymerElement("command-sequences-panel");
    resultBox_->add(CommandSequencePanel);

    this->add(resultBox_);

    setEvent("CommandSequencePanel::getObjectTypeArray", ajax::Eventable::OnTime, this, &CommandSequencePanel::getObjectTypeArray);
    setEvent("CommandSequencePanel::getDeviceArray", ajax::Eventable::OnTime, this, &CommandSequencePanel::getDeviceArray);
    setEvent("CommandSequencePanel::getCommandSequenceArray", ajax::Eventable::OnTime, this, &CommandSequencePanel::getCommandSequenceArray);
    setEvent("CommandSequencePanel::getCommandsList", ajax::Eventable::OnTime, this, &CommandSequencePanel::getCommandsList);
    setEvent("CommandSequencePanel::runCommandSequence", ajax::Eventable::OnTime, this, &CommandSequencePanel::runCommandSequence);
    setEvent("CommandSequencePanel::getCommandSequenceStatus", ajax::Eventable::OnTime, this, &CommandSequencePanel::getCommandSequenceStatus);
    setEvent("CommandSequencePanel::isGateKeeperReady", ajax::Eventable::OnTime, this, &CommandSequencePanel::isGateKeeperReady);
  }

  void CommandSequencePanel::getObjectTypeArray(cgicc::Cgicc& cgi,std::ostream& out){

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

  void CommandSequencePanel::getDeviceArray(cgicc::Cgicc& cgi,std::ostream& out){

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
        deviceTree.put("lockedby", lSnapshot.getRunningActions().front()->getId());
      }
      boost::property_tree::write_json(out, deviceTree);
      x++;
    }

    out << " ]";

  }

  void CommandSequencePanel::getCommandSequenceArray(cgicc::Cgicc& cgi,std::ostream& out){

    //std::cout << "> Replying with the command array" << std::endl;

    std::map<std::string,std::string> values(ajax::toolbox::getSubmittedValues(cgi));
    std::string selectedObjType = values["objecttype"];

    if (selectedObjType == "undefined") {
      out << "[]";
      return;
    }

    const std::set<std::string>& cmdSeqs = objTypeSequenceMap_.at(selectedObjType);

    if (cmdSeqs.empty()) {
      out << "[]";
    } else {

      int x = 0;
      out << "[ ";

      BOOST_FOREACH(const std::string& cmdSeq, cmdSeqs)
      {
        if (x > 0) out << " , ";
        out << "\"" << cmdSeq << "\"";
        x++;
      }

      out << " ]";

    }

  }

  void CommandSequencePanel::getCommandsList(cgicc::Cgicc& cgi,std::ostream& out){

    std::map<std::string,std::string> values(ajax::toolbox::getSubmittedValues(cgi));
    std::string selectedObjType = values["objecttype"];
    std::string selectedCmdSeqName = values["commandsequence"];

    CellContext::SharedGuard_t lGuard(cellContext_);
    swatch::system::System& lSystem = cellContext_.getSystem(lGuard);
    swatch::system::System::ActionableChildMap_t childMap = lSystem.getActionableChildren();

    if (selectedObjType == "undefined") {
      out << "[]";
      return;
    }

    if (selectedCmdSeqName == "undefined") {
      out << "[]";
      return;
    }

    if (!selectedObjType.empty() and !selectedCmdSeqName.empty()) {

      // Get the first actionable of the selected type
      swatch::action::ActionableObject* lActionable = childMap.at(objTypeActionableMap_[selectedObjType].begin()->first);

      swatch::action::CommandSequence& lCmdSequence = lActionable->getSequence(selectedCmdSeqName);


      int x = 0;
      out << "[ ";

      for (swatch::action::CommandVec::const_iterator it = lCmdSequence.begin(); it != lCmdSequence.end(); it++) {
        if (x > 0) out << " , ";
        boost::property_tree::ptree commandTree;
        commandTree.put("name", it->get().getId());
        if (!it->getNamespace().empty()) commandTree.put("namespace", it->getNamespace());
        boost::property_tree::write_json(out, commandTree);
        x++;
      }

      out << " ]";

    } else {
      //addPlainHtml(*sequenceControlBox_) << "None";
      out << "[]";
    }
    return;
  }

  void CommandSequencePanel::isGateKeeperReady(cgicc::Cgicc& cgi,std::ostream& out)
  {
    CellContext::SharedGuard_t lGuard(cellContext_);
    out << ((bool) getGateKeeper(lGuard));
  }

  void CommandSequencePanel::runCommandSequence(cgicc::Cgicc& cgi,std::ostream& out){

    std::map<std::string,std::string> values(ajax::toolbox::getSubmittedValues(cgi));

    std::string run = values["run"];
    if (run != "true")
    {
      out << "{\"status\": \"NotRunning\"}";
      return;
    }
    std::string selectedCmdSequenceName = values["commandsequence"];
    std::string selectedObjType = values["objecttype"];
    std::stringstream selectedObjIDsJSON;
    selectedObjIDsJSON << values["devices"];
    boost::property_tree::ptree selectedObjIDs;
    boost::property_tree::read_json(selectedObjIDsJSON, selectedObjIDs);

    typedef boost::property_tree::ptree::iterator treeIterator;




    /*
    for(treeIterator it = selectedObjIDs.begin(); it != selectedObjIDs.end(); it++){
      std::cout << it->second.data() << std::endl;
    }
    */

    if ((selectedCmdSequenceName == "undefined")||(selectedCmdSequenceName == "")) {
      out << "{}";
      return;
    }

    if ((selectedObjType == "undefined")||(selectedObjType == "")){
      out << "{}";
      return;
    }


    //Checking if something is busy
    CellContext::SharedGuard_t lGuard(cellContext_);
    swatch::system::System& lSystem = cellContext_.getSystem(lGuard);
    swatch::system::System::ActionableChildMap_t childMap = lSystem.getActionableChildren();

    for(treeIterator it = selectedObjIDs.begin(); it != selectedObjIDs.end(); it++){

      std::string objID = it -> second.data();
      swatch::action::ActionableObject* lActionable = childMap.at(it -> second.data());

      if (!(lActionable->getStatus().getRunningActions().empty())) {
        out << "{\"status\": \"Busy\"}";
        return;
      }
    }

    //Fetching the command sequence from all the selected devices

    typedef std::map<std::string, swatch::action::CommandSequence*> CommandSequenceMap;
    CommandSequenceMap lCommandSequences;
    for(treeIterator it = selectedObjIDs.begin(); it != selectedObjIDs.end(); it++){
      std::string id = it->second.data();
      swatch::action::ActionableObject* lActionable = childMap.at(id);
      swatch::action::CommandSequence& cs = lActionable->getSequence(selectedCmdSequenceName);
      lCommandSequences[id] = &cs;
    }

    //Checking if all the parameters are available

    typedef std::vector<swatch::action::CommandVec::MissingParam> tMissingParamVec;
    std::map<std::string, tMissingParamVec > lMissingMap;

    BOOST_FOREACH(CommandSequenceMap::value_type p, lCommandSequences)
    {
      std::vector<swatch::core::ReadOnlyXParameterSet> lParamSets;
      tMissingParamVec lMissingParams;

      p.second->checkForMissingParameters(*getGateKeeper(lGuard), lParamSets, lMissingParams);

      if (!lMissingParams.empty()) {
        // Save the info somewhere
        lMissingMap[p.first] = lMissingParams;
      }
    }

    //If some parameter is missing I return the information about it
    //The information tree (missingTree) is structured in this way:
    //
    //  * running status -> MissingParameter (error status)
    //  * devices -> Array of ptrees, every tree is a device with its collection of missing parameters
    //  * devices.missingparameters -> another array of ptree, each one containing info about the namespace, the command and the ID of the missing parameter


    if (!lMissingMap.empty()) {
      boost::property_tree::ptree missingTree;
      missingTree.put("status", "MissingParameter");
      std::map<std::string, tMissingParamVec>::const_iterator iMM;
      tMissingParamVec::const_iterator iMPar;
      boost::property_tree::ptree missingDevicesArrayTree;
      for (iMM = lMissingMap.begin(); iMM != lMissingMap.end(); ++iMM) {
        boost::property_tree::ptree missingDeviceTree;
        missingDeviceTree.put("id", iMM -> first);
        boost::property_tree::ptree missingParametersArrayTree;
        for (iMPar = iMM->second.begin(); iMPar != iMM->second.end(); ++iMPar) {
          boost::property_tree::ptree missingParameterTree;
          missingParameterTree.put("namespace", iMPar->nspace);
          missingParameterTree.put("command", iMPar->command);
          missingParameterTree.put("parameter", iMPar->parameter);
          missingParametersArrayTree.push_back(std::make_pair("", missingParameterTree));
        }
        missingDeviceTree.add_child("missingparameters", missingParametersArrayTree);
        missingDevicesArrayTree.push_back(std::make_pair("", missingDeviceTree));
      }
      missingTree.add_child("devices", missingDevicesArrayTree);

      boost::property_tree::write_json(out, missingTree);

      return;
    }


    LOG4CPLUS_INFO(this->getLogger(), "> CommandSequencePanel is now executing a command sequence");


    BOOST_FOREACH(CommandSequenceMap::value_type p, lCommandSequences)
    {
      p.second->exec(*getGateKeeper(lGuard), true);
    }

    out << "{\"status\": \"Running\"}";
    return;
  }


  // The method returns a JSON with the statuses of the selected devices that have run the selected command at least once.
  void CommandSequencePanel::getCommandSequenceStatus(cgicc::Cgicc& cgi,std::ostream& out){

    std::map<std::string,std::string> values(ajax::toolbox::getSubmittedValues(cgi));
    std::string selectedCmdSequenceName = values["commandsequence"];
    std::string selectedObjType = values["objecttype"];

    std::stringstream selectedObjIDsJSON;
    selectedObjIDsJSON << values["devices"];

     if ((selectedCmdSequenceName == "undefined")||(selectedCmdSequenceName == "")){
       out << "[]";
      return;
    }

    if ((selectedObjType == "undefined")||(selectedObjType == "")){
      out << "[]";
      return;
    }

    if ((selectedObjIDsJSON.str() == "undefined")||(selectedObjIDsJSON.str() == "")){
      out << "[]";
      return;
    }


    boost::property_tree::ptree selectedObjIDs;
    boost::property_tree::read_json(selectedObjIDsJSON, selectedObjIDs);

    typedef boost::property_tree::ptree::iterator treeIterator;

    //std::map<std::string, std::string>& actionableObjectMap = objTypeActionableMap_[selectedObjType];

    CellContext::SharedGuard_t lGuard(cellContext_);
    swatch::system::System& lSystem = cellContext_.getSystem(lGuard);
    swatch::system::System::ActionableChildMap_t childMap = lSystem.getActionableChildren();

    //Retrieving the command sequence map
    typedef std::map<std::string, swatch::action::CommandSequence*> CommandSequenceMap;
    CommandSequenceMap lCommandSequences;

    for(treeIterator it = selectedObjIDs.begin(); it != selectedObjIDs.end(); it++){
      std::string id = it->second.data();
      swatch::action::ActionableObject* lActionable = childMap.at(id);
      swatch::action::CommandSequence& cs = lActionable->getSequence(selectedCmdSequenceName);
      lCommandSequences[id] = &cs;
    }

    //FIXME: Update to better way of retrieving time of day as string
    timeval now_tv;
    gettimeofday(&now_tv, NULL);
    time_t now = now_tv.tv_sec;
    char timeText[21];
    strftime(timeText, sizeof timeText, "%H:%M:%S", gmtime(&now));

    int x = 0;

    std::map<std::string, swatch::action::CommandSequence*>::iterator it;

    out << "[";

    for (it = lCommandSequences.begin(); it != lCommandSequences.end(); ++it) {

      swatch::action::CommandSequenceSnapshot status = it->second->getStatus();

      if ((status.getState() != swatch::action::Functionoid::kInitial) && (status.getState() != swatch::action::Functionoid::kScheduled)) {



        if (x > 0) out << " , ";

        ++x;

        boost::property_tree::ptree cmdSeqStatus;

        cmdSeqStatus.put("device", it->first);
        cmdSeqStatus.put("status", status.getState());

        cmdSeqStatus.put("time", timeText);

        boost::property_tree::ptree cmdArray;

        std::ostringstream oss;

        oss << std::setprecision(3) << status.getRunningTime() << " s";
        cmdSeqStatus.put("runningtime", oss.str());
        oss.str("");

        oss << std::setprecision(5) << (status.getProgress() * 100.0) << " % ";
        cmdSeqStatus.put("progress", oss.str());
        oss.str("");

        oss << status.getNumberOfCompletedCommands();
        cmdSeqStatus.put("completedsteps", oss.str());
        oss.str("");

        oss << status.getTotalNumberOfCommands();
        cmdSeqStatus.put("ncommands", oss.str());
        oss.str("");

        if ( status.getState() != swatch::action::Functionoid::kRunning )
        {
          cmdSeqStatus.put("running", "false");
        }
        else
        {
          cmdSeqStatus.put("running", "true");
        }

        for(swatch::action::CommandSequenceSnapshot::const_iterator it = status.begin(); it != status.end(); it++){

          boost::property_tree::ptree cmdStatus;

          const swatch::action::CommandSnapshot& lStatus = *it;

          //Last part of the path is the command name
          std::vector<std::string> splittedPath = tstoolbox::Tools::split(lStatus.getActionPath(), ".");
          oss << splittedPath.back();
          cmdStatus.put("id", oss.str());
          oss.str("");

          cmdStatus.put("status", lStatus.getState());

          oss << std::setprecision(3) << lStatus.getRunningTime() << " s";
          cmdStatus.put("runningtime",oss.str());
          oss.str("");

          oss << std::setprecision(5) << (lStatus.getProgress() * 100.0);
          cmdStatus.put("progress", oss.str());
          oss.str("");

          cmdStatus.put("message", lStatus.getStatusMsg());

          if ( lStatus.getState() != swatch::action::Functionoid::kRunning )
          {
            cmdStatus.put("result", (lStatus.getResult() != NULL) ? lStatus.getResult()->toString() : "");
            cmdStatus.put("running", "false");
          }
          else
          {
            cmdStatus.put("result", "");
            cmdStatus.put("running", "true");
          }

          cmdArray.push_back(std::make_pair("", cmdStatus));
        }
        cmdSeqStatus.add_child("commands", cmdArray);
        boost::property_tree::write_json(out, cmdSeqStatus);
      }

    }

    out << "]";
  }

} // end ms swatchframework

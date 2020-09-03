#include "swatchcell/framework/StateMachinePanel.h"


#include "boost/foreach.hpp"

#include "ajax/PolymerElement.h"
#include "ajax/toolbox.h"

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"

#include "swatch/action/StateMachine.hpp"
#include "swatch/dtm/DaqTTCManager.hpp"
#include "swatch/system/System.hpp"
#include "swatch/processor/Processor.hpp"

#include "swatchcell/framework/tools/panelFunctions.h"
#include "swatchcell/framework/tools/polymerFunctions.h"

#include "jsoncpp/json/json.h"


namespace swatchcellframework
{

  StateMachinePanel::StateMachinePanel(tsframework::CellAbstractContext* context, log4cplus::Logger& logger) :
    CellPanel(context, logger),
    logger_(logger),
    cellContext_(dynamic_cast<swatchcellframework::CellContext&>(*context))
  {
  }


  StateMachinePanel::~StateMachinePanel()
  {
  }

  /*
  ██       █████  ██    ██  ██████  ██    ██ ████████
  ██      ██   ██  ██  ██  ██    ██ ██    ██    ██
  ██      ███████   ████   ██    ██ ██    ██    ██
  ██      ██   ██    ██    ██    ██ ██    ██    ██
  ███████ ██   ██    ██     ██████   ██████     ██
  */

  void StateMachinePanel::layout(cgicc::Cgicc& cgi)
  {
    remove();

    ajax::PolymerElement* stateMachinePanel = new ajax::PolymerElement("state-machine-panel");


    this -> setEvent("StateMachinePanel::getDeviceArray", ajax::Eventable::OnTime, this, &StateMachinePanel::getDeviceArray);
    this -> setEvent("StateMachinePanel::getStateMachineArray", ajax::Eventable::OnClick, this, &StateMachinePanel::getStateMachineArray);
    this -> setEvent("StateMachinePanel::getStateMachineInfo", ajax::Eventable::OnClick, this, &StateMachinePanel::getStateMachineInfo);
    this -> setEvent("StateMachinePanel::actStateMachine", ajax::Eventable::OnClick, this, &StateMachinePanel::actStateMachine);
    this -> setEvent("StateMachinePanel::getTransitionStatus", ajax::Eventable::OnTime, this, &StateMachinePanel::getTransitionStatus);


    this -> add(stateMachinePanel);

    LOG4CPLUS_DEBUG(getLogger(), "StateMachinePanel<" << this << "> : layout method called");


  }

  /*
  ██████  ███████ ██    ██ ██  ██████ ███████      █████  ██████  ██████   █████  ██    ██
  ██   ██ ██      ██    ██ ██ ██      ██          ██   ██ ██   ██ ██   ██ ██   ██  ██  ██
  ██   ██ █████   ██    ██ ██ ██      █████       ███████ ██████  ██████  ███████   ████
  ██   ██ ██       ██  ██  ██ ██      ██          ██   ██ ██   ██ ██   ██ ██   ██    ██
  ██████  ███████   ████   ██  ██████ ███████     ██   ██ ██   ██ ██   ██ ██   ██    ██
  */

  void StateMachinePanel::getDeviceArray(cgicc::Cgicc& cgi, std::ostream& out) {

    CellContext::SharedGuard_t lGuard(cellContext_);
    swatch::system::System& lSystem = cellContext_.getSystem(lGuard);

    Json::Value lDeviceArrayJson(Json::arrayValue);

    BOOST_FOREACH(swatch::processor::Processor* lProc, lSystem.getProcessors())
    {
      Json::Value lDeviceJson(Json::objectValue);
      lDeviceJson["id"] = lProc -> getId();
      serializeStateMachineStatus(lDeviceJson, lProc -> getStatus());
      lDeviceArrayJson.append(lDeviceJson);
    }

    BOOST_FOREACH(swatch::dtm::DaqTTCManager* lDaqttcMgr, lSystem.getDaqTTCs())
    {
      Json::Value lDeviceJson(Json::objectValue);
      lDeviceJson["id"] = lDaqttcMgr -> getId();
      serializeStateMachineStatus(lDeviceJson, lDaqttcMgr -> getStatus());
      lDeviceArrayJson.append(lDeviceJson);
    }

    out << lDeviceArrayJson;

    return;
  }

  /*
  ███████ ███    ███      █████  ██████  ██████   █████  ██    ██
  ██      ████  ████     ██   ██ ██   ██ ██   ██ ██   ██  ██  ██
  ███████ ██ ████ ██     ███████ ██████  ██████  ███████   ████
       ██ ██  ██  ██     ██   ██ ██   ██ ██   ██ ██   ██    ██
  ███████ ██      ██     ██   ██ ██   ██ ██   ██ ██   ██    ██
  */

  void StateMachinePanel::getStateMachineArray(cgicc::Cgicc& cgi, std::ostream& out) {

    CellContext::SharedGuard_t lGuard(cellContext_);
    swatch::system::System& lSystem = cellContext_.getSystem(lGuard);

    std::map<std::string,std::string> lValues(ajax::toolbox::getSubmittedValues(cgi));
    std::string lSelectedDevice = lValues["selecteddevice"];

    if(swatch::action::ActionableObject* lObj = lSystem.getObjPtr<swatch::action::ActionableObject>(lSelectedDevice))
    {
      Json::Value lStateMachineArrayJson(Json::arrayValue);
      if( !lObj->getStateMachines().empty() )
      {

        const std::set<std::string> lMachines = lObj->getStateMachines();
        for(std::set<std::string>::const_iterator it = lMachines.begin(); it != lMachines.end(); it++)
          lStateMachineArrayJson.append(*it);
      }
      out << lStateMachineArrayJson;
    }

    return;
  }

  /*
  ███████ ███    ███     ██ ███    ██ ███████  ██████
  ██      ████  ████     ██ ████   ██ ██      ██    ██
  ███████ ██ ████ ██     ██ ██ ██  ██ █████   ██    ██
       ██ ██  ██  ██     ██ ██  ██ ██ ██      ██    ██
  ███████ ██      ██     ██ ██   ████ ██       ██████
  */

  void StateMachinePanel::getStateMachineInfo(cgicc::Cgicc& cgi, std::ostream& out) {

    CellContext::SharedGuard_t lGuard(cellContext_);
    swatch::system::System& lSystem = cellContext_.getSystem(lGuard);

    std::map<std::string,std::string> lValues(ajax::toolbox::getSubmittedValues(cgi));
    std::string lSelectedDevice = lValues["selecteddevice"];
    std::string lSelectedStateMachine = lValues["selectedstatemachine"];

    Json::Value lStateMachineInfoJson(Json::objectValue);

    lStateMachineInfoJson["stateMachineName"] = lSelectedStateMachine;

    if (swatch::action::ActionableObject* lDevice = lSystem.getObjPtr<swatch::action::ActionableObject>(lSelectedDevice))
    {
      swatch::action::StateMachine& lStateMachine = lDevice -> getStateMachine(lSelectedStateMachine);

      swatch::action::ActionableObject::Status_t lState = lDevice -> getStatus();

      std::string lFsmCurrentState = lState.getState();

//      if (fsmState_ != lState.getState())
//        selectedTransition_ = "";

      typedef std::map<std::string, swatch::action::Transition*> tTransitionMap;

      Json::Value lFsmStructureJson(Json::arrayValue);

      for(std::vector<std::string>::const_iterator it = lStateMachine.getStates().begin(); it != lStateMachine.getStates().end(); it++)
      {

        Json::Value lFsmStateJson(Json::objectValue);

        if(*it == lStateMachine.getInitialState())
          lFsmStateJson["type"] = "Make";
        else if(*it == lStateMachine.getErrorState())
          lFsmStateJson["type"] = "Error";
        else
          lFsmStateJson["type"] = "Normal";

        lFsmStateJson["id"] = *it;

        Json::Value lStateTransitionsJson(Json::arrayValue);
        std::ostringstream transitionOss;
        for(tTransitionMap::const_iterator it2 = lStateMachine.getTransitions(*it).begin(); it2 != lStateMachine.getTransitions(*it).end(); it2++)
        {
          swatch::action::Transition& lTransition = *(it2 -> second);
          Json::Value lTransitionJson(Json::objectValue);
          lTransitionJson["id"] = it2->first;
          lTransitionJson["endState"] = lTransition.getEndState();
          Json::Value lCommandsJson(Json::arrayValue);
          for(swatch::action::CommandVec::const_iterator it3 = lTransition.begin(); it3 != lTransition.end(); it3++)
          {
            Json::Value lCommandJson(Json::objectValue);
            lCommandJson["id"] = it3 -> get().getId();
            lCommandJson["namespace"] = it3 -> getNamespace();
            lCommandsJson.append(lCommandJson);
          }
          lTransitionJson["commands"] = lCommandsJson;
          lStateTransitionsJson.append(lTransitionJson);
        }
        lFsmStateJson["transitions"] = lStateTransitionsJson;

        lFsmStructureJson.append(lFsmStateJson);
      }

      lStateMachineInfoJson["structure"] = lFsmStructureJson;


      if(lState.getStateMachineId() != lSelectedStateMachine)
      {
        lStateMachineInfoJson["state"] = "Not engaged";
        const swatch::action::GateKeeper* lGK = getGateKeeper(lGuard);
        lStateMachineInfoJson["gkStatus"] = (lGK ? "Available" : "Not available");
      }
      else // This FSM is engaged
      {
        lStateMachineInfoJson["state"] = lFsmCurrentState;
      }
    }

    out << lStateMachineInfoJson;

  }

  /*
   █████   ██████ ████████ ██  ██████  ███    ██
  ██   ██ ██         ██    ██ ██    ██ ████   ██
  ███████ ██         ██    ██ ██    ██ ██ ██  ██
  ██   ██ ██         ██    ██ ██    ██ ██  ██ ██
  ██   ██  ██████    ██    ██  ██████  ██   ████
  */


  void StateMachinePanel::actStateMachine(cgicc::Cgicc& cgi,std::ostream& out){

    CellContext::SharedGuard_t lGuard(cellContext_);
    swatch::system::System& lSystem = cellContext_.getSystem(lGuard);

    std::map<std::string,std::string> lValues(ajax::toolbox::getSubmittedValues(cgi));
    std::string lSelectedDevice = lValues["selecteddevice"];
    std::string lSelectedStateMachine = lValues["selectedstatemachine"];
    std::string lAction = lValues["action"];
    std::string lIsTransition = lValues["istransition"];

    swatch::action::ActionableObject& lObject = lSystem.getObj<swatch::action::ActionableObject>(lSelectedDevice);

    Json::Value lJsonOutput(Json::objectValue);

    const swatch::action::GateKeeper* lGk = this -> getGateKeeper(lGuard);

    if ( lGk == NULL ){
      lJsonOutput["status"] = "Failed";
      lJsonOutput["detail"] = "Error while engaging state machine: no GateKeeper found!";
    }
    else
    {
      try {

        if (lIsTransition == "false") {

          if(lAction == "engage"){
            LOG4CPLUS_INFO(this->getLogger(), "> StateMachinePanel is now engaging state machine " + lSelectedStateMachine);
            lObject.getStateMachine(lSelectedStateMachine).engage(*lGk);
          }

          if(lAction == "reset"){
            LOG4CPLUS_INFO(this->getLogger(), "> StateMachinePanel is now resetting state machine " + lSelectedStateMachine);
            lObject.getStateMachine(lSelectedStateMachine).reset(*lGk);
          }

          if(lAction == "disengage"){
            LOG4CPLUS_INFO(this->getLogger(), "> StateMachinePanel is now disengaging state machine " + lSelectedStateMachine);
            lObject.getStateMachine(lSelectedStateMachine).disengage();
          }

          lJsonOutput["status"] = "Success";
          lJsonOutput["detail"] = "";

        } else {


          const std::string& lCurrentState = lValues["currentstate"];


          swatch::action::StateMachine& lStateMachine = lObject.getStateMachine(lSelectedStateMachine);
          swatch::action::Transition& lTransition = *lStateMachine.getTransitions(lCurrentState).find(lAction)->second;
          //Checking for missing parameters

          typedef std::vector<swatch::action::CommandVec::MissingParam> tMissingParamVec;
          tMissingParamVec lMissingParams;
          std::vector<swatch::core::ReadOnlyXParameterSet> lParamSets;
          lTransition.checkForMissingParameters(*lGk, lParamSets, lMissingParams);



          if(lMissingParams.empty())
          {
            LOG4CPLUS_INFO(this->getLogger(), "StateMachinePanel is now executing transition '" << lTransition.getId() << "' from state '" << lTransition.getStartState() << "' on object '" << lTransition.getActionable() << "'");
            lTransition.exec( *lGk );
            //We are already inside a try statement, if an exception is thrown we will already handle it.
            lJsonOutput["status"] = "Success";
            lJsonOutput["detail"] = "";
          }
          else
          {

            //Missing param!

            lJsonOutput["status"] = "MissingParameters";

            Json::Value lMissingParametersJsonArray(Json::arrayValue);

            for(tMissingParamVec::const_iterator lIt=lMissingParams.begin(); lIt != lMissingParams.end(); lIt++)
            {
              Json::Value lMissingParameterJson(Json::objectValue);
              lMissingParameterJson["namespace"] = lIt -> nspace;
              lMissingParameterJson["command"] = lIt -> command;
              lMissingParameterJson["parameter"] = lIt -> parameter;
              lMissingParametersJsonArray.append(lMissingParameterJson);
            }
            lJsonOutput["detail"] = lMissingParametersJsonArray;
          }

        }

      }
      catch (const std::exception& lException)
      {
        lJsonOutput["status"] = "Failed";
        std::ostringstream lOss;
        lOss << "Exception thrown while running action \"" << lAction << "\" on state machine: " << lException.what();
        lJsonOutput["detail"] = lOss.str();
      }
    }

    out << lJsonOutput;

    return;
  }

  /*
  ████████ ██████   █████  ███    ██ ███████     ███████ ████████  █████  ████████ ██    ██ ███████
     ██    ██   ██ ██   ██ ████   ██ ██          ██         ██    ██   ██    ██    ██    ██ ██
     ██    ██████  ███████ ██ ██  ██ ███████     ███████    ██    ███████    ██    ██    ██ ███████
     ██    ██   ██ ██   ██ ██  ██ ██      ██          ██    ██    ██   ██    ██    ██    ██      ██
     ██    ██   ██ ██   ██ ██   ████ ███████     ███████    ██    ██   ██    ██     ██████  ███████
  */

  void StateMachinePanel::getTransitionStatus(cgicc::Cgicc& cgi,std::ostream& out){

    std::map<std::string,std::string> lValues(ajax::toolbox::getSubmittedValues(cgi));
    std::string lSelectedDevice = lValues["selecteddevice"];
    std::string lSelectedStateMachine = lValues["selectedstatemachine"];
    std::string lSelectedTransition = lValues["transition"];
    std::string lCurrentState = lValues["currentstate"];

    Json::Value lJsonOutput(Json::objectValue);

    if ((lSelectedDevice == "") || (lSelectedDevice == "undefined")){
      lJsonOutput["notFound"] = true;
      out << lJsonOutput;
      return;
    }

    if ((lSelectedStateMachine == "") || (lSelectedStateMachine == "undefined")){
      lJsonOutput["notFound"] = true;
      out << lJsonOutput;
      return;
    }

    CellContext::SharedGuard_t lGuard(cellContext_);
    swatch::system::System& lSystem = cellContext_.getSystem(lGuard);

    swatch::action::ActionableObject& lObject = lSystem.getObj<swatch::action::ActionableObject>(lSelectedDevice);
    swatch::action::StateMachine& lStateMachine = lObject.getStateMachine(lSelectedStateMachine);

    //Well.. this is messy:
    //I want to get the curremt transition state, in order to do that I have to get the current FSM state
    //Then I need to fetch the TransistionSnapshot and get the state from it

    //ActionableObject status
    swatch::action::ActionableSnapshot lActionableSnapshot = lObject.getStatus();
    //Getting the transition
    swatch::action::Transition& lTransition = *lStateMachine.getTransitions(lCurrentState).find(lSelectedTransition)->second;
    //Transition snapshot
    swatch::action::TransitionSnapshot lTransitionSnapshot = lTransition.getStatus();
    //Getting the transition status
    swatch::action::ActionSnapshot::State lTransitionState = lTransitionSnapshot.getState();

    //If the transition is running or is done I return the status
    if ((lTransitionState != swatch::action::Functionoid::kInitial) && (lTransitionState != swatch::action::Functionoid::kScheduled)) {

      //FIXME: Update to better way of retrieving time of day as string
      timeval lNow_tv;
      gettimeofday(&lNow_tv, NULL);
      time_t lNow = lNow_tv.tv_sec;
      char lTimeText[21];
      strftime(lTimeText, sizeof lTimeText, "%H:%M:%S", gmtime(&lNow));

      lJsonOutput["time"] = lTimeText;
      lJsonOutput["notFound"] = false;
      lJsonOutput["status"] = boost::lexical_cast<std::string>(lTransitionState);
      std::ostringstream lTimeOss;
      lTimeOss << std::setprecision(3) << lTransitionSnapshot.getRunningTime() << " seconds";
      lJsonOutput["runningTime"] = lTimeOss.str();
      std::ostringstream lProgressOss;
      lProgressOss << std::setprecision(5) << (lTransitionSnapshot.getProgress() * 100.0);
      lJsonOutput["progress"] = lProgressOss.str();
      lJsonOutput["completedCommands"] = (unsigned int) lTransitionSnapshot.getNumberOfCompletedCommands();
      lJsonOutput["totalCommands"] = (unsigned int) lTransitionSnapshot.getTotalNumberOfCommands();

      if( (lTransitionState == swatch::action::Functionoid::kDone) || (lTransitionState == swatch::action::Functionoid::kWarning) || (lTransitionState == swatch::action::Functionoid::kError))
      {
        LOG4CPLUS_DEBUG(logger_, "StateMachinePanel > Transition finished running!");
        lJsonOutput["done"] = true;
      } else {
        lJsonOutput["done"] = false;
      }
      out << lJsonOutput;
    }

    return;
  }

} // end ms swatchframework

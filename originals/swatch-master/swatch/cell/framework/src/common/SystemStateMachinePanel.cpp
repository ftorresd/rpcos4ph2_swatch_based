#include "swatchcell/framework/SystemStateMachinePanel.h"

#include <map>
#include <vector>

#include "boost/foreach.hpp"

#include "ajax/PolymerElement.h"
#include "ajax/toolbox.h"

#include "ts/toolbox/Tools.h"

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"

#include "swatch/action/StateMachine.hpp"
#include "swatch/dtm/DaqTTCManager.hpp"
#include "swatch/system/System.hpp"
#include "swatch/processor/Processor.hpp"
#include "swatch/action/SystemStateMachine.hpp"

#include "swatchcell/framework/tools/panelFunctions.h"
#include "swatchcell/framework/tools/polymerFunctions.h"

#include "jsoncpp/json/json.h"


namespace swatchcellframework
{

  SystemStateMachinePanel::SystemStateMachinePanel(tsframework::CellAbstractContext* context, log4cplus::Logger& logger) :
    CellPanel(context, logger),
    logger_(logger),
    cellContext_(dynamic_cast<swatchcellframework::CellContext&>(*context))
  {
  }


  SystemStateMachinePanel::~SystemStateMachinePanel()
  {
  }

  /*
  ██       █████  ██    ██  ██████  ██    ██ ████████
  ██      ██   ██  ██  ██  ██    ██ ██    ██    ██
  ██      ███████   ████   ██    ██ ██    ██    ██
  ██      ██   ██    ██    ██    ██ ██    ██    ██
  ███████ ██   ██    ██     ██████   ██████     ██
  */

  void SystemStateMachinePanel::layout(cgicc::Cgicc& cgi)
  {
    remove();

    ajax::PolymerElement* stateMachinePanel = new ajax::PolymerElement("system-state-machine-panel");

    this -> setEvent("SystemStateMachinePanel::getStateMachineArray", ajax::Eventable::OnClick, this, &SystemStateMachinePanel::getStateMachineArray);
    this -> setEvent("SystemStateMachinePanel::getStateMachineInfo", ajax::Eventable::OnClick, this, &SystemStateMachinePanel::getStateMachineInfo);
    this -> setEvent("SystemStateMachinePanel::actStateMachine", ajax::Eventable::OnClick, this, &SystemStateMachinePanel::actStateMachine);
    this -> setEvent("SystemStateMachinePanel::getTransitionStatus", ajax::Eventable::OnTime, this, &SystemStateMachinePanel::getTransitionStatus);


    this -> add(stateMachinePanel);

    LOG4CPLUS_DEBUG(getLogger(), "SystemStateMachinePanel<" << this << "> : layout method called");


  }

  /*
  ███████ ███    ███      █████  ██████  ██████   █████  ██    ██
  ██      ████  ████     ██   ██ ██   ██ ██   ██ ██   ██  ██  ██
  ███████ ██ ████ ██     ███████ ██████  ██████  ███████   ████
       ██ ██  ██  ██     ██   ██ ██   ██ ██   ██ ██   ██    ██
  ███████ ██      ██     ██   ██ ██   ██ ██   ██ ██   ██    ██
  */

  void SystemStateMachinePanel::getStateMachineArray(cgicc::Cgicc& cgi, std::ostream& out) {

    CellContext::SharedGuard_t lGuard(cellContext_);
    swatch::system::System& lSystem = cellContext_.getSystem(lGuard);

    if(!lSystem.getStateMachines().empty())
    {
      const std::set<std::string> lOperations = lSystem.getStateMachines();
      Json::Value lStateMachineArrayJson(Json::arrayValue);
      for(std::set<std::string>::const_iterator lStateMachineIterator = lOperations.begin(); lStateMachineIterator != lOperations.end(); lStateMachineIterator++) {
        lStateMachineArrayJson.append(*lStateMachineIterator);
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

  void SystemStateMachinePanel::getStateMachineInfo(cgicc::Cgicc& cgi, std::ostream& out) {

    CellContext::SharedGuard_t lGuard(cellContext_);
    swatch::system::System& lSystem = cellContext_.getSystem(lGuard);

    std::map<std::string,std::string> lValues(ajax::toolbox::getSubmittedValues(cgi));
    std::string lSelectedStateMachine = lValues["selectedstatemachine"];

    Json::Value lStateMachineInfoJson(Json::objectValue);

    lStateMachineInfoJson["stateMachineName"] = lSelectedStateMachine;

    swatch::action::SystemStateMachine& lStateMachine = lSystem.getStateMachine(lSelectedStateMachine);

    swatch::action::ActionableSystem::Status_t lState = lSystem.getStatus();

    std::string lFsmCurrentState = lState.getState();

//      if (fsmState_ != lState.getState())
//        selectedTransition_ = "";

    typedef std::map<std::string, swatch::action::SystemTransition*> tTransitionMap;

    Json::Value lFsmStructureJson(Json::arrayValue);

    //A state machine is made of
    //1) states which can have several
    //2) SystemTransition which have
    //3) Multiple steps with
    //4) SubTransitions that acts on
    //5) Childen objects
    //
    //5 levels of depth :S

    //1) SystemStates
    for(std::vector<std::string>::const_iterator lStateMachineIterator = lStateMachine.getStates().begin(); lStateMachineIterator != lStateMachine.getStates().end(); lStateMachineIterator++)
    {

      Json::Value lFsmStateJson(Json::objectValue);

      if(*lStateMachineIterator == lStateMachine.getInitialState())
        lFsmStateJson["type"] = "Make";
      else if(*lStateMachineIterator == lStateMachine.getErrorState())
        lFsmStateJson["type"] = "Error";
      else
        lFsmStateJson["type"] = "Normal";

      lFsmStateJson["id"] = *lStateMachineIterator;


      //2) System transitions
      Json::Value lSystemTransitionsJson(Json::arrayValue);
      for(tTransitionMap::const_iterator lSystemTransitionIterator = lStateMachine.getTransitions(*lStateMachineIterator).begin(); lSystemTransitionIterator != lStateMachine.getTransitions(*lStateMachineIterator).end(); lSystemTransitionIterator++)
      {
        swatch::action::SystemTransition& lSystemTransition = *(lSystemTransitionIterator -> second);
        Json::Value lSystemTransitionJson(Json::objectValue);
        //Incapsulating the code to keep it clean
        this -> serializeSystemTransition(lSystemTransition, lSystemTransitionJson);


        lSystemTransitionsJson.append(lSystemTransitionJson);
      }
      lFsmStateJson["transitions"] = lSystemTransitionsJson;

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

    out << lStateMachineInfoJson;

  }

  void SystemStateMachinePanel::serializeSystemTransition(const swatch::action::SystemTransition& lSystemTransition , Json::Value& lSystemTransitionJson) {

    lSystemTransitionJson["id"] = lSystemTransition.getId();
    lSystemTransitionJson["endState"] = lSystemTransition.getEndState();

    Json::Value lStepsJson (Json::arrayValue);

    if ( lSystemTransition.size() != 0 )
    {
      uint lStepIndex = 1;
      //3) Steps
      for(swatch::action::SystemTransition::const_iterator lStepsIterator = lSystemTransition.begin(); lStepsIterator!=lSystemTransition.end(); lStepsIterator++, lStepIndex++)
      {
        Json::Value lStepJson(Json::objectValue);

        lStepJson["index"] = lStepIndex;

        // Represent child transition data in reduced format - one row for each set of child transitions with same transition ID, start state ID & end state ID
        typedef std::map< std::pair<std::string,std::pair<std::string,std::string> >, std::vector<std::string> > tTransitionInfoMap;
        tTransitionInfoMap lChildTransitions;

        Json::Value lChildTransitionsJson(Json::arrayValue);

        for(std::vector<swatch::action::Transition*>::const_iterator lChildTransitionsIterator=lStepsIterator->cget().begin(); lChildTransitionsIterator!=lStepsIterator->cget().end(); lChildTransitionsIterator++)
        {
          const swatch::action::ActionableObject& lChild = (*lChildTransitionsIterator)->getStateMachine().getActionable();
          lChildTransitions[ std::make_pair((*lChildTransitionsIterator)->getId() , std::make_pair((*lChildTransitionsIterator)->getStartState(), (*lChildTransitionsIterator)->getEndState()) ) ].push_back(lChild.getId());
        }
        //4) childTransitions
        for(tTransitionInfoMap::const_iterator lChildTransitionsIterator=lChildTransitions.begin(); lChildTransitionsIterator!=lChildTransitions.end(); lChildTransitionsIterator++)
        {
          Json::Value lChildTransitionJson(Json::objectValue);
          const std::string& lTransitionId = lChildTransitionsIterator->first.first;
          const std::pair<std::string,std::string>& lStates = lChildTransitionsIterator->first.second;
          const std::vector<std::string>& lChildren = lChildTransitionsIterator->second;

          lChildTransitionJson["id"] = lTransitionId;
          lChildTransitionJson["beginState"] = lStates.first;
          lChildTransitionJson["endState"] = lStates.second;

          Json::Value lDevicesJson(Json::arrayValue);
          //5) Devices
          for(std::vector<std::string>::const_iterator lChildIt=lChildren.begin();lChildIt!=lChildren.end();lChildIt++)
          {
            lDevicesJson.append(*lChildIt);
          }
          lChildTransitionJson["devices"] = lDevicesJson;
          lChildTransitionsJson.append(lChildTransitionJson);
        }

        lStepJson["childTransitions"] = lChildTransitionsJson;
        lStepsJson.append(lStepJson);
      }
    }

    lSystemTransitionJson["steps"] = lStepsJson;

    return;
  }

  /*
   █████   ██████ ████████ ██  ██████  ███    ██
  ██   ██ ██         ██    ██ ██    ██ ████   ██
  ███████ ██         ██    ██ ██    ██ ██ ██  ██
  ██   ██ ██         ██    ██ ██    ██ ██  ██ ██
  ██   ██  ██████    ██    ██  ██████  ██   ████
  */


  void SystemStateMachinePanel::actStateMachine(cgicc::Cgicc& cgi,std::ostream& out){

    CellContext::SharedGuard_t lGuard(cellContext_);
    swatch::system::System& lSystem = cellContext_.getSystem(lGuard);

    std::map<std::string,std::string> lValues(ajax::toolbox::getSubmittedValues(cgi));
    std::string lSelectedStateMachine = lValues["selectedstatemachine"];
    std::string lAction = lValues["action"];
    std::string lIsTransition = lValues["istransition"];


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
            LOG4CPLUS_INFO(this->getLogger(), "> SystemStateMachinePanel is now engaging state machine " + lSelectedStateMachine);
            lSystem.getStateMachine(lSelectedStateMachine).engage(*lGk);
          }

          if(lAction == "reset"){
            LOG4CPLUS_INFO(this->getLogger(), "> SystemStateMachinePanel is now resetting state machine " + lSelectedStateMachine);
            lSystem.getStateMachine(lSelectedStateMachine).reset(*lGk);
          }

          if(lAction == "disengage"){
            LOG4CPLUS_INFO(this->getLogger(), "> SystemStateMachinePanel is now disengaging state machine " + lSelectedStateMachine);
            lSystem.getStateMachine(lSelectedStateMachine).disengage();
          }

          lJsonOutput["status"] = "Success";
          lJsonOutput["detail"] = "";
        } else {


          const std::string& lCurrentState = lValues["currentstate"];


          swatch::action::SystemStateMachine& lStateMachine = lSystem.getStateMachine(lSelectedStateMachine);
          swatch::action::SystemTransition& lTransition = *lStateMachine.getTransitions(lCurrentState).find(lAction)->second;
          //Checking for missing parameters

          typedef std::vector<swatch::action::CommandVec::MissingParam> tMissingParamVec;
          std::map<const swatch::action::Transition*, tMissingParamVec> lMissingParams;
          lTransition.checkForMissingParameters(*lGk, lMissingParams);

          if(lMissingParams.empty())
          {
            LOG4CPLUS_INFO(this->getLogger(), "SystemStateMachinePanel is now executing transition '" << lTransition.getId() << "' from state '" << lTransition.getStartState() << "'");
            lTransition.exec( *lGk );
            //We are already inside a try statement, if an exception is thrown we will already handle it.
            lJsonOutput["status"] = "Success";
            lJsonOutput["detail"] = "";
          }
          else
          {

            //Missing param!

            lJsonOutput["status"] = "MissingParameters";
            
            std::map<const swatch::action::Transition*, tMissingParamVec >::const_iterator lMissingParamsIterator = lMissingParams.begin();
            
            Json::Value lMissingParametersJsonArray(Json::arrayValue);
            
            for(; lMissingParamsIterator != lMissingParams.end(); lMissingParamsIterator++)
            {
              for(tMissingParamVec::const_iterator lMissingParamsIterator2 = lMissingParamsIterator -> second.begin(); lMissingParamsIterator2 != lMissingParamsIterator -> second.end(); lMissingParamsIterator2++)
              {
                Json::Value lMissingParameterJson(Json::objectValue);                
                lMissingParameterJson["device"] = lMissingParamsIterator -> first -> getActionable().getId();
                lMissingParameterJson["transition"] = lMissingParamsIterator -> first -> getId();
                lMissingParameterJson["namespace"] = lMissingParamsIterator2 -> nspace;
                lMissingParameterJson["command"] = lMissingParamsIterator2 -> command;
                lMissingParameterJson["parameter"] = lMissingParamsIterator2 -> parameter;
                lMissingParametersJsonArray.append(lMissingParameterJson);
              }
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

  void SystemStateMachinePanel::getTransitionStatus(cgicc::Cgicc& cgi,std::ostream& out){

    std::map<std::string,std::string> lValues(ajax::toolbox::getSubmittedValues(cgi));
    std::string lSelectedStateMachine = lValues["selectedstatemachine"];
    std::string lSelectedTransition = lValues["transition"];
    std::string lCurrentState = lValues["currentstate"];

    Json::Value lJsonOutput(Json::objectValue);

    if ((lSelectedStateMachine == "") || (lSelectedStateMachine == "undefined")){
      lJsonOutput["notFound"] = true;
      out << lJsonOutput;
      return;
    }

    CellContext::SharedGuard_t lGuard(cellContext_);
    swatch::system::System& lSystem = cellContext_.getSystem(lGuard);

    swatch::action::SystemStateMachine& lStateMachine = lSystem.getStateMachine(lSelectedStateMachine);
    
    //Well.. this is messy:
    //I want to get the current transition state, in order to do that I have to get the current FSM state
    //Then I need to fetch the TransistionSnapshot and get the state from it

    //ActionableObject status
    swatch::action::ActionableSnapshot lActionableSnapshot = lSystem.getStatus();
    //Getting the transition
    swatch::action::SystemTransition& lTransition = *lStateMachine.getTransitions(lCurrentState).at(lSelectedTransition);
    //Transition snapshot
    swatch::action::SystemTransitionSnapshot lTransitionSnapshot = lTransition.getStatus();
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
      lProgressOss << std::setprecision(4) << (lTransitionSnapshot.getProgress() * 100.0);
      lJsonOutput["progress"] = lProgressOss.str();
      lJsonOutput["stepIndex"] = (unsigned int) (lTransitionSnapshot.end() - lTransitionSnapshot.begin());
      lJsonOutput["totalSteps"] = (unsigned int) lTransitionSnapshot.getTotalNumberOfSteps();
      lJsonOutput["message"] = lTransitionSnapshot.getMessage();
      
      Json::Value lStepsJson(Json::arrayValue);
      
      swatch::action::SystemTransition::const_iterator lStepIt = lTransition.begin();
      swatch::action::SystemTransitionSnapshot::const_iterator lStepStatusIt= lTransitionSnapshot.begin();
      
      for(; lStepStatusIt!=lTransitionSnapshot.end(); lStepIt++,lStepStatusIt++)
      {
        Json::Value lStepJson (Json::objectValue);
        
        lStepJson["index"] = boost::lexical_cast<std::string>(((lStepIt - lTransition.begin()) + 1));

        Json::Value lChildTransitionsJson(Json::arrayValue);

        std::vector<swatch::action::Transition*>::const_iterator lChildTransitionIt=lStepIt->cget().begin();
        swatch::action::SystemTransitionSnapshot::Step_t::const_iterator lChildStatusIt=lStepStatusIt->begin();

        for(; lChildTransitionIt!=lStepIt->cget().end(); lChildTransitionIt++, lChildStatusIt++)
        {
          Json::Value lChildTransitionJson(Json::objectValue);
          
          lChildTransitionJson["device"] = (*lChildTransitionIt)->getStateMachine().getActionable().getId();
          lChildTransitionJson["id"] = (*lChildTransitionIt)->getId();
          if (lTransitionSnapshot.getEnabledChildren().count((*lChildTransitionIt)->getActionable().getPath()))
          {
            std::ostringstream lChildProgressOss;
            lChildProgressOss << std::setprecision(4) << ((*lChildStatusIt) -> getProgress() * 100.0);
            lChildTransitionJson["progress"] = lChildProgressOss.str();
            lTimeOss.str("");
            lTimeOss << std::setprecision(4) << (*lChildStatusIt)->getRunningTime() << "s";

            lChildTransitionJson["runningTime"] = lTimeOss.str();

            swatch::action::Functionoid::State lChildState = (*lChildStatusIt)->getState();
            if (((*lChildStatusIt)->end() != (*lChildStatusIt)->begin()) && ((lChildState == swatch::action::Functionoid::kRunning) || (lChildState == swatch::action::Functionoid::kError)))
            {
              const swatch::action::CommandSnapshot& lCmdStatus = *((*lChildStatusIt)->end()-1);

              std::vector<std::string> lTokens = tstoolbox::Tools::split(lCmdStatus.getActionPath(),".");

              lChildTransitionJson["lastCommandId"] = lTokens.back();
              
              lChildTransitionJson["commandIndex"] = (unsigned int) ((*lChildStatusIt)->end() - (*lChildStatusIt)->begin());
              lChildTransitionJson["totalCommands"] = (unsigned int) (*lChildStatusIt)->getTotalNumberOfCommands();
              lChildTransitionJson["message"] = lCmdStatus.getStatusMsg();
            }
          } else { /* this child  has been disabled */
            lChildTransitionJson["progress"] = "Disabled";
            lChildTransitionJson["lastCommandId"] = "N/A";
          }
          
          lChildTransitionsJson.append(lChildTransitionJson);
        }
        
        lStepJson["childTransitions"] = lChildTransitionsJson;
        lStepsJson.append(lStepJson);
      }
      
      lJsonOutput["steps"] = lStepsJson;
      
      if( (lTransitionState == swatch::action::Functionoid::kDone) || (lTransitionState == swatch::action::Functionoid::kWarning) || (lTransitionState == swatch::action::Functionoid::kError))
      {
        LOG4CPLUS_DEBUG(logger_, "SystemStateMachinePanel > Transition finished running!");
        lJsonOutput["done"] = true;
      } else {
        lJsonOutput["done"] = false;
      }
      out << lJsonOutput;
      
    }

  }

} // end ms swatchframework

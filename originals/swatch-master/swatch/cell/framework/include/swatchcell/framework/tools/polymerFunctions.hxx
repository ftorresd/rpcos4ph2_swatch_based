
#ifndef __SWATCHCELL_FRAMEWORK_TOOLS_POLYMERFUNCTIONS_HXX__
#define __SWATCHCELL_FRAMEWORK_TOOLS_POLYMERFUNCTIONS_HXX__


#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include "swatch/action/MaskableObject.hpp"
#include "swatch/core/MonitorableObject.hpp"
#include "swatch/dtm/AMCPortCollection.hpp"
#include "swatch/dtm/AMCPort.hpp"
#include "swatch/processor/PortCollection.hpp"
#include "swatch/processor/Port.hpp"

#include <csignal>

namespace swatchcellframework {

  /*
  ██████   █████  ███████ ██  ██████     ██ ███    ██ ███████  ██████
  ██   ██ ██   ██ ██      ██ ██          ██ ████   ██ ██      ██    ██
  ██████  ███████ ███████ ██ ██          ██ ██ ██  ██ █████   ██    ██
  ██   ██ ██   ██      ██ ██ ██          ██ ██  ██ ██ ██      ██    ██
  ██████  ██   ██ ███████ ██  ██████     ██ ██   ████ ██       ██████
  */
  /// basic info for MonitorableObjects
  template<typename MonitorableObj>
  void serializeMonitorableBasicInfo(Json::Value&  jsonOutput,
    const MonitorableObj & aObject
  ) {
    std::ostringstream status;
    status << aObject.getStatusFlag();
    jsonOutput["id"] = aObject.getId();
    jsonOutput["status"] = status.str();
    jsonOutput["monitoringstatus"] = monitoringStatusToString(aObject.getMonitoringStatus());
    const swatch::action::MaskableObject* maskableObject = dynamic_cast<const swatch::action::MaskableObject*> (&aObject);
    //Checking if it is a MaskableObject
    if (maskableObject != NULL)
    {
      jsonOutput["ismasked"] = maskableObject -> isMasked();
    }

  }


  /*
   ██████  ██████  ███    ███ ██████  ██      ███████ ████████ ███████     ██ ███    ██ ███████  ██████
  ██      ██    ██ ████  ████ ██   ██ ██      ██         ██    ██          ██ ████   ██ ██      ██    ██
  ██      ██    ██ ██ ████ ██ ██████  ██      █████      ██    █████       ██ ██ ██  ██ █████   ██    ██
  ██      ██    ██ ██  ██  ██ ██      ██      ██         ██    ██          ██ ██  ██ ██ ██      ██    ██
   ██████  ██████  ██      ██ ██      ███████ ███████    ██    ███████     ██ ██   ████ ██       ██████
  */

  /// Basic info + metrics for a MonitorableObject
  template<typename MonitorableObj>
  void serializeMonitorableCompleteInfo(Json::Value&  jsonOutput,
    const MonitorableObj & aObject
  ) {
    std::ostringstream status;
    status << aObject.getStatusFlag();
    jsonOutput["id"] = aObject.getId();
    jsonOutput["status"] = status.str();
    jsonOutput["monitoringstatus"] = monitoringStatusToString(aObject.getMonitoringStatus());
    const swatch::action::MaskableObject* maskableObject = dynamic_cast<const swatch::action::MaskableObject*> (&aObject);
    //Checking if it is a MaskableObject
    if (maskableObject != NULL)
    {
      jsonOutput["ismasked"] = maskableObject -> isMasked();
    }

    std::vector<std::string> metrics( aObject.getMetrics() );
    Json::Value metricsArray(Json::arrayValue);
    BOOST_FOREACH( std::string& metricId, metrics ) {
      swatch::core::MetricSnapshot snapshot = aObject.getMetric(metricId).getSnapshot();
      Json::Value metric(Json::objectValue);
      serializeMetric(metric, metricId, snapshot);
      metricsArray.append(metric);
    }
    jsonOutput["metrics"] = metricsArray;

  }

  template<typename PortObj>
  void serializePortForPortsTable( Json::Value & jsonOutput,
      const PortObj & aObject
    ) {

    jsonOutput["id"] = aObject.getId();
    std::ostringstream status;
    status << aObject.getStatusFlag();
    jsonOutput["status"] = status.str();
    jsonOutput["monitoringstatus"] = monitoringStatusToString(aObject.getMonitoringStatus());
    const swatch::action::MaskableObject* maskableObject = dynamic_cast<const swatch::action::MaskableObject*> (&aObject);
    //Checking if it is a MaskableObject
    if (maskableObject != NULL)
    {
      jsonOutput["ismasked"] = maskableObject -> isMasked();
    }
    std::vector<std::string> metrics( aObject.getMetrics() );
    Json::Value metricCollection (Json::objectValue);
    BOOST_FOREACH( std::string& metricId, metrics ) {
      swatch::core::MetricSnapshot snapshot = aObject.getMetric(metricId).getSnapshot();
      Json::Value metric(Json::objectValue);
      serializeMetricBasicInfo(metric, metricId, snapshot);
      metricCollection[metricId] = metric;
    }
    jsonOutput["metrics"] = metricCollection;
  }



// !!! WARNING !!!
// Beautiful template programming ahead
// Proceed with caution

  template<typename MonitorableObj>
  void serializeMonitorableTreeWithPortsStatus(Json::Value& aOutputJson, const Json::Value& aSubMonitorablesArrayJson, const MonitorableObj &aMonitorableObject)
  {
    Json::Value nullJson (Json::nullValue);

    serializeMonitorableCompleteInfo(aOutputJson, aMonitorableObject);

    //Serializing the submonitorables
    std::vector<std::string> lChildrenList = aMonitorableObject.getChildren();

    bool isSubMonitorablesArrayNull = aSubMonitorablesArrayJson.isNull();
    //Container for submonitorables array output
    Json::Value lSubMonitorablesArray(Json::arrayValue);
    //I save this subcomp separately because thay need a special treatment:
    //1) they must have the subcomponents in a special order
    //2) I have to flag them
    //3) They must be the last elements of the array
    Json::Value inputPortsNode;
    Json::Value outputPortsNode;
    Json::Value amcPortsNode;
    //Now we serialize only the infos of the monitorable objects
    //For each of the children object, check if it a MonitorableObject
    BOOST_FOREACH(const std::string& lSubMoniObjId, lChildrenList){
      if (const swatch::core::MonitorableObject* lSubMonitorable = aMonitorableObject. template getObjPtr<swatch::core::MonitorableObject>(lSubMoniObjId)){

        //If Monitorable prepare the container
        Json::Value lSubMonitorableJson;

        //Checking if it is in the openedSubmonitorables array
        if(!isSubMonitorablesArrayNull){
          bool lCompleteInfo = false;
          for (Json::ValueConstIterator it = aSubMonitorablesArrayJson.begin(); ((it != aSubMonitorablesArrayJson.end()) && (!lCompleteInfo)) ; it++) {
            //std::cout << lSubMoniObjId << " == " << it -> get("id", nullJson).asString() << " := " << (lSubMoniObjId == it -> get("id", nullJson).asString())  << std::endl;
            if (lSubMoniObjId == it -> get("id", nullJson).asString() ) {
              lCompleteInfo = true;
              Json::Value lSubSubMonitorables = it -> get("openedSubmonitorables", nullJson);
              if (const swatch::processor::OutputPortCollection* lOutputPortCollection = aMonitorableObject. template getObjPtr<swatch::processor::OutputPortCollection>(lSubMoniObjId)){
                serializePortCollectionTree(outputPortsNode,  lSubSubMonitorables, *lOutputPortCollection);
              }
              else if (const swatch::processor::InputPortCollection* lInputPortCollection = aMonitorableObject. template getObjPtr<swatch::processor::InputPortCollection>(lSubMoniObjId)){
                serializePortCollectionTree(inputPortsNode,  lSubSubMonitorables, *lInputPortCollection);
              }
              else if (const swatch::dtm::AMCPortCollection* lAMCPortCollection = aMonitorableObject. template getObjPtr<swatch::dtm::AMCPortCollection>(lSubMoniObjId)){
                serializePortCollectionTree(amcPortsNode,  lSubSubMonitorables, *lAMCPortCollection);
              }
              else serializeMonitorableTree(lSubMonitorableJson,  lSubSubMonitorables, *lSubMonitorable);
            };
          }

          if (!lCompleteInfo) {
            if (const swatch::processor::OutputPortCollection* lOutputPortCollection = aMonitorableObject. template getObjPtr<swatch::processor::OutputPortCollection>(lSubMoniObjId)){
              serializePortCollectionTree(outputPortsNode, nullJson, *lOutputPortCollection);
            }
            else if (const swatch::processor::InputPortCollection* lInputPortCollection = aMonitorableObject. template getObjPtr<swatch::processor::InputPortCollection>(lSubMoniObjId)){
              serializePortCollectionTree(inputPortsNode, nullJson , *lInputPortCollection);
            }
            else if (const swatch::dtm::AMCPortCollection* lAMCPortCollection = aMonitorableObject. template getObjPtr<swatch::dtm::AMCPortCollection>(lSubMoniObjId)){
              serializePortCollectionTree(amcPortsNode, nullJson, *lAMCPortCollection);
            }
            else serializeMonitorableBasicInfo(lSubMonitorableJson, *lSubMonitorable);
          }
        }
        //if the array is empty or non existing I serialise only the basic info
        else  {
          if (const swatch::processor::OutputPortCollection* lOutputPortCollection = aMonitorableObject. template getObjPtr<swatch::processor::OutputPortCollection>(lSubMoniObjId)){
            serializePortCollectionTree(outputPortsNode, nullJson, *lOutputPortCollection);
          }
          else if (const swatch::processor::InputPortCollection* lInputPortCollection = aMonitorableObject. template getObjPtr<swatch::processor::InputPortCollection>(lSubMoniObjId)){
            serializePortCollectionTree(inputPortsNode, nullJson , *lInputPortCollection);
          }
          else if (const swatch::dtm::AMCPortCollection* lAMCPortCollection = aMonitorableObject. template getObjPtr<swatch::dtm::AMCPortCollection>(lSubMoniObjId)){
            serializePortCollectionTree(amcPortsNode, nullJson, *lAMCPortCollection);
          }
          else serializeMonitorableBasicInfo(lSubMonitorableJson, *lSubMonitorable);
        }
        //Appending the json to the array
        //If it is null we have just serialized a portcollection!
        if(!lSubMonitorableJson.isNull()){
          lSubMonitorableJson["type"] = "component";
          lSubMonitorablesArray.append(lSubMonitorableJson);
        }
      }
    }
    //Appending the existing port collection for last
    if (!inputPortsNode.isNull()) {
      inputPortsNode["type"] = "inputPorts";
      lSubMonitorablesArray.append(inputPortsNode);
    }
    if (!outputPortsNode.isNull()) {
      outputPortsNode["type"] = "outputPorts";
      lSubMonitorablesArray.append(outputPortsNode);
    }
    if (!amcPortsNode.isNull()) {
      amcPortsNode["type"] = "amcPorts";
      lSubMonitorablesArray.append(amcPortsNode);
    }
    aOutputJson["components"] = lSubMonitorablesArray;
    return;
  }

  template<typename MonitorableObj>
  void serializeMonitorableTree(Json::Value& aOutputJson, const Json::Value& aSubMonitorablesArrayJson, const MonitorableObj &aMonitorableObject)
  {
    Json::Value nullJson (Json::nullValue);

    serializeMonitorableCompleteInfo(aOutputJson, aMonitorableObject);

    //Serializing the submonitorables
    std::vector<std::string> lChildrenList = aMonitorableObject.getChildren();

    bool isSubMonitorablesArrayNull = aSubMonitorablesArrayJson.isNull();
    //Container for submonitorables array output
    Json::Value lSubMonitorablesArray(Json::arrayValue);
    //I save this subcomp separately because thay need a special treatment:
    //1) they must have the subcomponents in a special order
    //2) I have to flag them
    //3) They must be the last elements of the array
    Json::Value inputPortsNode;
    Json::Value outputPortsNode;
    Json::Value amcPortsNode;
    //Now we serialize only the infos of the monitorable objects
    //For each of the children object, check if it a MonitorableObject
    BOOST_FOREACH(const std::string& lSubMoniObjId, lChildrenList){
      if (const swatch::core::MonitorableObject* lSubMonitorable = aMonitorableObject. template getObjPtr<swatch::core::MonitorableObject>(lSubMoniObjId)){

        //If Monitorable prepare the container
        Json::Value lSubMonitorableJson;

        //Checking if it is in the openedSubmonitorables array
        if(!isSubMonitorablesArrayNull){
          bool lCompleteInfo = false;
          for (Json::ValueConstIterator it = aSubMonitorablesArrayJson.begin(); ((it != aSubMonitorablesArrayJson.end()) && (!lCompleteInfo)) ; it++) {
            //std::cout << lSubMoniObjId << " == " << it -> get("id", nullJson).asString() << " := " << (lSubMoniObjId == it -> get("id", nullJson).asString())  << std::endl;
            if (lSubMoniObjId == it -> get("id", nullJson).asString() ) {
              lCompleteInfo = true;
              Json::Value lSubSubMonitorables = it -> get("openedSubmonitorables", nullJson);
              if (const swatch::processor::OutputPortCollection* lOutputPortCollection = aMonitorableObject. template getObjPtr<swatch::processor::OutputPortCollection>(lSubMoniObjId)){
                serializeMonitorableTree(outputPortsNode,  lSubSubMonitorables, *lOutputPortCollection);
              }
              else if (const swatch::processor::InputPortCollection* lInputPortCollection = aMonitorableObject. template getObjPtr<swatch::processor::InputPortCollection>(lSubMoniObjId)){
                serializeMonitorableTree(inputPortsNode,  lSubSubMonitorables, *lInputPortCollection);
              }
              else if (const swatch::dtm::AMCPortCollection* lAMCPortCollection = aMonitorableObject. template getObjPtr<swatch::dtm::AMCPortCollection>(lSubMoniObjId)){
                serializeMonitorableTree(amcPortsNode,  lSubSubMonitorables, *lAMCPortCollection);
              }
              else serializeMonitorableTree(lSubMonitorableJson,  lSubSubMonitorables, *lSubMonitorable);
            };
          }

          if (!lCompleteInfo) {
            if (const swatch::processor::OutputPortCollection* lOutputPortCollection = aMonitorableObject. template getObjPtr<swatch::processor::OutputPortCollection>(lSubMoniObjId)){
              serializeMonitorableBasicInfo(outputPortsNode, *lOutputPortCollection);
            }
            else if (const swatch::processor::InputPortCollection* lInputPortCollection = aMonitorableObject. template getObjPtr<swatch::processor::InputPortCollection>(lSubMoniObjId)){
              serializeMonitorableBasicInfo(inputPortsNode, *lInputPortCollection);
            }
            else if (const swatch::dtm::AMCPortCollection* lAMCPortCollection = aMonitorableObject. template getObjPtr<swatch::dtm::AMCPortCollection>(lSubMoniObjId)){
              serializeMonitorableBasicInfo(amcPortsNode, *lAMCPortCollection);
            }
            else serializeMonitorableBasicInfo(lSubMonitorableJson, *lSubMonitorable);
          }
        }
        //if the array is empty or non existing I serialise only the basic info
        else  {
          if (const swatch::processor::OutputPortCollection* lOutputPortCollection = aMonitorableObject. template getObjPtr<swatch::processor::OutputPortCollection>(lSubMoniObjId)){
            serializeMonitorableBasicInfo(outputPortsNode, *lOutputPortCollection);
          }
          else if (const swatch::processor::InputPortCollection* lInputPortCollection = aMonitorableObject. template getObjPtr<swatch::processor::InputPortCollection>(lSubMoniObjId)){
            serializeMonitorableBasicInfo(inputPortsNode, *lInputPortCollection);
          }
          else if (const swatch::dtm::AMCPortCollection* lAMCPortCollection = aMonitorableObject. template getObjPtr<swatch::dtm::AMCPortCollection>(lSubMoniObjId)){
            serializeMonitorableBasicInfo(amcPortsNode, *lAMCPortCollection);
          }
          else serializeMonitorableBasicInfo(lSubMonitorableJson, *lSubMonitorable);
        }
        //Appending the json to the array
        //If it is null we have just serialized a portcollection!
        if(!lSubMonitorableJson.isNull()){
          lSubMonitorableJson["type"] = "component";
          lSubMonitorablesArray.append(lSubMonitorableJson);
        }
      }
    }
    //Appending the existing port collection for last
    if (!inputPortsNode.isNull()) {
      inputPortsNode["type"] = "inputPorts";
      lSubMonitorablesArray.append(inputPortsNode);
    }
    if (!outputPortsNode.isNull()) {
      outputPortsNode["type"] = "outputPorts";
      lSubMonitorablesArray.append(outputPortsNode);
    }
    if (!amcPortsNode.isNull()) {
      amcPortsNode["type"] = "amcPorts";
      lSubMonitorablesArray.append(amcPortsNode);
    }
    aOutputJson["components"] = lSubMonitorablesArray;
    return;
  }

  //Serialize the port in a sorted way for AMC, Output and Input Ports using a single template <3 <3 <3
  template<typename tPortCollection>
  void serializePortCollectionTree(Json::Value& aOutputJson, const Json::Value& aSubMonitorablesArrayJson, const tPortCollection &aPortCollection)
  {
    Json::Value nullJson (Json::nullValue);

    serializeMonitorableCompleteInfo(aOutputJson, aPortCollection);


    bool isSubMonitorablesArrayNull = aSubMonitorablesArrayJson.isNull();
    //Container for submonitorables array output
    Json::Value lSubMonitorablesArray(Json::arrayValue);

    //Now we serialize only the infos of the monitorable objects
    //For each of the children object, check if it a MonitorableObject

    //auto = Input/Output/AMCPort based on the template <3
    BOOST_FOREACH(const auto* lSubMonitorable,
      aPortCollection.getPorts()
    ){

      Json::Value lSubMonitorableJson(Json::objectValue);
      std::string lSubMoniObjId = lSubMonitorable -> getId();

      //Checking if it is in the openedSubmonitorables array
      if(!isSubMonitorablesArrayNull){
        bool lCompleteInfo = false;
        for (Json::ValueConstIterator it = aSubMonitorablesArrayJson.begin(); ((it != aSubMonitorablesArrayJson.end()) && (!lCompleteInfo)) ; it++) {
          //std::cout << lSubMoniObjId << " == " << it -> get("id", nullJson).asString() << " := " << (lSubMoniObjId == it -> get("id", nullJson).asString())  << std::endl;
          if (lSubMoniObjId == it -> get("id", nullJson).asString() ) {
            lCompleteInfo = true;
            Json::Value lSubSubMonitorables = it -> get("openedSubmonitorables", nullJson);
            serializeMonitorableTree(lSubMonitorableJson,  lSubSubMonitorables, *lSubMonitorable);
          };
        }

        if (!lCompleteInfo) serializeMonitorableBasicInfo(lSubMonitorableJson, *lSubMonitorable);
      }
      //if the array is empty or non existing I serialise only the basic info
      else serializeMonitorableBasicInfo(lSubMonitorableJson, *lSubMonitorable);
      //Appending the json to the array
      lSubMonitorablesArray.append(lSubMonitorableJson);
    }
    aOutputJson["components"] = lSubMonitorablesArray;
    return;
  }


  template<typename tPortCollection>
  void serializePortCollectionForPortsTable(Json::Value& aOutputJson, const tPortCollection &aPortCollection)
  {
    Json::Value lComponentsJson(Json::arrayValue);

    std::map<std::string, int> lPortTypesToArrayMap;

    // auto = In/Out/AMC Port
    BOOST_FOREACH(const auto* lPort,
      aPortCollection.getPorts())
    {
      const std::string & lPortType = lPort -> getTypeName();
      auto lMapIterator = lPortTypesToArrayMap.find(lPortType);
      // Seeing if this type of port has been serialised once 
      if (lMapIterator != lPortTypesToArrayMap.end()){
        // has been serialised before
        Json::Value lComponent(Json::objectValue);
        serializePortForPortsTable(lComponent, *lPort);
        //get the collection index and push
        lComponentsJson[lMapIterator->second].append(lComponent);
      } else {
        // never been serialised
        Json::Value lPortArray(Json::arrayValue);
        Json::Value lComponent(Json::objectValue);
        serializePortForPortsTable(lComponent, *lPort);
        lPortArray.append(lComponent);
        lPortTypesToArrayMap.insert(std::pair<std::string, int>(lPort -> getTypeName(), lComponentsJson.size()));
        lComponentsJson.append(lPortArray);
      }

    }

    serializeMonitorableCompleteInfo(aOutputJson, aPortCollection);
    aOutputJson["components"] = lComponentsJson;

    return;
  }

  template<typename tActionableSnapshot>
  void serializeStateMachineStatus (Json::Value& aOutputJson, const tActionableSnapshot &aActionableSnapshot){

    if (aActionableSnapshot.isEngaged()) {
      aOutputJson["isengaged"] = true;
      aOutputJson["statemachineid"] = aActionableSnapshot.getStateMachineId();
      aOutputJson["state"] = aActionableSnapshot.getState();
    } else {
      aOutputJson["isengaged"] = false;
      aOutputJson["statemachineid"] = "";
      aOutputJson["state"] = "";
    }
    return;
  }


}



/*

CODE FOR CHILDREN
std::vector<std::string> childrenList = aObject.getChildren();

boost::property_tree::ptree subMonitorablesArray;

BOOST_FOREACH(const std::string& moniObjId,
    childrenList)
{
  const swatch::core::MonitorableObject* obj = aObject.templategetObjPtr<swatch::core::MonitorableObject>(moniObjId);

  if (obj != NULL){
    boost::property_tree::ptree subMonitorableTree;
    toMetricsArray(subMonitorableTree, *obj, true);
    subMonitorablesArray.push_back(std::make_pair("", subMonitorableTree));
  }
}
aTree.add_child("submonitorables", subMonitorablesArray);
*/

#endif /* __SWATCHCELL_FRAMEWORK_TOOLS_POLYMERFUNCTIONS_HXX__ */


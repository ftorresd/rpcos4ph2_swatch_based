#include "swatchcell/framework/tools/polymerFunctions.h"


#include <boost/foreach.hpp>
#include <sstream>

// SWATCH includes
#include "swatch/core/MonitorableObject.hpp"
#include "swatch/core/AbstractMetric.hpp"
#include "swatch/core/MetricView.hpp"
#include "swatch/system/Crate.hpp"
#include "swatch/dtm/AMCPort.hpp"
#include "swatch/dtm/AMCPortCollection.hpp"
#include "swatch/dtm/DaqTTCManager.hpp"
#include "swatch/dtm/SLinkExpress.hpp"
#include "swatch/processor/Processor.hpp"
#include "swatch/processor/Port.hpp"
#include "swatch/processor/PortCollection.hpp"
#include "swatch/system/Link.hpp"


namespace swatchcellframework {

  /// Metrics to JSON

  /*
  ███    ███ ███████ ████████ ██████  ██  ██████
  ████  ████ ██         ██    ██   ██ ██ ██
  ██ ████ ██ █████      ██    ██████  ██ ██
  ██  ██  ██ ██         ██    ██   ██ ██ ██
  ██      ██ ███████    ██    ██   ██ ██  ██████
  */

  void serializeMetric(Json::Value & jsonOutput, const std::string& aMetricId,
      const swatch::core::MetricSnapshot& aSnapshot)
  {

    jsonOutput["id"] = aMetricId;
    std::ostringstream status;
    status << aSnapshot.getStatusFlag();
    jsonOutput["status"] = status.str();
    jsonOutput["type"] = "metric";
    jsonOutput["errorCondition"] = aSnapshot.getErrorCondition() ? boost::lexical_cast<std::string>(*aSnapshot.getErrorCondition()) : "N/A";
    jsonOutput["warningCondition"] = aSnapshot.getWarningCondition() ? boost::lexical_cast<std::string>(*aSnapshot.getWarningCondition()) : "N/A";
    if (aSnapshot.getMonitoringStatus()==swatch::core::monitoring::kDisabled)
    {
      jsonOutput["value"] = "Disabled";
      jsonOutput["monitoringstatus"] = "Disabled";
    }
    else
    {
      jsonOutput["value"] = (aSnapshot.isValueKnown() ? aSnapshot.getValueAsString() : "Unknown");
      jsonOutput["monitoringstatus"] = monitoringStatusToString(aSnapshot.getMonitoringStatus());
    }
    time_t updateTime = boost::chrono::system_clock::to_time_t(aSnapshot.getUpdateTimestamp().system);
    char timeText[40];
    strftime(timeText, sizeof timeText, "%F   %T", gmtime(&updateTime));

    std::ostringstream oss;
    oss << timeText;

    jsonOutput["timestamp"] = oss.str();
    //I want the unix epoch time in msec
    oss.str("");
    oss << (updateTime*1000);
    jsonOutput["unixtimestamp"] = oss.str();
  }

  void serializeMetricBasicInfo(Json::Value& jsonOutput, const std::string& aMetricId,
      const swatch::core::MetricSnapshot& aSnapshot)
  {

    jsonOutput["id"] = aMetricId;
    std::ostringstream status;
    status << aSnapshot.getStatusFlag();
    jsonOutput["status"] = status.str();
    jsonOutput["value"] = (aSnapshot.isValueKnown() ? aSnapshot.getValueAsString() : "Unknown");
  }

  /*
  ██      ██ ███    ██ ██   ██ ███████
  ██      ██ ████   ██ ██  ██  ██
  ██      ██ ██ ██  ██ █████   ███████
  ██      ██ ██  ██ ██ ██  ██       ██
  ███████ ██ ██   ████ ██   ██ ███████
  */

  void serializeLinks(Json::Value& linksJson, const std::deque<swatch::system::Link*>& links, unsigned sysIdSize)
  {
    Json::Value linksArray(Json::arrayValue);
    swatch::core::StatusFlag linkStatus = swatch::core::kNoLimit;

    BOOST_FOREACH(swatch::system::Link* link, links)
    {
      Json::Value linkJson(Json::objectValue);
      linkJson["id"] = link->getId();
      swatch::processor::OutputPort* srcPort = link->getSrcPort();
      if (srcPort) {
        linkJson["src_port"] = srcPort->getPath().substr(sysIdSize + 1);
        std::ostringstream oss;
        oss << srcPort->getStatusFlag();
        linkJson["src_port_status"] = oss.str();
        linkStatus = linkStatus & srcPort->getStatusFlag();
      }
      swatch::processor::InputPort* dstPort = link->getDstPort();
      if (dstPort) {
        linkJson["dst_port"] = dstPort->getPath().substr(sysIdSize + 1);
        std::ostringstream oss;
        oss << dstPort->getStatusFlag();
        linkJson["dst_port_status"] = oss.str();
        linkStatus = linkStatus & dstPort->getStatusFlag();
      }
      linksArray.append(linkJson);
    }
    std::ostringstream statOss;
    statOss << linkStatus;
    linksJson["status"] = statOss.str();
    linksJson["links"] = linksArray;
  }

  /*
   ██████ ██████   █████  ████████ ███████     ███████ ██    ██ ███████     ██    ██ ██ ███████ ██     ██
  ██      ██   ██ ██   ██    ██    ██          ██       ██  ██  ██          ██    ██ ██ ██      ██     ██
  ██      ██████  ███████    ██    █████       ███████   ████   ███████     ██    ██ ██ █████   ██  █  ██
  ██      ██   ██ ██   ██    ██    ██               ██    ██         ██      ██  ██  ██ ██      ██ ███ ██
   ██████ ██   ██ ██   ██    ██    ███████     ███████    ██    ███████       ████   ██ ███████  ███ ███
  */

  void serializeCrateForSystemView(Json::Value& aJsonOutput ,const swatch::system::Crate& aCrate)
  {
    aJsonOutput["id"] = aCrate.getId();

    //Processors serialization
    Json::Value lProcessors(Json::arrayValue);
    serializeProcessorsForSystemView(lProcessors, aCrate);
    aJsonOutput["processors"] = lProcessors;

    //DAQTTC serialization
    Json::Value lDaqTTC(Json::objectValue);
    if (const swatch::dtm::DaqTTCManager * amc13 = aCrate.amc13()){
      aJsonOutput["fed"] = boost::lexical_cast<uint32_t>(amc13->getFedId());
      serializeAMC13ForSystemView(lDaqTTC, aCrate);
    }
    else {
      aJsonOutput["fed"] = "N/A";
      lDaqTTC["status"] = "Empty";
    }

    aJsonOutput["amc13"] = lDaqTTC;

    return;
  }

  /*
  ██████  ██████   ██████   ██████     ███████ ██    ██ ███████     ██    ██ ██ ███████ ██     ██
  ██   ██ ██   ██ ██    ██ ██          ██       ██  ██  ██          ██    ██ ██ ██      ██     ██
  ██████  ██████  ██    ██ ██          ███████   ████   ███████     ██    ██ ██ █████   ██  █  ██
  ██      ██   ██ ██    ██ ██               ██    ██         ██      ██  ██  ██ ██      ██ ███ ██
  ██      ██   ██  ██████   ██████     ███████    ██    ███████       ████   ██ ███████  ███ ███
  */

  void serializeProcessorsForSystemView(Json::Value & aProcessorsArrayJson, const swatch::system::Crate& aCrate )
  {

    for (uint32_t i = 1; i <= 12; ++i) {
      Json::Value lProcessorJson(Json::objectValue);
      if (aCrate.isAMCSlotTaken(i)) {
        Json::Value lNullJson(Json::nullValue);
        const swatch::processor::Processor& lProcessor = *aCrate.amc(i);
        lProcessorJson["type"] = "proc";
        lProcessorJson["crate"] = lProcessor.getCrateId();
        lProcessorJson["slot"] = boost::lexical_cast<uint32_t>(lProcessor.getSlot());
        serializeMonitorableTree(lProcessorJson, lNullJson, lProcessor);
      } else {
        lProcessorJson["slot"] = boost::lexical_cast<uint32_t>(i);
        lProcessorJson["status"] = "Empty";
      }
      aProcessorsArrayJson.append(lProcessorJson);
    }
  }

  /*
   █████  ███    ███  ██████  ██ ██████      ███████ ██    ██ ███████     ██    ██ ██ ███████ ██     ██
  ██   ██ ████  ████ ██      ███      ██     ██       ██  ██  ██          ██    ██ ██ ██      ██     ██
  ███████ ██ ████ ██ ██       ██  █████      ███████   ████   ███████     ██    ██ ██ █████   ██  █  ██
  ██   ██ ██  ██  ██ ██       ██      ██          ██    ██         ██      ██  ██  ██ ██      ██ ███ ██
  ██   ██ ██      ██  ██████  ██ ██████      ███████    ██    ███████       ████   ██ ███████  ███ ███
  */

  void serializeAMC13ForSystemView(Json::Value & aDaqTTCJson, const swatch::system::Crate& aCrate ){
    //swatch::core::StatusFlag amc13Status = swatch::core::kNoLimit;
    if (const swatch::dtm::DaqTTCManager * lDaqTTC = aCrate.amc13()) {

      Json::Value lNullJson(Json::nullValue);
      serializeMonitorableTree(aDaqTTCJson, lNullJson, *lDaqTTC);
      aDaqTTCJson["type"] = "daqttc";
      aDaqTTCJson["crate"] = lDaqTTC -> getCrateId();
      aDaqTTCJson["slot"] = boost::lexical_cast<uint32_t>(lDaqTTC -> getSlot());
    }
    return;
  }



  /*
  ██████  ██████   ██████   ██████      ██████  ██████  ███    ███ ██████      ██    ██ ██ ███████ ██     ██
  ██   ██ ██   ██ ██    ██ ██          ██      ██    ██ ████  ████ ██   ██     ██    ██ ██ ██      ██     ██
  ██████  ██████  ██    ██ ██          ██      ██    ██ ██ ████ ██ ██████      ██    ██ ██ █████   ██  █  ██
  ██      ██   ██ ██    ██ ██          ██      ██    ██ ██  ██  ██ ██           ██  ██  ██ ██      ██ ███ ██
  ██      ██   ██  ██████   ██████      ██████  ██████  ██      ██ ██            ████   ██ ███████  ███ ███
  */

  void serializeProcessorForComponentView(Json::Value& aJsonOutput,  const Json::Value& aSubMonitorablesArrayJson,
      const swatch::processor::Processor& aProcessor)
  {
    swatch::core::MetricReadGuard lReadGuard(aProcessor);

    serializeMonitorableTreeWithPortsStatus(aJsonOutput, aSubMonitorablesArrayJson, aProcessor);

    serializeStateMachineStatus(aJsonOutput, aProcessor.getStatus());

    aJsonOutput["type"] = "proc";
    aJsonOutput["crate"] = aProcessor.getCrateId();
    aJsonOutput["slot"] = boost::lexical_cast<uint32_t>(aProcessor.getSlot());

    const swatch::processor::ProcessorStub& lStub = aProcessor.getStub();

    Json::Value lStubTree(Json::arrayValue);

    Json::Value lPathTree (Json::objectValue);
    lPathTree["name"] = "Path";
    lPathTree["value"] = aProcessor.getPath();
    lStubTree.append(lPathTree);

    Json::Value  lHWTree(Json::objectValue);
    lHWTree["name"] = "Hardware type";
    lHWTree["value"] = lStub.hwtype;
    lStubTree.append(lHWTree);

    Json::Value  lRoleTree(Json::objectValue);
    lRoleTree["name"] = "Role";
    lRoleTree["value"] = lStub.role;
    lStubTree.append(lRoleTree);

    Json::Value  lCreatorTree(Json::objectValue);
    lCreatorTree["name"] = "Creator";
    lCreatorTree["value"] = lStub.creator;
    lStubTree.append(lCreatorTree);

    Json::Value  lURITree(Json::objectValue);
    lURITree["name"] = "URI";
    lURITree["value"] = lStub.uri;
    lStubTree.append(lURITree);

    Json::Value  lAddressTabletree(Json::objectValue);
    lAddressTabletree["name"] = "Address table";
    lAddressTabletree["value"] = lStub.addressTable;
    lStubTree.append(lAddressTabletree);

    Json::Value  lCrateTree(Json::objectValue);
    lCrateTree["name"] = "Crate";
    lCrateTree["value"] = lStub.crate;
    lStubTree.append(lCrateTree);

    Json::Value  lSlotTree(Json::objectValue);
    lSlotTree["name"] = "Slot";
    lSlotTree["value"] = boost::lexical_cast<std::string>(lStub.slot);
    lStubTree.append(lSlotTree);

    aJsonOutput["stub_info"] = lStubTree;

  }

  /*
  ██████  ██████   ██████   ██████     ██████  ██████   ██████   ██████     ██    ██ ██ ███████ ██     ██
  ██   ██ ██   ██ ██    ██ ██          ██   ██ ██   ██ ██    ██ ██          ██    ██ ██ ██      ██     ██
  ██████  ██████  ██    ██ ██          ██████  ██████  ██    ██ ██          ██    ██ ██ █████   ██  █  ██
  ██      ██   ██ ██    ██ ██          ██      ██   ██ ██    ██ ██           ██  ██  ██ ██      ██ ███ ██
  ██      ██   ██  ██████   ██████     ██      ██   ██  ██████   ██████       ████   ██ ███████  ███ ███
  */


  void serializeProcessorForProcessorsView(Json::Value& jsonOutput, const Json::Value& aSubMonitorablesArrayJson,
      const swatch::processor::Processor& aProcessor)
  {

    serializeMonitorableTree(jsonOutput, aSubMonitorablesArrayJson, aProcessor);
    jsonOutput["type"] = "proc";
    jsonOutput["crate"] = aProcessor.getCrateId();
    jsonOutput["slot"] = boost::lexical_cast<uint32_t>(aProcessor.getSlot());

    //Ports serialization, basically ports needs a different treatment, sadly
    //The components section can contain less data, so we use a different serialization

    //InputPorts is the second last  element of the components array
    Json::Value nullValue;
    Json::Value& lComponentsJson = jsonOutput["components"];
    Json::Value& lInputPortsJson = lComponentsJson[lComponentsJson.size() - 2];
    //lInputPortsJson["components"].clear();
    //InputPorts is the last  element of the components array
    Json::Value& lOutputPortsJson = lComponentsJson[lComponentsJson.size() - 1];
    //lOutputPortsJson["components"].clear();

    bool isInputPortsOpen = false;
    bool isOutputPortsOpen = false;
    //I check if input or output ports are selected
    for(Json::ValueConstIterator it = aSubMonitorablesArrayJson.begin(); it != aSubMonitorablesArrayJson.end(); it++) {
      if (it -> get("id", "").asString() == "inputPorts") isInputPortsOpen = true;
      if (it -> get("id", "").asString() == "outputPorts") isOutputPortsOpen = true;
    }

    //If it is open I serialize all the ports in the right way
    if(isInputPortsOpen) {

      Json::Value inputPortsTree(Json::objectValue);
      serializePortCollectionForPortsTable(inputPortsTree, aProcessor.getInputPorts());
      lInputPortsJson["components"] = inputPortsTree["components"];

    }

    if(isOutputPortsOpen) {

      Json::Value outputPortsTree(Json::objectValue);
      serializePortCollectionForPortsTable(outputPortsTree, aProcessor.getOutputPorts());
      lOutputPortsJson["components"] = outputPortsTree["components"];

    }

  }


  /*
   █████  ███    ███  ██████  ██ ██████       ██████  ██████  ███    ███ ██████      ██    ██ ██ ███████ ██     ██
  ██   ██ ████  ████ ██      ███      ██     ██      ██    ██ ████  ████ ██   ██     ██    ██ ██ ██      ██     ██
  ███████ ██ ████ ██ ██       ██  █████      ██      ██    ██ ██ ████ ██ ██████      ██    ██ ██ █████   ██  █  ██
  ██   ██ ██  ██  ██ ██       ██      ██     ██      ██    ██ ██  ██  ██ ██           ██  ██  ██ ██      ██ ███ ██
  ██   ██ ██      ██  ██████  ██ ██████       ██████  ██████  ██      ██ ██            ████   ██ ███████  ███ ███
  */


  void serializeAMC13ForComponentView(Json::Value& aJsonOutput, const Json::Value& aSubMonitorablesArrayJson, const swatch::dtm::DaqTTCManager& aDaqTTC)
  {
    swatch::core::MetricReadGuard lReadGuard(aDaqTTC);

    serializeMonitorableTreeWithPortsStatus(aJsonOutput, aSubMonitorablesArrayJson, aDaqTTC);

    serializeStateMachineStatus(aJsonOutput, aDaqTTC.getStatus());

    aJsonOutput["type"] = "daqttc";
    aJsonOutput["crate"] = aDaqTTC.getCrateId();
    aJsonOutput["slot"] = boost::lexical_cast<uint32_t>(aDaqTTC.getSlot());

    const swatch::dtm::DaqTTCStub& lStub = aDaqTTC.getStub();
    Json::Value lStubTree(Json::arrayValue);

    Json::Value lPathTree(Json::objectValue);
    lPathTree["name"] = "Path";
    lPathTree["value"] = aDaqTTC.getPath();
    lStubTree.append(lPathTree);

    Json::Value lFedIdTree(Json::objectValue);
    lFedIdTree["name"] = "FedId";
    lFedIdTree["value"] = boost::lexical_cast<std::string>(lStub.fedId);
    lStubTree.append(lFedIdTree);

    Json::Value lRoleTree(Json::objectValue);
    lRoleTree["name"] = "Role";
    lRoleTree["value"] = lStub.role;
    lStubTree.append(lRoleTree);

    Json::Value lCreatorTree(Json::objectValue);
    lCreatorTree["name"] = "Creator";
    lCreatorTree["value"] = lStub.creator;
    lStubTree.append(lCreatorTree);

    Json::Value lURIT1Tree(Json::objectValue);
    lURIT1Tree["name"] = "URI_T1";
    lURIT1Tree["value"] = lStub.uriT1;
    lStubTree.append(lURIT1Tree);

    Json::Value lURIT2Tree(Json::objectValue);
    lURIT2Tree["name"] = "URI_T2";
    lURIT2Tree["value"] = lStub.uriT2;
    lStubTree.append(lURIT2Tree);

    Json::Value lAddressTableT1Tree(Json::objectValue);
    lAddressTableT1Tree["name"] = "T1 address table";
    lAddressTableT1Tree["value"] = lStub.addressTableT1;
    lStubTree.append(lAddressTableT1Tree);

    Json::Value lAddressTableT2Tree(Json::objectValue);
    lAddressTableT2Tree["name"] = "T2 address table";
    lAddressTableT2Tree["value"] = lStub.addressTableT2;
    lStubTree.append(lAddressTableT2Tree);

    Json::Value lCrateTree(Json::objectValue);
    lCrateTree["name"] = "Crate";
    lCrateTree["value"] = lStub.crate;
    lStubTree.append(lCrateTree);

    Json::Value lSlotTree(Json::objectValue);
    lSlotTree["name"] = "Slot";
    lSlotTree["value"] = boost::lexical_cast<std::string>(lStub.slot);
    lStubTree.append(lSlotTree);

  //  boost::property_tree::ptree amcSlotstree;
  //  amcSlotstree.put("name", "AMC slots");
  //  amcSlotstree.put("value", swatch::core::shortVecFmt<uint32_t>(stub.amcSlots));
  //  stubtree.push_back(std::make_pair("", amcSlotstree));

    aJsonOutput["stub_info"] = lStubTree;

  }

  /*
  ███    ███ ███████ ████████ ██████  ██  ██████     ██    ██ ██ ███████ ██     ██
  ████  ████ ██         ██    ██   ██ ██ ██          ██    ██ ██ ██      ██     ██
  ██ ████ ██ █████      ██    ██████  ██ ██          ██    ██ ██ █████   ██  █  ██
  ██  ██  ██ ██         ██    ██   ██ ██ ██           ██  ██  ██ ██      ██ ███ ██
  ██      ██ ███████    ██    ██   ██ ██  ██████       ████   ██ ███████  ███ ███
  */

  void serializeMetricView(Json::Value& jsonOutput,
      const swatch::core::MetricView& aView )
  {
    auto it = aView.begin();
    auto itend = aView.end();

    for ( ; it != itend; ++it ) {

      bool plottable = false;

      if (dynamic_cast< swatch::core::Metric<bool>* >((it -> second))) plottable = true;
      else if (dynamic_cast< swatch::core::Metric<int>* >((it -> second))) plottable = true;
      else if (dynamic_cast< swatch::core::Metric<float>* >((it -> second))) plottable = true;
      else if (dynamic_cast< swatch::core::Metric<double>* >((it -> second))) plottable = true;
      else if (dynamic_cast< swatch::core::Metric<unsigned int>* >((it -> second))) plottable = true;
      else if (dynamic_cast< swatch::core::Metric<uint16_t>* >((it -> second))) plottable = true;
      else if (dynamic_cast< swatch::core::Metric<uint32_t>* >((it -> second))) plottable = true;
      else if (dynamic_cast< swatch::core::Metric<uint64_t>* >((it -> second))) plottable = true;

      swatch::core::AbstractMetric& metric = *(it->second);
      size_t pathSplitIdx = it->first.rfind(".");
      size_t pathSplitIdx2 = it->first.find(".");
      std::string metricId = it->first.substr(pathSplitIdx + 1);
      Json::Value metricJson(Json::objectValue);
      serializeMetric(metricJson, metricId, metric.getSnapshot());
      metricJson["path"] = it->first.substr(pathSplitIdx2+1, pathSplitIdx - pathSplitIdx2 - 1);
      metricJson["plottable"] = plottable;
      jsonOutput.append(metricJson);
    }
  }

  std::string monitoringStatusToString(swatch::core::monitoring::Status monitoringStatus)
  {


    switch (monitoringStatus) {
      case swatch::core::monitoring::kEnabled:
        return "Enabled";
        break;
      case swatch::core::monitoring::kDisabled:
        return "Disabled";
        break;
      case swatch::core::monitoring::kNonCritical:
        return "Non critical";
        break;
    }
    //This shouldn't happen
    return "Undefined";
  }


} //namespace swatchcellframework

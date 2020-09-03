
#ifndef __SWATCHCELL_FRAMEWORK_TOOLS_POLYMERFUNCTIONS_H__
#define __SWATCHCELL_FRAMEWORK_TOOLS_POLYMERFUNCTIONS_H__


// Forward decl of swatch objects
namespace swatch{
namespace core{
  class AbstractMetric;
  class MonitorableObject;
  class MetricSnapshot;
}
namespace processor {
  class Processor;
}
namespace system {
  class Crate;
  class Link;
}
}

#include "swatch/system/System.hpp"
#include "swatch/core/MetricView.hpp"
#include "jsoncpp/json/json.h"

#include <iostream>

namespace swatchcellframework {

  /// basic info for MonitorableObject: id, status, monitoringstatus and masking, if maskable
  template<typename MonitorableObj>
  void serializeMonitorableBasicInfo( Json::Value& jsonOutput,
		     const MonitorableObj & aObject);


  /// Metrics to JSON
  void serializeMetric(Json::Value & jsonOutput, const std::string& aMetricId,
		      const swatch::core::MetricSnapshot& aSnapshot);

/// Basic info + metrics for a MonitorableObject
  template<typename MonitorableObj>
  void serializeMonitorableCompleteInfo( Json::Value&  jsonOutput,
		       const MonitorableObj & aObject);

  /// Link serialization for System View
  void serializeLinks(Json::Value& linksJson, const std::deque<swatch::system::Link*>& links, unsigned sysIdSize);

  /// Serialization of all the processors in a crate for System view
  void serializeProcessorsForSystemView(Json::Value & processors, const swatch::system::Crate& aCrate );

  /// Processor serialization optimized for Processors and Component View
  void serializeProcessorForProcessorsView(Json::Value& jsonOutput,  const Json::Value& aSubMonitorablesArrayJson, const swatch::processor::Processor& aProcessor);
  void serializeProcessorForComponentView(Json::Value& jsonOutput,  const Json::Value& aSubMonitorablesArrayJson ,const swatch::processor::Processor& aProcessor);

  /// Port serialization optimized for Ports View
  //void serializePortsForPortsView(Json::Value& jsonOutput, const Json::Value& aSubMonitorablesArrayJson, const swatch::processor::Processor& aProcessor);

  /// Serialization of DaqTTCManager for System and Component view
  void serializeAMC13ForSystemView(Json::Value & ttcDaq, const swatch::system::Crate& aCrate );
  void serializeAMC13ForComponentView(Json::Value& jsonOutput, const Json::Value& aSubMonitorablesArrayJson, const swatch::dtm::DaqTTCManager& aDaqTTC);

  /// Serialization for MetricView
  void serializeMetricView(Json::Value& jsonOutput, const swatch::core::MetricView& aView );

  /// Serialization of a crate to JSON
  void serializeCrateForSystemView(Json::Value& jsonOutput, const swatch::system::Crate& aCrate);

  std::string monitoringStatusToString(swatch::core::monitoring::Status monitoringStatus);

  void serializeMetric(Json::Value & jsonOutput, const std::string& aMetricId, const swatch::core::MetricSnapshot& aSnapshot);

  void serializeMetricBasicInfo(Json::Value& aTree, const std::string& aMetricId, const swatch::core::MetricSnapshot& aSnapshot);

  //Recursive function which serializes the needed metrics
  //It expects to receive an array of this objects:
  // {
  //    id: id of the MonitorableObject
  //    openedSubmonitorables: array of objects as this one (recursive)
  // }
  // the function serializes all the metrics of the MonitorableObject with id "id" and then
  // it cycles over all the submonitorables. If it is in the openedSubmonitorables array then the function call itself on that object
  // otherwise only the top level status is serialized

  //aOutputJson: objectValue json which contains all the serialized info about aMonitorableObject
  //aSubMonitorablesArrayJson: arrayValue which contains the array of the submonitorables on which the recursion will be applied
  //aMonitorableObject: object of which we need the complete set of info (metrics + sub component)

  template<typename MonitorableObj>
  void serializeMonitorableTree(Json::Value& aOutputJson, const Json::Value& aSubMonitorablesArrayJson, const MonitorableObj &aMonitorableObject);

  template<typename tPortCollection>
  void serializePortCollectionTree(Json::Value& aOutputJson, const Json::Value& aSubMonitorablesArrayJson, const tPortCollection &aPortCollection);

  template<typename PortObj>
  void serializePortForPortsTable( Json::Value & jsonOutput, const PortObj & aObject);

  template<typename tPortCollection>
  void serializePortCollectionForPortsTable(Json::Value& aOutputJson, const tPortCollection &aPortCollection);

  //Serialize from actionablesnapshot if the ActionableObject or ActionableSystem is engaged and in what state machine
  template<typename tActionableSnapshot>
  void serializeStateMachineStatus (Json::Value& aOutputJson, const tActionableSnapshot &aActionableSnapshot);

} // namespace swatchcellframework

#include "swatchcell/framework/tools/polymerFunctions.hxx"

#endif /* _SWATCHCELL_FRAMEWORK_TOOLS_POLYMERFUNCTIONS_H_ */

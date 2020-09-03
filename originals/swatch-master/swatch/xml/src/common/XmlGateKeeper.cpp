#include "swatch/xml/XmlGateKeeper.hpp"


#include <exception>                    // for exception
#include <fstream>
#include <map>                          // for map

// boost headers
#include "boost/unordered/unordered_map.hpp"  // for unordered_map
#include "boost/unordered/unordered_set.hpp"  // for unordered_set

// log4cplus headers
#include <log4cplus/loggingmacros.h>

// pugixml headers
#include "pugixml.hpp"          // for xml_node, xml_attribute, etc

// XDAQ headers
#include "xdata/Serializable.h"

// SWATCH headers
#include "swatch/logger/Logger.hpp"
#include "swatch/action/MonitoringSetting.hpp"
#include "swatch/core/utilities.hpp"
#include "swatch/xml/XmlReader.hpp"
#include "swatch/xml/XmlSerializer.hpp"


namespace swatch {
namespace xml {
using namespace swatch::core;


//------------------------------------------------------------------------------------------------------------------
XmlGateKeeper::XmlGateKeeper(const std::string& aFileName, const std::string& aKey) :
  GateKeeper(aKey),
  mFileName(swatch::core::shellExpandPath(aFileName)),
  mSerializer(new XmlSerializer()),
  mLogger(swatch::logger::Logger::getInstance("swatch.xml.XmlGateKeeper"))
{
  pugi::xml_document lXmlDoc;
  XmlReader lReader;
  lXmlDoc.load(lReader.readXmlConfig(mFileName).c_str());

  readXmlDocument(lXmlDoc, aKey);
}


//------------------------------------------------------------------------------------------------------------------
XmlGateKeeper::XmlGateKeeper(const pugi::xml_document& aXmlDoc, const std::string& aRunKey) :
  GateKeeper(aRunKey),
  mFileName(""),
  mSerializer(new XmlSerializer()),
  mLogger(swatch::logger::Logger::getInstance("swatch.xml.XmlGateKeeper"))
{
  readXmlDocument(aXmlDoc, aRunKey);
}


//------------------------------------------------------------------------------------------------------------------
void XmlGateKeeper::readXmlDocument(const pugi::xml_document& aXmlDoc, const std::string& aRunKey)
{
  pugi::xml_node lKey(aXmlDoc.child("db").find_child_by_attribute("key", "id", aRunKey.c_str()));

  if (!lKey)
    XCEPT_RAISE(action::NonExistentKey,"Could not find key ' " +  aRunKey + "'");

  for (pugi::xml_node lContext(lKey.child("context")); lContext; lContext = lContext.next_sibling("context")) {
    std::pair < std::string, GateKeeper::ParametersContext_t > lParameterContext(createContext(lContext));
    add(lParameterContext.first, lParameterContext.second);

    std::pair < std::string, GateKeeper::SettingsContext_t > lSettingsContext(createSettingsContext(lContext));
    add(lSettingsContext.first, lSettingsContext.second);

    std::pair < std::string, GateKeeper::MasksContext_t> lMasksContext(createMasksContext(lContext));
    add(lMasksContext.first, lMasksContext.second);
  }

  for (pugi::xml_node lNode(lKey.child("disable")); lNode; lNode = lNode.next_sibling("disable"))
    addToDisabledSet(parseDisableNode(lNode));
}


//------------------------------------------------------------------------------------------------------------------
XmlGateKeeper::~XmlGateKeeper()
{
}


//------------------------------------------------------------------------------------------------------------------
std::pair<std::string, action::GateKeeper::Parameter_t> XmlGateKeeper::createParameter(pugi::xml_node& aParamNode)
{
  pugi::xml_attribute lParamAttribute = aParamNode.attribute("id");
  pugi::xml_attribute lCmdAttribute = aParamNode.attribute("cmd");
  pugi::xml_attribute lNamespaceAttribute = aParamNode.attribute("ns");

  if (!lParamAttribute)
    XCEPT_RAISE(AttributeMissing,"Parameter tag is missing required attribute 'id'");

  std::string lParamId(lParamAttribute.value());
  if (lParamId.empty())
    XCEPT_RAISE(InvalidAttributeValue,"Parameter tag has empty value for 'id' attribute");
  else if (lParamId.find('.') != std::string::npos)
    XCEPT_RAISE(InvalidAttributeValue,"Parameter with ID '"+lParamId+"': id attribute contains invalid character '.'");

  if (lCmdAttribute) {
    if (std::string(lCmdAttribute.value()).find('.') != std::string::npos)
      XCEPT_RAISE(InvalidAttributeValue,"Parameter with ID '"+lParamId+"': cmd attribute value '"+lCmdAttribute.value()+"' contains invalid character '.'");
    else
      lParamId = std::string(lCmdAttribute.value()) + "." + lParamId;

    if (lNamespaceAttribute) {
      if (std::string(lNamespaceAttribute.value()).find('.') != std::string::npos)
        XCEPT_RAISE(InvalidAttributeValue,"Parameter with ID '"+lParamId+"': ns attribute value '"+lNamespaceAttribute.value()+"' contains invalid character '.'");
      else
        lParamId = std::string(lNamespaceAttribute.value()) + "." + lParamId;
    }
  }
  else {
    // Namespace attribute should not exist if command attribute doesn't exist
    if (lNamespaceAttribute)
      XCEPT_RAISE(InvalidAttribute,"Parameter with ID '"+lParamId+"': namespace attribute cannot be defined without cmd attribute defined");
  }

  std::string lType(aParamNode.attribute("type").value());
  std::string lValue(aParamNode.child_value());
  xdata::Serializable* lSerializable(mSerializer->import(aParamNode));

  return std::make_pair(lParamId, GateKeeper::Parameter_t(lSerializable));
}


//------------------------------------------------------------------------------------------------------------------
std::pair<std::string, action::GateKeeper::ParametersContext_t> XmlGateKeeper::createContext(pugi::xml_node& aContext)
{
  std::string lContextId(aContext.attribute("id").value());
  boost::shared_ptr<Parameters_t> lParameterContext(new Parameters_t());

  for (pugi::xml_node lParam(aContext.child("param")); lParam; lParam = lParam.next_sibling("param")) {
    std::pair<std::string, GateKeeper::Parameter_t> lParameter;
    try {
      lParameter = createParameter(lParam);
    }
    catch (const std::exception& lExc) {
      std::string lParamId(lParam.attribute("id").value());
      std::string lMsg("Error parsing parameter '" + lParamId + "' from context '" + lContextId + "'");
      LOG4CPLUS_ERROR(mLogger, lMsg + ": " << lExc.what());
      throw;
    }

    if (lParameterContext->find(lParameter.first) != lParameterContext->end()) {
      XCEPT_RAISE(action::ParameterWithGivenIdAlreadyExistsInContext,"Parameter with ID '" + lParameter.first + "' already exists in context '" + lContextId + "' in file '" + mFileName + "'");
    }

    lParameterContext->insert(lParameter);
  }
  return std::make_pair(lContextId, lParameterContext);
}


//------------------------------------------------------------------------------------------------------------------
std::pair<std::string, action::GateKeeper::SettingsContext_t> XmlGateKeeper::createSettingsContext(
  const pugi::xml_node& aContext) const
{
  std::string lContextId(aContext.attribute("id").value());
  boost::shared_ptr<MonitoringSettings_t> lSettingsContext(new MonitoringSettings_t());

  for (pugi::xml_node lParam(aContext.child("state")); lParam;
       lParam = lParam.next_sibling("state")) {
    std::string lStateId(lParam.attribute("id").value());
    //metric + mon-obj
    for (pugi::xml_node lMetric(lParam.child("metric")); lMetric; lMetric =
           lMetric.next_sibling("metric")) {
      std::pair<std::string, GateKeeper::MonitoringSetting_t> lSetting(
        createMonitoringSetting(lMetric));

      std::string lStatePath = lStateId + "." + lSetting.first;
      lSetting.first = lStatePath;
      if (lSettingsContext->find(lStatePath) != lSettingsContext->end()) {
        XCEPT_RAISE(action::MSettingWithGivenIdAlreadyExistsInContext,
          "Monitoring setting with ID '" + lStatePath
          + "' already exists in context '" + lContextId + "' in file '"
          + mFileName + "'");
      }

      lSettingsContext->insert(lSetting);

    }

    for (pugi::xml_node lMonObj(lParam.child("mon-obj")); lMonObj; lMonObj =
           lMonObj.next_sibling("mon-obj")) {
      std::pair<std::string, GateKeeper::MonitoringSetting_t> lSetting(
        createMonitoringSetting(lMonObj));

      std::string lStatePath = lStateId + "." + lSetting.first;
      lSetting.first = lStatePath;
      if (lSettingsContext->find(lStatePath) != lSettingsContext->end()) {
        XCEPT_RAISE(action::MSettingWithGivenIdAlreadyExistsInContext,
          "Monitoring setting with ID '" + lStatePath
          + "' already exists in context '" + lContextId + "' in file '"
          + mFileName + "'");
      }

      lSettingsContext->insert(lSetting);
    }
  }
  return std::make_pair(lContextId, lSettingsContext);
}


//------------------------------------------------------------------------------------------------------------------
std::pair<std::string, action::GateKeeper::MonitoringSetting_t> XmlGateKeeper::createMonitoringSetting(const pugi::xml_node& aParam) const
{
  std::string lId(aParam.attribute("id").value());
  std::string lStatus(aParam.attribute("status").value());

  monitoring::Status lMonStatus(monitoring::kStringToStatus.at(lStatus)); // add check
  GateKeeper::MonitoringSetting_t lMonSetting(new action::MonitoringSetting(lId, lMonStatus));
  return std::make_pair(lId, lMonSetting);
}

//------------------------------------------------------------------------------------------------------------------
std::pair<std::string, action::GateKeeper::MasksContext_t> XmlGateKeeper::createMasksContext(const pugi::xml_node& aContext) const
{
  std::string lContextId(aContext.attribute("id").value());
  boost::shared_ptr<Masks_t> lMaskContext(new Masks_t());

  for (pugi::xml_node lParam(aContext.child("mask")); lParam; lParam = lParam.next_sibling("mask")) {

    std::string lId = lParam.attribute("id").value();

    if (lMaskContext->find(lId) != lMaskContext->end())
      XCEPT_RAISE(action::ParameterWithGivenIdAlreadyExistsInContext,"Mask with ID '" + lId + "' already exists in context '" + lContextId + "' in file '" + mFileName + "'");

    lMaskContext->insert(lId);
  }
  return std::make_pair(lContextId, lMaskContext);
}


//------------------------------------------------------------------------------------------------------------------
std::string XmlGateKeeper::parseDisableNode(const pugi::xml_node& aNode) const
{
  std::string lId(aNode.attribute("id").value());
  return lId;
}



} // namespace core
} // namespace swatch

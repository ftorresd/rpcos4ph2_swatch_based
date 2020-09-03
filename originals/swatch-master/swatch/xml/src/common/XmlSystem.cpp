#include "swatch/xml/XmlSystem.hpp"


// external headers
#include "pugixml.hpp"

#include <boost/foreach.hpp>
#include <boost/range/algorithm/copy.hpp>
#include "boost/lexical_cast.hpp"

#include <log4cplus/loggingmacros.h>
#include "log4cplus/logger.h"           // for Logger

// SWATCH headers
#include "swatch/core/toolbox/IdSliceParser.hpp"
#include "swatch/core/utilities.hpp"
#include "swatch/logger/Logger.hpp"
#include "swatch/dtm/DaqTTCStub.hpp"    // for DaqTTCStub
#include "swatch/processor/ProcessorStub.hpp"  // for ProcessorPortStub, etc
#include "swatch/system/CrateStub.hpp"  // for CrateStub
#include "swatch/system/LinkStub.hpp"   // for LinkStub
#include "swatch/system/SystemStub.hpp"  // for SystemStub, etc
// only for now
#include "swatch/xml/XmlReader.hpp"


namespace swatch {
namespace xml {
namespace system {

const char* kMainTag = "system";

swatch::system::SystemStub xmlFileToSystemStub(const std::string& aFileName)
{
  log4cplus::Logger lLogger(swatch::logger::Logger::getInstance("swatch.xml.system.xmlFileToSystemStub"));
  LOG4CPLUS_DEBUG(lLogger, "Reading sytem XML file '" + aFileName + "'");

  pugi::xml_document lXmlDoc;
  detail::loadFromFile(aFileName, lXmlDoc);

  return xmlToSystemStub(lXmlDoc);
}

swatch::system::SystemStub xmlToSystemStub(const pugi::xml_document& aXmlDoc)
{
  using namespace detail;
  log4cplus::Logger lLogger(swatch::logger::Logger::getInstance("swatch.xml.system.xmlToSystemStub"));
  std::string lErrorMsg("");
  if (!detail::validateSystemXml(aXmlDoc, lErrorMsg)) {
    XCEPT_RAISE(InvalidSystemDescription,
      "The given XML is not a valid description of swatch::system::System: \n" + lErrorMsg);
  }

  pugi::xml_node lSystem = aXmlDoc.child(kMainTag);
  pugi::xml_attribute lId = lSystem.attribute("id");

  swatch::system::SystemStub lStub(lId.value());
  lStub.loggerName = lStub.id;
  lStub.creator = lSystem.child_value("creator");
  if (pugi::xml_node lAliasNode = lSystem.child("alias"))
    lStub.alias = lAliasNode.child_value();

  BOOST_FOREACH (pugi::xml_node lNode, lSystem.children()) {
    std::string lTag(lNode.name());
    if (lTag == "creator" || lTag == "connected-fed" || lTag == "exclude") {
      // <creator> is already done
      // need to read everything else before <connected-fed>
      // need to read all processors and daqttc-mgrs before <exclude>
      continue;
    }
    BOOST_FOREACH(pugi::xml_node lSubNode, lNode.children()) {
      std::string lSubTag(lSubNode.name());
      if (lSubTag == "crate") {
        lStub.crates.emplace_back(xmlToCrateStub(lSubNode));
      }
      else if (lSubTag == "processor") {
        swatch::processor::ProcessorStub lPStub(xmlToProcessorStub(lSubNode));
        lPStub.loggerName = lStub.id + "." + lPStub.id;
        lStub.processors.emplace_back(lPStub);
      }
      else if (lSubTag == "daqttc-mgr") {
        swatch::dtm::DaqTTCStub lDaqStub(xmlToDaqTTCStub(lSubNode));
        lDaqStub.loggerName = lStub.id + "." + lDaqStub.id;
        lStub.daqttcs.emplace_back(lDaqStub);
      }
      else if (lSubTag == "link") {
        swatch::system::SystemStub::Links_t lLinks = xmlToLinkStubs(lSubNode);
        lStub.links.reserve(lStub.links.size() + lLinks.size());
        lStub.links.insert(lStub.links.end(), lLinks.begin(), lLinks.end());
      }
    } // end for each subnode
  } // end for each node

  // second loop just for the excluded processors (after processor parsing, before connected-FEDs
  pugi::xml_node lExcludedBoards = lSystem.child("excluded-boards");
  if (lExcludedBoards) {
    std::vector<std::string> lKnownIds;
    BOOST_FOREACH(swatch::processor::ProcessorStub lProc, lStub.processors) {
      lKnownIds.push_back(lProc.id);
    }
    BOOST_FOREACH(swatch::dtm::DaqTTCStub lProc, lStub.daqttcs) {
      lKnownIds.push_back(lProc.id);
    }
    BOOST_FOREACH(pugi::xml_node lNode, lExcludedBoards.children()) {
      lStub.excludedBoards.emplace_back(xmlToExcludedBoard(lNode, lKnownIds));
    }
  }

  // third loop just for the connected FEDs as we need to exclude links to excluded boards
  pugi::xml_node lConnectedFeds = lSystem.child("connected-feds");
  if (lConnectedFeds) {
    BOOST_FOREACH(pugi::xml_node lNode, lConnectedFeds.children()) {
      xmlToConnectedFeds(lNode, lStub.connectedFEDs, lStub.excludedBoards);
    }
  }

  // final checks
  checkForConnectionMapDuplicates(lStub.connectedFEDs);

  return lStub;
}

namespace detail {
swatch::system::CrateStub xmlToCrateStub(const pugi::xml_node& aNode)
{
  std::string lErrorMsg("");
  if (!validateCrateXml(aNode, lErrorMsg))
    XCEPT_RAISE(InvalidSystemDescription,"Invalid <crate> tag found: \n" + lErrorMsg);
  log4cplus::Logger lLogger(swatch::logger::Logger::getInstance("swatch.xml.system.xmlToCrateStub"));
  std::string lId = aNode.attribute("id").value();
  std::string lDescription = aNode.child_value("description");
  std::string lLocation = aNode.child_value("location");

  swatch::system::CrateStub lStub(lId);
  if (aNode.child("alias"))
    lStub.alias = aNode.child_value("alias");
  lStub.description = lDescription;
  lStub.location = lLocation;

  return lStub;
}

swatch::processor::ProcessorStub xmlToProcessorStub(const pugi::xml_node& aNode)
{
  std::string lErrorMsg("");
  if (!validateProcessorXml(aNode, lErrorMsg))
    XCEPT_RAISE(InvalidSystemDescription,"Invalid <processor> tag found: \n" + lErrorMsg);
  log4cplus::Logger lLogger(swatch::logger::Logger::getInstance("swatch.xml.system.xmlToProcessorStub"));
  pugi::xml_attribute lId = aNode.attribute("id");
  // one of each
  std::string lCreator = aNode.child_value("creator");
  std::string lHwType = aNode.child_value("hw-type");
  std::string lRole = aNode.child_value("role");
  std::string lCrate = aNode.child_value("crate");
  std::string lSlot = aNode.child_value("slot");
  // ==1 for processor, ==2 for DaqTTC-mgr
  std::string lUri = aNode.child_value("uri");
  std::string lAddrTable = aNode.child_value("address-table");
  // one or more
  swatch::processor::ProcessorStub lStub(lId.value());
  if (aNode.child("alias"))
    lStub.alias = aNode.child_value("alias");
  for (pugi::xml_named_node_iterator lTxPort = aNode.children("tx-port").begin();
       lTxPort != aNode.children("tx-port").end(); ++lTxPort) {
    std::string lName(lTxPort->attribute("name").value());
    std::string lAlias(lTxPort->attribute("alias").value());
    std::string lPid(lTxPort->attribute("pid").value());
    pushBackPortStubs(lStub.txPorts, lName, lAlias, lPid);
  }
  for (pugi::xml_named_node_iterator lRxPort = aNode.children("rx-port").begin();
       lRxPort != aNode.children("rx-port").end(); ++lRxPort) {
    std::string lName(lRxPort->attribute("name").value());
    std::string lAlias(lRxPort->attribute("alias").value());
    std::string lPid(lRxPort->attribute("pid").value());
    pushBackPortStubs(lStub.rxPorts, lName, lAlias, lPid);
  }
  lStub.creator = lCreator;
  lStub.hwtype = lHwType;
  lStub.role = lRole;
  lStub.crate = lCrate;
  try {
    lStub.slot = boost::lexical_cast<uint32_t>(lSlot);
  }
  catch (boost::bad_lexical_cast& lExcpt) {
    std::stringstream lMsg;
    lMsg << "Could not parse <slot> for <processor id=\"" << lId << "\">. Expected uint32, got '" << lSlot << "'. "
         << lExcpt.what();
    LOG4CPLUS_ERROR(lLogger, lMsg.str());
    XCEPT_RAISE(InvalidSystemDescription,lMsg.str());
  }
  lStub.loggerName = lStub.id;

  lStub.uri = lUri;
  lStub.addressTable = lAddrTable;

  return lStub;
}

swatch::dtm::DaqTTCStub xmlToDaqTTCStub(const pugi::xml_node& aNode)
{
  std::string lErrorMsg("");
  if (!validateDAQTTCXml(aNode, lErrorMsg))
    XCEPT_RAISE(InvalidSystemDescription,"Invalid <daqttc-mgr> tag found: \n" + lErrorMsg);
  log4cplus::Logger lLogger(swatch::logger::Logger::getInstance("swatch.xml.system.xmlToDaqTTCStub"));
  std::string lId = aNode.attribute("id").value();
  std::string lCreator = aNode.child_value("creator");
  std::string lRole = aNode.child_value("role");
  std::string lCrate = aNode.child_value("crate");
  std::string lSlot = aNode.child_value("slot");
  std::string lFedId = aNode.child_value("fed-id");

  swatch::dtm::DaqTTCStub lStub(lId);
  if (aNode.child("alias"))
    lStub.alias = aNode.child_value("alias");
  lStub.creator = lCreator;
  lStub.role = lRole;
  lStub.crate = lCrate;
  lStub.slot = boost::lexical_cast<uint32_t>(lSlot);
  lStub.loggerName = lStub.id;
  lStub.fedId = boost::lexical_cast<uint16_t>(lFedId);
  lStub.loggerName = lStub.id;

  BOOST_FOREACH(pugi::xml_node lNode, aNode.children("uri")) {
    if (strcmp(lNode.attribute("id").value(), "t1") == 0) {
      lStub.uriT1 = lNode.child_value();
    }
    if (strcmp(lNode.attribute("id").value(), "t2") == 0) {
      lStub.uriT2 = lNode.child_value();
    }
  }
  BOOST_FOREACH(pugi::xml_node lNode, aNode.children("address-table")) {
    if (strcmp(lNode.attribute("id").value(), "t1") == 0) {
      lStub.addressTableT1 = lNode.child_value();
    }
    if (strcmp(lNode.attribute("id").value(), "t2") == 0) {
      lStub.addressTableT2 = lNode.child_value();
    }
  }

  return lStub;
}

std::vector<swatch::system::LinkStub> xmlToLinkStubs(const pugi::xml_node& aNode)
{
  std::string lErrorMsg("");
  if (!validateLinkXml(aNode, lErrorMsg))
    XCEPT_RAISE(InvalidSystemDescription,"Invalid <link> tag found: \n" + lErrorMsg);
  log4cplus::Logger lLogger(swatch::logger::Logger::getInstance("swatch.xml.system.xmlToLinkStubs"));
  std::string lId = aNode.attribute("id").value();

  std::string lAliasStr;
  if (aNode.child("alias"))
    lAliasStr = aNode.child_value("alias");

  std::string lSource = aNode.child_value("from");
  std::string lDestination = aNode.child_value("to");
  std::string lSrcPortsStr = aNode.child_value("tx-port");
  std::string lDstPortsStr = aNode.child_value("rx-port");

  std::vector<std::string> lIDs = core::toolbox::IdSliceParser::parse(lId);
  std::vector<std::string> lAliases = (lAliasStr.empty() ? std::vector<std::string>(lIDs.size(), "") : core::toolbox::IdSliceParser::parse(lAliasStr));
  std::vector<std::string> lSrcPorts = core::toolbox::IdSliceParser::parse(lSrcPortsStr);
  std::vector<std::string> lDstPorts = core::toolbox::IdSliceParser::parse(lDstPortsStr);

  if (lAliases.size() != lIDs.size()) {
    std::ostringstream lMsg;
    lMsg << "Link Stub creation error: Link ids '" << lId << "' do not have the same multiplicity as aliases '"
         << lAliasStr << "'. N(ids)=" << lIDs.size() << " N(aliases)=" << lAliases.size();
    LOG4CPLUS_ERROR(lLogger, lMsg.str());
    XCEPT_RAISE(InvalidSystemDescription,lMsg.str());
  }

  if (lSrcPorts.size() != lIDs.size()) {
    std::ostringstream lMsg;
    lMsg << "Link Stub creation error: Link ids '" << lId << "' do not have the same multiplicity as source ids '"
         << lSource << "', '" << lSrcPortsStr << "'. N(ids)=" << lIDs.size() << " N(srcs)=" << lSrcPorts.size();
    LOG4CPLUS_ERROR(lLogger, lMsg.str());
    XCEPT_RAISE(InvalidSystemDescription,lMsg.str());
  }

  if (lDstPorts.size() != lIDs.size()) {
    std::ostringstream lMsg;
    lMsg << "Link Stub creation error: Link ids '" << lId << "' do not have the same multiplicity as destination ids '"
         << lDestination << "', '" << lDstPortsStr << "'. N(ids)=" << lIDs.size() << " N(dsts)=" << lDstPorts.size();
    LOG4CPLUS_ERROR(lLogger, lMsg.str());
    XCEPT_RAISE(InvalidSystemDescription,lMsg.str());
  }

  std::vector<swatch::system::LinkStub> lStubs;
  for (size_t i = 0; i < lIDs.size(); ++i) {
    swatch::system::LinkStub lStub(lIDs.at(i));
    lStub.alias = lAliases.at(i);
    lStub.srcProcessor = lSource;
    lStub.dstProcessor = lDestination;
    lStub.srcPort = lSrcPorts.at(i);
    lStub.dstPort = lDstPorts.at(i);
    lStubs.push_back(lStub);
  }

  return lStubs;
}

void xmlToConnectedFeds(const pugi::xml_node& aNode, swatch::system::SystemStub::FEDInputPortsMap_t& aFedMap,
                        const std::vector<std::string>& aExcludedBoards)
{
  std::string lErrorMsg("");
  if (!validateConnectedFedXml(aNode, lErrorMsg))
    XCEPT_RAISE(InvalidSystemDescription,"Invalid <connected-fed> tag found: \n" + lErrorMsg);
  log4cplus::Logger lLogger(swatch::logger::Logger::getInstance("swatch.xml.system.xmlToConnectedFeds"));

  uint32_t lFedID;
  std::string lId(aNode.attribute("id").value());
  try {
    lFedID = boost::lexical_cast<uint32_t>(lId);
  }
  catch (boost::bad_lexical_cast& lExcpt) {
    std::stringstream lMsg;
    lMsg << "Could not parse id attribute for <connected-fed>. Expected uint32, got '" << lId << "'. " << lExcpt.what();
    LOG4CPLUS_ERROR(lLogger, lMsg.str());
    XCEPT_RAISE(InvalidSystemDescription,lMsg.str());
  }
  if (aFedMap.count(lFedID)) {
    std::ostringstream lMsg;
    lMsg << "FED " << lFedID << " is referenced multiple times in 'CONNECTED FEDS'";
    LOG4CPLUS_ERROR(lLogger, lMsg.str());
    XCEPT_RAISE(InvalidSystemDescription,lMsg.str());
  }

  std::set<std::string> lRxPorts;
  // Map to track duplicates
  std::map<std::string, uint32_t> lDuplicates;

  BOOST_FOREACH(pugi::xml_node lPort, aNode.children("port")) {
    std::string lPortId = lPort.attribute("id").value();
    std::vector<std::string> lPortIds = core::toolbox::IdSliceParser::parse(lPortId);
    BOOST_FOREACH( std::string lPortPath, lPortIds) {

      const std::string lProcId = lPortPath.substr(0, lPortPath.find('.'));
      if (std::count(aExcludedBoards.begin(), aExcludedBoards.end(), lProcId) > 0)
        continue;

      // Continue if insertion succeeds i.e. no duplicates
      if (lRxPorts.insert(lPortPath).second)
        continue;

      // Otherwise Increase duplicate count for id
      lDuplicates[lPortPath]++;
    }
  }

  if (!lDuplicates.empty()) {
    std::ostringstream lMsg;
    lMsg << "Duplicates found while parsing CONNECTED FEDS map, FED " << lFedID << ". ";
    BOOST_FOREACH( auto count, lDuplicates ) {
      lMsg << "'" << count.first << "': " << count.second << " ";
    }
    LOG4CPLUS_ERROR(lLogger, lMsg.str());
    XCEPT_RAISE(InvalidSystemDescription,lMsg.str());
  }

  // Fill the map
  boost::copy(lRxPorts, std::back_inserter(aFedMap[lFedID]));
}

std::string xmlToExcludedBoard(const pugi::xml_node& aNode, const std::vector<std::string>& aKnownIds)
{
  log4cplus::Logger lLogger(swatch::logger::Logger::getInstance("swatch.xml.system.xmlToExcludedBoards"));

  if (!hasAttr(aNode, "id")) {
    std::ostringstream lMsg;
    lMsg << "<" << aNode.name() << "> tag has no 'id' attribute!";
    LOG4CPLUS_ERROR(lLogger, lMsg.str());
    XCEPT_RAISE(InvalidSystemDescription,lMsg.str());
  }

  std::string lId = aNode.attribute("id").value();
  if (std::count(aKnownIds.begin(), aKnownIds.end(), lId) == 0) {
    std::ostringstream lMsg;
    lMsg << "EXCLUDED BOARD ID '" << lId << "' does not match any of the system's processors/DaqTTC managers";
    LOG4CPLUS_ERROR(lLogger, lMsg.str());
    XCEPT_RAISE(InvalidSystemDescription,lMsg.str());
  }

  return lId;
}

bool validateSystemXml(const pugi::xml_document& aXmlDoc, std::string& aErrorMsg)
{
  const std::vector<std::string> lSubTags = { "creator", "alias", "crates", "processors", "daqttc-mgrs", "links",
                                              "connected-feds", "excluded-boards"
                                            };
  const std::vector<bool> lIsOptional = { 0, 1, 1, 1, 1, 1, 1, 1 };
  const std::vector<size_t> lSubTagMults = { 1, 1, 1, 1, 1, 1, 1, 1 };
  const std::vector<size_t> lSubTagMultsAtLeast = { 0, 0, 0, 0, 0, 0, 0, 0 };

  std::ostringstream lErrorMsg;
  pugi::xml_node lSystem = aXmlDoc.child(kMainTag);

// only one <system> tag per config
  bool lResult = std::distance(aXmlDoc.children(kMainTag).begin(), aXmlDoc.children(kMainTag).end()) == 1;
  if (!lResult) {
    lErrorMsg << "More than one <" + std::string(kMainTag) + "> tag detected.\n";
  }
  if (std::distance(aXmlDoc.children().begin(), aXmlDoc.children().end()) > 1) {
    lResult = false;
    lErrorMsg << "Other tags than <" + std::string(kMainTag) + "> tag detected in XML document.\n";
  }

  if (!hasAttr(lSystem, "id")) {
    lResult = false;
    lErrorMsg << "<" + std::string(kMainTag) + "> tag has no 'id' attribute!\n";
  }

  std::string lTmpMsg("");
  if (!detail::checkChildMultiplicity(lSystem, lSubTags, lIsOptional, lSubTagMults, lSubTagMultsAtLeast, lTmpMsg)) {
    lResult = false;
    lErrorMsg << lTmpMsg;
  }
  lTmpMsg = "";
  if (!detail::checkForUnknownSubTags(lSystem, lSubTags, lTmpMsg)) {
    lResult = false;
    lErrorMsg << lTmpMsg;
  }

  BOOST_FOREACH(pugi::xml_node lChild, lSystem.children()) {
    std::string lName = lChild.name();
    if ((lName == "creator") or (lName == "alias")) {
      lTmpMsg = "";
      if (!validateChildXml(lChild, lTmpMsg)) {
        lResult = false;
        lErrorMsg << lTmpMsg;
      }
    }
    else {
      // all tags other of <creator> have 0 or more children
      std::vector<std::string> lKnownGrandChildTags;
      if (lName == "excluded-boards") {
        lKnownGrandChildTags.push_back("exclude");
      }
      else {
        // just remove the 's' to the end
        std::string lExpectedChildTag = lName.substr(0, lName.size() - 1);
        lKnownGrandChildTags.push_back(lExpectedChildTag);
      }
      lTmpMsg = "";
      if (!detail::checkForUnknownSubTags(lChild, lKnownGrandChildTags, lTmpMsg)) {
        lResult = false;
        lErrorMsg << lTmpMsg;
      }

      BOOST_FOREACH(pugi::xml_node lGrandChild, lChild.children()) {
        std::string lGName = lGrandChild.name();
        lTmpMsg = "";
        if (!validateChildXml(lGrandChild, lTmpMsg)) {
          lResult = false;
          lErrorMsg << lTmpMsg;
        } // if
      } // for each grandchild
    } // else
  } // for each child

  if (!lResult) {
    log4cplus::Logger lLogger(swatch::logger::Logger::getInstance("swatch.xml.system.validateSystemXml"));
    LOG4CPLUS_ERROR(lLogger, lErrorMsg.str());
    aErrorMsg += lErrorMsg.str();
  }
  return lResult;
}

bool validateChildXml(const pugi::xml_node& aChildNode, std::string& aErrorMsg)
{
  std::string lName = aChildNode.name();
  if ((lName == "creator") or (lName == "alias")) {
    return checkNodeNotEmpty(aChildNode, aErrorMsg);
  }
  else if (lName == "crate") {
    return validateCrateXml(aChildNode, aErrorMsg);
  }
  else if (lName == "processor") {
    return validateProcessorXml(aChildNode, aErrorMsg);
  }
  else if (lName == "daqttc-mgr") {
    return validateDAQTTCXml(aChildNode, aErrorMsg);
  }
  else if (lName == "link") {
    return validateLinkXml(aChildNode, aErrorMsg);
  }
  else if (lName == "connected-fed") {
    return validateConnectedFedXml(aChildNode, aErrorMsg);
  }
  else {
    return true;
  }
  return true;
}

bool checkNodeNotEmpty(const pugi::xml_node& aNode, std::string& aErrorMsg)
{
  bool lResult = true;
  std::stringstream lErrorStream;

  if (strcmp(aNode.child_value(), "") == 0) {
    lErrorStream << "<" << aNode.name() << "> tag is empty!\n";
    lResult = false;
  }

  if (!lResult) {
    log4cplus::Logger lLogger(swatch::logger::Logger::getInstance("swatch.xml.system.checkXmlNodeNonEmpty"));
    LOG4CPLUS_ERROR(lLogger, lErrorStream.str());
    aErrorMsg += lErrorStream.str();
  }
  return lResult;
}

bool validateCrateXml(const pugi::xml_node& aNode, std::string& aErrorMsg)
{
  const std::vector<std::string> lSubTags = { "alias", "location", "description" };
  const std::vector<bool> lIsOptional = { 1, 0, 0 };
  const std::vector<size_t> lSubTagMults = { 1, 1, 1 };
  const std::vector<size_t> lSubTagMultsAtLeast = { 0, 0, 0 };

  bool lResult = true;
  std::stringstream lErrorStream;

  if (!hasAttr(aNode, "id")) {
    lErrorStream << "<" << aNode.name() << "> tag has no 'id' attribute!\n";
    lResult = false;
  }

  std::string lTmpMsg("");
  if (!checkChildMultiplicity(aNode, lSubTags, lIsOptional, lSubTagMults, lSubTagMultsAtLeast, lTmpMsg)) {
    lResult = false;
    lErrorStream << lTmpMsg;
  }
  lTmpMsg = "";
  if (!checkForUnknownSubTags(aNode, lSubTags, lTmpMsg)) {
    lResult = false;
    lErrorStream << lTmpMsg;
  }

  if (!lResult) {
    log4cplus::Logger lLogger(swatch::logger::Logger::getInstance("swatch.xml.system.validateCrateXml"));
    LOG4CPLUS_ERROR(lLogger, lErrorStream.str());
    aErrorMsg += lErrorStream.str();
  }
  return lResult;
}

bool validateProcessorXml(const pugi::xml_node& aNode, std::string& aErrorMsg)
{
  const std::vector<std::string> lSubTags = {  "alias", "creator", "hw-type", "role", "uri", "address-table", "crate", "slot",
                                              "rx-port", "tx-port"
                                            };
  const std::vector<bool> lIsOptional = { 1, 0, 0, 0, 0, 0, 0, 0, 1, 1 };
  const std::vector<size_t> lSubTagMults = { 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
  const std::vector<size_t> lSubTagMultsAtLeast = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

  bool lResult = true;
  std::stringstream lErrorStream;

  if (!hasAttr(aNode, "id")) {
    lErrorStream << "<" << aNode.name() << "> tag has no 'id' attribute!\n";
    lResult = false;
  }
  const std::string lId(aNode.attribute("id").value());

  std::string lTmpMsg("");
  if (!checkChildMultiplicity(aNode, lSubTags, lIsOptional, lSubTagMults, lSubTagMultsAtLeast, lTmpMsg)) {
    lResult = false;
    lErrorStream << lTmpMsg;
  }
  lTmpMsg = "";
  if (!checkForUnknownSubTags(aNode, lSubTags, lTmpMsg)) {
    lResult = false;
    lErrorStream << lTmpMsg;
  }

  // if optional *-port tags exist, check them for name and PID
  BOOST_FOREACH(pugi::xml_node lTag, aNode.children("rx-port")) {
    if (!hasAttr(lTag, "name")) {
      lErrorStream << "<processor id=\"" << lId << "\"> has an <rx-port> tag without 'name' attribute.\n";
      lResult = false;
    }
    if (!hasAttr(lTag, "pid")) {
      lErrorStream << "<processor id=\"" << lId << "\"> has an <rx-port> tag without 'pid' attribute.\n";
      lResult = false;
    }

    try {
      std::vector<swatch::processor::ProcessorPortStub> lPortStubs;
      pushBackPortStubs(lPortStubs, lTag.attribute("name").value(), lTag.attribute("alias").value(), lTag.attribute("pid").value());
    }
    catch (const std::exception& lException) {
      lResult = false;
      lErrorStream << "<processor id=\"" << lId << "\">, problem in rx-port '" << lTag.attribute("name").value() << "': " << lException.what() << "\n";
    }
  }

  BOOST_FOREACH(pugi::xml_node lTag, aNode.children("tx-port")) {
    if (!hasAttr(lTag, "name")) {
      lErrorStream << "<processor id=\"" << lId << "\"> has a <tx-port> tag without 'name' attribute.\n";
      lResult = false;
    }
    if (!hasAttr(lTag, "pid")) {
      lErrorStream << "<processor id=\"" << lId << "\"> has a <tx-port> tag without 'pid' attribute.\n";
      lResult = false;
    }

    try {
      std::vector<swatch::processor::ProcessorPortStub> lPortStubs;
      pushBackPortStubs(lPortStubs, lTag.attribute("name").value(), lTag.attribute("alias").value(), lTag.attribute("pid").value());
    }
    catch (const std::exception& lException) {
      lResult = false;
      lErrorStream << "<processor id=\"" << lId << "\">, problem in tx-port '" << lTag.attribute("name").value() << "': " << lException.what() << "\n";
    }
  }

  const std::string lSlot = aNode.child_value("slot");
  try {
    boost::lexical_cast<uint16_t>(lSlot);
  }
  catch (boost::bad_lexical_cast& lExcpt) {
    lErrorStream << "<processor id=\"" << lId << "\"> : Could not parse <slot> value. Expected uint16, got '" << lSlot
                 << "'. " << lExcpt.what() << "\n";
    lResult = false;
  }

  if (!lResult) {
    log4cplus::Logger lLogger(swatch::logger::Logger::getInstance("swatch.xml.system.validateProcessorXml"));
    LOG4CPLUS_ERROR(lLogger, lErrorStream.str());
    aErrorMsg += lErrorStream.str();
  }
  return lResult;
}

bool validateDAQTTCXml(const pugi::xml_node& aNode, std::string& aErrorMsg)
{
  const std::vector<std::string> lSubTags = { "alias", "creator", "role", "uri", "address-table", "crate", "slot", "fed-id" };
  const std::vector<bool> lIsOptional = { 1, 0, 0, 0, 0, 0, 0, 0 };
  const std::vector<size_t> lSubTagMults = { 1, 1, 1, 2, 2, 1, 1, 1 };
  const std::vector<size_t> lSubTagMultsAtLeast = { 0, 0, 0, 0, 0, 0, 0, 0 };

  bool lResult = true;
  std::stringstream lErrorStream;

  // default checks
  if (!hasAttr(aNode, "id")) {
    lErrorStream << "<" << aNode.name() << "> tag has no 'id' attribute!\n";
    lResult = false;
  }

  std::string lTmpMsg("");
  if (!checkChildMultiplicity(aNode, lSubTags, lIsOptional, lSubTagMults, lSubTagMultsAtLeast, lTmpMsg)) {
    lResult = false;
    lErrorStream << lTmpMsg;
  }
  lTmpMsg = "";
  if (!checkForUnknownSubTags(aNode, lSubTags, lTmpMsg)) {
    lResult = false;
    lErrorStream << lTmpMsg;
  }

  // custom checks
  std::string lId = aNode.attribute("id").value();
  const std::string lFedId = aNode.child_value("fed-id");
  const std::string lSlot = aNode.child_value("slot");
  try {
    boost::lexical_cast<uint16_t>(lFedId);
  }
  catch (boost::bad_lexical_cast& lExcpt) {
    lErrorStream << "<daqttc-mgr id=\"" << lId << "\"> : Could not parse <fed-id> value. Expected uint16, got '"
                 << lFedId << "'. " << lExcpt.what() << "\n";
    lResult = false;
  }
  try {
    boost::lexical_cast<uint16_t>(lSlot);
  }
  catch (boost::bad_lexical_cast& lExcpt) {
    lErrorStream << "<daqttc-mgr id=\"" << lId << "\"> : Could not parse <slot> value. Expected uint16, got '" << lSlot
                 << "'. " << lExcpt.what() << "\n";
    lResult = false;
  }

  BOOST_FOREACH(pugi::xml_node lNode, aNode.children("uri")) {
    if (!hasAttr(lNode, "id")) {
      lErrorStream << "<" << aNode.name() << " id=\"" << lId << "\"> tag has invalid 'uri' attribute: ";
      lErrorStream << "<" << lNode.name() << "> tag has no 'id' attribute!\n";
      lResult = false;
    }
    std::string lUriId = lNode.attribute("id").value();
    if (lUriId != "t1" && lUriId != "t2") {
      lResult = false;
      lErrorStream << "<" << aNode.name() << " id=\"" << lId << "\"> tag has invalid 'uri' attribute: ";
      lErrorStream << "<uri> attribute 'id' can only be 't1' or 't2' but found \"" << lUriId << "\"!\n";
    }
  }
  BOOST_FOREACH(pugi::xml_node lNode, aNode.children("address-table")) {
    if (!hasAttr(lNode, "id")) {
      lErrorStream << "<" << aNode.name() << " id=\"" << lId << "\"> tag has invalid 'address-table' attribute: ";
      lErrorStream << "<address-table> tag has no 'id' attribute!\n";
      lResult = false;
    }
    std::string lAddrId = lNode.attribute("id").value();
    if (lAddrId != "t1" && lAddrId != "t2") {
      lResult = false;
      lErrorStream << "<" << aNode.name() << " id=\"" << lId << "\"> tag has invalid 'address-table' attribute: ";
      lErrorStream << "<address-table> attribute 'id' can only be 't1' or 't2' but found \"" << lAddrId << "\"!";
    }
  }

  if (!lResult) {
    log4cplus::Logger lLogger(swatch::logger::Logger::getInstance("swatch.xml.system.validateDAQTTCXml"));
    LOG4CPLUS_ERROR(lLogger, lErrorStream.str());
    aErrorMsg += lErrorStream.str();
  }
  return lResult;
}

bool validateLinkXml(const pugi::xml_node& aNode, std::string& aErrorMsg)
{
  const std::vector<std::string> lSubTags = { "alias", "from", "to", "tx-port", "rx-port" };
  const std::vector<bool> lIsOptional = { 1, 0, 0, 0, 0 };
  const std::vector<size_t> lSubTagMults = { 1, 1, 1, 1, 1 };
  const std::vector<size_t> lSubTagMultsAtLeast = { 0, 0, 0, 0, 0 };
  bool lResult = true;
  std::stringstream lErrorStream;

  if (!hasAttr(aNode, "id")) {
    lErrorStream << "<" << aNode.name() << "> tag has no 'id' attribute!\n";
    lResult = false;
  }

  std::string lTmpMsg("");
  if (!checkChildMultiplicity(aNode, lSubTags, lIsOptional, lSubTagMults, lSubTagMultsAtLeast, lTmpMsg)) {
    lResult = false;
    lErrorStream << lTmpMsg;
  }
  lTmpMsg = "";
  if (!checkForUnknownSubTags(aNode, lSubTags, lTmpMsg)) {
    lResult = false;
    lErrorStream << lTmpMsg;
  }

  if (!lResult) {
    log4cplus::Logger lLogger(swatch::logger::Logger::getInstance("swatch.xml.system.validateLinkXml"));
    LOG4CPLUS_ERROR(lLogger, lErrorStream.str());
    aErrorMsg += lErrorStream.str();
  }
  return lResult;
}

bool validateConnectedFedXml(const pugi::xml_node& aNode, std::string& aErrorMsg)
{
  const std::vector<std::string> lSubTags = { "port" };
  bool lResult = true;
  std::stringstream lErrorStream;

  if (!hasAttr(aNode, "id")) {
    lErrorStream << "<" << aNode.name() << "> tag has no 'id' attribute!\n";
    lResult = false;
  }

  std::string lTmpMsg("");
  if (!checkForUnknownSubTags(aNode, lSubTags, lTmpMsg)) {
    lResult = false;
    lErrorStream << lTmpMsg;
  }

  if (numberOfChildren(aNode, "port") == 0) {
    lErrorStream << getNodeSummary(aNode) << ": No <port> tags found in children. At least one expected.\n";
    lResult = false;
  }
  size_t i(1);
  for (pugi::xml_named_node_iterator lPort = aNode.children("port").begin(); lPort != aNode.children("port").end();
       ++lPort) {
    if (!hasAttr(*lPort, "id")) {
      lErrorStream << getNodeSummary(aNode) << ": " << i << ". entry of <port> child tag has no 'id' attribute!\n";
      lResult = false;
      ++i;
    }
  }

  if (!lResult) {
    log4cplus::Logger lLogger(swatch::logger::Logger::getInstance("swatch.xml.system.validateConnectedFedXml"));
    LOG4CPLUS_ERROR(lLogger, lErrorStream.str());
    aErrorMsg += lErrorStream.str();
  }
  return lResult;
}

bool hasAttr(const pugi::xml_node& aNode, const std::string& aAttr)
{
  pugi::xml_attribute lAttribute = aNode.attribute(aAttr.c_str());
  if (!lAttribute)
    return false;

  std::string lAttrValue(lAttribute.value());
  if (lAttrValue.empty())
    return false;

  return true;
}

bool childContentNonEmpty(const pugi::xml_node& aNode, const std::string& aChildName)
{
  log4cplus::Logger lLogger(swatch::logger::Logger::getInstance("swatch.xml.system.childContentNonEmpty"));
  unsigned int i = 1;
  bool lResult(aNode.child(aChildName.c_str()));
  for (pugi::xml_named_node_iterator lChild = aNode.children(aChildName.c_str()).begin();
       lChild != aNode.children(aChildName.c_str()).end(); ++lChild) {
    if (strcmp(lChild->child_value(), "") == 0) {
      if (hasAttr(aNode, "id")) {
        std::string lId(aNode.attribute("id").value());
        LOG4CPLUS_ERROR(lLogger,
                        i << ". entry of <" << aChildName << "> tag for <" << aNode.name() << " id=\"" << lId << "\"> is empty!");
      }
      else {
        LOG4CPLUS_ERROR(lLogger, i << ". entry of <" << aChildName << "> tag for <" << aNode.name() << "> is empty!");
      }
      lResult = false;
    }
  }

  return lResult;
}

//TODO: move this to general XML functions
void loadFromFile(const std::string& aFilePath, pugi::xml_document& aDocToLoadInto)
{
  const std::string lExpandedPath = core::shellExpandPath(aFilePath);

  log4cplus::Logger lLogger(swatch::logger::Logger::getInstance("swatch.xml.loadFromFile"));
  pugi::xml_parse_result lLoadResult = aDocToLoadInto.load_file(lExpandedPath.c_str());
  if (!lLoadResult) {
    LOG4CPLUS_ERROR(lLogger,
                    "Error reading XML file '" << lExpandedPath << "'. Details: " << lLoadResult.description());
    XCEPT_RAISE(XmlFileError,"Error reading XML file '" + lExpandedPath + "' : " + lLoadResult.description());
  }
}

size_t numberOfChildren(const pugi::xml_node& aNode)
{
  return std::distance(aNode.children().begin(), aNode.children().end());
}

size_t numberOfChildren(const pugi::xml_node& aNode, const std::string& aChildName)
{
  return std::distance(aNode.children(aChildName.c_str()).begin(), aNode.children(aChildName.c_str()).end());
}

void pushBackPortStubs(std::vector<swatch::processor::ProcessorPortStub>& aPortStubs, const std::string& aName,
                       const std::string& aAlias, const std::string& aIndex)
{
  log4cplus::Logger lLogger(swatch::logger::Logger::getInstance("swatch.xml.system.pushBackPortStubs"));
  std::vector<std::string> lNames = core::toolbox::IdSliceParser::parse(aName);
  std::vector<std::string> lAliases(lNames.size(), "");
  if (not aAlias.empty())
    lAliases = core::toolbox::IdSliceParser::parse(aAlias);
  std::vector<std::string> lIndices = core::toolbox::IdSliceParser::parse(aIndex);

  if (lNames.size() != lAliases.size()) {
    std::ostringstream lMsg;
    lMsg << lNames.size() << " port names created from name \"" << aName << "\" using slice syntax, but "
         << lAliases.size() << " aliases created from \"" << aAlias << "\"";
    LOG4CPLUS_ERROR(lLogger, lMsg.str());
    XCEPT_RAISE(InvalidSystemDescription,lMsg.str());
  }

  if (lNames.size() != lIndices.size()) {
    std::ostringstream lMsg;
    lMsg << lNames.size() << " port names created from name \"" << aName << "\" using slice syntax, but "
         << lIndices.size() << " indices created from \"" << aIndex << "\"";
    LOG4CPLUS_ERROR(lLogger, lMsg.str());
    XCEPT_RAISE(InvalidSystemDescription,lMsg.str());
  }

  for (size_t i = 0; i < lNames.size(); i++) {
    swatch::processor::ProcessorPortStub lPortStub(lNames.at(i));
    lPortStub.alias = lAliases.at(i);
    lPortStub.number = boost::lexical_cast<unsigned>(lIndices.at(i));
    aPortStubs.push_back(lPortStub);
  }
}

void checkForConnectionMapDuplicates(const swatch::system::SystemStub::FEDInputPortsMap_t& aFedMap)
{
  log4cplus::Logger lLogger(swatch::logger::Logger::getInstance("swatch.xml.system.checkForConnectionMapDuplicates"));
  std::set<std::string> lAllRxPorts, lDuplicates;

  BOOST_FOREACH( auto lFed, aFedMap ) {
    BOOST_FOREACH( std::string lId, lFed.second ) {
      // Continue if insertion succeeds i.e. no duplicates
      if (lAllRxPorts.insert(lId).second)
        continue;

      lDuplicates.insert(lId);
    }
  }

  if (!lDuplicates.empty()) {
// Map to track duplicates across different feds
    std::map<std::string, std::vector<uint32_t> > lDuplicateMap;
    BOOST_FOREACH( std::string lId, lDuplicates ) {
      BOOST_FOREACH( auto lFed, aFedMap ) {

        if (std::find(lFed.second.begin(), lFed.second.end(), lId) == lFed.second.end())
          continue;

        lDuplicateMap[lId].push_back(lFed.first);
      }
    }
    std::ostringstream lMsg;
    lMsg << "Duplicates found across multiple FEDs while parsing CONNECTED FEDS map. ";

    BOOST_FOREACH(auto lCount, lDuplicateMap) {
      lMsg << "id: '" << lCount.first << "': ";
      boost::copy(lCount.second, std::ostream_iterator<uint32_t>(lMsg, " "));
    }

    LOG4CPLUS_ERROR(lLogger, lMsg.str());
    XCEPT_RAISE(InvalidSystemDescription,lMsg.str());
  }
}

bool checkForUnknownSubTags(const pugi::xml_node& aNode, const std::vector<std::string>& aKnownSubTags,
                            std::string& aErrorMsg)
{
  bool lResult(true);
  std::stringstream lErrorMsg;

  BOOST_FOREACH(pugi::xml_node lTag, aNode.children()) {
    bool lFound(false);
    BOOST_FOREACH(std::string lTagName, aKnownSubTags) {
      if (lTag.name() == lTagName) {
        lFound = true;
        break;
      }
    }
    if (!lFound) {
      lResult = false;
      lErrorMsg << getNodeSummary(aNode);
      lErrorMsg << ": Found unknown child tag <" << lTag.name() << ">!\n";
    }
  }

  if (!lResult) {
    lErrorMsg << getNodeSummary(aNode);
    lErrorMsg << ": Valid child tags are:";
    for (auto lIt = aKnownSubTags.begin(); lIt != aKnownSubTags.end(); lIt++)
      lErrorMsg << "  '" << *lIt << "'";
    lErrorMsg << "\n";
  }

  aErrorMsg += lErrorMsg.str();
  return lResult;
}

bool checkChildMultiplicity(const pugi::xml_node& aNode, const std::vector<std::string>& aKnownSubTags,
                            const std::vector<bool>& aIsOptional, const std::vector<size_t>& aSubTagMults,
                            const std::vector<size_t>& aSubTagMultsAtLeast, std::string& aErrorMsg)
{
  bool lResult(true);
  std::stringstream lErrorMsg;

  assert(aKnownSubTags.size() == aSubTagMults.size());
  assert(aSubTagMults.size() == aSubTagMultsAtLeast.size());

  for (size_t lIndex = 0; lIndex < aKnownSubTags.size(); ++lIndex) {
    const std::string lTag(aKnownSubTags.at(lIndex));
    const bool lIsOptional(aIsOptional.at(lIndex));
    const size_t lMultiplicity(aSubTagMults.at(lIndex));
    const size_t lMultAtLeast(aSubTagMultsAtLeast.at(lIndex));

    if (!lIsOptional && !childContentNonEmpty(aNode, lTag)) {
      lErrorMsg << getNodeSummary(aNode) << ": child tag <" << lTag << "> not found!\n";
      lResult = false;
    }
    size_t lMultFound = numberOfChildren(aNode, lTag);
//    lErrorMsg << "found: " << lMultFound << "== " << lMultiplicity << " or >= " << lMultAtLeast << "\n";
    bool lHasCorrectMult = lMultiplicity > 0 ? lMultFound == lMultiplicity : lMultFound >= lMultAtLeast;
    if (!lHasCorrectMult && !(lIsOptional && lMultFound == 0)) {
      lErrorMsg << getNodeSummary(aNode) << ": ";
      if (lMultiplicity > 0)
        lErrorMsg << "Expected to find ";
      else
        lErrorMsg << "Expected to find at least ";
      lErrorMsg << lMultiplicity << " of child tag <" << lTag << ">,";
      lErrorMsg << " but found " << lMultFound << "\n";
      lResult = false;
    }
  }

  aErrorMsg += lErrorMsg.str();
  return lResult;
}

std::string getNodeSummary(const pugi::xml_node& aNode)
{
  std::ostringstream lResultStream;
  lResultStream << "<" << aNode.name();
  if (hasAttr(aNode, "id"))
    lResultStream << " id='" << aNode.attribute("id").value() << "'";
  lResultStream << ">";
  return lResultStream.str();
}

void XmlToStringWriter::write(const void* aData, size_t aSize)
{
  result += std::string(static_cast<const char*>(aData), aSize);
}

} // detail
} // system
} // xml
} // swatch

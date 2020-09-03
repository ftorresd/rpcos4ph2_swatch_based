#include "swatch/xml/XmlReader.hpp"


#include <algorithm>                    // for count
#include <iterator>
#include <ostream>                      // for operator<<, basic_ostream
#include <string.h>                     // for strcmp, strlen
#include <vector>                       // for vector

// boost headers
#include "boost/algorithm/string/predicate.hpp"
#include "boost/filesystem/operations.hpp"  // for absolute
#include "boost/filesystem/path.hpp"    // for path

// external headers
#include "pugixml.hpp"
#include <log4cplus/loggingmacros.h>

// SWATCH headers
#include "swatch/logger/Logger.hpp"


namespace swatch {
namespace xml {


const std::string XmlReader::kModuleNameInfra = "infra";
const std::string XmlReader::kModuleNameAlgo = "algo";
const std::string XmlReader::kModuleNameRunSettings = "run-settings";


XmlReader::XmlReader() :
  mLogger(swatch::logger::Logger::getInstance("swatch.xml.XmlReader"))
{
}


XmlReader::~XmlReader()
{
}


const std::string XmlReader::readXmlConfig(const std::string& aFileName)
{
  pugi::xml_document lXmlDoc, lTmpDoc, lNewDoc;
  loadFromFile(aFileName, lXmlDoc);
  std::string lErrorMsg("");
  if (!checkMainConfig(lXmlDoc, lErrorMsg))
    XCEPT_RAISE(InvalidConfig,"The given XML file is not a valid SWATCH XML main config: " + aFileName + "\n" + lErrorMsg);

  pugi::xml_node lDb = lXmlDoc.child("db");
  pugi::xml_node lTmpDb = lTmpDoc.append_child("db");
  pugi::xml_node lNewDb = lNewDoc.append_child("db");

  for (pugi::xml_node lKey = lDb.first_child(); lKey; lKey = lKey.next_sibling()) {
    pugi::xml_node lTmpKey = lTmpDb.append_child("key");
    lTmpKey.append_attribute("id") = lKey.attribute("id").value();

    mergeSubConfigFiles(lKey, lTmpKey, aFileName);

    pugi::xml_node lNewKey = lNewDb.append_child("key");
    lNewKey.append_attribute("id") = lKey.attribute("id").value();

    mergeContexts(lTmpKey, lNewKey);
  }

  return docToString(lNewDoc);
}


void XmlReader::mergeSubConfigFiles(const pugi::xml_node& aKeyNode, pugi::xml_node& aNewKeyNode,
                                    const std::string& aMainConfigPath) const
{
  for (pugi::xml_node lLoad = aKeyNode.first_child(); lLoad; lLoad = lLoad.next_sibling()) {
    // load sub config
    std::string lSubConfigFile = lLoad.attribute("module").value();
    lSubConfigFile = normaliseSubConfigPath(aMainConfigPath, lSubConfigFile);
    pugi::xml_document lSubConfig;
    loadFromFile(lSubConfigFile, lSubConfig);

    std::string lErrorMsg;
    if (!checkSubConfig(lSubConfig, lErrorMsg))
      XCEPT_RAISE(InvalidConfig,"The given XML file is not a valid SWATCH XML sub-config: " + lSubConfigFile + "\n" + lErrorMsg);

    appendSubConfig(lSubConfig, aNewKeyNode);
  }
}


void XmlReader::appendSubConfig(const pugi::xml_document& aSubConfig, pugi::xml_node& aDestinationNode)
{
  // check sub config
  std::string lErrorMsg("");
  if (!checkSubConfig(aSubConfig, lErrorMsg))
    XCEPT_RAISE(InvalidConfig,"The supplied XML is not a valid SWATCH XML sub-config: \n" + lErrorMsg);

  // import sub config's nodes
  pugi::xml_node lModule = aSubConfig.first_child();
  const std::string lModuleId = lModule.attribute("id").value();

  // Iterate over sub config's <context> & <disable> tags
  for (pugi::xml_node lSubTag = lModule.first_child(); lSubTag; lSubTag = lSubTag.next_sibling()) {
    // 1) Attach the module's ID tag as prefix to id attributes in <context> and <disable> tags
    const std::string lSubTagId = lSubTag.attribute("id").value();
    if (lSubTagId.empty())
      lSubTag.attribute("id").set_value(lModuleId.data());
    else
      lSubTag.attribute("id").set_value((lModuleId + "." + lSubTagId).data());

    // 2) Add <context>/<disable> tag with modified ID to the output
    aDestinationNode.append_copy(lSubTag);
  }
}



const std::string XmlReader::normaliseSubConfigPath(const std::string& aMainConfigPath,
    const std::string& aSubConfigPath)
{
  if (boost::starts_with(aSubConfigPath, "/")) {
    // absolute path, do nothing
    return aSubConfigPath;
  }
  else {
    std::string lAbsolutePath = boost::filesystem::absolute((aMainConfigPath)).parent_path().string();
    return lAbsolutePath + "/" + aSubConfigPath;
  }
}


void XmlReader::mergeContexts(const pugi::xml_node& aKeyNode, pugi::xml_node& aNewKeyNode)
{
  // this can be probably simplified
  for (pugi::xml_node lChild = aKeyNode.first_child(); lChild; lChild = lChild.next_sibling()) {
    // check if node has been already added
    pugi::xml_node lNewChild = aNewKeyNode.find_child_by_attribute(lChild.name(), "id", lChild.attribute("id").value());
    if (lNewChild) {
      for (pugi::xml_node lLeaf = lChild.first_child(); lLeaf; lLeaf = lLeaf.next_sibling()) {
        if (strcmp(lLeaf.name(), "state") == 0) {
          // merge states
          pugi::xml_node lNewLeaf = lNewChild.find_child_by_attribute("state", "id", lLeaf.attribute("id").value());
          if (!lNewLeaf)
            lNewChild.append_copy(lLeaf);
          else {
            for (pugi::xml_node lStateLeaf = lLeaf.first_child(); lStateLeaf; lStateLeaf = lStateLeaf.next_sibling()) {
              lNewLeaf.append_copy(lStateLeaf);
            }
          }
        }
        else {
          lNewChild.append_copy(lLeaf);
        }
      }
    }
    else {
      aNewKeyNode.append_copy(lChild);
    }
  }

}

const std::string XmlReader::docToString(const pugi::xml_document& aDoc)
{
  XmlToStringWriter lWriter;
  aDoc.save(lWriter);
  return lWriter.result;
}

void XmlReader::XmlToStringWriter::write(const void* aData, size_t aSize)
{
  result += std::string(static_cast<const char*>(aData), aSize);
}

bool XmlReader::checkMainConfig(const pugi::xml_document& aMainConfig, std::string& aErrorMsg) const
{
  // top level node is db
  pugi::xml_node lDb = aMainConfig.child("db");
  // only one <db> tag per config
  bool lResult = std::distance(aMainConfig.children("db").begin(), aMainConfig.children("db").end()) == 1;
  if (!lResult)
    aErrorMsg += "More than one <db> tag detected\n";
  // the next level is one or more <key>
  for (pugi::xml_node lKey = lDb.first_child(); lKey; lKey = lKey.next_sibling()) {
    bool lIsKeyTag = strcmp(lKey.name(), "key") == 0;
    if (!lIsKeyTag)
      aErrorMsg += "A non <key> detected as child of <db>: <" +  std::string(lKey.name()) + ">\n";
    lResult = lResult && lIsKeyTag;
    // a <key> tag should only contain <load> tags
    for (pugi::xml_node lLoad = lKey.first_child(); lLoad; lLoad = lLoad.next_sibling()) {
      bool lIsLoadTag = strcmp(lLoad.name(), "load") == 0;
      if (!lIsLoadTag)
        aErrorMsg += "A non <load> detected as child of <key>: <" +  std::string(lLoad.name()) + ">\n";
      lResult = lResult && lIsLoadTag;
    }
  }

  return lResult;
}

bool XmlReader::checkSubConfig(const pugi::xml_document& aSubConfig, std::string& aErrorMsg)
{
  // There can only one top-level tag per sub-config
  bool lResult = std::distance(aSubConfig.children().begin(), aSubConfig.children().end()) == 1;
  if (!lResult)
    aErrorMsg += "More than one top-level tag detected in configuration module file\n";

  // Top level node is either 'infra', 'algo' or 'run-settings'
  pugi::xml_node lModule = aSubConfig.first_child();
  std::string lModuleTagName = lModule.name();
  ModuleType lModuleType = kInfra;
  if (lModuleTagName == kModuleNameInfra)
    lModuleType = kInfra;
  else if (lModuleTagName == kModuleNameAlgo)
    lModuleType = kAlgo;
  else if (lModuleTagName == kModuleNameRunSettings)
    lModuleType = kRunSettings;
  else {
    aErrorMsg += ("Invalid top-level tag '" + lModuleTagName + "' detected in configuration module file\n");
    return false;
  }

  // Check that top-level node has "id" attribute, and that it's not an empty string
  pugi::xml_attribute lModuleIdAttribute = lModule.attribute("id");
  if (!lModuleIdAttribute) {
    aErrorMsg += ("The top-level tag '" + lModuleTagName + "' in config module file must have a (non-empty) 'id' attribute\n");
    lResult = false;
  }
  else if (strlen(lModuleIdAttribute.value()) == 0) {
    aErrorMsg += ("The 'id' attribute of the top-level tag '" + lModuleTagName + " in a config file cannot be empty\n");
    lResult = false;
  }

  // the next level is zero or more <context> or <disable> tags
  for (pugi::xml_node lSubTag = lModule.first_child(); lSubTag; lSubTag = lSubTag.next_sibling()) {
    std::string lSubTagName(lSubTag.name());
    if ((lSubTagName != "context") && (lSubTagName != "disable") ) {
      aErrorMsg += "A non <context> or <disable> detected as child of a module tag: <" + lSubTagName + ">\n";
      lResult = false;
    }

    // Check that the context/disable tag's id attribute exists, and follows the rules
    pugi::xml_attribute lSubTagIdAttribute = lSubTag.attribute("id");
    if (!lSubTagIdAttribute) {
      aErrorMsg += "The module child tag, <" + lSubTagName + ">, MUST have an 'id' attribute\n";
      lResult = false;
    }
    else if ((lSubTagName != "context") && (strlen(lSubTagIdAttribute.value()) == 0)) {
      aErrorMsg += "The 'id' attribute of the module child tag, <" + lSubTagName + ">, CANNOT be empty\n";
      lResult = false;
    }
    else if (std::string(lSubTagIdAttribute.value()).find('.') != std::string::npos) {
      aErrorMsg += lSubTagName + " tag with ID '" + lSubTagIdAttribute.value() + "': id attribute contains invalid character '.'\n";
      lResult = false;
    }

    if (lSubTagName == "context") {
      // Check that child tags of context are valid
      std::vector<std::string> lPossibleChildTags = {"mask", "param", "state"};
      for (pugi::xml_node lChildTag = lSubTag.first_child(); lChildTag; lChildTag = lChildTag.next_sibling()) {
        if (std::count(lPossibleChildTags.begin(), lPossibleChildTags.end(), lChildTag.name()) == 0) {
          aErrorMsg += "An unknown tag was detected as child of <context>: <" + std::string(lChildTag.name()) + ">\n";
          lResult = false;
        }
        else if ((lModuleType != kRunSettings) && (strcmp(lChildTag.name(), "param") != 0)) {
          aErrorMsg += std::string("Cannot use tag <") + lChildTag.name() + "> in a '" + lModuleTagName + "' configuration module\n";
          lResult = false;
        }
        else if ((lModuleType != kInfra) && (strcmp(lChildTag.name(), "param") == 0)) {
          for (pugi::xml_node::attribute_iterator lAttIt = lChildTag.attributes_begin(); lAttIt != lChildTag.attributes_end(); lAttIt++) {
            if (strcmp(lAttIt->name(),"ns") == 0 || strcmp(lAttIt->name(),"cmd") == 0) {
              aErrorMsg += std::string("Cannot specify attribute '") + lAttIt->name() + "' in a <param> tag within a '" + lModuleTagName + "' configuration module\n";
              lResult = false;
            }
          }
        }
      }
    } // end: if lSubTagName == "context"
    else if ((lSubTagName == "disable") && (lModuleType != kRunSettings)) {
      aErrorMsg += "Cannot use tag <" + lSubTagName + "> in a '" + lModuleTagName + "' configuration module\n";
      lResult = false;
    } // end: else if lSubTagName == "disable"
  }

  return lResult;
}

void XmlReader::loadFromFile(const std::string& aFilePath, pugi::xml_document& aDocToLoadInto) const
{
  pugi::xml_parse_result lLoadResult = aDocToLoadInto.load_file(aFilePath.c_str());
  if (!lLoadResult) {
    LOG4CPLUS_ERROR(mLogger, "Error reading XML file '" << aFilePath << "'. Details: " << lLoadResult.description());
    XCEPT_RAISE(XmlFileError,"Error reading XML file '" + aFilePath + "' : " + lLoadResult.description());
  }
}

}
}

#ifndef __SWATCH_XML_XMLREADER_HPP__
#define __SWATCH_XML_XMLREADER_HPP__


// Standard headers
#include <stddef.h>                     // for size_t
#include <string>

// external
#include "log4cplus/logger.h"           // for Logger
#include "pugixml.hpp"

// SWATCH headers
#include "swatch/core/exception.hpp"


namespace swatch {
namespace xml {
/**
 * The XML reader is a helper class to read, verify and transform XML files.
 * The main config is expected to have the form
 * <db>
 *  <key id="MyDummyKey`>
 *    <load module="<path to sub config>" />
 *    ...
 *  </key>
 * </db>
 *
 * Where <path to sub config> is either an absolute path or it will be intepreted
 * as a relative path with respect to the location of the main config.
 */
class XmlReader {
public:

  static const std::string kModuleNameInfra;
  static const std::string kModuleNameAlgo;
  static const std::string kModuleNameRunSettings;

  enum ModuleType {
    kInfra,
    kAlgo,
    kRunSettings
  };

  XmlReader();
  ~XmlReader();

  /**
   * Reads the XML configuration from a file, loads sub configuration files,
   * checks for validity and returns the combined result.
   */
  const std::string readXmlConfig(const std::string& aFileName);

  /**
   * Checks if the main config has the correct structure:
   * <db>
   *   <key id="keyId>
   *     <load ... /> <!-- one or more -->
   *     ...
   *   </key>
   * </db>
   */
  bool checkMainConfig(const pugi::xml_document& aMainConfig, std::string& aErrorMsg) const;

  /**
   * Checks if a sub config has the correct structure:
   * <module>
   *   <context id="contextId> <!-- 0 or more -->
   *     <param ... /> <!-- one or more -->
   *     ...
   *   </context>
   *   <disable id="system.brokenProcessor" /> <!-- 0 or more -->
   * </module>
   */
  static bool checkSubConfig(const pugi::xml_document& aSubConfig, std::string& aErrorMsg);

  /**
   * Returns the string representation of a XML document
   */
  static const std::string docToString(const pugi::xml_document& aDoc);

  /**
   * Takes a file path (aFilePath) and loads it into an XML document (aDocToLoadInto).
   * Throws an exception if file cannot be opened.
   */
  void loadFromFile(const std::string& aFilePath, pugi::xml_document& aDocToLoadInto) const;

  static void appendSubConfig(const pugi::xml_document& aSubConfigNode, pugi::xml_node& aDestinationNode);

  /**
   * Takes an XML document merges leafes from <context> & <state> under the same <key> and with the same ID.
   */
  static void mergeContexts(const pugi::xml_node& aKeyNode, pugi::xml_node& aNewKeyNode);


private:
  log4cplus::Logger mLogger;

  struct XmlToStringWriter: pugi::xml_writer {
    std::string result;
    virtual void write(const void* aData, size_t aSize);
  };

  void mergeSubConfigFiles(const pugi::xml_node& aKeyNode, pugi::xml_node& aNewKeyNode,
                           const std::string& aMainConfigPath) const;



  /**
   * Normalises the path of subconfigs included with the <load> tag.
   * If the SubConfigPath is absolute, this function will not do anything.
   * If the path is relative it will be transformed into an absolute path
   * with repect to the MainConfigPath.
   */
  static const std::string normaliseSubConfigPath(const std::string& aMainConfigPath, const std::string& aSubConfigPath);
};

SWATCH_DEFINE_EXCEPTION(InvalidConfig)
SWATCH_DEFINE_EXCEPTION(XmlFileError)

}
}

#endif  /* __SWATCH_XML_XMLREADER_HPP__ */

#ifndef __SWATCH_XML_XMLSYSTEM_HPP__
#define __SWATCH_XML_XMLSYSTEM_HPP__

// Standard headers
#include <stddef.h>                     // for size_t
#include <string>
#include <vector>                       // for vector

// SWATCH headers
#include "swatch/core/exception.hpp"
#include "swatch/processor/ProcessorStub.hpp"
#include "swatch/dtm/DaqTTCStub.hpp"
#include "swatch/system/SystemStub.hpp"
#include "swatch/system/CrateStub.hpp"

// external
#include "pugixml.hpp"


namespace swatch {

namespace system {
class LinkStub;
}


namespace xml {
namespace system {
extern const char* kMainTag;

/**
 * The xmlFileToSystemStub is a function to read, verify and transform XML files
 * into a swatch::system::SystemStub
 * The system XML is expected to have the form
 * <system id="calol2" creator="calol2::System">
 *  <crate id="S2D11-29" ... />
 *  <processor id="MP8" ... />
 *  <daqttc-mgr id="AMC13" .. />
 *    ...
 * </system>
 *
 * For a full example please see swatch/xml/test/etc/swatch/test/system.xml
 */
swatch::system::SystemStub xmlFileToSystemStub(const std::string& aFileName);

/**
 * Same as xmlFileToSystemStub, but reads from a pugi::xml_document instead of a file.
 */
swatch::system::SystemStub xmlToSystemStub(const pugi::xml_document& aXmlDoc);

SWATCH_DEFINE_EXCEPTION(InvalidSystemDescription)

namespace detail {

struct XmlToStringWriter: pugi::xml_writer {
  std::string result;
  virtual void write(const void* aData, size_t aSize);
};

/**
 * Converts a <daqttc-mgr> tag to swatch::dtm::DaqTTCStub, e.g.
 * <daqttc-mgr id="...">
 *   <creator>...</creator>
 *   <hw-type>...</hw-type>
 *   <role>...</role>
 *   <uri>...</uri>
 *   <uri>...</uri>
 *   <address-table>...</address-table>
 *   <address-table>...</address-table>
 *   <crate>...</crate>
 *   <slot>...</slot>
 *   <fed-id>...</fed-id>
 * </daqttc-mgr>
 */
swatch::dtm::DaqTTCStub xmlToDaqTTCStub(const pugi::xml_node& aNode);

/**
 * Converts a <processor> tag to swatch::processor::ProcessorStub, e.g.
 * <processor id="...">
 *   <creator>...</creator>
 *   <hw-type>...</hw-type>
 *   <role>...</role>
 *   <uri>...</uri>
 *   <address-table>...</address-table>
 *   <crate>...</crate>
 *   <slot>...</slot>
 *   <rx-port name="..." pid="..."/>
 *   <tx-port name="..." pid="..."/>
 * </processor>
 */
swatch::processor::ProcessorStub xmlToProcessorStub(const pugi::xml_node& aNode);

/**
 * Converts the <tx-port> and <rx-port> subtags of <processor> into a vector of swatch::processor::ProcessorPortStub
 */
std::vector<swatch::processor::ProcessorPortStub> xmlToProcessorPortStub(const pugi::xml_node& aNode);

/**
 * Converts a <crate> tag to swatch::system::CrateStub, e.g.
 * <crate id="...">
 *  <location>...</location>
 *  <description>...</description>
 * </crate>
 */
swatch::system::CrateStub xmlToCrateStub(const pugi::xml_node& aNode);

/**
 * Converts a <link> tag to a vector of swatch::system::LinkStub, e.g.
 * <link id="...">
 *   <from>...</from>
 *   <to>...</to>
 *   <tx-port>...</tx-port>
 *   <rz-port>...</rx-port>
 * </link>
 */
std::vector<swatch::system::LinkStub> xmlToLinkStubs(const pugi::xml_node& aNode);

/**
 * Converts a <connected-fed> tag to swatch::system::SystemStub::FEDInputPortsMap (aFedMap).
 * Checks for duplicates (not allowed).
 * Example:
 * <connected-fed id="...">
 *   <port id="...]"/>
 *   <port id="..."/>
 * </connected-fed>
 */
void xmlToConnectedFeds(const pugi::xml_node& aNode, swatch::system::SystemStub::FEDInputPortsMap_t& aFedMap,
                        const std::vector<std::string>& aExcludedBoards);

/**
 * Converts an <exclude id="..." tag to a SystemStub::ExcludedBoards entry.
 * The id has to exist (processor or daqttc).
 */
std::string xmlToExcludedBoard(const pugi::xml_node& aNode, const std::vector<std::string>& aKnownIds);
/**
 * Validates the swatch::system::System XML description from an XML document
 * <system>
 *   ...
 * </system>
 */
bool validateSystemXml(const pugi::xml_document& aXmlDoc, std::string& aErrorMsg);

/**
 * Validates a single instance of a <system> child tag (e.g <crate>, <creator>, etc)
 */
bool validateChildXml(const pugi::xml_node& aChildNode, std::string& aErrorMsg);

/**
 * Validates the <creator> or <alias> tag
 * e.g
 * <creator>calol2::MainProcessor</creator>
 */
bool checkNodeNotEmpty(const pugi::xml_node& aNode, std::string& aErrorMsg);

/**
 * Validates the <crate> tag
 * <crate id="...">
 *  <location>...</location>
 *  <description>...</description>
 * </crate>
 */
bool validateCrateXml(const pugi::xml_node& aNode, std::string& aErrorMsg);

/**
 * Validates the <processor> tag
 * <processor id="...">
 *   <creator>...</creator>
 *   <hw-type>...</hw-type>
 *   <role>...</role>
 *   <uri>...</uri>
 *   <address-table>...</address-table>
 *   <crate>...</crate>
 *   <slot>...</slot>
 *   <rx-port name="..." pid="..."/>
 *   <tx-port name="..." pid="..."/>
 * </processor>
 */
bool validateProcessorXml(const pugi::xml_node& aNode, std::string& aErrorMsg);

/**
 * Validates the <daqttc-mgr> tag
 * <daqttc-mgr id="...">
 *   <creator>...</creator>
 *   <hw-type>...</hw-type>
 *   <role>...</role>
 *   <uri>...</uri>
 *   <uri>...</uri>
 *   <address-table>...</address-table>
 *   <address-table>...</address-table>
 *   <crate>...</crate>
 *   <slot>...</slot>
 *   <fed-id>...</fed-id>
 * </daqttc-mgr>
 */
bool validateDAQTTCXml(const pugi::xml_node& aNode, std::string& aErrorMsg);

/**
 * Validates a single instance of <link>:
 * <link id="...">
 *   <from>...</from>
 *   <to>...</to>
 *   <tx-port>...</tx-port>
 *   <rz-port>...</rx-port>
 * </link>
 */
bool validateLinkXml(const pugi::xml_node& aNode, std::string& aErrorMsg);

/**
 * Validates a single instance of <connected-feds>:
 * <connected-fed id="...">
 *   <port id="...]"/>
 *   <port id="..."/>
 * </connected-fed>
 */
bool validateConnectedFedXml(const pugi::xml_node& aNode, std::string& aErrorMsg);
/**
 * Checks if the content of a child node, <child> is not empty
 * <aNode>
 *   <aChildName>CONTENT</aChildName>
 * </aNode>
 */
bool childContentNonEmpty(const pugi::xml_node& aNode, const std::string& aChildName, std::string& aErrorMsg);

/**
 * Checks if aNode has a non-empty attribute of name aAttr
 * <aNode aAttr="..." />
 */
bool hasAttr(const pugi::xml_node& aNode, const std::string& aAttr);

/**
 * Returns the number of children of aNode
 * <aNode>
 *   <child1 />
 *   <child2 />
 *   ...
 * </aNode>
 */
size_t numberOfChildren(const pugi::xml_node& aNode);

/**
 * Returns the number of children of aNode with specific name
 * <aNode>
 *   <aChildName /> <!-- counted -->
 *   <aChildName /> <!-- counted -->
 *   <child2 /> <!-- not counted -->
 *   ...
 * </aNode>
 */
size_t numberOfChildren(const pugi::xml_node& aNode, const std::string& aChildName);

bool checkForUnknownSubTags(const pugi::xml_node& aNode, const std::vector<std::string>& aKnownSubTags,
                            std::string& aErrorMsg);
bool checkChildMultiplicity(const pugi::xml_node& aNode, const std::vector<std::string>& aKnownSubTags,
                            const std::vector<bool>& aIsOptional, const std::vector<size_t>& aSubTagMults,
                            const std::vector<size_t>& aSubTagMultsMoreThan, std::string& aErrorMsg);

/**
 * Loads an XML file, aFilePath, into a pugi::xml_document, aDocToLoadInto
 */
void loadFromFile(const std::string& aFilePath, pugi::xml_document& aDocToLoadInto);

/**
 * Reads the `name` (aName) and `pid` (aIndex) attributes from the processors <rx-port> and <tx-port> tags
 * and translates them into std::vector<swatch::processor::ProcessorPortStub>.
 * All results are appended to the aPortStubs parameter.
 */
void pushBackPortStubs(std::vector<swatch::processor::ProcessorPortStub>& aPortStubs, const std::string& aName,
                       const std::string& aAlias, const std::string& aIndex);

void checkForConnectionMapDuplicates(const swatch::system::SystemStub::FEDInputPortsMap_t& aFedMap);

//! Returns summary information about node, of the format "node-name tag, id='the-id'" (e.g. for prefix of error messages)
std::string getNodeSummary(const pugi::xml_node& aNode);

} //ns: detail
} //ns: system
} //ns: xml
} //ns: swatch

#endif  /* __SWATCH_XML_XMLSYSTEM_HPP__ */

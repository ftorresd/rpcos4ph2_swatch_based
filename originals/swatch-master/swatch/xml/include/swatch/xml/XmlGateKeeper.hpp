/**
 * @file    XmlGateKeeper.hpp
 * @author  arose
 * @brief   Guardian to the underworld that is the online database
 * @date    May 2015
 *
 */

#ifndef __SWATCH_XML_XMLGATEKEEPER_HPP__
#define __SWATCH_XML_XMLGATEKEEPER_HPP__


// C++ Headers
#include <string>
#include <utility>                      // for pair

// boost headers
#include "boost/smart_ptr/shared_ptr.hpp"  // for shared_ptr

// log4cplus headers
#include "log4cplus/logger.h"           // for Logger

// SWATCH headers
#include "swatch/core/exception.hpp"
#include "swatch/action/GateKeeper.hpp"


namespace pugi {
class xml_document;
class xml_node;
}

namespace swatch {
namespace xml {

class XmlSerializer;


class XmlGateKeeper: public action::GateKeeper {
public:

  /// Constructor
  XmlGateKeeper(const std::string& aFileName, const std::string& aKey);
  XmlGateKeeper(const pugi::xml_document& aXmlDoc, const std::string& aKey);

  /// Destructor
  virtual ~XmlGateKeeper();

private:

  void readXmlDocument(const pugi::xml_document& aXmlDoc, const std::string& aRunKey);

  std::pair<std::string, GateKeeper::Parameter_t> createParameter(pugi::xml_node& aParam);
  std::pair<std::string, GateKeeper::ParametersContext_t> createContext(pugi::xml_node& aContext);

  std::pair<std::string, GateKeeper::MonitoringSetting_t> createMonitoringSetting(
    const pugi::xml_node& aParam) const;
  std::pair<std::string, GateKeeper::SettingsContext_t> createSettingsContext(
    const pugi::xml_node& aContext) const;

  std::pair < std::string, GateKeeper::MasksContext_t> createMasksContext(const pugi::xml_node& aContext) const;

  std::string parseDisableNode(const pugi::xml_node& aContext) const;

  std::string mFileName;
  boost::shared_ptr<XmlSerializer> mSerializer;
  log4cplus::Logger mLogger;
};


SWATCH_DEFINE_EXCEPTION ( AttributeMissing )
SWATCH_DEFINE_EXCEPTION ( InvalidAttribute )
SWATCH_DEFINE_EXCEPTION ( InvalidAttributeValue )


} /* namespace xml */
} /* namespace swatch */

#endif /* __SWATCH_XML_XmlGateKeeper_HPP__ */

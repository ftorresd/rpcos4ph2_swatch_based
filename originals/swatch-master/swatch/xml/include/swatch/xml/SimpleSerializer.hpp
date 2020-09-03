#ifndef __SWATCH_XML_SIMPLESERIALIZER_HPP__
#define __SWATCH_XML_SIMPLESERIALIZER_HPP__

// Standard headers

// boost headers

//xdata
#include "swatch/xml/AbstractSerializer.hpp"
#include "xdata/Serializable.h"
#include "pugixml.hpp"

namespace swatch {
namespace xml {

/**
 * Implementation of the XML object serializer for basic types and string
 */
template<class T>
class SimpleSerializer : public AbstractSerializer {
public:

  SimpleSerializer();
  virtual ~SimpleSerializer();

  //TODO: const only possible once we switch to XDAQ 13 (where Serializable::toString() finally becomes const)
//  const xdata::Serializable* import(const pugi::xml_node& aNode);
  // for now do these two
  /**
   * Converts an XML element into a xdata::Serializable
   */
  xdata::Serializable* import(const pugi::xml_node& aNode);
  const xdata::Serializable* constImport(const pugi::xml_node& aNode);

  /**
   * Implementation of xdata::ObjectSerializer::type;
   */
  std::string type()  const;
};

} // namespace xml
} // namespace swatch

#include "swatch/xml/SimpleSerializer.hxx"

#endif  /* __SWATCH_XML_SINGLEOBJECTSERIALIZER_HPP__ */


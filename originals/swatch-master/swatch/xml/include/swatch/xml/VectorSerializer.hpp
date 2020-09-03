#ifndef __SWATCH_XML_VECTORSERIALIZER_HPP__
#define __SWATCH_XML_VECTORSERIALIZER_HPP__

//xdata
#include "swatch/xml/AbstractSerializer.hpp"
#include "xdata/Serializable.h"
#include "xdata/Vector.h"
// SWATCH headers
#include "pugixml.hpp"

namespace swatch {
namespace xml {

template<class T>
class VectorSerializer: public AbstractSerializer {
public:

  VectorSerializer();
  virtual ~VectorSerializer();

  // TODO: const only possible once we switch to XDAQ 13 (where Serializable::toString() finally becomes const)
  //  const xdata::Serializable* import(const pugi::xml_node& aNode);
  /**
   * Converts an XML element into a xdata::Serializable
   */
  xdata::Serializable* import(const pugi::xml_node& aNode);
  const xdata::Serializable* constImport(const pugi::xml_node& aNode);
  /**
   * Implementation of xdata::ObjectSerializer::type;
   */
  std::string type() const;

private:
  /**
   * Converts a string of form "x, y, z" into a vector containing type T
   */
  xdata::Vector<T> convertString(const std::string& str, const std::string& aDeliminter) const;
};

} // namespace xml
} // namespace swatch

#include "swatch/xml/VectorSerializer.hxx"

#endif  /* __SWATCH_XML_VECTORSERIALIZER_HPP__ */


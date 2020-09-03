#ifndef __SWATCH_XML_ABSTRACTSERIALIZER_HPP__
#define __SWATCH_XML_ABSTRACTSERIALIZER_HPP__

// Standard headers

// boost headers

//xdata
#include "xdata/Serializable.h"
#include "xdata/ObjectSerializer.h"
// SWATCH headers
#include "swatch/core/exception.hpp"
// others
#include "pugixml.hpp"

namespace swatch {
namespace xml {

/**
 * Abstract base XML serializer for xdata serializables
 */
class AbstractSerializer : public xdata::ObjectSerializer {
public:

  virtual ~AbstractSerializer()
  {
    // TODO: move this into hxx which currently does not work for some bizzare reason (multiple definitions)
  }

  //TODO: const only possible once we switch to XDAQ 13 (where Serializable::toString() finally becomes const)
//  const xdata::Serializable* import(const pugi::xml_node& aNode);
  // for now do these two
  /**
   * Converts an XML element into a xdata::Serializable
   */
  virtual xdata::Serializable* import(const pugi::xml_node& aNode) = 0;
  /**
   * Same as ObjectSerializer::import but with dynamic cast to const
   */
  virtual const xdata::Serializable* constImport(const pugi::xml_node& aNode) = 0;


};

SWATCH_DEFINE_EXCEPTION(ValueError)

} // namespace xml
} // namespace swatch

#endif  /* __SWATCH_XML_ABSTRACTSERIALIZER_HPP__ */


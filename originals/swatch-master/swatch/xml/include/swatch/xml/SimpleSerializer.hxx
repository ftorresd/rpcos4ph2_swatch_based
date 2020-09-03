#ifndef __SWATCH_XML_SIMPLESERIALIZER_HXX__
#define __SWATCH_XML_SIMPLESERIALIZER_HXX__

// IWYU pragma: private, include "swatch/xml/SimpleSerializer.hpp"

// Standard headers

// boost headers

// SWATCH headers
#include "swatch/core/exception.hpp"
// xdata
#include <xdata/Integer.h>
#include <xdata/UnsignedInteger.h>
#include <xdata/String.h>
#include <xdata/Boolean.h>
#include <xdata/Float.h>
#include "swatch/xml/SimpleSerializer.hpp"

namespace swatch {
namespace xml {

template<class T>
SimpleSerializer<T>::SimpleSerializer()
{
}

template<class T>
SimpleSerializer<T>::~SimpleSerializer()
{
}

//template<class T> const xdata::Serializable* SingleObjectSerializer<T>::import(const pugi::xml_node& aNode) {
template<class T>
xdata::Serializable* SimpleSerializer<T>::import(const pugi::xml_node& aNode)
{
  xdata::Serializable* lSerializable(new T());

  std::string lValue(aNode.child_value());
  try {
    lSerializable->fromString(lValue);
  }
  catch (const xdata::exception::Exception& lExc) {
    delete lSerializable;
    const std::string lType(aNode.attribute("type").value());
    XCEPT_RAISE(ValueError,"Could not parse '" + lValue + "' into type '" + lType + "'.");
  }

  return lSerializable;
}

template<class T>
const xdata::Serializable* SimpleSerializer<T>::constImport(const pugi::xml_node& aNode)
{
  return const_cast<xdata::Serializable*>(import(aNode));
}

template<class T>
std::string SimpleSerializer<T>::type() const
{
  T lTmp;
  return lTmp.type();
}


} // namespace xml
} // namespace swatch

#endif	/* __SWATCH_XML_SIMPLESERIALIZER_HXX__ */


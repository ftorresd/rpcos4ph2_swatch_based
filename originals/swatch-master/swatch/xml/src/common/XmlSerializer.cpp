#include "swatch/xml/XmlSerializer.hpp"


// Standard headers
#include <string>                       // for string, allocator, etc

// boost headers

// pugixml headers
#include "pugixml.hpp"          // for xml_attribute, xml_node

// log4cplus headers
#include <log4cplus/loggingmacros.h>

// XDAQ headers
#include "xdata/Boolean.h"              // for Boolean
#include "xdata/Float.h"                // for Float
#include "xdata/Integer.h"              // for Integer
#include "xdata/ObjectSerializer.h"     // for ObjectSerializer
#include "xdata/Serializable.h"         // for Serializable
#include "xdata/String.h"               // for String
#include "xdata/UnsignedInteger.h"      // for UnsignedInteger
#include "xdata/UnsignedInteger64.h"    // for UnsignedInteger64

// SWATCH headers
#include "swatch/logger/Logger.hpp"
#include "swatch/xml/AbstractSerializer.hpp"
#include "swatch/xml/SimpleSerializer.hpp"
#include "swatch/xml/TableSerializer.hpp"
#include "swatch/xml/VectorSerializer.hpp"


namespace swatch {
namespace xml {

XmlSerializer::XmlSerializer() :
  mLogger(swatch::logger::Logger::getInstance("swatch.xml.XmlSerializer"))
{
  // register simple serliazers
  this->addObjectSerializer(new SimpleSerializer<xdata::UnsignedInteger>());
  this->addObjectSerializer(new SimpleSerializer<xdata::UnsignedInteger64>());
  this->addObjectSerializer(new SimpleSerializer<xdata::Integer>());
  this->addObjectSerializer(new SimpleSerializer<xdata::Float>());
  this->addObjectSerializer(new SimpleSerializer<xdata::Boolean>());
  this->addObjectSerializer(new SimpleSerializer<xdata::String>());
  // register vector serializers
  this->addObjectSerializer(new VectorSerializer<xdata::UnsignedInteger>());
  this->addObjectSerializer(new VectorSerializer<xdata::UnsignedInteger64>());
  this->addObjectSerializer(new VectorSerializer<xdata::Integer>());
  this->addObjectSerializer(new VectorSerializer<xdata::Float>());
  this->addObjectSerializer(new VectorSerializer<xdata::Boolean>());
  this->addObjectSerializer(new VectorSerializer<xdata::String>());
  // register table serializer
  this->addObjectSerializer(new TableSerializer());
}

XmlSerializer::~XmlSerializer()
{

}

xdata::Serializable* XmlSerializer::import(const pugi::xml_node& aNode)
{
  LOG4CPLUS_DEBUG(mLogger, "Calling XmlSerializer::import");
  std::string lType(aNode.attribute("type").value());
  LOG4CPLUS_DEBUG(mLogger, "Normalising type: " + lType);
  lType = normaliseType(lType);
  LOG4CPLUS_DEBUG(mLogger, "Getting serializer for type " + lType);
  AbstractSerializer* lSerializer(0x0);
  try {
    lSerializer = dynamic_cast<AbstractSerializer*>(this->getObjectSerializer(lType));
  }
  catch (const xdata::exception::Exception& lExc) {
    XCEPT_RAISE(UnknownDataType,"No serializer for type '" + lType + "' found.");
  }
  LOG4CPLUS_DEBUG(mLogger, "Found serializer with type " + lSerializer->type());
  xdata::Serializable* lSerializable(lSerializer->import(aNode));

  return lSerializable;
}

const std::string XmlSerializer::normaliseType(const std::string& aType) const
{
  std::string lType(aType);
  // expand type names
  if (lType == "vector:uint")
    lType = "vector:unsigned int";
  if (lType == "uint")
    lType = "unsigned int";

  if (lType == "vector:uint64")
    lType = "vector:unsigned int 64";
  if (lType == "uint64")
    lType = "unsigned int 64";

  return lType;
}

}
}

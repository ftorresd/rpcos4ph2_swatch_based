/*
 * File:   TableSerializer.cpp
 * Author: ale
 *
 * Created on March 3, 2016, 1:35 PM
 */

#include "swatch/xml/TableSerializer.hpp"


// Standard headers
#include <algorithm>                    // for for_each
#include <ostream>                      // for operator<<, basic_ostream, etc
#include <stddef.h>                     // for size_t
#include <stdint.h>                     // for uint32_t
#include <utility>                      // for pair

// boost headers
#include <boost/tokenizer.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/algorithm/string.hpp>
//#include <boost/range/adaptor/transformed.hpp>
//#include <boost/bind.hpp>

// XDAQ headers
#include "xdata/Boolean.h"              // for Boolean
#include "xdata/Float.h"                // for Float
#include "xdata/Integer.h"              // for Integer
#include "xdata/Serializable.h"         // for Serializable
#include "xdata/String.h"               // for String
#include "xdata/Table.h"
#include "xdata/TableIterator.h"
#include "xdata/UnsignedInteger.h"      // for UnsignedInteger
#include "xdata/UnsignedInteger64.h"    // for UnsignedInteger64

// pugixml headers
#include "pugixml.hpp"          // for xml_node, xml_attribute


namespace swatch {
namespace xml {

const std::string TableSerializer::kColsTag = "columns";
const std::string TableSerializer::kTypesTag = "types";
const std::string TableSerializer::kRowsTag = "rows";
const std::string TableSerializer::kRowTag = "row";

TableSerializer::TableSerializer()
{
  // register simple serliazers
  this->addDefault(new xdata::UnsignedInteger());
  this->addDefault(new xdata::UnsignedInteger64());
  this->addDefault(new xdata::Integer());
  this->addDefault(new xdata::Float());
  this->addDefault(new xdata::Boolean());
  this->addDefault(new xdata::String());
}


TableSerializer::~TableSerializer()
{
  std::map<std::string, xdata::Serializable*>::iterator lIt;
  for ( lIt = mDefaults.begin(); lIt != mDefaults.end(); ++lIt) {
    if (lIt->second)
      delete lIt->second;
  }
}


void TableSerializer::addDefault(xdata::Serializable* aSerializable)
{
  mDefaults[aSerializable->type()] = aSerializable;
}


std::vector<std::string> TableSerializer::splitAndTrim(const std::string& aStr, const std::string aDelimiter)
{
  // Instantiate the container for the column definition
  std::vector<std::string> lTokens;

  // Split the string according to the chosen delimiter
  boost::split(lTokens, aStr, boost::is_any_of(aDelimiter));

  // trim whitespaces
  std::for_each(
    lTokens.begin(),
    lTokens.end(),
    boost::bind(boost::algorithm::trim<std::string>, _1, std::locale())
  );
  return lTokens;
}

/*
void TableSerializer::apply(const pugi::xml_node& aNode )
{

    // And the main nodes of the table
  pugi::xml_node lColsNode = aNode.child(kColsTag.c_str());
  pugi::xml_node lTypesNode = aNode.child(kTypesTag.c_str());
  pugi::xml_node lRows = aNode.child(kRowsTag.c_str());

  if ( !( lColsNode && lTypesNode && lRows) ) {
    std::ostringstream msg;
    msg << "Missing nodes: ";
    if ( !lColsNode ) msg << "'" << kColsTag << "' ";
    if ( !lTypesNode ) msg << "'" << kTypesTag << "' ";
    if ( !lRows ) msg << "'" << kRowsTag << "' ";

    XCEPT_RAISE(ValueError,msg.str());
  }

  uint32_t iRow(0);
  for (pugi::xml_node lKey = lRows.first_child(); lKey; lKey = lKey.next_sibling()) {
    if ( lKey.name() != kRowTag ) {
      std::ostringstream msg;
      msg << "Invalid tag <" << lKey.name() << "> found (child " << iRow << ").";
      XCEPT_RAISE(ValueError,msg.str());
    } else {
      ++iRow;
    }
  }
}*/

xdata::Serializable* TableSerializer::import(const pugi::xml_node& aNode)
{
//  aNode.print(std::cout, "", pugi::format_raw);std::cout << std::endl;

  // Get the delimiter
  pugi::xml_attribute lDelimiterAttr(aNode.attribute("delimiter"));
  std::string lDelimiter(lDelimiterAttr.empty() ? "," : lDelimiterAttr.value());
  // And the main nodes of the table
  pugi::xml_node lColsNode = aNode.child(kColsTag.c_str());
  pugi::xml_node lTypesNode = aNode.child(kTypesTag.c_str());
  pugi::xml_node lRows = aNode.child(kRowsTag.c_str());

//  apply(aNode);
  if ( !( lColsNode && lTypesNode && lRows) ) {
    std::ostringstream lMsg;
    lMsg << "Missing nodes: ";
    if ( !lColsNode )
      lMsg << "'" << kColsTag << "' ";
    if ( !lTypesNode )
      lMsg << "'" << kTypesTag << "' ";
    if ( !lRows )
      lMsg << "'" << kRowsTag << "' ";

    XCEPT_RAISE(ValueError,lMsg.str());
  }

  uint32_t lNrRows(0);
  for (pugi::xml_node lKey = lRows.first_child(); lKey; lKey = lKey.next_sibling()) {
    if ( lKey.name() != kRowTag ) {
      std::ostringstream lMsg;
      lMsg << "Invalid tag <" << lKey.name() << "> found (child " << lNrRows << ").";
      XCEPT_RAISE(ValueError,lMsg.str());
    }
    else {
      ++lNrRows;
    }
  }

  std::vector<std::string> lColNames = splitAndTrim(lColsNode.child_value(), lDelimiter);
  std::vector<std::string> lTypeNames = splitAndTrim(lTypesNode.child_value(), lDelimiter);


  if ( lColNames.size() != lTypeNames.size()) {
    std::ostringstream lMsg;
    lMsg << "Column/Types mismatch: n(columns)" << lColNames.size() << " n(types) " << lTypeNames.size();
    XCEPT_RAISE(ValueError,lMsg.str());
  }

  xdata::Table* lTable = new xdata::Table();
  lTable->reserve(lNrRows);

  std::vector<xdata::Serializable*> lColumnDefaults(lColNames.size());
  std::string lNormalizedType;
  for ( size_t lColIdx(0); lColIdx<lColNames.size(); ++lColIdx ) {
    lNormalizedType = normaliseType(lTypeNames[lColIdx]);
    lTable->addColumn(lColNames[lColIdx], lNormalizedType);
    lColumnDefaults[lColIdx] = mDefaults[lNormalizedType];
  }

  std::vector<std::string> lElements;
  lElements.reserve(lColNames.size());

  uint32_t lRowIdx(0);
  for (pugi::xml_node lKey = lRows.first_child(); lKey; lKey = lKey.next_sibling()) {

    lElements = splitAndTrim(lKey.child_value(),lDelimiter);

    if ( lColNames.size() != lElements.size()) {
      delete lTable;
      std::ostringstream lMsg;
      lMsg << "Column/Cell mismatchin in row " << lRowIdx << ": n(columns)" << lColNames.size() << " n(cells) " << lElements.size();
      XCEPT_RAISE(ValueError,lMsg.str());
    }

    xdata::Table::Row& lRow = *lTable->append();

    for ( size_t k(0); k<lColNames.size(); ++k) {
      lRow.setField(lColNames[k],*lColumnDefaults[k]);
      lRow.getField(lColNames[k])->fromString(lElements[k]);
    }

    ++lRowIdx;
  }

  return lTable;
}


const xdata::Serializable* TableSerializer::constImport(const pugi::xml_node& aNode)
{
  return const_cast<xdata::Serializable*>(import(aNode));
}


std::string TableSerializer::type() const
{
  xdata::Table lTmp;
  return lTmp.type();
}


// Copied from XmlSerializer
std::string TableSerializer::normaliseType(const std::string& aType)
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

} // namespace xml
} // namespace swatch

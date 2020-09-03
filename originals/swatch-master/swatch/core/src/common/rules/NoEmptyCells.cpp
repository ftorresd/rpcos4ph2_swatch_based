#include "swatch/core/rules/NoEmptyCells.hpp"

#include "swatch/core/utilities.hpp"

#include "xdata/Boolean.h"
#include "xdata/Integer.h"
#include "xdata/Integer32.h"
#include "xdata/Integer64.h"
#include "xdata/UnsignedInteger.h"
#include "xdata/UnsignedInteger32.h"
#include "xdata/UnsignedInteger64.h"
#include "xdata/UnsignedShort.h"
#include "xdata/UnsignedLong.h"
#include "xdata/Float.h"
#include "xdata/Double.h"
#include "xdata/String.h"
#include "xdata/Vector.h"
#include "xdata/Table.h"

#include <map>

namespace swatch {
namespace core {
namespace rules {

const size_t NoEmptyCells::kMaxColShown = 5;

// ----------------------------------------------------------------------------
void NoEmptyCells::describe(std::ostream& aStream) const
{
  aStream << "!any(x,isEmpty)";
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
template<typename T> 
bool isEmpty( const xdata::Serializable& aValue ) {
  return !dynamic_cast<const T&>(aValue).isFinite();  
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
template<> 
bool isEmpty<xdata::String>( const xdata::Serializable& aValue ) {
  return dynamic_cast<const xdata::String&>(aValue).value_.empty();  
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
template<typename T>
std::vector<uint32_t> findEmptyElements( const xdata::AbstractVector & aVec, size_t lRowCount ) {

  std::vector<uint32_t> lEmptyIndexes;
  const xdata::Vector<T>& lVec = dynamic_cast<const xdata::Vector<T>&>(aVec);
  for ( size_t i = 0; i < lRowCount; ++i ) {
    // If finite, the cell is full
    if ( !isEmpty<T>(lVec[i]) ) continue;

    // Otherwise mark it as empty
    lEmptyIndexes.push_back(i);
  }
  return lEmptyIndexes;
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
XMatch NoEmptyCells::verify ( const xdata::Table& aValue ) const{

  std::map<std::string, std::vector<uint32_t>> lEmptyCells;

  std::map<std::string, std::string> lUnsupportedColumnTypes;

  size_t lRowCount = aValue.getRowCount ();
  size_t lNumEmptyCells(0);

  for ( const auto& lCol : aValue.columnData_ ) {
    const std::string& lType = aValue.columnDefinitions_.at(lCol.first);
    std::vector<uint32_t> lEmpties;
    
    if ( lType == "unsigned long" ) {
      lEmpties = findEmptyElements<xdata::UnsignedLong>(*lCol.second, lRowCount);
    } else if ( lType == "unsigned int" ) {
      lEmpties = findEmptyElements<xdata::UnsignedInteger>(*lCol.second, lRowCount);
    } else if ( lType == "unsigned int 32" ) {
      lEmpties = findEmptyElements<xdata::UnsignedInteger32>(*lCol.second, lRowCount);
    } else if ( lType == "unsigned int 64" ) {
      lEmpties = findEmptyElements<xdata::UnsignedInteger64>(*lCol.second, lRowCount);
    } else if ( lType == "unsigned short" ) {
      lEmpties = findEmptyElements<xdata::UnsignedShort>(*lCol.second, lRowCount);
    } else if ( lType == "float" ) {
      lEmpties = findEmptyElements<xdata::Float>(*lCol.second, lRowCount);
    } else if ( lType == "int 32" ) {
      lEmpties = findEmptyElements<xdata::Integer32>(*lCol.second, lRowCount);
    } else if ( lType == "int 64" ) {
      lEmpties = findEmptyElements<xdata::Integer64>(*lCol.second, lRowCount);
    } else if ( lType == "int" ) {
      lEmpties = findEmptyElements<xdata::Integer>(*lCol.second, lRowCount);
    } else if ( lType == "bool" ) {
      lEmpties = findEmptyElements<xdata::Boolean>(*lCol.second, lRowCount);
    } else if ( lType == "double" ) {
      lEmpties = findEmptyElements<xdata::Double>(*lCol.second, lRowCount);
    } else if ( lType == "string") {
      lEmpties = findEmptyElements<xdata::String>(*lCol.second, lRowCount);
    } else {
      lUnsupportedColumnTypes[lCol.first] = lType;
    }

    if ( lEmpties.empty() ) continue;

    lNumEmptyCells += lEmpties.size();
    lEmptyCells[lCol.first].swap(lEmpties);
  
  }

  if ( lEmptyCells.empty() && lUnsupportedColumnTypes.empty())
    // All good, let's get out of here
    return XMatch(true);

  std::vector<std::string> lDetails;

  // Process unsupported column types first
  if ( !lUnsupportedColumnTypes.empty() ) {
    std::ostringstream lDetail;
    
    // Header
    lDetail << lUnsupportedColumnTypes.size() << " column(s) have unsupported types";
    if ( lUnsupportedColumnTypes.size() > kMaxColShown )
      lDetail << " (only first " << kMaxColShown << " columns shown)";
    lDetail << ". ";

    // Column counter
    size_t k(0);
    // Loop over columns
    for ( const auto& lEntry : lUnsupportedColumnTypes ) {
      lDetail << "'" << lEntry.first << "': " << lEntry.second;
      ++k;
      if ( k >= kMaxColShown || k >= lUnsupportedColumnTypes.size() ) break;
      lDetail << ", ";
    }

    // Turn . into ... if there are more columns
    if ( lUnsupportedColumnTypes.size() > kMaxColShown )
      lDetail << ", ...";
    else
      lDetail << ".";

    lDetails.push_back(lDetail.str());
  }

  // Continue with empty cells
  if ( !lEmptyCells.empty() ) {
    std::ostringstream lDetail;
    
    // Header
    lDetail << lNumEmptyCells << " empty cell(s) found in " << lEmptyCells.size() << " columns";
    if ( lEmptyCells.size() > kMaxColShown )
      lDetail << " (only first " << kMaxColShown << " columns shown)";
    lDetail << ". ";

    // Column counter
    size_t k(0);

    // Loop over columns
    for ( const auto& lEntry : lEmptyCells ) {
      lDetail << "'" << lEntry.first << "': " << core::shortVecFmt(lEntry.second);
      ++k;
      if ( k >= kMaxColShown || k >= lEmptyCells.size() ) break;
      lDetail << ", ";
    }

    // Turn . into ... if there are more columns
    if ( lEmptyCells.size() > kMaxColShown )
      lDetail << ", ...";
    else
      lDetail << ".";

    lDetails.push_back(lDetail.str());
  }

  return XMatch(false, join(lDetails));

}

} // namespace rules
} // namespace core
} // namespace swatch

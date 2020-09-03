#include "swatch/core/XParameterSet.hpp"


// C++ headers
#include <ostream>                      // for operator<<, basic_ostream
#include <typeinfo>

// boost headers
#include "boost/foreach.hpp"

// XDAQ headers
#include "xdata/Serializable.h"         // for Serializable
#include "xdata/String.h"

// SWATCH headers
#include "swatch/core/utilities.hpp"    // for demangleName


namespace swatch {
namespace core {


//---
XParameterSet::XParameterSet() {}


//---
XParameterSet::XParameterSet(const XParameterSet& aOther) {}


//---
XParameterSet::~XParameterSet() {}


//---
std::ostream& operator<< ( std::ostream& aStr , const swatch::core::XParameterSet& aXParameterSet )
{
  std::set<std::string> lKeys( aXParameterSet.keys() );

  aStr << '{';
  BOOST_FOREACH( const std::string& name, lKeys ) {
    const xdata::Serializable& lData = aXParameterSet.get(name);

    if (name != *lKeys.begin())
      aStr << ',';

    aStr << "\"" << name << "\":(<" << demangleName(typeid(lData).name()) << ">, ";

    if ( typeid(lData) == typeid(xdata::String) ) {
      aStr << "\"" << aXParameterSet.get(name).toString() << "\"";
    }
    else {
      aStr << aXParameterSet.get(name).toString();
    }
    aStr << ")";
  }

  aStr << '}';

  return aStr;
}

} // core
} // swatch


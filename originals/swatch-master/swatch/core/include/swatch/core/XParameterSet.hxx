/**
 * @file    XParameterSet.hxx
 * @author  Alessandro Thea
 * @date    February 2015
 */

#ifndef __SWATCH_CORE_XPARAMETERSET_HXX__
#define __SWATCH_CORE_XPARAMETERSET_HXX__


// IWYU pragma: private, include "swatch/core/XParameterSet.hpp"


#include <boost/type_traits/is_base_of.hpp>

// SWATCH headers
#include "swatch/core/XParameterSet.hpp"
#include "swatch/core/utilities.hpp"


namespace swatch {
namespace core {


//---
template<typename T>
const T& XParameterSet::get( const std::string& aName ) const
{
  BOOST_STATIC_ASSERT( (boost::is_base_of<xdata::Serializable,T>::value) );
  try {
    return dynamic_cast<const T&>( get(aName) );
  }
  catch ( const std::bad_cast& ) {
    const std::type_info* lFrom( &typeid(get(aName)) );
    const std::type_info* lTo( &typeid(T) );
    XCEPT_RAISE(XParameterFailedCast,"Unable to cast '"+aName+"' from '"+demangleName( lFrom->name() )+"' to '"+ demangleName( lTo->name() ) + "'" );
  }
}


} // namespace core
} // namespace swatch


#endif /* __SWATCH_CORE_XPARAMETERSET_HXX__ */

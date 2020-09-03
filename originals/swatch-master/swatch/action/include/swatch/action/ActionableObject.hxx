
#ifndef __SWATCH_ACTION_ACTIONABLEOBJECT_HXX__
#define __SWATCH_ACTION_ACTIONABLEOBJECT_HXX__

// IWYU pragma: private, include "swatch/action/ActionableObject.hpp"


#include <boost/type_traits/is_base_of.hpp>
#include <boost/static_assert.hpp>


namespace swatch {
namespace action {


template < typename T >
T& ActionableObject::registerCommand( const std::string& aId )
{
  BOOST_STATIC_ASSERT( (boost::is_base_of<swatch::action::Command,T>::value) );
  T* lObj( new T( aId, *this ) );
  registerCommand( aId , lObj );
  return *lObj;
}


template < typename T >
T& ActionableObject::registerCommand( const std::string& aId, const std::string& aAlias )
{
  BOOST_STATIC_ASSERT( (boost::is_base_of<swatch::action::Command,T>::value) );
  T* lObj( new T( aId, aAlias, *this ) );
  registerCommand( aId , lObj );
  return *lObj;
}


}
}

#endif  /* __SWATCH_ACTION_ACTIONABLEOBJECT_HXX__ */

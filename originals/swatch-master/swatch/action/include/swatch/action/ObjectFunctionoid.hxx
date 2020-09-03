
#ifndef __SWATCH_ACTION_OBJECTFUNCTIONOID_HXX__
#define __SWATCH_ACTION_OBJECTFUNCTIONOID_HXX__

// IWYU pragma: private, include "swatch/action/ObjectFunctionoid.hpp"


#include <boost/type_traits/is_base_of.hpp>
#include <boost/static_assert.hpp>


namespace swatch {
namespace action {


template<typename T>
const T& ObjectFunctionoid::getActionable() const
{
  BOOST_STATIC_ASSERT( (boost::is_base_of<swatch::action::ActionableObject,T>::value) );

  return dynamic_cast<const T&>( mActionable );
}


template<typename T>
T& ObjectFunctionoid::getActionable()
{
  BOOST_STATIC_ASSERT( (boost::is_base_of<swatch::action::ActionableObject,T>::value) );

  return dynamic_cast<T&>( mActionable );
}


}
}

#endif  /* __SWATCH_ACTION_OBJECTFUNCTIONOID_HXX__ */

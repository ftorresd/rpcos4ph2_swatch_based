/**
 * @file    ReadWriteXParameterSet.hxx
 * @author  Tom Williams, Alessandro Thea
 * @brief   Generic container for xdata::Serializable objects
 * @date    June 2015
 *
 */

#ifndef __SWATCH_CORE_READWRITEXPARAMETERSET_HXX__
#define __SWATCH_CORE_READWRITEXPARAMETERSET_HXX__


// IWYU pragma: private, include "swatch/core/ReadWriteXParameterSet.hpp"


// boost headers
#include <boost/type_traits/is_base_of.hpp>

// SWATCH headers
#include "swatch/core/ReadWriteXParameterSet.hpp"
#include "swatch/core/utilities.hpp"


namespace swatch {
namespace core {


//---
template<typename T>
void ReadWriteXParameterSet::adopt( const std::string& aName , const boost::shared_ptr<T>& aData )
{
  BOOST_STATIC_ASSERT( (boost::is_base_of<xdata::Serializable,T>::value) );
  /*
  if ( entries_.count(name) ) {
    XCEPT_RAISE(XParameterExists,name + " is already defined");
  }

  // Uncomment emplace when XDAQ moves to C++0X
  // entries_.emplace( name, &typeid(T), static_cast<XCloner>(cloner_<T>), data );
  entries_[ name ] = XEntry(&typeid(T), static_cast<XCloner>(cloner_<T>), data) ;
  */
  std::pair<EntryMap_t::iterator, bool> lIt = emplace( aName, &typeid(T), static_cast<XCloner_t>(clone<T>), boost::shared_ptr<xdata::Serializable>(aData) );
  // Throw if failed to emplace
  if ( !lIt.second )
    XCEPT_RAISE(XParameterExists, aName + " is already defined");

  //C+98 compliant
  /*
  // This is a dirty trick you should not look at. Needed because we cannot use emplace
  std::pair<EntryMap::iterator, bool> done = entries_.insert(std::make_pair(name, XEntry(&typeid(T), static_cast<XCloner>(cloner_<T>), static_cast<T*>(0x0))) );

  if ( not done.second ) {
      XCEPT_RAISE(XParameterExists,name + " is already defined");
  }

  entries_.find(name)->second.object = data;
  */
}


//---
template<typename T>
void ReadWriteXParameterSet::add( const std::string& aName , const T& aData )
{
  BOOST_STATIC_ASSERT( (boost::is_base_of<xdata::Serializable,T>::value) );

  XCloner_t lCloner = static_cast<XCloner_t>(clone<T>);
  T* lClone = static_cast<T*>(lCloner(&aData));

  adopt(aName, boost::shared_ptr<T>(lClone));
}


//---
template<typename T>
const T& ReadWriteXParameterSet::get( const std::string& aName ) const 
{
  return this->XParameterSet::get<T>(aName);
}

//---
template<typename T>
T& ReadWriteXParameterSet::get( const std::string& aName ) 
{
  return const_cast<T&>(this->XParameterSet::get<T>(aName));
}


//---
template<typename T>
xdata::Serializable* ReadWriteXParameterSet::clone( const xdata::Serializable* aOther )
{
  const T* lXOther = dynamic_cast<const T*>(aOther);
  return new T(*lXOther);
}


} // namespace core
} // namespace swatch


#endif /* __SWATCH_CORE_READWRITEXPARAMETERSET_HXX__ */

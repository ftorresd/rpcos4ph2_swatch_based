/**
 * @file    GenericView.hxx
 * @author  Luke Kreczko
 * @date    February 2016
 */

#ifndef __SWATCH_CORE_GENERICVIEW_HXX__
#define __SWATCH_CORE_GENERICVIEW_HXX__


// IWYU pragma: private, include "swatch/core/GateKeeperView.hpp"

#include <string>
#include "swatch/core/GenericView.hpp"


namespace swatch {
namespace core {

template<typename T>
GenericView<T>::GenericView(const T& aObject, const std::string& aObjectId, const std::string& aContext) :
  mObject(aObject), mObjectId(aObjectId), mContext(aContext)
{

}

template<typename T>
GenericView<T>::~GenericView()
{

}

template<typename T>
const std::string& GenericView<T>::getContext() const
{
  return mContext;
}

template<typename T>
const std::string& GenericView<T>::getObjectId() const
{
  return mObjectId;
}

template<typename T>
const T& GenericView<T>::getObject() const
{
  return mObject;
}

template<typename T>
bool GenericView<T>::operator==(const GenericView<T>& aOther) const
{
  // each object is uniquely identified by its context and ID
  return mContext == aOther.getContext() && mObjectId == aOther.getObjectId();
}

template<typename T>
std::size_t hash_value(GenericView<T> const& aGenericView)
{
  std::size_t lHash(0);
  boost::hash_combine(lHash, aGenericView.getContext());
  boost::hash_combine(lHash, aGenericView.getObjectId());

  return lHash;
}

} // namespace core
} // namespace swatch

#endif	/* __SWATCH_CORE_GENERICVIEW_HPP__ */


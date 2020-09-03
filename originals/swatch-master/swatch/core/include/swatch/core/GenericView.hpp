/**
 * @file    GenericView.hpp
 * @author  Luke Kreczko
 * @date    Feb 2016
 */

#ifndef __SWATCH_CORE_GENERICVIEW_HPP__
#define __SWATCH_CORE_GENERICVIEW_HPP__

#include <string>

namespace swatch {
namespace core {

/**
 * Class for a generic view of the GateKeeper quantities (parameters,
 * monitoring settings, masks and disable flags.
 */
template<typename T> class GenericView {
public:
  GenericView(const T& aObject, const std::string& aObjectId, const std::string& aContext = "none");
  ~GenericView();

  const std::string& getContext() const;
  const std::string& getObjectId() const;
  const T& getObject() const;

  // need comparison operator for the use of GenericView in a set
  bool operator==(const GenericView<T>& aOther) const;

private:
  T mObject;
  std::string mObjectId;
  std::string mContext;

};

/**
 * Hash function for the GenericView class so it can be stored in unordered_set
 */
template<typename T>
std::size_t hash_value(GenericView<T> const& aGenericView);

} // namespace core
} // namespace swatch

#include "swatch/core/GenericView.hxx"

#endif  /* __SWATCH_CORE_GENERICVIEW_HPP__ */


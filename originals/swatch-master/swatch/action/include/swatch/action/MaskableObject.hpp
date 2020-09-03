/*
 * @file    MaskableObject.hpp
 * @author  Tom Williams
 * @date    December 2015
 */

#ifndef __SWATCH_ACTION_MASKABLEOBJECT_HPP__
#define __SWATCH_ACTION_MASKABLEOBJECT_HPP__


#include <string>                       // for string

// SWATCH headers
#include "swatch/core/MonitorableObject.hpp"


namespace swatch {
namespace action {


class MaskableObject : public core::MonitorableObject {
public:

  explicit MaskableObject(const std::string& aId, const std::string& aAlias = "");

  bool isMasked() const;

  void setMasked(bool aMask=true);

private:
  bool mMasked;
};


} // namespace action
} // namespace swatch


#endif  /* SWATCH_ACTION_MASKABLEOBJECT_HPP */


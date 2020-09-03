/**
 * @file    ObjectFunctionoid.hpp
 * @author  Tom Williams
 * @date    May 2016
 *
 */

#ifndef __SWATCH_ACTION_OBJECTFUNCTIONOID_HPP__
#define __SWATCH_ACTION_OBJECTFUNCTIONOID_HPP__


#include <string>

// SWATCH headers
#include "swatch/action/Functionoid.hpp"


namespace swatch {
namespace action {


class ActionableObject;


class ObjectFunctionoid : public Functionoid {
public:
  virtual ~ObjectFunctionoid() { }

  const ActionableObject& getActionable() const;

  ActionableObject& getActionable();

  template<typename T> const T& getActionable() const ;

  template<typename T> T& getActionable();

protected:
  ObjectFunctionoid(const std::string& aId, const std::string& aAlias, ActionableObject& aActionable);

private:
  ActionableObject& mActionable;
};


} // namespace action
} // namespace swatch


#include "swatch/action/ObjectFunctionoid.hxx"


#endif  /* __SWATCH_ACTION_OBJECTFUNCTIONOID_HPP__ */


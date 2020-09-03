
#ifndef __SWATCH_ACTION_SYSTEMFUNCTIONOID_HPP__
#define __SWATCH_ACTION_SYSTEMFUNCTIONOID_HPP__


// Standard headers
#include <set>                          // for set
#include <string>                       // for string

// SWATCH headers
#include "swatch/action/Functionoid.hpp"


namespace swatch {
namespace action {

class ActionableObject;
class ActionableSystem;


class SystemFunctionoid : public Functionoid {
public:
  virtual ~SystemFunctionoid() { }

  const ActionableSystem& getActionable() const;

  ActionableSystem& getActionable();

  const std::set<ActionableObject*>& getParticipants();

protected:
  SystemFunctionoid(const std::string& aId, const std::string& aAlias, ActionableSystem& aActionable);

  //! Register a child of the system as participating in this functionoid (if child is already in participants list, method completes successfully but has no effect)
  void addParticipant(ActionableObject& aObj);

private:
  ActionableSystem& mActionable;
  std::set<ActionableObject*> mParticipants;
};


} // namespace action
} // namespace swatch

#endif  /* __SWATCH_ACTION_SYSTEMFUNCTIONOID_HPP__ */


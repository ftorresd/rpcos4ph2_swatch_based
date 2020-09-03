
#ifndef __SWATCH_ACTION_SYSTEMBUSYGUARD_HPP__
#define __SWATCH_ACTION_SYSTEMBUSYGUARD_HPP__


// Standard headers
#include <iosfwd>                       // for ostream
#include <map>                          // for map

// boost headers
#include "boost/function.hpp"
#include "boost/noncopyable.hpp"
#include "boost/smart_ptr/shared_ptr.hpp"  // for shared_ptr

// SWATCH headers
#include "swatch/action/ActionableStatus.hpp"
#include "swatch/action/ActionableSystem.hpp"


namespace swatch {
namespace action {

class ActionableObject;
class BusyGuard;
class SystemFunctionoid;


class SystemBusyGuard : public boost::noncopyable {
public:
  typedef boost::function<void(const ActionableStatusGuard&, std::ostream&)> Callback_t;

  SystemBusyGuard(SystemFunctionoid& aAction, ActionableSystem::StatusContainer& aStatusMap, const ActionableSystem::GuardMap_t& aStatusGuardMap, const Callback_t& aCallback);
  ~SystemBusyGuard();

  const BusyGuard& getChildGuard(const ActionableObject& aChild) const;

private:
  ActionableSystem& mSystem;
  ActionableStatus& mSysStatus;
  const SystemFunctionoid& mAction;
  const Callback_t mPostActionCallback;
  typedef boost::shared_ptr<const BusyGuard> ChildGuardPtr_t;
  std::map<const ActionableObject*, ChildGuardPtr_t> mChildGuardMap;
};


}
}

#endif  /* __SWATCH_ACTION_SYSTEMBUSYGUARD_HPP__ */


/**
 * @file    BusyGuard.hpp
 * @author  Tom Williams
 * @date    May 2016
 *
 */

#ifndef __SWATCH_ACTION_BUSYGUARD_HPP__
#define __SWATCH_ACTION_BUSYGUARD_HPP__


// Standard headers
#include <iosfwd>                       // for ostream
#include <stddef.h>                     // for NULL
#include <string>

// boost headers
#include "boost/function.hpp"
#include "boost/noncopyable.hpp"

// SWATCH headers
#include "swatch/action/ActionableStatus.hpp"
#include "swatch/core/exception.hpp"


namespace swatch {
namespace action {


class ActionableObject;
class Functionoid;
class ObjectFunctionoid;


class BusyGuard : public boost::noncopyable {
public:
  struct Adopt {};

  typedef boost::function<void(const ActionableStatusGuard&, std::ostream&)> Callback_t;

  BusyGuard(ObjectFunctionoid& aAction, ActionableStatus& aStatus, const BusyGuard* aOuterGuard=NULL);
  BusyGuard(ObjectFunctionoid& aAction, ActionableStatus& aStatus, ActionableStatusGuard& aStatusGuard, const Callback_t& aCallback, const BusyGuard* aOuterGuard=NULL);
  BusyGuard(ActionableObject& aResource, ActionableStatus& aStatus, ActionableStatusGuard& aStatusGuard, const Functionoid& aAction, const BusyGuard* aOuterGuard=NULL);
  BusyGuard(ActionableObject& aResource, ActionableStatus& aStatus, ActionableStatusGuard& aStatusGuard, const Functionoid& aAction, const Adopt);

  ~BusyGuard();

  struct ActionFmt {
  public:
    ActionFmt(const Functionoid* aAction);
    ~ActionFmt();

    std::string str() const;

    const Functionoid* const action;
  };

private:
  ActionableObject& mActionableObj;
  ActionableStatus& mStatus;
  const Functionoid& mAction;
  const BusyGuard* mOuterGuard;
  const Callback_t mPostActionCallback;

  void initialise(ActionableStatusGuard& aStatusGuard);
};


std::ostream& operator<<(std::ostream& aStream, const BusyGuard::ActionFmt& aActionFmt);


SWATCH_DEFINE_EXCEPTION(WrongBusyGuard)


} // namespace action
} // namespace swatch

#endif  /* __SWATCH_ACTION_BUSYGUARD_HPP__ */


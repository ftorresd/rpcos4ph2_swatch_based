#include "swatch/action/test/DummyCommand.hpp"


#include "boost/foreach.hpp"
#include "boost/chrono.hpp"

#include "xdata/Boolean.h"
#include "xdata/Integer.h"
#include "xdata/UnsignedInteger.h"
#include "xdata/String.h"

#include "swatch/action/test/DummyActionableObject.hpp"


namespace swatch {
namespace action {
namespace test {

DummyConstraint::DummyConstraint(const std::map<std::string, std::type_index>& aParameters)
{
  for (const auto& lItem : aParameters) {
    if (lItem.second == std::type_index(typeid(xdata::Integer)))
      require<xdata::Integer>(lItem.first);
    else if (lItem.second == std::type_index(typeid(xdata::UnsignedInteger)))
      require<xdata::UnsignedInteger>(lItem.first);
    else if (lItem.second == std::type_index(typeid(xdata::Float)))
      require<xdata::Float>(lItem.first);
    else if (lItem.second == std::type_index(typeid(xdata::String)))
      require<xdata::String>(lItem.first);
    else
      assert(false);
  }
}

const std::string DummyConstraint::sDescription = "Dummy constraint for unit tests";
core::XMatch DummyConstraint::sResult = true;



const xdata::Integer DummyCommand::kDefaultResult(-1);

const std::string DummyCommand::kParamX("x");
const std::string DummyCommand::kParamToDo("todo");

const std::string DummyCommand::kFinalMsgUseResource("Dummy command successfully used resource");
const std::string DummyCommand::kFinalMsgSleep("Dummy command finished sleeping");


DummyCommand::DummyCommand(const std::string& aId, ActionableObject& aActionable) :
  Command(aId, aActionable, kDefaultResult)
{
  registerParameter(kParamX, xdata::Integer(15), DummyXRule<xdata::Integer>());
  registerParameter(kParamToDo, xdata::String(""), DummyXRule<xdata::String>());

  addConstraint("constraintA", DummyConstraint({{DummyCommand::kParamX, std::type_index(typeid(xdata::Integer))}}));
  addConstraint("constraintB", DummyConstraint({{DummyCommand::kParamX, std::type_index(typeid(xdata::Integer))}, {DummyCommand::kParamToDo, std::type_index(typeid(xdata::String))}}));
}


DummyCommand::~DummyCommand()
{
}


void DummyCommand::setExecutionDetails(const core::ReadOnlyXParameterSet& aInfoSet)
{
  mNextExecutionDetails = aInfoSet;
}


Command::State DummyCommand::code(const core::XParameterSet& aParams)
{
  // Filling the detailed info XParameterSet: Fill up each entry with a dummy value, then reset to the requested value
  // (set each entry twice so unit tests will check that initial value is used )
  std::set<std::string> lItemNames = mNextExecutionDetails.keys();
  for (auto lIt=lItemNames.begin(); lIt != lItemNames.end(); lIt++) {
    this->addExecutionDetails(*lIt, xdata::Integer(-99));
    const xdata::Serializable& lDataItem = mNextExecutionDetails.get(*lIt);
    if (const xdata::Boolean* lBool = dynamic_cast<const xdata::Boolean*>(&lDataItem))
      addExecutionDetails(*lIt, xdata::Boolean(*lBool));
    else if (const xdata::UnsignedInteger* lUnsigned = dynamic_cast<const xdata::UnsignedInteger*>(&lDataItem))
      addExecutionDetails(*lIt, xdata::UnsignedInteger(*lUnsigned));
    else if (const xdata::Integer* lInteger = dynamic_cast<const xdata::Integer*>(&lDataItem))
      addExecutionDetails(*lIt, xdata::Integer(*lInteger));
    else if (const xdata::String* lString = dynamic_cast<const xdata::String*>(&lDataItem))
      addExecutionDetails(*lIt, xdata::String(*lString));
    else
      assert(false);
  }

  // Now actually use the resource ...
  DummyActionableObject& res = getActionable<DummyActionableObject>();

  std::string todo = aParams.get("todo").toString();
  setResult(aParams.get("x"));

  if (todo == "useResource") {

    res.setNumber(54);
    setProgress(0.1);

    setProgress(0.99, kFinalMsgUseResource);
    return State::kDone;
  }
  if (todo == "sleep") {
    setProgress(0.01, "Dummy command just started");
    unsigned int milliseconds(aParams.get<xdata::Integer>("milliseconds").value_);

    for (unsigned int i = 0; i < milliseconds; ++i) {
      boost::this_thread::sleep_for(boost::chrono::milliseconds(1));
      setProgress(0.01 + 0.99 * float(i) / milliseconds, "Dummy command progressed");
    }

    setStatusMsg(kFinalMsgSleep);
    return State::kDone;
  }
  else {
    return State::kDone;
  }
}


//-----------------------//
/*  DummyWarningCommand  */

const xdata::Integer DummyWarningCommand::kDefaultResult(-1);
const std::string DummyWarningCommand::kFinalMsg("Dummy command did something, but ended up in warning");

DummyWarningCommand::DummyWarningCommand(const std::string& aId, ActionableObject& aActionable) :
  Command(aId, aActionable, kDefaultResult)
{
}


DummyWarningCommand::~DummyWarningCommand()
{
}


void DummyWarningCommand::setExecutionDetails(const core::ReadOnlyXParameterSet& aInfoSet)
{
  mNextExecutionDetails = aInfoSet;
}


Command::State DummyWarningCommand::code(const core::XParameterSet& aParams)
{
  // Filling the detailed info XParameterSet: Fill up each entry with a dummy value, then reset to the requested value
  // (set each entry twice so unit tests will check that initial value is used )
  std::set<std::string> lItemNames = mNextExecutionDetails.keys();
  for (auto lIt=lItemNames.begin(); lIt != lItemNames.end(); lIt++) {
    this->addExecutionDetails(*lIt, xdata::Integer(-99));
    const xdata::Serializable& lDataItem = mNextExecutionDetails.get(*lIt);
    if (const xdata::Boolean* lBool = dynamic_cast<const xdata::Boolean*>(&lDataItem))
      addExecutionDetails(*lIt, xdata::Boolean(*lBool));
    else if (const xdata::UnsignedInteger* lUnsigned = dynamic_cast<const xdata::UnsignedInteger*>(&lDataItem))
      addExecutionDetails(*lIt, xdata::UnsignedInteger(*lUnsigned));
    else if (const xdata::Integer* lInteger = dynamic_cast<const xdata::Integer*>(&lDataItem))
      addExecutionDetails(*lIt, xdata::Integer(*lInteger));
    else if (const xdata::String* lString = dynamic_cast<const xdata::String*>(&lDataItem))
      addExecutionDetails(*lIt, xdata::String(*lString));
    else
      assert(false);
  }

  // Now actually use the resource ...
  setProgress(0.5049, kFinalMsg);
  return State::kWarning;
}



//---------------------//
/*  DummyErrorCommand  */

const xdata::Integer DummyErrorCommand::kDefaultResult(-1);
const std::string DummyErrorCommand::kFinalMsg("Dummy command did something, but ended up in error");
const float DummyErrorCommand::kFinalProgress(0.5049);

DummyErrorCommand::DummyErrorCommand(const std::string& aId, ActionableObject& aActionable) :
  Command(aId, aActionable, kDefaultResult)
{
}

DummyErrorCommand::~DummyErrorCommand()
{
}

void DummyErrorCommand::setExecutionDetails(const core::ReadOnlyXParameterSet& aInfoSet)
{
  mNextExecutionDetails = aInfoSet;
}

Command::State DummyErrorCommand::code(const core::XParameterSet& aParams)
{
  // Filling the detailed info XParameterSet: Fill up each entry with a dummy value, then reset to the requested value
  // (set each entry twice so unit tests will check that initial value is used )
  std::set<std::string> lItemNames = mNextExecutionDetails.keys();
  for (auto lIt=lItemNames.begin(); lIt != lItemNames.end(); lIt++) {
    this->addExecutionDetails(*lIt, xdata::Integer(-99));
    const xdata::Serializable& lDataItem = mNextExecutionDetails.get(*lIt);
    if (const xdata::Boolean* lBool = dynamic_cast<const xdata::Boolean*>(&lDataItem))
      addExecutionDetails(*lIt, xdata::Boolean(*lBool));
    else if (const xdata::UnsignedInteger* lUnsigned = dynamic_cast<const xdata::UnsignedInteger*>(&lDataItem))
      addExecutionDetails(*lIt, xdata::UnsignedInteger(*lUnsigned));
    else if (const xdata::Integer* lInteger = dynamic_cast<const xdata::Integer*>(&lDataItem))
      addExecutionDetails(*lIt, xdata::Integer(*lInteger));
    else if (const xdata::String* lString = dynamic_cast<const xdata::String*>(&lDataItem))
      addExecutionDetails(*lIt, xdata::String(*lString));
    else
      assert(false);
  }

  // Now actually use the resource ...
  setProgress(kFinalProgress, kFinalMsg);
  return State::kError;
}


//---------------------//
/*  DummyThrowCommand  */

const xdata::Integer DummyThrowCommand::kDefaultResult(-1);
const std::string DummyThrowCommand::kExceptionMsg("My test exception message");
const float DummyThrowCommand::kFinalProgress(0.4032);

DummyThrowCommand::DummyThrowCommand(const std::string& aId, ActionableObject& aActionable) :
  Command(aId, aActionable, kDefaultResult)
{
}

DummyThrowCommand::~DummyThrowCommand()
{
}

void DummyThrowCommand::setExecutionDetails(const core::ReadOnlyXParameterSet& aInfoSet)
{
  mNextExecutionDetails = aInfoSet;
}

Command::State DummyThrowCommand::code(const core::XParameterSet& aParams)
{
  // Filling the detailed info XParameterSet: Fill up each entry with a dummy value, then reset to the requested value
  // (set each entry twice so unit tests will check that initial value is used )
  std::set<std::string> lItemNames = mNextExecutionDetails.keys();
  for (auto lIt=lItemNames.begin(); lIt != lItemNames.end(); lIt++) {
    this->addExecutionDetails(*lIt, xdata::Integer(-99));
    const xdata::Serializable& lDataItem = mNextExecutionDetails.get(*lIt);
    if (const xdata::Boolean* lBool = dynamic_cast<const xdata::Boolean*>(&lDataItem))
      addExecutionDetails(*lIt, xdata::Boolean(*lBool));
    else if (const xdata::UnsignedInteger* lUnsigned = dynamic_cast<const xdata::UnsignedInteger*>(&lDataItem))
      addExecutionDetails(*lIt, xdata::UnsignedInteger(*lUnsigned));
    else if (const xdata::Integer* lInteger = dynamic_cast<const xdata::Integer*>(&lDataItem))
      addExecutionDetails(*lIt, xdata::Integer(*lInteger));
    else if (const xdata::String* lString = dynamic_cast<const xdata::String*>(&lDataItem))
      addExecutionDetails(*lIt, xdata::String(*lString));
    else
      assert(false);
  }

  // Now actually use the resource ...
  setProgress(kFinalProgress);
  XCEPT_RAISE(core::RuntimeError,kExceptionMsg);
}


} /* namespace test */
} /* namespace action */
} /* namespace swatch */

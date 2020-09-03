/*
 * TestCommand.cpp
 *
 *  Created on: August 2015
 *      Author: Tom Williams
 */

#include <boost/test/unit_test.hpp>
#include <boost/foreach.hpp>

// SWATCH headers
#include "swatch/action/Command.hpp"
#include "swatch/action/test/DummyCommand.hpp"
#include "swatch/action/test/DummyActionableObject.hpp"

//xdaq headers
#include <xdata/Boolean.h>
#include <xdata/Integer.h>
#include <xdata/String.h>


using namespace swatch::core;


namespace swatch {
namespace action {
namespace test {

struct CommandTestSetup {
  CommandTestSetup();
  ~CommandTestSetup() = default;

  boost::shared_ptr<DummyActionableObject> obj;
  Command& cmd;
  Command& warningCmd;
  Command& errorCmd;
  Command& throwCmd;
  core::ReadWriteXParameterSet params;
};

CommandTestSetup::CommandTestSetup():
  obj( new DummyActionableObject("dummyObj"), ActionableObject::Deleter() ),
  cmd( obj->registerCommand<DummyCommand>("dummy_cmd") ),
  warningCmd( obj->registerCommand<DummyWarningCommand>("dummy_warning_cmd") ),
  errorCmd( obj->registerCommand<DummyErrorCommand>("dummy_error_cmd") ),
  throwCmd( obj->registerCommand<DummyThrowCommand>("dummy_throw_cmd") )
{
}

//CommandTestSetup::~CommandTestSetup()
//{
//}


BOOST_AUTO_TEST_SUITE(CommandTestSuite)


BOOST_AUTO_TEST_CASE(TestConstruction)
{
  DummyActionableObject obj("dummy");
  swatch::action::Command& cmd = obj.registerCommand<DummyCommand>("myTestCommand");

  BOOST_CHECK_EQUAL(cmd.getId(), "myTestCommand");
  BOOST_CHECK_EQUAL(cmd.getPath(), obj.getId()+"."+cmd.getId());
  BOOST_CHECK_EQUAL(&obj.getCommand(cmd.getId()), &cmd);
  BOOST_CHECK_EQUAL(&obj.getObj(cmd.getId()), (Object*) &cmd);

  BOOST_CHECK_EQUAL(&cmd.getActionable(), &obj);
}


BOOST_FIXTURE_TEST_CASE(TestDefaultParams, CommandTestSetup)
{
  BOOST_CHECK_EQUAL(cmd.getDefaultParams().get<xdata::Integer>("x").value_ , 15);
  BOOST_CHECK_EQUAL(cmd.getDefaultParams().get<xdata::String>("todo").value_ , "");
}



BOOST_FIXTURE_TEST_CASE(TestUnregisterThrows, CommandTestSetup)
{
  BOOST_CHECK_THROW(cmd.unregisterParameter("dummy_cmd"), CommandParameterRegistrationFailed);
}


BOOST_FIXTURE_TEST_CASE(TestCommandInitialState,  CommandTestSetup)
{
  const std::vector<const Command*> cmds = { &cmd, &warningCmd, &errorCmd, &throwCmd};

  for (auto it=cmds.begin(); it != cmds.end(); it++) {
    BOOST_CHECK_EQUAL( (*it)->getState(), Functionoid::kInitial );

    CommandSnapshot s = (*it)->getStatus();
    BOOST_CHECK_EQUAL(s.getActionPath(), obj->getPath() + "." + (*it)->getId());
    BOOST_CHECK_EQUAL(s.getActionId(), (*it)->getId());
    BOOST_CHECK_EQUAL(s.getActionableId(), obj->getId());
    BOOST_CHECK_EQUAL(s.getState(), Functionoid::kInitial);
    BOOST_CHECK_EQUAL(s.getProgress(), 0.0);
    BOOST_CHECK_EQUAL(s.getRunningTime(), 0.0);
    BOOST_CHECK_EQUAL(s.getStatusMsg(), "initialised");
    BOOST_CHECK_EQUAL(s.getParameters().size(), size_t(0));
    BOOST_CHECK_EQUAL(s.getResult(), (const xdata::Serializable*)NULL);
    BOOST_CHECK_EQUAL(s.getExecutionDetails().size(), size_t(0));
  }
}


BOOST_FIXTURE_TEST_CASE(TestSuccessfulCommand,  CommandTestSetup)
{
  BOOST_CHECK_NE(obj->getNumber(), size_t(54));

  params.add("todo", xdata::String("useResource"));
  params.add("x", xdata::Integer(42));
  BOOST_REQUIRE_NE(params.get<xdata::Integer>("x").value_, DummyCommand::kDefaultResult.value_);
  core::ReadOnlyXParameterSet lExecutionDetails;
  lExecutionDetails.adopt("itemTheFirst", boost::shared_ptr<xdata::Boolean>(new xdata::Boolean(false)));
  lExecutionDetails.adopt("item2", boost::shared_ptr<xdata::Integer>(new xdata::Integer(-42)));
  lExecutionDetails.adopt("thirdThing", boost::shared_ptr<xdata::String>(new xdata::String("Hello World!")));
  dynamic_cast<DummyCommand&>(cmd).setExecutionDetails(lExecutionDetails);

  cmd.exec(params);
  do {
    boost::this_thread::sleep_for( boost::chrono::milliseconds(1) );
  }
  while ( (cmd.getState() == Functionoid::kScheduled) || (cmd.getState() == Functionoid::kRunning) );

  BOOST_CHECK_EQUAL(obj->getNumber(), uint32_t(54));

  CommandSnapshot s = cmd.getStatus();
  BOOST_CHECK_EQUAL(s.getActionPath(), obj->getPath() + "." + cmd.getId());
  BOOST_CHECK_EQUAL(s.getActionId(), cmd.getId());
  BOOST_CHECK_EQUAL(s.getActionableId(), obj->getId());
  BOOST_CHECK_EQUAL(s.getState(), Functionoid::kDone);
  BOOST_CHECK_EQUAL(s.getProgress(), 1.0);
  BOOST_CHECK_EQUAL(s.getStatusMsg(), DummyCommand::kFinalMsgUseResource);
  BOOST_CHECK_EQUAL(s.getParameters().size(), size_t(2));
  BOOST_CHECK_EQUAL(s.getParameters().get<xdata::Integer>("x").value_, 42);
  BOOST_CHECK_EQUAL(s.getParameters().get<xdata::String>("todo").value_, "useResource");
  BOOST_REQUIRE(s.getResult() != NULL);
  BOOST_CHECK_EQUAL(s.getResult()->toString(), params.get("x").toString());
  BOOST_CHECK_EQUAL(s.getExecutionDetails().size(), size_t(3));
  BOOST_CHECK_EQUAL(s.getExecutionDetails().get<xdata::Boolean>("itemTheFirst").value_, false);
  BOOST_CHECK_EQUAL(s.getExecutionDetails().get<xdata::Integer>("item2").value_, xdata::Integer(-42).value_);
  BOOST_CHECK_EQUAL(s.getExecutionDetails().get<xdata::String>("thirdThing").value_, "Hello World!");
}


BOOST_FIXTURE_TEST_CASE(TestCommandWarning,  CommandTestSetup)
{
  core::ReadOnlyXParameterSet lExecutionDetails;
  lExecutionDetails.adopt("itemTheFirst", boost::shared_ptr<xdata::Boolean>(new xdata::Boolean(false)));
  lExecutionDetails.adopt("item2", boost::shared_ptr<xdata::Integer>(new xdata::Integer(-42)));
  lExecutionDetails.adopt("thirdThing", boost::shared_ptr<xdata::String>(new xdata::String("Hello World!")));
  dynamic_cast<DummyWarningCommand&>(warningCmd).setExecutionDetails(lExecutionDetails);

  warningCmd.exec(params);
  do {
    boost::this_thread::sleep_for( boost::chrono::milliseconds(1) );
  }
  while ( (warningCmd.getState() == Functionoid::kScheduled) || (warningCmd.getState() == Functionoid::kRunning) );

  CommandSnapshot s = warningCmd.getStatus();
  BOOST_CHECK_EQUAL(s.getActionPath(), obj->getPath() + "." + warningCmd.getId());
  BOOST_CHECK_EQUAL(s.getActionId(), warningCmd.getId());
  BOOST_CHECK_EQUAL(s.getActionableId(), obj->getId());
  BOOST_CHECK_EQUAL(s.getState(), Functionoid::kWarning);
  BOOST_CHECK_EQUAL(s.getProgress(), 1.0);
  BOOST_CHECK_EQUAL(s.getStatusMsg(), DummyWarningCommand::kFinalMsg);
  BOOST_CHECK_EQUAL(s.getParameters().size(), size_t(0));
  BOOST_REQUIRE(s.getResult() != NULL);
  BOOST_CHECK_EQUAL(s.getResult()->toString(), boost::lexical_cast<std::string>(DummyWarningCommand::kDefaultResult.value_));
  BOOST_CHECK_EQUAL(s.getExecutionDetails().size(), size_t(3));
  BOOST_CHECK_EQUAL(s.getExecutionDetails().get<xdata::Boolean>("itemTheFirst").value_, false);
  BOOST_CHECK_EQUAL(s.getExecutionDetails().get<xdata::Integer>("item2").value_, xdata::Integer(-42).value_);
  BOOST_CHECK_EQUAL(s.getExecutionDetails().get<xdata::String>("thirdThing").value_, "Hello World!");
}


BOOST_FIXTURE_TEST_CASE(TestCommandError,  CommandTestSetup)
{
  core::ReadOnlyXParameterSet lExecutionDetails;
  lExecutionDetails.adopt("itemTheFirst", boost::shared_ptr<xdata::Boolean>(new xdata::Boolean(false)));
  lExecutionDetails.adopt("item2", boost::shared_ptr<xdata::Integer>(new xdata::Integer(-42)));
  lExecutionDetails.adopt("thirdThing", boost::shared_ptr<xdata::String>(new xdata::String("Hello World!")));
  dynamic_cast<DummyErrorCommand&>(errorCmd).setExecutionDetails(lExecutionDetails);

  errorCmd.exec(params);
  do {
    boost::this_thread::sleep_for( boost::chrono::milliseconds(1) );
  }
  while ( (errorCmd.getState() == Functionoid::kScheduled) || (errorCmd.getState() == Functionoid::kRunning) );

  CommandSnapshot s = errorCmd.getStatus();
  BOOST_CHECK_EQUAL(s.getActionPath(), obj->getPath() + "." + errorCmd.getId());
  BOOST_CHECK_EQUAL(s.getActionId(), errorCmd.getId());
  BOOST_CHECK_EQUAL(s.getActionableId(), obj->getId());
  BOOST_CHECK_EQUAL(s.getState(), Functionoid::kError);
  BOOST_CHECK_EQUAL(s.getProgress(), DummyErrorCommand::kFinalProgress);
  BOOST_CHECK_EQUAL(s.getStatusMsg(), DummyErrorCommand::kFinalMsg);
  BOOST_CHECK_EQUAL(s.getParameters().size(), size_t(0));
  BOOST_REQUIRE(s.getResult() != NULL);
  BOOST_CHECK_EQUAL(s.getResult()->toString(), boost::lexical_cast<std::string>(DummyErrorCommand::kDefaultResult.value_));
  BOOST_CHECK_EQUAL(s.getExecutionDetails().size(), size_t(3));
  BOOST_CHECK_EQUAL(s.getExecutionDetails().get<xdata::Boolean>("itemTheFirst").value_, false);
  BOOST_CHECK_EQUAL(s.getExecutionDetails().get<xdata::Integer>("item2").value_, xdata::Integer(-42).value_);
  BOOST_CHECK_EQUAL(s.getExecutionDetails().get<xdata::String>("thirdThing").value_, "Hello World!");
}


BOOST_FIXTURE_TEST_CASE(TestThrowingCommand,  CommandTestSetup)
{
  core::ReadOnlyXParameterSet lExecutionDetails;
  lExecutionDetails.adopt("itemTheFirst", boost::shared_ptr<xdata::Boolean>(new xdata::Boolean(false)));
  lExecutionDetails.adopt("item2", boost::shared_ptr<xdata::Integer>(new xdata::Integer(-42)));
  lExecutionDetails.adopt("thirdThing", boost::shared_ptr<xdata::String>(new xdata::String("Hello World!")));
  dynamic_cast<DummyThrowCommand&>(throwCmd).setExecutionDetails(lExecutionDetails);

  throwCmd.exec(params);
  do {
    boost::this_thread::sleep_for( boost::chrono::milliseconds(1) );
  }
  while ( (throwCmd.getState() == Functionoid::kScheduled) || (throwCmd.getState() == Functionoid::kRunning) );

  CommandSnapshot s = throwCmd.getStatus();
  BOOST_CHECK_EQUAL(s.getActionPath(), obj->getPath() + "." + throwCmd.getId());
  BOOST_CHECK_EQUAL(s.getActionId(), throwCmd.getId());
  BOOST_CHECK_EQUAL(s.getActionableId(), obj->getId());
  BOOST_CHECK_EQUAL(s.getState(), Functionoid::kError);
  BOOST_CHECK_EQUAL(s.getProgress(), DummyThrowCommand::kFinalProgress);
  BOOST_CHECK_EQUAL(s.getStatusMsg(), "An exception of type 'swatch::core::RuntimeError' was thrown in Command::code(): " + DummyThrowCommand::kExceptionMsg);
  BOOST_CHECK_EQUAL(s.getParameters().size(), size_t(0));
  BOOST_REQUIRE(s.getResult() != NULL);
  BOOST_CHECK_EQUAL(s.getResult()->toString(), boost::lexical_cast<std::string>(DummyThrowCommand::kDefaultResult.value_));
  BOOST_CHECK_EQUAL(s.getExecutionDetails().size(), size_t(3));
  BOOST_CHECK_EQUAL(s.getExecutionDetails().get<xdata::Boolean>("itemTheFirst").value_, false);
  BOOST_CHECK_EQUAL(s.getExecutionDetails().get<xdata::Integer>("item2").value_, xdata::Integer(-42).value_);
  BOOST_CHECK_EQUAL(s.getExecutionDetails().get<xdata::String>("thirdThing").value_, "Hello World!");
}


BOOST_FIXTURE_TEST_CASE(TestNullCommand,  CommandTestSetup)
{

}


BOOST_FIXTURE_TEST_CASE(TestCommandResetCorrectly,  CommandTestSetup)
{

}


//BOOST_FIXTURE_TEST_CASE(TestRunPrint,  CommandTestSetup) {
//  cmd.exec( params );
//  do {
//  } while ( (print.getState() == swatch::action::Command::kScheduled) || (print.getState() == swatch::action::Command::kRunning) );
//
//
//  CommandStatus status = print.getStatus();
//  BOOST_CHECK_EQUAL(status.getProgress(), 1.0);
//  BOOST_REQUIRE_EQUAL(status.getState(), Command::kDone);
//  BOOST_CHECK_EQUAL(status.getResult()->type(), "int");
//  xdata::Integer result(*dynamic_cast<const xdata::Integer*>(status.getResult()));
//  BOOST_CHECK(result.equals(xdata::Integer(99)));
//}
//
//BOOST_FIXTURE_TEST_CASE(TestRunNothing,  CommandTestSetup) {
//  nothing.exec( params );
//
//  do {
//  } while ( (nothing.getState() == swatch::action::Command::kScheduled) || (nothing.getState() == swatch::action::Command::kRunning) );
//
//  CommandStatus status = nothing.getStatus();
//  BOOST_CHECK_EQUAL(status.getProgress(), 1.0);
//  BOOST_CHECK_EQUAL(status.getState(), Command::kWarning);
//  BOOST_CHECK_EQUAL(status.getStatusMsg(), "Nothing was done");
//}
//
//BOOST_FIXTURE_TEST_CASE(TestRunError,  CommandTestSetup) {
//  error.exec( params );
//
//  do {
//  } while ( (error.getState() == swatch::action::Command::kScheduled) || (error.getState() == swatch::action::Command::kRunning) );
//
//  CommandStatus status = error.getStatus();
//  BOOST_CHECK_CLOSE(status.getProgress(), 0.5049, 0.0001);
//  BOOST_CHECK_EQUAL(status.getState(), Command::kError);
//  BOOST_CHECK_EQUAL(status.getStatusMsg(), "But ended up in error");
//}

// ----- N.B. Migrated from TestConstraint, may need cleanup --------------- // 

// ----------------------------------------------------------------------------
struct ConstraintTestSetup
{
  boost::shared_ptr<DummyActionableObject> obj;

  ConstraintTestSetup() :
    obj( new DummyActionableObject("dummyObj"), ActionableObject::Deleter() )  {
  }
};
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
class DummySumSmallerThan : public core::XPSetConstraint
{
public:
  DummySumSmallerThan( const std::string& a, const std::string& b, size_t max) :
    a(a),
    b(b),
    max(max)
  {
    require<xdata::UnsignedInteger>(a);
    require<xdata::UnsignedInteger>(b);
  }
  virtual ~DummySumSmallerThan() = default;

private:
  void describe(std::ostream& aStream) const override
  {
    aStream << "(" << a << " + " << b << ") < " << max;
  }
 
  XMatch verify(const core::XParameterSet& aParams) const override
  {
    // Need to check whether values are not NaN, since implicit cast is performed when adding variables, but 
    // ...  xdata::SimpleType implementation of implicit cast doesn't check if value is NaN
    if ( aParams.get<xdata::UnsignedInteger>(a).isNaN() || aParams.get<xdata::UnsignedInteger>(b).isNaN() )
      return false;
    else
      return ( aParams.get<xdata::UnsignedInteger>(a) + aParams.get<xdata::UnsignedInteger>(b) ) < max;
  }

  const std::string a;
  const std::string b;
  const size_t max;

};

// ----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(ConstraintRegistrationTest, ConstraintTestSetup) {

  // Instatntiate a dummy command
  DummyCommand aCommand("abc", *obj);

  // Add 2 commands
  BOOST_CHECK_NO_THROW(aCommand.registerParameter("first", xdata::UnsignedInteger(5)));
  BOOST_CHECK_NO_THROW(aCommand.registerParameter("second", xdata::UnsignedInteger(10)));

  // Add a parameter that doesn't satisfy the default uint rule
  BOOST_CHECK_THROW(aCommand.registerParameter("third", xdata::UnsignedInteger()), CommandParameterCheckFailed);

  // Add a constraint
  BOOST_CHECK_NO_THROW(aCommand.addConstraint("constr1",DummySumSmallerThan("first","second",20)));

  // Add a constraint with the same name
  BOOST_CHECK_THROW(aCommand.addConstraint("constr1",DummySumSmallerThan("first","second",5)), CommandConstraintRegistrationFailed);

  //
  BOOST_CHECK_THROW(aCommand.addConstraint("constr2",DummySumSmallerThan("first","second",5)), CommandConstraintRegistrationFailed);

  BOOST_CHECK_THROW(aCommand.unregisterParameter("second"), CommandParameterRegistrationFailed);

}

// ----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(RuntimeParameterCheckTest, ConstraintTestSetup) {


  DummyCommand aCommand("abc", *obj);
  aCommand.registerParameter("first", xdata::UnsignedInteger(5));
  aCommand.registerParameter("second", xdata::UnsignedInteger(10));
  aCommand.addConstraint("constr1",DummySumSmallerThan("first","second", 20));

  core::ReadWriteXParameterSet lParams;
  lParams.add("first", xdata::UnsignedInteger());
  lParams.add("second", xdata::UnsignedInteger());

  BOOST_CHECK_THROW(aCommand.exec(lParams), CommandParameterCheckFailed);

  lParams.erase("first");
  lParams.erase("second");
  lParams.add("first", xdata::UnsignedInteger(10));
  lParams.add("second", xdata::UnsignedInteger(15));

  BOOST_CHECK_THROW(aCommand.exec(lParams, false), CommandParameterCheckFailed);
  // ThreadPool::reset();

  // aCommand.exec(lParams);
  // do {
  //   boost::this_thread::sleep_for( boost::chrono::milliseconds(1) );
  // }
  // while ( (aCommand.getState() == Functionoid::kScheduled) || (aCommand.getState() == Functionoid::kRunning) );

}
// ----------------------------------------------------------------------------


BOOST_AUTO_TEST_SUITE_END() // CommandTestSuite

} /* namespace test */
} /* namespace action */
} /* namespace swatch */


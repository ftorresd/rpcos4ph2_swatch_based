/*
 * Command_test.cpp
 *
 *  Created on: 16 Feb 2015
 *      Author: kreczko
 */
#include <boost/test/unit_test.hpp>

// boost headers
#include "boost/smart_ptr/scoped_ptr.hpp"

// XDAQ headers
#include "xdata/UnsignedInteger.h"

// SWATCH headers
#include "swatch/core/ReadWriteXParameterSet.hpp"
#include "swatch/action/test/DummyActionableObject.hpp"
#include "swatch/action/test/DummyCommand.hpp"
#include "swatch/action/test/DummyGateKeeper.hpp"
#include "swatch/action/test/DummySleepCommand.hpp"
#include "swatch/action/StateMachine.hpp"


using namespace swatch::core;

namespace swatch {
namespace action {
namespace test {

struct ActionableObjectTestSetup {
  ActionableObjectTestSetup();
  ~ActionableObjectTestSetup();

  DummyActionableObject* handler;
  boost::scoped_ptr<Object::Deleter> deleter;
  StateMachine& testOp;
  Transition* transitionItoA;
  Transition* transitionAtoB;
  Transition* transitionBtoI;
  static const std::string kFSMInitialState;
  static const std::string kFSMStateA;
  static const std::string kFSMStateB;
};


ActionableObjectTestSetup::ActionableObjectTestSetup() :
  handler( new DummyActionableObject("dummyObj", "a dummy alias") ),
  deleter( new ActionableObject::Deleter() ),
  testOp( handler->registerStateMachine("myTestOp" , kFSMInitialState, "StateE") )
{
  handler->registerCommand<DummyCommand>("dummy_1");//, new DummyCommand(&handler));
  handler->registerCommand<DummyCommand>("dummy_2");//, new DummyCommand(&handler));
  handler->registerCommand<DummyCommand>("dummy_3");//, new DummyCommand(&handler));
  handler->registerCommand<DummySleepCommand>("sleep");
  handler->registerStateMachine("test_2", "0", "err");

  testOp.addState(kFSMStateA);
  testOp.addState(kFSMStateB);
  transitionItoA = & testOp.addTransition("t1", kFSMInitialState, kFSMStateA);
  transitionAtoB = & testOp.addTransition("t2", kFSMStateA, kFSMStateB);
  transitionBtoI = & testOp.addTransition("t3", kFSMStateB, kFSMInitialState);
}


ActionableObjectTestSetup::~ActionableObjectTestSetup()
{
  if (deleter.get() != NULL)
    (*deleter)(handler);
  else
    delete handler;
}

const std::string ActionableObjectTestSetup::kFSMInitialState = "s0";
const std::string ActionableObjectTestSetup::kFSMStateA = "sA";
const std::string ActionableObjectTestSetup::kFSMStateB = "sB";



BOOST_AUTO_TEST_SUITE( ActionableObjectTestSuite)


BOOST_FIXTURE_TEST_CASE(TestConstruction, ActionableObjectTestSetup)
{
  BOOST_CHECK_EQUAL(handler->getId(), "dummyObj");
  BOOST_CHECK_EQUAL(handler->getAlias(), "a dummy alias");

  // Check that status is correct
  ActionableObject::Status_t lStatus = handler->getStatus();
  BOOST_CHECK_EQUAL(lStatus.isAlive(), true);
  BOOST_CHECK_EQUAL(lStatus.isRunning(), false);
  BOOST_CHECK_EQUAL(lStatus.getRunningActions().empty(), true);
  BOOST_CHECK_EQUAL(lStatus.getFirstRunningActionOfType<Functionoid>(), (const Functionoid*) NULL);
  BOOST_CHECK_EQUAL(lStatus.getLastRunningAction(), (const Functionoid*) NULL);
  BOOST_CHECK_EQUAL(lStatus.isUpdatingMetrics(), false);

  BOOST_CHECK_EQUAL(lStatus.isEngaged(), false);
  BOOST_CHECK_EQUAL(lStatus.getStateMachineId(), ActionableObject::Status_t::kNullStateMachineId);
  BOOST_CHECK_EQUAL(lStatus.getState(), ActionableObject::Status_t::kNullStateId);
  BOOST_CHECK_EQUAL(lStatus.isEnabled(), true);
}

BOOST_FIXTURE_TEST_CASE(TestRegisterCommand,  ActionableObjectTestSetup)
{
  size_t n_commands = handler->getCommands().size();
  Command& registeredCmd = handler->registerCommand<DummyCommand>("dummy_5000");

  BOOST_CHECK_EQUAL(registeredCmd.getId(), "dummy_5000");
  BOOST_CHECK_EQUAL(registeredCmd.getPath(), handler->getId()+"."+registeredCmd.getId());

  size_t n_commands_after = handler->getCommands().size();
  BOOST_CHECK_EQUAL(n_commands_after, n_commands + 1);
  BOOST_CHECK_EQUAL( & handler->getCommand("dummy_5000"), & registeredCmd);
}


BOOST_FIXTURE_TEST_CASE(TestGetCommand,  ActionableObjectTestSetup)
{
  Command& dummy_1 = handler->getCommand("dummy_1");
  BOOST_CHECK_EQUAL(dummy_1.getState(), Functionoid::kInitial);
}


BOOST_FIXTURE_TEST_CASE(TestRegisterStateMachine,  ActionableObjectTestSetup)
{
  size_t n_fsm = handler->getStateMachines().size();
  StateMachine& registeredSM = handler->registerStateMachine("dummyOp_5000", "NULL", "ERR");

  BOOST_CHECK_EQUAL(registeredSM.getId(), "dummyOp_5000");
  BOOST_CHECK_EQUAL(registeredSM.getPath(), handler->getId()+"."+registeredSM.getId());

  size_t n_fsm_after = handler->getStateMachines().size();
  BOOST_CHECK_EQUAL(n_fsm_after, n_fsm + 1);
}


BOOST_FIXTURE_TEST_CASE(TestGetStateMachine,  ActionableObjectTestSetup)
{
  BOOST_CHECK_EQUAL( &testOp, & handler->getStateMachine("myTestOp"));
}


BOOST_FIXTURE_TEST_CASE(TestActionableIntialState,  ActionableObjectTestSetup)
{
  BOOST_CHECK( !handler->getStatus().isRunning() );
  BOOST_CHECK( handler->getStatus().isAlive() );
  BOOST_CHECK( !handler->getStatus().isEngaged() );
  BOOST_CHECK_EQUAL( handler->getStatus().getStateMachineId(), ActionableSnapshot::kNullStateMachineId);
  BOOST_CHECK_EQUAL( handler->getStatus().getState(), ActionableSnapshot::kNullStateId);
}


BOOST_AUTO_TEST_CASE(TestSafeDeletionOfBusyActionableObject)
{
  {
    ActionableObjectTestSetup setup;

    Command& sleepCommand = setup.handler->getCommand("sleep");

    ReadWriteXParameterSet params;
    params.add("n", xdata::UnsignedInteger(5000));
    params.add("millisecPerSleep", xdata::UnsignedInteger(5));

    sleepCommand.exec(params);

    boost::this_thread::sleep_for(boost::chrono::milliseconds(25));
  }

  // Sleep for 25ms after the actionable object has been destroyed, ...
  // ... in order to ensure that if the object has been deleted before the command has finished, ...
  // ... then the object is accessed by the Command::code method in the execution thread after the object's deletion ...
  // ... i.e. to reveal bug explained in ticket #1230
  boost::this_thread::sleep_for(boost::chrono::milliseconds(25));
  //swatch::logger::Log::setLogThreshold( lLogThr );
}


BOOST_AUTO_TEST_SUITE_END() // ProcessorTestSuite

} /* namespace test */
} /* namespace core */
} /* namespace swatch */


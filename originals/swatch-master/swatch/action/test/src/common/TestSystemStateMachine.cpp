
#include <boost/test/unit_test.hpp>


// SWATCH headers
#include "swatch/action/CommandSequence.hpp"
#include "swatch/action/StateMachine.hpp"
#include "swatch/action/SystemStateMachine.hpp"
#include "swatch/action/test/DummyActionableSystem.hpp"
#include "swatch/action/test/DummyActionableObject.hpp"
#include "swatch/action/test/DummyCommand.hpp"
#include "swatch/action/test/DummyGateKeeper.hpp"
#include "swatch/action/test/DummyMaskableObject.hpp"




namespace swatch {
namespace action {
namespace test {

struct SystemStateMachineTestSetup {
  SystemStateMachineTestSetup();
  ~SystemStateMachineTestSetup();

  void addMonSettingsToGateKeeper(const std::string& aStateId) // Copied from TestStateMachine.cpp
  {
    typedef boost::shared_ptr<MonitoringSetting> MonSettingPtr_t;
    // for child1
    GateKeeper::MonitoringSettings_t settings_child1 {
      {aStateId + ".monChild", MonSettingPtr_t(new MonitoringSetting("monChild", core::monitoring::kNonCritical))},
      {aStateId + ".monChild.grandChild1", MonSettingPtr_t(new MonitoringSetting("monChild.grandChild1", core::monitoring::kDisabled))},
      // this one should fail, as paths need to be relative to obj!
      {aStateId + ".grandChild2", MonSettingPtr_t(new MonitoringSetting("grandChild2", core::monitoring::kDisabled))},
      // for child1 metric
      {aStateId + ".monChild.dummyMetric", MonSettingPtr_t(new MonitoringSetting("monChild.dummyMetric", core::monitoring::kDisabled))}
    };
    gk.addSettingsContext(sys->getId()+".common", settings_child1);
  }


  struct Child {
    Child(DummyActionableObject& aObj);

    DummyActionableObject& obj;
    Command& cmdNormal1;
    Command& cmdNormal2;
    Command& cmdWarning;
    Command& cmdError;
    StateMachine& fsm;
    Transition* trItoA;
    Transition* trItoB;
    Transition* trAtoB;
    DummyActionableObject::MonChild& monChild;
    DummyActionableObject::MonChild& monGrandChild1;
    DummyActionableObject::MonChild& monGrandChild2;
    DummyMaskableObject& maskableA;
    DummyMaskableObject& maskableB;
    DummyMaskableObject& maskableC;
  };

  static std::string analyseError(const SystemTransitionSnapshot& aSnapshot);
  static std::string analyseWarning(const SystemTransitionSnapshot& aSnapshot);

  boost::shared_ptr<DummyActionableSystem> sys;
  SystemStateMachine& fsm;
  Child child1, child2, child3;
  std::vector<DummyActionableObject*> children;
  SystemTransition* sysItoA;
  SystemTransition* sysAtoB;
  DummyGateKeeper gk;

  static const std::string kChildState0, kChildStateErr, kChildStateA, kChildStateB;

  static const std::string kSysState0, kSysStateErr, kSysStateA, kSysStateB;
};



SystemStateMachineTestSetup::SystemStateMachineTestSetup() :
  sys(new DummyActionableSystem("dummySys"), ActionableSystem::Deleter()),
  fsm( sys->registerStateMachine("mySysFSM", kSysState0, kSysStateErr) ),
  child1( sys->addActionable(new DummyActionableObject("child1")) ),
  child2( sys->addActionable(new DummyActionableObject("child2")) ),
  child3( sys->addActionable(new DummyActionableObject("child3")) ),
  children{&child1.obj, &child2.obj, &child3.obj}
{
  fsm.addState(kSysStateA);
  fsm.addState(kSysStateB);
  sysItoA = &fsm.addTransition("sysIToA", kSysState0, kSysStateA);
  sysAtoB = &fsm.addTransition("sysAToB", kSysStateA, kSysStateB);

  typedef boost::shared_ptr<const xdata::Serializable> ParamPtr_t;
  GateKeeper::ParametersContext_t lTbl(new GateKeeper::Parameters_t{
    {DummyCommand::kParamToDo, ParamPtr_t(new xdata::String(""))},
    {DummyCommand::kParamX, ParamPtr_t(new xdata::Integer(42))}
  });
  gk.addContext(sys->getId()+".common", lTbl);

  typedef GateKeeper::Masks_t Masks_t;
  // Add mask for "maskableA", "child1.maskableB", and "maskableC"
  // "maskableC": FALSE ENTRY (i.e. should not take effect on child1.maskableC)
  //   - here to check that masks are applied using MaskableObject's ID path relative to the ActionableObject (not just using ID string)
  gk.addMasksContext(sys->getId()+".common", Masks_t{"maskableA", "monChild.maskableB", "maskableC"});

  DummyConstraint::setResult(true);
  DummyXRule<xdata::Integer>::setResult(true);
  DummyXRule<xdata::String>::setResult(true);
}

SystemStateMachineTestSetup::~SystemStateMachineTestSetup()
{
  DummyConstraint::setResult(true);
  DummyXRule<xdata::Integer>::setResult(true);
  DummyXRule<xdata::String>::setResult(true);
}

SystemStateMachineTestSetup::Child::Child(DummyActionableObject& aObj) :
  obj(aObj),
  cmdNormal1( obj.registerCommand<DummyCommand>("cmdNormal1") ),
  cmdNormal2( obj.registerCommand<DummyCommand>("cmdNormal2") ),
  cmdWarning( obj.registerCommand<DummyWarningCommand>("cmdWarning") ),
  cmdError( obj.registerCommand<DummyErrorCommand>("cmdError") ),
  fsm( obj.registerStateMachine("anFSM", kChildState0, kChildStateErr) ),
  trItoA(NULL),
  trItoB(NULL),
  trAtoB(NULL),
  monChild( obj.addMonitorable(new DummyActionableObject::MonChild("monChild", obj)) ),
  monGrandChild1( monChild.addMonitorable(new DummyActionableObject::MonChild("grandChild1", obj)) ),
  monGrandChild2( monChild.addMonitorable(new DummyActionableObject::MonChild("grandChild2", obj)) ),
  maskableA( obj.addMonitorable(new DummyMaskableObject("maskableA")) ),
  maskableB( monChild.addMonitorable(new DummyMaskableObject("maskableB")) ),
  maskableC( monChild.addMonitorable(new DummyMaskableObject("maskableC")) )
{
  fsm.addState(kChildStateA);
  fsm.addState(kChildStateB);
  trItoA = & fsm.addTransition("initialToA", kChildState0, kChildStateA);
  trItoB = & fsm.addTransition("initialToB", kChildStateB, kChildStateA);
  trAtoB = & fsm.addTransition("AtoB", kChildStateA, kChildStateB);
}

std::string SystemStateMachineTestSetup::analyseError(const SystemTransitionSnapshot& aSnapshot)
{
  std::ostringstream lMsg;
  lMsg << "Error analysis: Execution time " << aSnapshot.getRunningTime() << "s, " << aSnapshot.getNumberOfCompletedSteps() << " steps completed, progress " << aSnapshot.getProgress();
  return lMsg.str();
}

std::string SystemStateMachineTestSetup::analyseWarning(const SystemTransitionSnapshot& aSnapshot)
{
  std::ostringstream lMsg;
  lMsg << "Warning analysis: Execution time " << aSnapshot.getRunningTime() << "s, " << aSnapshot.getNumberOfCompletedSteps() << " steps completed, progress " << aSnapshot.getProgress();
  return lMsg.str();
}


const std::string SystemStateMachineTestSetup::kChildState0 = "s0";
const std::string SystemStateMachineTestSetup::kChildStateErr = "sE";
const std::string SystemStateMachineTestSetup::kChildStateA = "sA";
const std::string SystemStateMachineTestSetup::kChildStateB = "sB";

const std::string SystemStateMachineTestSetup::kSysState0 = "state0";
const std::string SystemStateMachineTestSetup::kSysStateErr = "error";
const std::string SystemStateMachineTestSetup::kSysStateA = "stateA";
const std::string SystemStateMachineTestSetup::kSysStateB = "stateB";



BOOST_AUTO_TEST_SUITE( SystemStateMachineTestSuite )



/* ------------------------------------------------------------------------- */
/*   PART 1: CONSTRUCTION & INITIALISATION (defining states & transitions)   */
/* ------------------------------------------------------------------------- */

BOOST_AUTO_TEST_CASE(TestConstruction)
{
  DummyActionableSystem sys("dummySys");
  swatch::action::SystemStateMachine& fsm = sys.registerStateMachine("aFSM", "someState", "myErrState");

  BOOST_CHECK_EQUAL(&fsm.getActionable(), &sys);
  BOOST_CHECK_EQUAL(fsm.getInitialState(), "someState");
  BOOST_CHECK_EQUAL(fsm.getErrorState(), "myErrState");
  BOOST_CHECK_EQUAL( &sys.getObj(fsm.getId()), (core::Object*) &fsm);
  BOOST_CHECK_EQUAL( fsm.getParticipants().size(), size_t(0));

  std::vector<std::string> expectedStates = {"someState", "myErrState"};
  BOOST_CHECK_EQUAL_COLLECTIONS( fsm.getStates().begin(), fsm.getStates().end(), expectedStates.begin(), expectedStates.end() );
}


BOOST_AUTO_TEST_CASE(TestAddState)
{
  DummyActionableSystem sys("dummySys");
  swatch::action::SystemStateMachine& fsm = sys.registerStateMachine("aFSM", "someState", "myErrState");

  std::vector<std::string> expectedStates = {"someState", "myErrState"};
  BOOST_REQUIRE_EQUAL_COLLECTIONS( fsm.getStates().begin(), fsm.getStates().end(), expectedStates.begin(), expectedStates.end() );

  // addState should throw if state already defined
  BOOST_CHECK_THROW(fsm.addState("someState"), swatch::action::StateAlreadyDefined);
  BOOST_CHECK_THROW(fsm.addState("myErrState"), swatch::action::StateAlreadyDefined);

  // addState should add the state if not already defined
  BOOST_CHECK_NO_THROW(fsm.addState("anotherState"));
  expectedStates.push_back("anotherState");
  BOOST_CHECK_EQUAL_COLLECTIONS( fsm.getStates().begin(), fsm.getStates().end(), expectedStates.begin(), expectedStates.end() );
}


BOOST_AUTO_TEST_CASE(TestAddTransition)
{
  DummyActionableSystem sys("dummySys");
  swatch::action::SystemStateMachine& fsm = sys.registerStateMachine("aFSM", "state0", "errState");
  fsm.addState("state1");

  BOOST_CHECK_EQUAL( fsm.getTransitions("state0").size(), size_t(0) );
  BOOST_CHECK_EQUAL( fsm.getTransitions("state1").size(), size_t(0) );

  // 1) Add transition
  SystemTransition& tA = fsm.addTransition("transitionA", "state0", "state1");

  BOOST_CHECK_EQUAL(tA.getId(), "transitionA");
  BOOST_CHECK_EQUAL(tA.getPath(), fsm.getPath()+".state0.transitionA");
  BOOST_CHECK_EQUAL(tA.getStartState(), "state0");
  BOOST_CHECK_EQUAL(tA.getEndState(), "state1");
  BOOST_CHECK_EQUAL(&tA.getStateMachine(), &fsm);

  SystemTransitionSnapshot s = tA.getStatus();
  BOOST_CHECK_EQUAL( s.getActionPath(), tA.getPath() );
  BOOST_CHECK_EQUAL( s.getActionId(), tA.getId());
  BOOST_CHECK_EQUAL( s.getActionableId(), sys.getId());
  BOOST_CHECK_EQUAL( s.getState(), Functionoid::State::kInitial );
  BOOST_CHECK_EQUAL( s.getProgress(), 0.0 );
  BOOST_CHECK_EQUAL( s.getRunningTime(), 0.0);
  BOOST_CHECK_EQUAL( s.getMessage(), "");
  BOOST_CHECK( s.begin() == s.end());
  BOOST_CHECK_EQUAL( s.end() - s.begin(), 0);
  BOOST_CHECK_THROW( s.at(0), std::out_of_range);
  BOOST_CHECK_EQUAL( s.getNumberOfCompletedSteps(), size_t(0));
  BOOST_CHECK_EQUAL( s.getTotalNumberOfSteps(), size_t(0));
  BOOST_CHECK_EQUAL( s.getEnabledChildren().size(), size_t(0));

  BOOST_CHECK_EQUAL(fsm.getTransitions("state0").size(), size_t(1));
  BOOST_CHECK_EQUAL(fsm.getTransitions("state0").find("transitionA")->second, &tA);
  BOOST_CHECK_EQUAL(fsm.getTransitions("state1").size(), size_t(0));

  // 2) Check that exception is thrown in each error case
  BOOST_CHECK_THROW( fsm.addTransition("newId", "s", "state1"), StateNotDefined);
  BOOST_CHECK_THROW( fsm.addTransition("newId", "state1", "s"), StateNotDefined);
  BOOST_CHECK_EQUAL( fsm.getTransitions("state1").size(), size_t(0));
  fsm.addState("state2");
  BOOST_CHECK_THROW( fsm.addTransition("transitionA", "state0", "state2"), TransitionAlreadyDefined);
  BOOST_CHECK_EQUAL( fsm.getTransitions("state0").size(), size_t(1));
}


BOOST_FIXTURE_TEST_CASE(TestAddTransitionSteps, SystemStateMachineTestSetup)
{
  // Confirm expected construction before starting tests
  BOOST_REQUIRE_EQUAL(fsm.getParticipants().size(), size_t(0));
  BOOST_REQUIRE_EQUAL(sysItoA->size(), size_t(0));
  BOOST_REQUIRE_EQUAL(sysAtoB->size(), size_t(0));

  std::vector<Transition*> transitions;

  // 1) Adding empty step: should throw, leaving transition unchanged
  BOOST_CHECK_THROW( sysItoA->add(transitions), InvalidSystemTransition);
  BOOST_CHECK_EQUAL( sysItoA->size(), size_t(0));
  BOOST_CHECK( sysItoA->begin() == sysItoA->end());
  BOOST_CHECK_EQUAL(fsm.getParticipants().size(), size_t(0));

  // 2) Adding transition vector containing NULL ptr: should throw, leaving transition unchanged
  BOOST_CHECK_THROW( sysItoA->add(std::vector<Transition*> {NULL}), InvalidSystemTransition);
  BOOST_CHECK_EQUAL( sysItoA->size(), size_t(0));
  BOOST_CHECK( sysItoA->begin() == sysItoA->end());
  BOOST_CHECK_EQUAL(fsm.getParticipants().size(), size_t(0));

  // 3) Adding step for non-child: should throw & leave transition unchanged
  DummyActionableObject nonChildObj("nonChildObj");
  StateMachine& nonChildFSM = nonChildObj.registerStateMachine("someFSM", "state0", "ERROR");
  nonChildFSM.addState("state1");
  transitions = { &nonChildFSM.addTransition("t", "state0", "state1") };
  BOOST_CHECK_THROW( sysItoA->add(transitions), InvalidSystemTransition);
  BOOST_CHECK_EQUAL( sysItoA->size(), size_t(0));
  BOOST_CHECK( sysItoA->begin() == sysItoA->end());
  BOOST_CHECK_EQUAL(fsm.getParticipants().size(), size_t(0));

  // 4) Adding step with two transitions from same child: should throw & leave transition unchanged
  transitions = { child1.trItoA , child1.trItoB };
  BOOST_CHECK_THROW( sysItoA->add(transitions), InvalidSystemTransition);
  BOOST_CHECK_EQUAL( sysItoA->size(), size_t(0));
  BOOST_CHECK( sysItoA->begin() == sysItoA->end());
  BOOST_CHECK_EQUAL(fsm.getParticipants().size(), size_t(0));

  // 5) Add valid step (just child1), using vector
  std::set<const StateMachine*> expectedParticipants = {&child1.fsm};
  sysItoA->add( std::vector<Transition*> {child1.trItoA} );
  BOOST_CHECK_EQUAL_COLLECTIONS( fsm.getParticipants().begin(), fsm.getParticipants().end(), expectedParticipants.begin(), expectedParticipants.end());
  BOOST_REQUIRE_EQUAL( sysItoA->size(), size_t(1));
  BOOST_REQUIRE( (sysItoA->begin() + 1) == sysItoA->end() );
  BOOST_REQUIRE_EQUAL( sysItoA->begin()->get().size(), size_t(1));
  BOOST_CHECK_EQUAL( sysItoA->begin()->get().at(0), child1.trItoA);
  BOOST_REQUIRE_EQUAL( sysItoA->begin()->cget().size(), size_t(1));
  BOOST_CHECK_EQUAL( sysItoA->begin()->cget().at(0), child1.trItoA);

  // 6) Add another valid step (child2 & child3), using iterators to children this time
  sysItoA->add(children.begin()+1, children.end(), child1.fsm.getId(), kChildState0, child1.trItoA->getId());
  expectedParticipants = {&child1.fsm, &child2.fsm, &child3.fsm};
  BOOST_CHECK_EQUAL_COLLECTIONS( fsm.getParticipants().begin(), fsm.getParticipants().end(), expectedParticipants.begin(), expectedParticipants.end());
  BOOST_REQUIRE_EQUAL( sysItoA->size(), size_t(2));
  BOOST_REQUIRE_EQUAL( (sysItoA->end() - sysItoA->begin()), 2 );
  SystemTransition::iterator lIt = sysItoA->begin() + 1;
  BOOST_REQUIRE_EQUAL( lIt->get().size(), size_t(2));
  BOOST_CHECK_EQUAL( lIt->get().at(0), child2.trItoA);
  BOOST_CHECK_EQUAL( lIt->get().at(1), child3.trItoA);
  BOOST_REQUIRE_EQUAL( lIt->cget().size(), size_t(2));
  BOOST_CHECK_EQUAL( lIt->cget().at(0), child2.trItoA);
  BOOST_CHECK_EQUAL( lIt->cget().at(1), child3.trItoA);

  // 7) Adding step which is inconsistent with previous steps (start/end states of children don't match)
  //    ... should throw & leave transition unchanged
  BOOST_CHECK_THROW(sysItoA->add(std::vector<Transition*> {child1.trItoB}), InvalidSystemTransition);
  BOOST_CHECK_EQUAL( sysItoA->size(), size_t(2));
  BOOST_CHECK_EQUAL( sysItoA->end() - sysItoA->begin(), 2);

  // 8) Adding step with child transition from different child FSM than used in other steps
  //    ... should throw & leave transition unchanged
  StateMachine& child1otherFSM = child1.obj.registerStateMachine("anotherFSM", kChildState0, kChildStateErr);
  child1otherFSM.addState(kChildStateA);
  Transition& child1otherTransition = child1otherFSM.addTransition(child1.trItoA->getId(), kChildState0, kChildStateA);
  SystemTransition& t = fsm.addTransition("freshTransition", kSysState0, kSysStateA);
  transitions = {&child1otherTransition};
  BOOST_CHECK_THROW( t.add(transitions) , InvalidSystemTransition);
  BOOST_CHECK_EQUAL( sysItoA->size(), size_t(2));
  BOOST_CHECK_EQUAL( sysItoA->end() - sysItoA->begin(), 2);
}



/* ---------------------------------------------------------------- */
/*   PART 2: ENGAGING & DISENGAGING                                 */
/* ---------------------------------------------------------------- */

BOOST_FIXTURE_TEST_CASE(TestEngageFSM, SystemStateMachineTestSetup)
{
  // Finish off setup: Add child1 & child2 to state machine; child3 is left out
  sysItoA->add(children.begin(), children.end()-1, child1.fsm.getId(), kChildState0, child1.trItoA->getId());

  // Confirm correct construction before staring tests
  BOOST_REQUIRE_EQUAL( sys->getStatus().getStateMachineId(), ActionableSnapshot::kNullStateMachineId );
  BOOST_REQUIRE_EQUAL( sys->getStatus().getStateMachineId(), ActionableSnapshot::kNullStateMachineId );
  BOOST_REQUIRE_EQUAL( sys->getStatus().getState(), ActionableSnapshot::kNullStateId );
  BOOST_REQUIRE(fsm.getParticipants() == (std::set<const StateMachine*> {&child1.fsm, &child2.fsm}));

  // Engaging FSM: should put system & children into FSM's initial state
  BOOST_CHECK_NO_THROW( fsm.engage(DummyGateKeeper()) );
  BOOST_CHECK_EQUAL( sys->getStatus().getStateMachineId(), fsm.getId() );
  BOOST_CHECK_EQUAL( sys->getStatus().getState(), kSysState0 );
  BOOST_CHECK_EQUAL( child1.obj.getStatus().getStateMachineId(), child1.fsm.getId());
  BOOST_CHECK_EQUAL( child1.obj.getStatus().getState(), kChildState0);
  BOOST_CHECK_EQUAL( child2.obj.getStatus().getStateMachineId(), child2.fsm.getId());
  BOOST_CHECK_EQUAL( child2.obj.getStatus().getState(), kChildState0);
  BOOST_CHECK_EQUAL( child3.obj.getStatus().getStateMachineId(), ActionableSnapshot::kNullStateMachineId);
  BOOST_CHECK_EQUAL( child3.obj.getStatus().getState(), ActionableSnapshot::kNullStateId);

  // Trying to engage FSM again: should throw, and leave
  BOOST_CHECK_THROW( fsm.engage(DummyGateKeeper()), ResourceInWrongStateMachine );
  BOOST_CHECK_EQUAL( sys->getStatus().getStateMachineId(), fsm.getId() );
  BOOST_CHECK_EQUAL( sys->getStatus().getState(), kSysState0 );
  BOOST_CHECK_EQUAL( child1.obj.getStatus().getStateMachineId(), child1.fsm.getId());
  BOOST_CHECK_EQUAL( child1.obj.getStatus().getState(), kChildState0);
  BOOST_CHECK_EQUAL( child2.obj.getStatus().getStateMachineId(), child2.fsm.getId());
  BOOST_CHECK_EQUAL( child2.obj.getStatus().getState(), kChildState0);
  BOOST_CHECK_EQUAL( child3.obj.getStatus().getStateMachineId(), ActionableSnapshot::kNullStateMachineId);
  BOOST_CHECK_EQUAL( child3.obj.getStatus().getState(), ActionableSnapshot::kNullStateId);

  // Trying to engage another FSM also shouldn't work
  SystemStateMachine& anotherFSM = sys->registerStateMachine("anotherFSM", "otherInitialState", "otherErrorState");
  BOOST_CHECK_THROW( anotherFSM.engage(DummyGateKeeper()), ResourceInWrongStateMachine );
  BOOST_CHECK_EQUAL( sys->getStatus().getStateMachineId(), fsm.getId() );
  BOOST_CHECK_EQUAL( sys->getStatus().getState(), kSysState0 );
  BOOST_CHECK_EQUAL( child1.obj.getStatus().getStateMachineId(), child1.fsm.getId());
  BOOST_CHECK_EQUAL( child1.obj.getStatus().getState(), kChildState0);
  BOOST_CHECK_EQUAL( child2.obj.getStatus().getStateMachineId(), child2.fsm.getId());
  BOOST_CHECK_EQUAL( child2.obj.getStatus().getState(), kChildState0);
  BOOST_CHECK_EQUAL( child3.obj.getStatus().getStateMachineId(), ActionableSnapshot::kNullStateMachineId);
  BOOST_CHECK_EQUAL( child3.obj.getStatus().getState(), ActionableSnapshot::kNullStateId);
}


BOOST_FIXTURE_TEST_CASE(TestEngageFSMChildComplication, SystemStateMachineTestSetup)
{
  // Finish off setup: Add child1 & child2 to state machine; child3 is left out
  sysItoA->add(children.begin(), children.end()-1, child1.fsm.getId(), kChildState0, child1.trItoA->getId());

  // Confirm correct construction before staring tests
  BOOST_REQUIRE_EQUAL( sys->getStatus().getStateMachineId(), ActionableSnapshot::kNullStateMachineId );
  BOOST_REQUIRE_EQUAL( sys->getStatus().getState(), ActionableSnapshot::kNullStateId );
  BOOST_REQUIRE(fsm.getParticipants() == (std::set<const StateMachine*> {&child1.fsm, &child2.fsm}));

  // Engaging FSM when a child is already engaged: should throw, and have no effect
  child2.fsm.engage(DummyGateKeeper());
  BOOST_CHECK_THROW( fsm.engage(DummyGateKeeper()), ResourceInWrongStateMachine );
  BOOST_CHECK_EQUAL( sys->getStatus().getStateMachineId(), ActionableSnapshot::kNullStateMachineId );
  BOOST_CHECK_EQUAL( sys->getStatus().getState(), ActionableSnapshot::kNullStateId );
  BOOST_CHECK_EQUAL( child1.obj.getStatus().getStateMachineId(), ActionableSnapshot::kNullStateMachineId);
  BOOST_CHECK_EQUAL( child1.obj.getStatus().getState(), ActionableSnapshot::kNullStateId);
  BOOST_CHECK_EQUAL( child2.obj.getStatus().getStateMachineId(), child2.fsm.getId());
  BOOST_CHECK_EQUAL( child2.obj.getStatus().getState(), kChildState0);
  BOOST_CHECK_EQUAL( child3.obj.getStatus().getStateMachineId(), ActionableSnapshot::kNullStateMachineId);
  BOOST_CHECK_EQUAL( child3.obj.getStatus().getState(), ActionableSnapshot::kNullStateId);

  // Engaging FSM after disengaging child2 & engaging child3 FSM:
  // ... should put system & children into FSM's initial state
  child2.fsm.disengage();
  child3.fsm.engage(DummyGateKeeper());
  BOOST_CHECK_NO_THROW( fsm.engage(DummyGateKeeper()) );
  BOOST_CHECK_EQUAL( sys->getStatus().getStateMachineId(), fsm.getId() );
  BOOST_CHECK_EQUAL( sys->getStatus().getState(), kSysState0 );
  BOOST_CHECK_EQUAL( child1.obj.getStatus().getStateMachineId(), child1.fsm.getId());
  BOOST_CHECK_EQUAL( child1.obj.getStatus().getState(), kChildState0);
  BOOST_CHECK_EQUAL( child2.obj.getStatus().getStateMachineId(), child2.fsm.getId());
  BOOST_CHECK_EQUAL( child2.obj.getStatus().getState(), kChildState0);
  BOOST_CHECK_EQUAL( child3.obj.getStatus().getStateMachineId(), child3.fsm.getId());
  BOOST_CHECK_EQUAL( child3.obj.getStatus().getState(), kChildState0);
}


BOOST_FIXTURE_TEST_CASE(TestDisengageFSM, SystemStateMachineTestSetup)
{
  // Finish off setup: Add child1 & child2 to state machine; child3 is left out
  sysItoA->add(children.begin(), children.end()-1, child1.fsm.getId(), kChildState0, child1.trItoA->getId());

  // Confirm correct construction before starting tests
  BOOST_REQUIRE_EQUAL( sys->getStatus().getStateMachineId(), ActionableSnapshot::kNullStateMachineId );
  BOOST_REQUIRE_EQUAL( sys->getStatus().getState(), ActionableSnapshot::kNullStateId );
  BOOST_REQUIRE(fsm.getParticipants() == (std::set<const StateMachine*> {&child1.fsm, &child2.fsm}));

  // Disengaging before FSM is engaged: Should throw, and leave state unchanged
  BOOST_CHECK_THROW( fsm.disengage(), ResourceInWrongStateMachine);
  BOOST_CHECK_EQUAL( sys->getStatus().getStateMachineId(), ActionableSnapshot::kNullStateMachineId);
  BOOST_CHECK_EQUAL( sys->getStatus().getState(), ActionableSnapshot::kNullStateId);
  BOOST_CHECK_EQUAL( child1.obj.getStatus().getStateMachineId(), ActionableSnapshot::kNullStateMachineId);
  BOOST_CHECK_EQUAL( child1.obj.getStatus().getState(), ActionableSnapshot::kNullStateId);
  BOOST_CHECK_EQUAL( child2.obj.getStatus().getStateMachineId(), ActionableSnapshot::kNullStateMachineId);
  BOOST_CHECK_EQUAL( child2.obj.getStatus().getState(), ActionableSnapshot::kNullStateId);
  BOOST_CHECK_EQUAL( child3.obj.getStatus().getStateMachineId(), ActionableSnapshot::kNullStateMachineId);
  BOOST_CHECK_EQUAL( child3.obj.getStatus().getState(), ActionableSnapshot::kNullStateId);

  // Engage FSM before continuing unit tests for disengage
  fsm.engage(DummyGateKeeper());
  BOOST_REQUIRE_EQUAL( sys->getStatus().getStateMachineId(), fsm.getId());
  BOOST_REQUIRE_EQUAL( sys->getStatus().getState(), kSysState0);
  BOOST_REQUIRE_EQUAL( child1.obj.getStatus().getStateMachineId(), child1.fsm.getId());
  BOOST_REQUIRE_EQUAL( child1.obj.getStatus().getState(), kChildState0);
  BOOST_REQUIRE_EQUAL( child2.obj.getStatus().getStateMachineId(), child2.fsm.getId());
  BOOST_REQUIRE_EQUAL( child2.obj.getStatus().getState(), kChildState0);
  BOOST_REQUIRE_EQUAL( child3.obj.getStatus().getStateMachineId(), ActionableSnapshot::kNullStateMachineId);
  BOOST_REQUIRE_EQUAL( child3.obj.getStatus().getState(), ActionableSnapshot::kNullStateId);

  // Disengaging another FSM: Should throw, and leave state unchanged
  SystemStateMachine& otherFSM = sys->registerStateMachine("anotherFSM", "otherInitialState", "otherErrorState");
  BOOST_CHECK_THROW( otherFSM.disengage(), ResourceInWrongStateMachine );
  BOOST_CHECK_EQUAL( sys->getStatus().getStateMachineId(), fsm.getId());
  BOOST_CHECK_EQUAL( sys->getStatus().getState(), kSysState0);
  BOOST_CHECK_EQUAL( child1.obj.getStatus().getStateMachineId(), child1.fsm.getId());
  BOOST_CHECK_EQUAL( child1.obj.getStatus().getState(), kChildState0);
  BOOST_CHECK_EQUAL( child2.obj.getStatus().getStateMachineId(), child2.fsm.getId());
  BOOST_CHECK_EQUAL( child2.obj.getStatus().getState(), kChildState0);
  BOOST_CHECK_EQUAL( child3.obj.getStatus().getStateMachineId(), ActionableSnapshot::kNullStateMachineId);
  BOOST_CHECK_EQUAL( child3.obj.getStatus().getState(), ActionableSnapshot::kNullStateId);

  // Disengaging the engaged FSM: Should return system & children to constructed state
  BOOST_CHECK_NO_THROW( fsm.disengage() );
  BOOST_CHECK_EQUAL( sys->getStatus().getStateMachineId(), ActionableSnapshot::kNullStateMachineId);
  BOOST_CHECK_EQUAL( sys->getStatus().getState(), ActionableSnapshot::kNullStateId);
  BOOST_CHECK_EQUAL( child1.obj.getStatus().getStateMachineId(), ActionableSnapshot::kNullStateMachineId);
  BOOST_CHECK_EQUAL( child1.obj.getStatus().getState(), ActionableSnapshot::kNullStateId);
  BOOST_CHECK_EQUAL( child2.obj.getStatus().getStateMachineId(), ActionableSnapshot::kNullStateMachineId);
  BOOST_CHECK_EQUAL( child2.obj.getStatus().getState(), ActionableSnapshot::kNullStateId);
  BOOST_CHECK_EQUAL( child3.obj.getStatus().getStateMachineId(), ActionableSnapshot::kNullStateMachineId);
  BOOST_CHECK_EQUAL( child3.obj.getStatus().getState(), ActionableSnapshot::kNullStateId);
}


BOOST_FIXTURE_TEST_CASE(TestDisengageFSMChildComplications, SystemStateMachineTestSetup)
{
  // Finish off setup: Add child1 & child2 to state machine; child3 is left out
  sysItoA->add(children.begin(), children.end()-1, child1.fsm.getId(), kChildState0, child1.trItoA->getId());

  // EngageFSM & confirm correct state before starting tests
  BOOST_REQUIRE(fsm.getParticipants() == (std::set<const StateMachine*> {&child1.fsm, &child2.fsm}));
  fsm.engage(DummyGateKeeper());
  BOOST_REQUIRE_EQUAL( sys->getStatus().getStateMachineId(), fsm.getId());
  BOOST_REQUIRE_EQUAL( sys->getStatus().getState(), kSysState0);
  BOOST_REQUIRE_EQUAL( child1.obj.getStatus().getStateMachineId(), child1.fsm.getId());
  BOOST_REQUIRE_EQUAL( child1.obj.getStatus().getState(), kChildState0);
  BOOST_REQUIRE_EQUAL( child2.obj.getStatus().getStateMachineId(), child2.fsm.getId());
  BOOST_REQUIRE_EQUAL( child2.obj.getStatus().getState(), kChildState0);
  BOOST_REQUIRE_EQUAL( child3.obj.getStatus().getStateMachineId(), ActionableSnapshot::kNullStateMachineId);
  BOOST_REQUIRE_EQUAL( child3.obj.getStatus().getState(), ActionableSnapshot::kNullStateId);

  // Disengaging system FSM after already disengaging a child: Should throw, and leave state unchanged
  child2.fsm.disengage();
  BOOST_CHECK_THROW( fsm.disengage(), ResourceInWrongStateMachine);
  BOOST_CHECK_EQUAL( sys->getStatus().getStateMachineId(), fsm.getId());
  BOOST_CHECK_EQUAL( sys->getStatus().getState(), kSysState0);
  BOOST_CHECK_EQUAL( child1.obj.getStatus().getStateMachineId(), child1.fsm.getId());
  BOOST_CHECK_EQUAL( child1.obj.getStatus().getState(), kChildState0);
  BOOST_CHECK_EQUAL( child2.obj.getStatus().getStateMachineId(), ActionableSnapshot::kNullStateMachineId);
  BOOST_CHECK_EQUAL( child2.obj.getStatus().getState(), ActionableSnapshot::kNullStateId);
  BOOST_CHECK_EQUAL( child3.obj.getStatus().getStateMachineId(), ActionableSnapshot::kNullStateMachineId);
  BOOST_CHECK_EQUAL( child3.obj.getStatus().getState(), ActionableSnapshot::kNullStateId);

  // Disengaging system FSM after re-engaging child FSM: Should return system & children to constructed state
  child2.fsm.engage(DummyGateKeeper());
  BOOST_CHECK_NO_THROW( fsm.disengage() );
  BOOST_CHECK_EQUAL( sys->getStatus().getStateMachineId(), ActionableSnapshot::kNullStateMachineId);
  BOOST_CHECK_EQUAL( sys->getStatus().getState(), ActionableSnapshot::kNullStateId);
  BOOST_CHECK_EQUAL( child1.obj.getStatus().getStateMachineId(), ActionableSnapshot::kNullStateMachineId);
  BOOST_CHECK_EQUAL( child1.obj.getStatus().getState(), ActionableSnapshot::kNullStateId);
  BOOST_CHECK_EQUAL( child2.obj.getStatus().getStateMachineId(), ActionableSnapshot::kNullStateMachineId);
  BOOST_CHECK_EQUAL( child2.obj.getStatus().getState(), ActionableSnapshot::kNullStateId);
  BOOST_CHECK_EQUAL( child3.obj.getStatus().getStateMachineId(), ActionableSnapshot::kNullStateMachineId);
  BOOST_CHECK_EQUAL( child3.obj.getStatus().getState(), ActionableSnapshot::kNullStateId);
}


BOOST_FIXTURE_TEST_CASE(TestMonitoringSettingsAppliedDuringEngage, SystemStateMachineTestSetup)
{
  // GOAL: Check that monitoring settings are reset, and those for initial state applied, during engage
  //       ONLY for actionable objects that are enabled
  addMonSettingsToGateKeeper(kChildState0);
  gk.addDisabledId(child2.obj.getPath());

  // Add children to state machine
  typedef std::vector<Child*>::const_iterator ChildIt_t;
  const std::vector<Child*> lChildren = {&child1, &child2, &child3};
  sysItoA->add(children.begin(), children.end(), child1.fsm.getId(), kChildState0, child1.trItoA->getId());
  // THE SETUP: Set to 'kDisabled', the monitorables that don't have settings in gatekeeper
  //            ... in order to ensure their settings are reset to 'kEnabled'
  for (ChildIt_t lIt=lChildren.begin(); lIt != lChildren.end(); lIt++) {
    (*lIt)->monGrandChild2.setMonitoringStatus(core::monitoring::kDisabled);
    (*lIt)->monGrandChild1.getMetric("dummyMetric").setMonitoringStatus(core::monitoring::kDisabled);
    (*lIt)->monGrandChild2.getMetric("dummyMetric").setMonitoringStatus(core::monitoring::kDisabled);

    // CHECK ASSUMPTIONS: Child monitorable objects and metrics have correct setting value before engage
    BOOST_REQUIRE_EQUAL((*lIt)->monChild.getMonitoringStatus(), core::monitoring::kEnabled);
    BOOST_REQUIRE_EQUAL((*lIt)->monGrandChild1.getMonitoringStatus(), core::monitoring::kEnabled);
    BOOST_REQUIRE_EQUAL((*lIt)->monGrandChild2.getMonitoringStatus(), core::monitoring::kDisabled);
    BOOST_REQUIRE_EQUAL((*lIt)->monChild.getMetric("dummyMetric").getStatus().second, core::monitoring::kEnabled);
    BOOST_REQUIRE_EQUAL((*lIt)->monGrandChild1.getMetric("dummyMetric").getStatus().second, core::monitoring::kDisabled);
    BOOST_REQUIRE_EQUAL((*lIt)->monGrandChild2.getMetric("dummyMetric").getStatus().second, core::monitoring::kDisabled);
  }

  // THE TEST: Gatekeeper's monitoring settings for initial state should be applied during state machine engage
  fsm.engage(gk);

  std::vector<Child*> lEnabledChildren = {&child1, &child3};
  for (ChildIt_t lIt=lEnabledChildren.begin(); lIt != lEnabledChildren.end(); lIt++) {
    BOOST_CHECK_EQUAL((*lIt)->monChild.getMonitoringStatus(), core::monitoring::kNonCritical);
    BOOST_CHECK_EQUAL((*lIt)->monGrandChild1.getMonitoringStatus(), core::monitoring::kDisabled);
    // this one should fail, as paths need to be relative to obj and this setting has been made with ID 'grandChild2'
    // not 'child1.grandChild2'
    BOOST_CHECK_EQUAL((*lIt)->monGrandChild2.getMonitoringStatus(), core::monitoring::kEnabled);
    // metrics
    BOOST_CHECK_EQUAL((*lIt)->monChild.getMetric("dummyMetric").getStatus().second, core::monitoring::kDisabled);
    BOOST_CHECK_EQUAL((*lIt)->monGrandChild1.getMetric("dummyMetric").getStatus().second, core::monitoring::kEnabled);
    BOOST_CHECK_EQUAL((*lIt)->monGrandChild2.getMetric("dummyMetric").getStatus().second, core::monitoring::kEnabled);
  }
  BOOST_CHECK_EQUAL(child2.monChild.getMonitoringStatus(), core::monitoring::kEnabled);
  BOOST_CHECK_EQUAL(child2.monGrandChild1.getMonitoringStatus(), core::monitoring::kEnabled);
  BOOST_CHECK_EQUAL(child2.monGrandChild2.getMonitoringStatus(), core::monitoring::kDisabled);
  BOOST_CHECK_EQUAL(child2.monChild.getMetric("dummyMetric").getStatus().second, core::monitoring::kEnabled);
  BOOST_CHECK_EQUAL(child2.monGrandChild1.getMetric("dummyMetric").getStatus().second, core::monitoring::kDisabled);
  BOOST_CHECK_EQUAL(child2.monGrandChild2.getMetric("dummyMetric").getStatus().second, core::monitoring::kDisabled);
}


BOOST_FIXTURE_TEST_CASE(TestMaskablesMaskedDuringEngage, SystemStateMachineTestSetup)
{
  typedef std::vector<Child*>::const_iterator ChildIt_t;
  const std::vector<Child*> lChildren = {&child1, &child2, &child3};

  // Finish off setup: Add children to state machine
  sysItoA->add(children.begin(), children.end(), child1.fsm.getId(), kChildState0, child1.trItoA->getId());

  // PART A :
  //  * Goal: Check that MaskableObjects we ask it to mask are being masked
  //  * Start off with everything unmasked

  // A,1) Require that all are unmasked before tests start
  for (ChildIt_t lIt=lChildren.begin(); lIt!=lChildren.end(); lIt++) {
    BOOST_REQUIRE_EQUAL((*lIt)->maskableA.isMasked(), false);
    BOOST_REQUIRE_EQUAL((*lIt)->maskableB.isMasked(), false);
    BOOST_REQUIRE_EQUAL((*lIt)->maskableC.isMasked(), false);
  }
  // A.2) Engage FSM; check that A & B now masked, but C remains unmasked
  fsm.engage(gk);
  for (ChildIt_t lIt=lChildren.begin(); lIt!=lChildren.end(); lIt++) {
    BOOST_CHECK_EQUAL((*lIt)->maskableA.isMasked(), true);
    BOOST_CHECK_EQUAL((*lIt)->maskableB.isMasked(), true);
    BOOST_CHECK_EQUAL((*lIt)->maskableC.isMasked(), false);
  }

  // PART B :
  //   * Goal: Check that all MaskableObjects we don't ask it to mask are being reset to unmasked state
  //   * Start off with A & B unmasked, but C masked
  // B.1) Disengage, unmask A & B, mask C; require that starting assumptions are correct before testing
  fsm.disengage();
  for (ChildIt_t lIt=lChildren.begin(); lIt!=lChildren.end(); lIt++) {
    (*lIt)->maskableA.setMasked(false);
    (*lIt)->maskableB.setMasked(false);
    (*lIt)->maskableC.setMasked(true);
    BOOST_REQUIRE_EQUAL((*lIt)->maskableA.isMasked(), false);
    BOOST_REQUIRE_EQUAL((*lIt)->maskableB.isMasked(), false);
    BOOST_REQUIRE_EQUAL((*lIt)->maskableC.isMasked(), true);
  }

  // B.2) Engage FSM; check that A & B now masked, but C remains unmasked
  fsm.engage(gk);
  for (ChildIt_t lIt=lChildren.begin(); lIt!=lChildren.end(); lIt++) {
    BOOST_CHECK_EQUAL((*lIt)->maskableA.isMasked(), true);
    BOOST_CHECK_EQUAL((*lIt)->maskableB.isMasked(), true);
    BOOST_CHECK_EQUAL((*lIt)->maskableC.isMasked(), false);
  }
}


BOOST_FIXTURE_TEST_CASE(TestChildrenDisabledDuringEngage, SystemStateMachineTestSetup)
{
  typedef std::vector<Child*>::const_iterator ChildIt_t;
  const std::vector<Child*> lChildren = {&child1, &child2, &child3};

  // Finish off setup: Add child1 & child2 to state machine
  sysItoA->add(children.begin(), children.end()-1, child1.fsm.getId(), kChildState0, child1.trItoA->getId());

  // PURPOSE: Check that engage ...
  //    - Disables children specified in gatekeeper (i.e. child2)
  //    - Leaves all other children enabled (i.e. child1, child3)
  //    - Doesn't care if disabled children are in other state machines
  //    - Only changes masks on children that are enabled (i.e. child1)

  // SETUP:
  gk.addDisabledId(child2.obj.getPath());

  // 1) Engage child2 and child3 other single-object FSMs
  //    (to make sure that system FSM doesn't care if disabled objects already engaged)
  child2.obj.registerStateMachine("anotherFSM", "anotherInitialState", "sE").engage(DummyGateKeeper());
  child3.obj.registerStateMachine("anotherFSM", "anotherInitialState", "sE").engage(DummyGateKeeper());
  BOOST_REQUIRE_EQUAL(child2.obj.getStatus().getStateMachineId(), "anotherFSM");
  BOOST_REQUIRE_EQUAL(child3.obj.getStatus().getStateMachineId(), "anotherFSM");

  // 2) Mask maskableC on each child (giving inverse of final result); require that starting assumptions are correct before testing
  for (ChildIt_t lIt=lChildren.begin(); lIt!=lChildren.end(); lIt++) {
    (*lIt)->maskableC.setMasked(true);
    BOOST_REQUIRE_EQUAL((*lIt)->maskableA.isMasked(), false);
    BOOST_REQUIRE_EQUAL((*lIt)->maskableB.isMasked(), false);
    BOOST_REQUIRE_EQUAL((*lIt)->maskableC.isMasked(), true);
  }

  child1.obj.setMonitoringStatus(core::monitoring::kDisabled);
  BOOST_REQUIRE_EQUAL(child1.obj.getMonitoringStatus(), core::monitoring::kDisabled);
  child2.obj.setMonitoringStatus(core::monitoring::kEnabled);
  BOOST_REQUIRE_EQUAL(child2.obj.getMonitoringStatus(), core::monitoring::kEnabled);

  // THE TEST:
  //  * Engage FSM; check that for child1,  A & B now masked, but C unmasked; no change on child2 & 3
  BOOST_CHECK_NO_THROW(fsm.engage(gk));

  BOOST_CHECK_EQUAL(sys->getStatus().getStateMachineId(), fsm.getId());
  BOOST_CHECK_EQUAL(sys->getStatus().getState(), kSysState0);

  BOOST_CHECK_EQUAL(child1.obj.getStatus().isEnabled(), true);
  BOOST_CHECK_EQUAL(child2.obj.getStatus().isEnabled(), false);
  BOOST_CHECK_EQUAL(child3.obj.getStatus().isEnabled(), true);
  BOOST_CHECK_EQUAL(child1.obj.getMonitoringStatus(), core::monitoring::kEnabled);
  BOOST_CHECK_EQUAL(child2.obj.getMonitoringStatus(), core::monitoring::kNonCritical);

  BOOST_CHECK_EQUAL(child1.obj.getStatus().getStateMachineId(), child1.fsm.getId());
  BOOST_CHECK_EQUAL(child1.obj.getStatus().getState(), kChildState0);
  BOOST_CHECK_EQUAL(child1.maskableA.isMasked(), true);
  BOOST_CHECK_EQUAL(child1.maskableB.isMasked(), true);
  BOOST_CHECK_EQUAL(child1.maskableC.isMasked(), false);

  for (ChildIt_t lIt=lChildren.begin()+1; lIt!=lChildren.end(); lIt++) {
    const Child& lChild = **lIt;

    BOOST_CHECK_EQUAL(lChild.obj.getStatus().getStateMachineId(), "anotherFSM");
    BOOST_CHECK_EQUAL(lChild.obj.getStatus().getState(), "anotherInitialState");
    BOOST_CHECK_EQUAL(lChild.maskableA.isMasked(), false);
    BOOST_CHECK_EQUAL(lChild.maskableB.isMasked(), false);
    BOOST_CHECK_EQUAL(lChild.maskableC.isMasked(), true);
  }
}


BOOST_FIXTURE_TEST_CASE(TestChildSettingsNotChangedByFailedEngage, SystemStateMachineTestSetup)
{
  typedef std::vector<Child*>::const_iterator ChildIt_t;
  const std::vector<Child*> lChildren = {&child1, &child2, &child3};

  // SETUP:
  //  * child1 & child2 are in state machine
  sysItoA->add(children.begin(), children.end()-1, child1.fsm.getId(), kChildState0, child1.trItoA->getId());
  //  * But child1 is already engaged in another state machine
  child1.obj.registerStateMachine("anotherFSM", "anotherInitialState", "sE").engage(DummyGateKeeper());
  BOOST_REQUIRE_EQUAL(child1.obj.getStatus().getStateMachineId(), "anotherFSM");
  //  * Maskables A & B are unmasked, but C is masked (inverse of result from successful "engage")
  for (ChildIt_t lIt=lChildren.begin(); lIt!=lChildren.end(); lIt++) {
    (*lIt)->maskableA.setMasked(false);
    (*lIt)->maskableB.setMasked(false);
    (*lIt)->maskableC.setMasked(true);
    BOOST_REQUIRE_EQUAL((*lIt)->maskableA.isMasked(), false);
    BOOST_REQUIRE_EQUAL((*lIt)->maskableB.isMasked(), false);
    BOOST_REQUIRE_EQUAL((*lIt)->maskableC.isMasked(), true);
  }
  //  * GateKeeper (GK): child2 is set as disabled
  gk.addDisabledId(child2.obj.getPath());


  // GOAL:
  //  * In case the engage was successful:
  //    - child2 would be disabled based on GK info
  //    - child3 would be disabled since not in system state machine
  //    - maskable children of child1 would be masked/unmasked based on GK info
  //  * This test case checks that none of these changes occur if child1 is already engaged :)

  BOOST_CHECK_THROW(fsm.engage(gk), ResourceInWrongStateMachine);

  BOOST_CHECK_EQUAL( sys->getStatus().getStateMachineId(), ActionableSystem::Status_t::kNullStateMachineId);
  BOOST_CHECK_EQUAL( sys->getStatus().getState(), ActionableSystem::Status_t::kNullStateId );
  BOOST_CHECK_EQUAL( child1.obj.getStatus().isEnabled(), true);
  BOOST_CHECK_EQUAL( child1.obj.getStatus().getStateMachineId(), "anotherFSM");
  BOOST_CHECK_EQUAL( child1.obj.getStatus().getState(), "anotherInitialState");
  BOOST_CHECK_EQUAL( child3.obj.getStatus().isEnabled(), true);
  BOOST_CHECK_EQUAL( child2.obj.getStatus().getStateMachineId(), ActionableObject::Status_t::kNullStateMachineId);
  BOOST_CHECK_EQUAL( child2.obj.getStatus().getState(), ActionableObject::Status_t::kNullStateId);
  BOOST_CHECK_EQUAL( child3.obj.getStatus().isEnabled(), true);
  BOOST_CHECK_EQUAL( child3.obj.getStatus().getStateMachineId(), ActionableObject::Status_t::kNullStateMachineId);
  BOOST_CHECK_EQUAL( child3.obj.getStatus().getState(), ActionableObject::Status_t::kNullStateId);

  for (ChildIt_t lIt=lChildren.begin(); lIt!=lChildren.end(); lIt++) {
    BOOST_CHECK_EQUAL((*lIt)->maskableA.isMasked(), false);
    BOOST_CHECK_EQUAL((*lIt)->maskableB.isMasked(), false);
    BOOST_CHECK_EQUAL((*lIt)->maskableC.isMasked(), true);
  }
}


/* ---------------------------------------------------------------- */
/*   PART 3: RUNNING TRANSITIONS                                    */
/* ---------------------------------------------------------------- */


BOOST_FIXTURE_TEST_CASE(TestRunEmptyTransition, SystemStateMachineTestSetup)
{
  DummyGateKeeper lEmptyGK;

  // Preparation
  sysAtoB->add(children.begin(), children.end(), child1.fsm.getId(), kChildStateA, child1.trAtoB->getId());
  fsm.engage(lEmptyGK);
  child1.fsm.disengage();
  child2.fsm.disengage();
  child3.fsm.disengage();
  BOOST_REQUIRE_EQUAL(sys->getStatus().getStateMachineId(), fsm.getId());
  BOOST_REQUIRE_EQUAL(sys->getStatus().getState(), kSysState0);
  BOOST_REQUIRE_EQUAL(child1.obj.getStatus().getStateMachineId(), ActionableSnapshot::kNullStateMachineId);
  BOOST_REQUIRE_EQUAL(child2.obj.getStatus().getStateMachineId(), ActionableSnapshot::kNullStateMachineId);
  BOOST_REQUIRE_EQUAL(child3.obj.getStatus().getStateMachineId(), ActionableSnapshot::kNullStateMachineId);
  BOOST_REQUIRE_EQUAL(child1.obj.getStatus().getState(), ActionableSnapshot::kNullStateId);
  BOOST_REQUIRE_EQUAL(child2.obj.getStatus().getState(), ActionableSnapshot::kNullStateId);
  BOOST_REQUIRE_EQUAL(child3.obj.getStatus().getState(), ActionableSnapshot::kNullStateId);

  // Running empty transition: Should work, regardless of child state
  BOOST_CHECK_NO_THROW( sysItoA->exec(lEmptyGK, false) );
  BOOST_CHECK_EQUAL(sys->getStatus().getStateMachineId(), fsm.getId());
  BOOST_CHECK_EQUAL(sys->getStatus().getState(), kSysStateA);
  BOOST_CHECK_EQUAL(child1.obj.getStatus().getStateMachineId(), ActionableSnapshot::kNullStateMachineId);
  BOOST_CHECK_EQUAL(child2.obj.getStatus().getStateMachineId(), ActionableSnapshot::kNullStateMachineId);
  BOOST_CHECK_EQUAL(child3.obj.getStatus().getStateMachineId(), ActionableSnapshot::kNullStateMachineId);
  BOOST_CHECK_EQUAL(child1.obj.getStatus().getState(), ActionableSnapshot::kNullStateId);
  BOOST_CHECK_EQUAL(child2.obj.getStatus().getState(), ActionableSnapshot::kNullStateId);
  BOOST_CHECK_EQUAL(child3.obj.getStatus().getState(), ActionableSnapshot::kNullStateId);
  SystemTransitionSnapshot s = sysItoA->getStatus();
  BOOST_CHECK_EQUAL( s.getActionPath(), sysItoA->getPath() );
  BOOST_CHECK_EQUAL( s.getActionId(), sysItoA->getId());
  BOOST_CHECK_EQUAL( s.getActionableId(), sys->getId());
  BOOST_CHECK_EQUAL( s.getState(), Functionoid::State::kDone );
  BOOST_CHECK_EQUAL( s.getProgress(), 1.0 );
  BOOST_CHECK_GE( s.getRunningTime(), 0.0);
  BOOST_CHECK_EQUAL( s.getMessage(), "");
  BOOST_CHECK( s.begin() == s.end());
  BOOST_CHECK_EQUAL( s.end() - s.begin(), 0);
  BOOST_CHECK_THROW( s.at(0), std::out_of_range);
  BOOST_CHECK_EQUAL( s.getNumberOfCompletedSteps(), size_t(0));
  BOOST_CHECK_EQUAL( s.getTotalNumberOfSteps(), size_t(0));
  BOOST_CHECK_EQUAL( s.getEnabledChildren().size(), size_t(0));
}


BOOST_FIXTURE_TEST_CASE(TestRunTransitionDisengagedFSM, SystemStateMachineTestSetup)
{
  DummyGateKeeper lEmptyGK;

  // Running transition before FSM is engaged: should throw, and leave state/TransitionStatus unchanged
  BOOST_CHECK_THROW( sysItoA->exec(lEmptyGK, false), ResourceInWrongStateMachine);
  BOOST_CHECK_EQUAL( sysItoA->getStatus().getState(), Functionoid::State::kInitial);
  BOOST_CHECK_EQUAL( sys->getStatus().getStateMachineId(), ActionableSnapshot::kNullStateMachineId);
  BOOST_CHECK_EQUAL( sys->getStatus().getState(), ActionableSnapshot::kNullStateId);
  BOOST_CHECK_EQUAL( child1.obj.getStatus().getStateMachineId(), ActionableSnapshot::kNullStateMachineId);
  BOOST_CHECK_EQUAL( child1.obj.getStatus().getState(), ActionableSnapshot::kNullStateId);
  BOOST_CHECK_EQUAL( child2.obj.getStatus().getStateMachineId(), ActionableSnapshot::kNullStateMachineId);
  BOOST_CHECK_EQUAL( child2.obj.getStatus().getState(), ActionableSnapshot::kNullStateId);
  BOOST_CHECK_EQUAL( child3.obj.getStatus().getStateMachineId(), ActionableSnapshot::kNullStateMachineId);
  BOOST_CHECK_EQUAL( child3.obj.getStatus().getState(), ActionableSnapshot::kNullStateId);

  // Running transition with other FSM engaged: should throw, and leave state/TransitionStatus unchanged
  SystemStateMachine& otherFSM = sys->registerStateMachine("anotherFSM", kSysState0, kSysStateErr);
  otherFSM.engage(lEmptyGK);
  BOOST_REQUIRE_EQUAL( sys->getStatus().getStateMachineId(), otherFSM.getId() );
  BOOST_REQUIRE_EQUAL( sys->getStatus().getState(), kSysState0);
  BOOST_CHECK_THROW( sysItoA->exec(lEmptyGK, false), ResourceInWrongStateMachine );
  BOOST_CHECK_EQUAL( sysItoA->getStatus().getState(), Functionoid::State::kInitial);
  BOOST_CHECK_EQUAL( sys->getStatus().getStateMachineId(), otherFSM.getId());
  BOOST_CHECK_EQUAL( sys->getStatus().getState(), kSysState0);
}


BOOST_FIXTURE_TEST_CASE(TestRunTransitionMissingParams, SystemStateMachineTestSetup)
{
  sysItoA->add(std::vector<Transition*> {child1.trItoA, child2.trItoA});
  sysItoA->add(std::vector<Transition*> {child3.trItoA});
  child1.trItoA->add(child1.cmdNormal1);
  child3.trItoA->add(child3.cmdNormal2);

  // Engage FSM, and confirm objects are ready for following tests
  fsm.engage(DummyGateKeeper());
  BOOST_REQUIRE_EQUAL( sys->getStatus().getStateMachineId(), fsm.getId());
  BOOST_REQUIRE_EQUAL( sys->getStatus().getState(), kSysState0);
  BOOST_REQUIRE_EQUAL( child1.obj.getStatus().getStateMachineId(), child1.fsm.getId());
  BOOST_REQUIRE_EQUAL( child2.obj.getStatus().getStateMachineId(), child2.fsm.getId());
  BOOST_REQUIRE_EQUAL( child3.obj.getStatus().getStateMachineId(), child3.fsm.getId());
  BOOST_REQUIRE_EQUAL( child1.obj.getStatus().getState(), kChildState0);
  BOOST_REQUIRE_EQUAL( child2.obj.getStatus().getState(), kChildState0);
  BOOST_REQUIRE_EQUAL( child3.obj.getStatus().getState(), kChildState0);

  // Running transition without parameter values defined: Should throw, and leave state unchanged
  BOOST_CHECK_THROW( sysItoA->exec(DummyGateKeeper(), false), ParameterNotFound );
  BOOST_CHECK_EQUAL( sysItoA->getStatus().getState(), Functionoid::State::kInitial );
  BOOST_CHECK_EQUAL( child1.trItoA->getState(), Functionoid::State::kInitial );
  BOOST_CHECK_EQUAL( child2.trItoA->getState(), Functionoid::State::kInitial );
  BOOST_CHECK_EQUAL( child3.trItoA->getState(), Functionoid::State::kInitial );
  BOOST_CHECK_EQUAL( child1.obj.getStatus().getStateMachineId(), child1.fsm.getId());
  BOOST_CHECK_EQUAL( child2.obj.getStatus().getStateMachineId(), child2.fsm.getId());
  BOOST_CHECK_EQUAL( child3.obj.getStatus().getStateMachineId(), child3.fsm.getId());
  BOOST_CHECK_EQUAL( child1.obj.getStatus().getState(), kChildState0);
  BOOST_CHECK_EQUAL( child2.obj.getStatus().getState(), kChildState0);
  BOOST_CHECK_EQUAL( child3.obj.getStatus().getState(), kChildState0);

  // checkMissingParams method should return correct list of missing parameters
  typedef CommandVec::MissingParam MissingParam_t;
  std::vector<MissingParam_t> result, expected;
  expected.push_back(MissingParam_t("", child1.cmdNormal1.getId(), DummyCommand::kParamToDo));
  expected.push_back(MissingParam_t("", child1.cmdNormal1.getId(), DummyCommand::kParamX));

  std::map<const Transition*, std::vector<MissingParam_t> > missingParamMap;
  sysItoA->checkForMissingParameters(DummyGateKeeper(), missingParamMap);
  BOOST_CHECK_EQUAL(missingParamMap.size(), size_t(2));
  BOOST_REQUIRE_EQUAL( missingParamMap.count(child1.trItoA), size_t(1));
  BOOST_CHECK_EQUAL_COLLECTIONS(missingParamMap[child1.trItoA].begin(), missingParamMap[child1.trItoA].end(), expected.begin(), expected.end());

  expected.clear();
  expected.push_back(MissingParam_t("", child3.cmdNormal2.getId(), DummyCommand::kParamToDo));
  expected.push_back(MissingParam_t("", child3.cmdNormal2.getId(), DummyCommand::kParamX));
  BOOST_REQUIRE_EQUAL( missingParamMap.count(child3.trItoA), size_t(1));
  BOOST_CHECK_EQUAL_COLLECTIONS(missingParamMap[child3.trItoA].begin(), missingParamMap[child3.trItoA].end(), expected.begin(), expected.end());
}


BOOST_FIXTURE_TEST_CASE(TestCheckMissingParamsWithChildrenDisabled, SystemStateMachineTestSetup)
{
  sysItoA->add(std::vector<Transition*> {child1.trItoA, child2.trItoA});
  sysItoA->add(std::vector<Transition*> {child3.trItoA});
  child1.trItoA->add(child1.cmdNormal1);
  child3.trItoA->add(child3.cmdNormal2);

  // Engage FSM, and confirm objects are ready for following tests
  DummyGateKeeper lGK;
  lGK.addDisabledId(child2.obj.getPath());
  lGK.addDisabledId(child3.obj.getPath());

  fsm.engage(lGK);
  BOOST_REQUIRE_EQUAL( sys->getStatus().getStateMachineId(), fsm.getId());
  BOOST_REQUIRE_EQUAL( sys->getStatus().getState(), kSysState0);
  BOOST_REQUIRE_EQUAL( child1.obj.getStatus().getStateMachineId(), child1.fsm.getId());
  BOOST_REQUIRE_EQUAL( child1.obj.getStatus().getState(), kChildState0);
  BOOST_REQUIRE_EQUAL( child2.obj.getStatus().isEngaged(), false);
  BOOST_REQUIRE_EQUAL( child3.obj.getStatus().isEngaged(), false);
  BOOST_REQUIRE_EQUAL( child2.obj.getStatus().getState(), ActionableSnapshot::kNullStateId);
  BOOST_REQUIRE_EQUAL( child3.obj.getStatus().getState(), ActionableSnapshot::kNullStateId);

  // checkMissingParams method should return missing parameters  for child1's transition ...
  // ... but NOT for child3 (since child3 has been disabled)
  typedef CommandVec::MissingParam MissingParam_t;
  std::vector<MissingParam_t> result, expected;
  expected.push_back(MissingParam_t("", child1.cmdNormal1.getId(), DummyCommand::kParamToDo));
  expected.push_back(MissingParam_t("", child1.cmdNormal1.getId(), DummyCommand::kParamX));

  std::map<const Transition*, std::vector<MissingParam_t> > missingParamMap;
  sysItoA->checkForMissingParameters(DummyGateKeeper(), missingParamMap);
  BOOST_CHECK_EQUAL(missingParamMap.size(), size_t(1));
  BOOST_REQUIRE_EQUAL( missingParamMap.count(child1.trItoA), size_t(1));
  BOOST_CHECK_EQUAL_COLLECTIONS(missingParamMap[child1.trItoA].begin(), missingParamMap[child1.trItoA].end(), expected.begin(), expected.end());
}



BOOST_FIXTURE_TEST_CASE(TestRunTransitionInvalidParams, SystemStateMachineTestSetup)
{
  sysItoA->add(std::vector<Transition*> {child1.trItoA, child2.trItoA});
  sysItoA->add(std::vector<Transition*> {child3.trItoA});
  child1.trItoA->add(child1.cmdNormal1);
  child3.trItoA->add(child3.cmdNormal2);

  // Engage FSM, and confirm objects are ready for following tests
  fsm.engage(DummyGateKeeper());
  BOOST_REQUIRE_EQUAL( sys->getStatus().getStateMachineId(), fsm.getId());
  BOOST_REQUIRE_EQUAL( sys->getStatus().getState(), kSysState0);
  BOOST_REQUIRE_EQUAL( child1.obj.getStatus().getStateMachineId(), child1.fsm.getId());
  BOOST_REQUIRE_EQUAL( child2.obj.getStatus().getStateMachineId(), child2.fsm.getId());
  BOOST_REQUIRE_EQUAL( child3.obj.getStatus().getStateMachineId(), child3.fsm.getId());
  BOOST_REQUIRE_EQUAL( child1.obj.getStatus().getState(), kChildState0);
  BOOST_REQUIRE_EQUAL( child2.obj.getStatus().getState(), kChildState0);
  BOOST_REQUIRE_EQUAL( child3.obj.getStatus().getState(), kChildState0);

  // Running transition with any combinations of parameter rules & constraints failing: Should throw, and leave state unchanged
  DummyConstraint::setResult(false);
  BOOST_CHECK_THROW( sysItoA->exec(gk, false), CommandParameterCheckFailed );
  DummyConstraint::setResult(true);
  DummyXRule<xdata::Integer>::setResult(false);
  BOOST_CHECK_THROW( sysItoA->exec(gk, false), CommandParameterCheckFailed );
  DummyXRule<xdata::Integer>::setResult(true);
  DummyXRule<xdata::String>::setResult(false);
  BOOST_CHECK_THROW( sysItoA->exec(gk, false), CommandParameterCheckFailed );

  BOOST_CHECK_EQUAL( sysItoA->getStatus().getState(), Functionoid::State::kInitial );
  BOOST_CHECK_EQUAL( child1.trItoA->getState(), Functionoid::State::kInitial );
  BOOST_CHECK_EQUAL( child2.trItoA->getState(), Functionoid::State::kInitial );
  BOOST_CHECK_EQUAL( child3.trItoA->getState(), Functionoid::State::kInitial );
  BOOST_CHECK_EQUAL( child1.obj.getStatus().getStateMachineId(), child1.fsm.getId());
  BOOST_CHECK_EQUAL( child2.obj.getStatus().getStateMachineId(), child2.fsm.getId());
  BOOST_CHECK_EQUAL( child3.obj.getStatus().getStateMachineId(), child3.fsm.getId());
  BOOST_CHECK_EQUAL( child1.obj.getStatus().getState(), kChildState0);
  BOOST_CHECK_EQUAL( child2.obj.getStatus().getState(), kChildState0);
  BOOST_CHECK_EQUAL( child3.obj.getStatus().getState(), kChildState0);

  // checkForInvalidParameters method should return correct information
  typedef Command::ParamRuleViolation RuleViolation_t;
  typedef CommandVec::ParamRuleViolationList RuleViolationList_t;
  std::map<const Transition*, std::vector<RuleViolationList_t> > lRuleViolationMap;
  DummyConstraint::setResult(true);
  DummyXRule<xdata::String>::setResult(true);
  sysItoA->checkForInvalidParameters(gk, lRuleViolationMap);
  BOOST_CHECK_EQUAL(lRuleViolationMap.size(), size_t(0));
  
  DummyConstraint::setResult( core::XMatch(false, "Details about constraint failure") );
  DummyXRule<xdata::String>::setResult( core::XMatch(false, "Details about rule failure!") );
  sysItoA->checkForInvalidParameters(gk, lRuleViolationMap);
  BOOST_CHECK_EQUAL(lRuleViolationMap.size(), size_t(2));

  core::ReadOnlyXParameterSet lParamSet;
  typedef boost::shared_ptr<const xdata::Serializable> XSharedPtr_t;
  lParamSet.adopt(DummyCommand::kParamToDo, XSharedPtr_t(new xdata::String("")));
  lParamSet.adopt(DummyCommand::kParamX, XSharedPtr_t(new xdata::Integer(42)));
  std::vector<RuleViolationList_t> lExpected = {RuleViolationList_t(child1.cmdNormal1.getId(), 0, lParamSet, {}) };
  lExpected.back().violations.push_back( RuleViolation_t({DummyCommand::kParamToDo}, DummyXRule<xdata::String>::sDescription, "Details about rule failure!"));
  lExpected.back().violations.push_back( RuleViolation_t({DummyCommand::kParamToDo, DummyCommand::kParamX}, DummyConstraint::sDescription, "Details about constraint failure"));
  lExpected.back().violations.push_back( RuleViolation_t({DummyCommand::kParamX}, DummyConstraint::sDescription, "Details about constraint failure"));
  BOOST_REQUIRE_EQUAL( lRuleViolationMap.count(child1.trItoA), size_t(1));
  BOOST_CHECK_EQUAL_COLLECTIONS(lRuleViolationMap[child1.trItoA].begin(), lRuleViolationMap[child1.trItoA].end(), lExpected.begin(), lExpected.end());

  lExpected = {RuleViolationList_t(child2.cmdNormal2.getId(), 0, lParamSet, {})};
  lExpected.back().violations.push_back( RuleViolation_t({DummyCommand::kParamToDo}, DummyXRule<xdata::String>::sDescription, "Details about rule failure!"));
  lExpected.back().violations.push_back( RuleViolation_t({DummyCommand::kParamToDo, DummyCommand::kParamX}, DummyConstraint::sDescription, "Details about constraint failure"));
  lExpected.back().violations.push_back( RuleViolation_t({DummyCommand::kParamX}, DummyConstraint::sDescription, "Details about constraint failure"));
  BOOST_REQUIRE_EQUAL( lRuleViolationMap.count(child3.trItoA), size_t(1));
  BOOST_CHECK_EQUAL_COLLECTIONS(lRuleViolationMap[child3.trItoA].begin(), lRuleViolationMap[child3.trItoA].end(), lExpected.begin(), lExpected.end());
}


BOOST_FIXTURE_TEST_CASE(TestCheckInvalidParamsWithChildrenDisabled, SystemStateMachineTestSetup)
{
  sysItoA->add(std::vector<Transition*> {child1.trItoA, child2.trItoA});
  sysItoA->add(std::vector<Transition*> {child3.trItoA});
  child1.trItoA->add(child1.cmdNormal1);
  child3.trItoA->add(child3.cmdNormal2);

  // Engage FSM, and confirm objects are ready for following tests
  DummyGateKeeper lGK;
  lGK.addDisabledId(child2.obj.getPath());
  lGK.addDisabledId(child3.obj.getPath());

  fsm.engage(lGK);
  BOOST_REQUIRE_EQUAL( sys->getStatus().getStateMachineId(), fsm.getId());
  BOOST_REQUIRE_EQUAL( sys->getStatus().getState(), kSysState0);
  BOOST_REQUIRE_EQUAL( child1.obj.getStatus().getStateMachineId(), child1.fsm.getId());
  BOOST_REQUIRE_EQUAL( child1.obj.getStatus().getState(), kChildState0);
  BOOST_REQUIRE_EQUAL( child2.obj.getStatus().isEngaged(), false);
  BOOST_REQUIRE_EQUAL( child3.obj.getStatus().isEngaged(), false);
  BOOST_REQUIRE_EQUAL( child2.obj.getStatus().getState(), ActionableSnapshot::kNullStateId);
  BOOST_REQUIRE_EQUAL( child3.obj.getStatus().getState(), ActionableSnapshot::kNullStateId);

  // checkMissingParams method should return correct information for child1's transition ...
  // ... but NOT for child3 (since child3 has been disabled)
  typedef Command::ParamRuleViolation RuleViolation_t;
  typedef CommandVec::ParamRuleViolationList RuleViolationList_t;
  std::map<const Transition*, std::vector<RuleViolationList_t> > lRuleViolationMap;
  DummyConstraint::setResult( core::XMatch(false, "Details about constraint failure") );
  DummyXRule<xdata::String>::setResult( core::XMatch(false, "Details about rule failure!") );
  sysItoA->checkForInvalidParameters(gk, lRuleViolationMap);
  BOOST_CHECK_EQUAL(lRuleViolationMap.size(), size_t(1));

  core::ReadOnlyXParameterSet lParamSet;
  typedef boost::shared_ptr<const xdata::Serializable> XSharedPtr_t;
  lParamSet.adopt(DummyCommand::kParamToDo, XSharedPtr_t(new xdata::String("")));
  lParamSet.adopt(DummyCommand::kParamX, XSharedPtr_t(new xdata::Integer(42)));
  std::vector<RuleViolationList_t> lExpected = {RuleViolationList_t(child1.cmdNormal1.getId(), 0, lParamSet, {})};
  lExpected.back().violations.push_back(RuleViolation_t({DummyCommand::kParamToDo}, DummyXRule<xdata::String>::sDescription, "Details about rule failure!"));
  lExpected.back().violations.push_back(RuleViolation_t({DummyCommand::kParamToDo, DummyCommand::kParamX}, DummyConstraint::sDescription, "Details about constraint failure"));
  lExpected.back().violations.push_back(RuleViolation_t({DummyCommand::kParamX}, DummyConstraint::sDescription, "Details about constraint failure"));
  BOOST_REQUIRE_EQUAL( lRuleViolationMap.count(child1.trItoA), size_t(1));
  BOOST_CHECK_EQUAL_COLLECTIONS(lRuleViolationMap[child1.trItoA].begin(), lRuleViolationMap[child1.trItoA].end(), lExpected.begin(), lExpected.end());
}


BOOST_FIXTURE_TEST_CASE(TestRunGoodTransition, SystemStateMachineTestSetup)
{
  // Preparation: Add child transitions & commands, engage FSM; then check state before starting unit tests
  child1.trItoA->add(child1.cmdNormal1);
  child2.trItoA->add(child2.cmdNormal1);
  child2.trItoA->add(child2.cmdNormal2);
  child3.trItoA->add(child3.cmdNormal1);
  child3.trItoA->add(child3.cmdNormal2);
  child3.trItoA->add(child3.cmdNormal1);
  sysItoA->add( std::vector<Transition*> {child1.trItoA, child2.trItoA});
  sysItoA->add( std::vector<Transition*> {child3.trItoA});
  fsm.engage(DummyGateKeeper());
  BOOST_REQUIRE_EQUAL(sys->getStatus().getStateMachineId(), fsm.getId());
  BOOST_REQUIRE_EQUAL(sys->getStatus().getState(), kSysState0);
  BOOST_REQUIRE_EQUAL(sysItoA->getStatus().getState(), Functionoid::State::kInitial);
  BOOST_REQUIRE_EQUAL(child1.obj.getStatus().getStateMachineId(), child1.fsm.getId() );
  BOOST_REQUIRE_EQUAL(child2.obj.getStatus().getStateMachineId(), child2.fsm.getId() );
  BOOST_REQUIRE_EQUAL(child3.obj.getStatus().getStateMachineId(), child3.fsm.getId() );
  BOOST_REQUIRE_EQUAL(child1.obj.getStatus().getState(), kChildState0);
  BOOST_REQUIRE_EQUAL(child2.obj.getStatus().getState(), kChildState0);
  BOOST_REQUIRE_EQUAL(child3.obj.getStatus().getState(), kChildState0);

  // Running transition: Should work, and move system/children into correct state
  BOOST_CHECK_NO_THROW( sysItoA->exec(gk, false) );
  BOOST_CHECK_EQUAL( sys->getStatus().getStateMachineId(), fsm.getId());
  BOOST_CHECK_EQUAL( sys->getStatus().getState(), kSysStateA );
  BOOST_CHECK_EQUAL( child1.obj.getStatus().getStateMachineId(), child1.fsm.getId()  );
  BOOST_CHECK_EQUAL( child2.obj.getStatus().getStateMachineId(), child2.fsm.getId()  );
  BOOST_CHECK_EQUAL( child3.obj.getStatus().getStateMachineId(), child3.fsm.getId()  );
  BOOST_CHECK_EQUAL( child1.obj.getStatus().getState(), kChildStateA );
  BOOST_CHECK_EQUAL( child2.obj.getStatus().getState(), kChildStateA );
  BOOST_CHECK_EQUAL( child3.obj.getStatus().getState(), kChildStateA );

  // Check the transition status object
  SystemTransitionSnapshot s = sysItoA->getStatus();
  BOOST_CHECK_EQUAL( s.getActionPath(), sysItoA->getPath() );
  BOOST_CHECK_EQUAL( s.getActionId(), sysItoA->getId());
  BOOST_CHECK_EQUAL( s.getActionableId(), sys->getId());
  BOOST_CHECK_EQUAL( s.getState(), Functionoid::State::kDone );
  BOOST_CHECK_EQUAL( s.getProgress(), 1.0 );
  BOOST_CHECK_GT( s.getRunningTime(), 0.0 );
  BOOST_CHECK_EQUAL( s.getMessage(), "" );
  BOOST_CHECK_EQUAL( s.getNumberOfCompletedSteps(), size_t(2) );
  BOOST_CHECK_EQUAL( s.getTotalNumberOfSteps(), size_t(2) );
  std::set<std::string> lExpectedEnabled = {child1.obj.getPath(), child2.obj.getPath(), child3.obj.getPath()};
  BOOST_CHECK_EQUAL_COLLECTIONS(s.getEnabledChildren().begin(), s.getEnabledChildren().end(), lExpectedEnabled.begin(), lExpectedEnabled.end());
  BOOST_REQUIRE( (s.begin()+2) == s.end());
  BOOST_REQUIRE_EQUAL( s.end() - s.begin(), 2);
  BOOST_CHECK_THROW( s.at(2), std::out_of_range);

  // Check status of step 1
  BOOST_CHECK_EQUAL( &*s.begin(), &s.at(0));
  BOOST_REQUIRE_EQUAL( s.begin()->size(), size_t(2) );
  const TransitionSnapshot& child1Status = *s.begin()->at(0);
  BOOST_CHECK_EQUAL( child1Status.getActionPath(), child1.trItoA->getPath() );
  BOOST_CHECK_EQUAL( child1Status.getActionId(), child1.trItoA->getId());
  BOOST_CHECK_EQUAL( child1Status.getActionableId(), child1.obj.getId());
  BOOST_CHECK_EQUAL( child1Status.getState(), Functionoid::State::kDone );
  BOOST_CHECK_EQUAL( child1Status.getProgress(), 1.0 );
  BOOST_CHECK_GT( child1Status.getRunningTime(), 0.0 );
  BOOST_CHECK_EQUAL( child1Status.getNumberOfCompletedCommands(), size_t(1) );
  BOOST_CHECK_EQUAL( child1Status.getTotalNumberOfCommands(), size_t(1) );
  BOOST_CHECK_EQUAL( child1Status.end() - child1Status.begin(), 1 );
  BOOST_CHECK_EQUAL( child1Status.getResults().size(), size_t(1) );
  const TransitionSnapshot& child2Status = *s.begin()->at(1);
  BOOST_CHECK_EQUAL( child2Status.getActionPath(), child2.trItoA->getPath() );
  BOOST_CHECK_EQUAL( child2Status.getActionId(), child2.trItoA->getId());
  BOOST_CHECK_EQUAL( child2Status.getActionableId(), child2.obj.getId());
  BOOST_CHECK_EQUAL( child2Status.getState(), Functionoid::State::kDone );
  BOOST_CHECK_EQUAL( child2Status.getProgress(), 1.0 );
  BOOST_CHECK_GT( child2Status.getRunningTime(), 0.0 );
  BOOST_CHECK_EQUAL( child2Status.getNumberOfCompletedCommands(), size_t(2) );
  BOOST_CHECK_EQUAL( child2Status.getTotalNumberOfCommands(), size_t(2) );
  BOOST_CHECK_EQUAL( child2Status.end() - child2Status.begin(), 2 );
  BOOST_CHECK_EQUAL( child2Status.getResults().size(), size_t(2) );

  // Check status of step 2
  BOOST_CHECK_EQUAL( &*(s.begin()+1), &s.at(1));
  BOOST_REQUIRE_EQUAL( (s.begin()+1)->size(), size_t(1) );
  const TransitionSnapshot& child3Status = *(s.begin()+1)->at(0);
  BOOST_CHECK_EQUAL( child3Status.getActionPath(), child3.trItoA->getPath() );
  BOOST_CHECK_EQUAL( child3Status.getActionId(), child3.trItoA->getId());
  BOOST_CHECK_EQUAL( child3Status.getActionableId(), child3.obj.getId());
  BOOST_CHECK_EQUAL( child3Status.getState(), Functionoid::State::kDone );
  BOOST_CHECK_EQUAL( child3Status.getProgress(), 1.0 );
  BOOST_CHECK_GT( child3Status.getRunningTime(), 0.0 );
  BOOST_CHECK_EQUAL( child3Status.getNumberOfCompletedCommands(), size_t(3) );
  BOOST_CHECK_EQUAL( child3Status.getTotalNumberOfCommands(), size_t(3) );
  BOOST_CHECK_EQUAL( child3Status.end() - child3Status.begin(), 3 );
  BOOST_CHECK_EQUAL( child3Status.getResults().size(), size_t(3) );
}


BOOST_FIXTURE_TEST_CASE(TestRunGoodTransitionWithChildrenDisabled, SystemStateMachineTestSetup)
{
  // SETUP:
  //   * Add child transitions & commands (from all children)
  child1.trItoA->add(child1.cmdNormal1);
  child2.trItoA->add(child2.cmdNormal1);
  child2.trItoA->add(child2.cmdNormal2);
  child3.trItoA->add(child3.cmdNormal1);
  child3.trItoA->add(child3.cmdNormal2);
  child3.trItoA->add(child3.cmdNormal1);
  sysItoA->add( std::vector<Transition*> {child1.trItoA, child2.trItoA});
  sysItoA->add( std::vector<Transition*> {child3.trItoA});
  //   * Engage FSM with child2 and child3 disabled
  DummyGateKeeper lGK;
  lGK.addDisabledId(child2.obj.getPath());
  lGK.addDisabledId(child3.obj.getPath());
  fsm.engage(lGK);

  // Check assumptions before starting unit tests
  BOOST_REQUIRE_EQUAL(sys->getStatus().getStateMachineId(), fsm.getId());
  BOOST_REQUIRE_EQUAL(sys->getStatus().getState(), kSysState0);
  BOOST_REQUIRE_EQUAL(sysItoA->getStatus().getState(), Functionoid::State::kInitial);
  BOOST_REQUIRE_EQUAL(child1.obj.getStatus().getStateMachineId(), child1.fsm.getId() );
  BOOST_REQUIRE_EQUAL(child1.obj.getStatus().getState(), kChildState0);
  BOOST_REQUIRE_EQUAL(child2.obj.getStatus().isEngaged(), false );
  BOOST_REQUIRE_EQUAL(child3.obj.getStatus().isEngaged(), false );
  BOOST_REQUIRE_EQUAL(child2.obj.getStatus().getState(), ActionableSnapshot::kNullStateId);
  BOOST_REQUIRE_EQUAL(child3.obj.getStatus().getState(), ActionableSnapshot::kNullStateId);

  // Running transition: Should work, but only call child1's transition
  BOOST_CHECK_NO_THROW( sysItoA->exec(gk, false) );
  BOOST_CHECK_EQUAL( sys->getStatus().getStateMachineId(), fsm.getId());
  BOOST_CHECK_EQUAL( sys->getStatus().getState(), kSysStateA );
  BOOST_CHECK_EQUAL( child1.obj.getStatus().getStateMachineId(), child1.fsm.getId()  );
  BOOST_CHECK_EQUAL( child1.obj.getStatus().getState(), kChildStateA );
  BOOST_CHECK_EQUAL( child2.obj.getStatus().isEngaged(), false );
  BOOST_CHECK_EQUAL( child3.obj.getStatus().isEngaged(), false );
  BOOST_CHECK_EQUAL( child2.obj.getStatus().getState(), ActionableSnapshot::kNullStateId );
  BOOST_CHECK_EQUAL( child3.obj.getStatus().getState(), ActionableSnapshot::kNullStateId );

  // Check the transition status object
  SystemTransitionSnapshot s = sysItoA->getStatus();
  BOOST_CHECK_EQUAL( s.getActionPath(), sysItoA->getPath() );
  BOOST_CHECK_EQUAL( s.getActionId(), sysItoA->getId());
  BOOST_CHECK_EQUAL( s.getActionableId(), sys->getId());
  BOOST_CHECK_EQUAL( s.getState(), Functionoid::State::kDone );
  BOOST_CHECK_EQUAL( s.getProgress(), 1.0 );
  BOOST_CHECK_GT( s.getRunningTime(), 0.0 );
  BOOST_CHECK_EQUAL( s.getMessage(), "");
  BOOST_CHECK_EQUAL( s.getNumberOfCompletedSteps(), size_t(2) );
  BOOST_CHECK_EQUAL( s.getTotalNumberOfSteps(), size_t(2) );
  std::set<std::string> lExpectedEnabled = {child1.obj.getPath()};
  BOOST_CHECK_EQUAL_COLLECTIONS(s.getEnabledChildren().begin(), s.getEnabledChildren().end(), lExpectedEnabled.begin(), lExpectedEnabled.end());
  BOOST_REQUIRE( (s.begin()+2) == s.end());
  BOOST_REQUIRE_EQUAL( s.end() - s.begin(), 2);
  BOOST_CHECK_THROW( s.at(2), std::out_of_range);

  // Check status of step 1:
  //  * Should be 2-element vector
  BOOST_CHECK_EQUAL( &*s.begin(), &s.at(0));
  BOOST_REQUIRE_EQUAL( s.begin()->size(), size_t(2) );
  //  * 1st element: child1's status
  BOOST_REQUIRE( s.begin()->at(0) != NULL );
  const TransitionSnapshot& child1Status = *s.begin()->at(0);
  BOOST_CHECK_EQUAL( child1Status.getActionPath(), child1.trItoA->getPath() );
  BOOST_CHECK_EQUAL( child1Status.getActionId(), child1.trItoA->getId());
  BOOST_CHECK_EQUAL( child1Status.getActionableId(), child1.obj.getId());
  BOOST_CHECK_EQUAL( child1Status.getState(), Functionoid::State::kDone );
  BOOST_CHECK_EQUAL( child1Status.getProgress(), 1.0 );
  BOOST_CHECK_GT( child1Status.getRunningTime(), 0.0 );
  BOOST_CHECK_EQUAL( child1Status.getNumberOfCompletedCommands(), size_t(1) );
  BOOST_CHECK_EQUAL( child1Status.getTotalNumberOfCommands(), size_t(1) );
  BOOST_CHECK_EQUAL( child1Status.end() - child1Status.begin(), 1 );
  BOOST_CHECK_EQUAL( child1Status.getResults().size(), size_t(1) );
  //  * 2nd element: child2's transition - child2 was disabled, so pointer should be NULL
  BOOST_CHECK( s.begin()->at(1) == NULL );
  //    Also check that child2's commands weren't run
  BOOST_CHECK_EQUAL( child2.cmdNormal1.getStatus().getState(), Functionoid::State::kInitial );
  BOOST_CHECK_EQUAL( child2.cmdNormal2.getStatus().getState(), Functionoid::State::kInitial );

  // Check status of step 2
  // * Should be 1-element vector
  // * 1st element: child3's transition - child3 was disabled, so pointer should be NULL
  BOOST_CHECK_EQUAL( &*(s.begin()+1), &s.at(1));
  BOOST_REQUIRE_EQUAL( (s.begin()+1)->size(), size_t(1) );
  BOOST_CHECK( (s.begin()+1)->at(0) == NULL );
  //    Also check that child3's commands weren't run
  BOOST_CHECK_EQUAL( child3.cmdNormal1.getStatus().getState(), Functionoid::State::kInitial );
  BOOST_CHECK_EQUAL( child3.cmdNormal2.getStatus().getState(), Functionoid::State::kInitial );
}


void testRunWarningTransition(bool aRegisterAnalyser)
{
  SystemStateMachineTestSetup lSetup;
  typedef SystemStateMachineTestSetup TestSetup_t;

  // Preparation: Add child transitions & commands, engage FSM; then check state before starting unit tests
  lSetup.child1.trItoA->add(lSetup.child1.cmdNormal1);
  lSetup.child2.trItoA->add(lSetup.child2.cmdWarning);
  lSetup.child2.trItoA->add(lSetup.child2.cmdNormal2);
  lSetup.child3.trItoA->add(lSetup.child3.cmdNormal1);
  lSetup.child3.trItoA->add(lSetup.child3.cmdNormal2);
  lSetup.child3.trItoA->add(lSetup.child3.cmdNormal1);
  lSetup.sysItoA->add( std::vector<Transition*> {lSetup.child1.trItoA, lSetup.child2.trItoA});
  lSetup.sysItoA->add( std::vector<Transition*> {lSetup.child3.trItoA});
  if (aRegisterAnalyser)
    lSetup.sysItoA->registerWarningAnalyser(TestSetup_t::analyseWarning);
  lSetup.fsm.engage(DummyGateKeeper());
  BOOST_REQUIRE_EQUAL(lSetup.sys->getStatus().getStateMachineId(), lSetup.fsm.getId());
  BOOST_REQUIRE_EQUAL(lSetup.sys->getStatus().getState(), TestSetup_t::kSysState0);
  BOOST_REQUIRE_EQUAL(lSetup.sysItoA->getStatus().getState(), Functionoid::State::kInitial);
  BOOST_REQUIRE_EQUAL(lSetup.child1.obj.getStatus().getStateMachineId(), lSetup.child1.fsm.getId() );
  BOOST_REQUIRE_EQUAL(lSetup.child2.obj.getStatus().getStateMachineId(), lSetup.child2.fsm.getId() );
  BOOST_REQUIRE_EQUAL(lSetup.child3.obj.getStatus().getStateMachineId(), lSetup.child3.fsm.getId() );
  BOOST_REQUIRE_EQUAL(lSetup.child1.obj.getStatus().getState(), TestSetup_t::kChildState0);
  BOOST_REQUIRE_EQUAL(lSetup.child2.obj.getStatus().getState(), TestSetup_t::kChildState0);
  BOOST_REQUIRE_EQUAL(lSetup.child3.obj.getStatus().getState(), TestSetup_t::kChildState0);

  // Running transition: Should work, and move system/children into correct state
  BOOST_CHECK_NO_THROW( lSetup.sysItoA->exec(lSetup.gk, false) );
  BOOST_CHECK_EQUAL( lSetup.sys->getStatus().getStateMachineId(), lSetup.fsm.getId());
  BOOST_CHECK_EQUAL( lSetup.sys->getStatus().getState(), TestSetup_t::kSysStateA );
  BOOST_CHECK_EQUAL( lSetup.child1.obj.getStatus().getStateMachineId(), lSetup.child1.fsm.getId()  );
  BOOST_CHECK_EQUAL( lSetup.child2.obj.getStatus().getStateMachineId(), lSetup.child2.fsm.getId()  );
  BOOST_CHECK_EQUAL( lSetup.child3.obj.getStatus().getStateMachineId(), lSetup.child3.fsm.getId()  );
  BOOST_CHECK_EQUAL( lSetup.child1.obj.getStatus().getState(), TestSetup_t::kChildStateA );
  BOOST_CHECK_EQUAL( lSetup.child2.obj.getStatus().getState(), TestSetup_t::kChildStateA );
  BOOST_CHECK_EQUAL( lSetup.child3.obj.getStatus().getState(), TestSetup_t::kChildStateA );

  // Check the transition status object
  SystemTransitionSnapshot s = lSetup.sysItoA->getStatus();
  BOOST_CHECK_EQUAL( s.getActionPath(), lSetup.sysItoA->getPath() );
  BOOST_CHECK_EQUAL( s.getActionId(), lSetup.sysItoA->getId());
  BOOST_CHECK_EQUAL( s.getActionableId(), lSetup.sys->getId());
  BOOST_CHECK_EQUAL( s.getState(), Functionoid::State::kWarning );
  BOOST_CHECK_EQUAL( s.getProgress(), 1.0 );
  BOOST_CHECK_GT( s.getRunningTime(), 0.0 );
  if (aRegisterAnalyser)
    BOOST_CHECK_EQUAL( s.getMessage(), TestSetup_t::analyseWarning(s) );
  else
    BOOST_CHECK_EQUAL( s.getMessage(), "" );
  BOOST_CHECK_EQUAL( s.getNumberOfCompletedSteps(), size_t(2) );
  BOOST_CHECK_EQUAL( s.getTotalNumberOfSteps(), size_t(2) );
  std::set<std::string> lExpectedEnabled = {lSetup.child1.obj.getPath(), lSetup.child2.obj.getPath(), lSetup.child3.obj.getPath()};
  BOOST_CHECK_EQUAL_COLLECTIONS(s.getEnabledChildren().begin(), s.getEnabledChildren().end(), lExpectedEnabled.begin(), lExpectedEnabled.end());
  BOOST_REQUIRE( (s.begin()+2) == s.end());
  BOOST_REQUIRE_EQUAL( s.end() - s.begin(), 2 );
  BOOST_CHECK_THROW( s.at(2), std::out_of_range);

  // Check status of step 1
  BOOST_CHECK_EQUAL( &*s.begin(), &s.at(0));
  BOOST_REQUIRE_EQUAL( s.begin()->size(), size_t(2) );
  const TransitionSnapshot& child1Status = *s.begin()->at(0);
  BOOST_CHECK_EQUAL( child1Status.getActionPath(), lSetup.child1.trItoA->getPath() );
  BOOST_CHECK_EQUAL( child1Status.getActionId(), lSetup.child1.trItoA->getId());
  BOOST_CHECK_EQUAL( child1Status.getActionableId(), lSetup.child1.obj.getId());
  BOOST_CHECK_EQUAL( child1Status.getState(), Functionoid::State::kDone );
  BOOST_CHECK_EQUAL( child1Status.getProgress(), 1.0 );
  BOOST_CHECK_GT( child1Status.getRunningTime(), 0.0 );
  BOOST_CHECK_EQUAL( child1Status.getNumberOfCompletedCommands(), size_t(1) );
  BOOST_CHECK_EQUAL( child1Status.getTotalNumberOfCommands(), size_t(1) );
  BOOST_CHECK_EQUAL( child1Status.end() - child1Status.begin(), 1 );
  BOOST_CHECK_EQUAL( child1Status.getResults().size(), size_t(1) );
  const TransitionSnapshot& child2Status = *s.begin()->at(1);
  BOOST_CHECK_EQUAL( child2Status.getActionPath(), lSetup.child2.trItoA->getPath() );
  BOOST_CHECK_EQUAL( child2Status.getActionId(), lSetup.child2.trItoA->getId());
  BOOST_CHECK_EQUAL( child2Status.getActionableId(), lSetup.child2.obj.getId());
  BOOST_CHECK_EQUAL( child2Status.getState(), Functionoid::State::kWarning );
  BOOST_CHECK_EQUAL( child2Status.getProgress(), 1.0 );
  BOOST_CHECK_GT( child2Status.getRunningTime(), 0.0 );
  BOOST_CHECK_EQUAL( child2Status.getNumberOfCompletedCommands(), size_t(2) );
  BOOST_CHECK_EQUAL( child2Status.getTotalNumberOfCommands(), size_t(2) );
  BOOST_CHECK_EQUAL( child2Status.end() - child2Status.begin(), 2 );
  BOOST_CHECK_EQUAL( child2Status.getResults().size(), size_t(2) );

  // Check status of step 2
  BOOST_CHECK_EQUAL( &*(s.begin()+1), &s.at(1));
  BOOST_REQUIRE_EQUAL( (s.begin()+1)->size(), size_t(1) );
  const TransitionSnapshot& child3Status = *(s.begin()+1)->at(0);
  BOOST_CHECK_EQUAL( child3Status.getActionPath(), lSetup.child3.trItoA->getPath() );
  BOOST_CHECK_EQUAL( child3Status.getActionId(), lSetup.child3.trItoA->getId());
  BOOST_CHECK_EQUAL( child3Status.getActionableId(), lSetup.child3.obj.getId());
  BOOST_CHECK_EQUAL( child3Status.getState(), Functionoid::State::kDone );
  BOOST_CHECK_EQUAL( child3Status.getProgress(), 1.0 );
  BOOST_CHECK_GT( child3Status.getRunningTime(), 0.0 );
  BOOST_CHECK_EQUAL( child3Status.getNumberOfCompletedCommands(), size_t(3) );
  BOOST_CHECK_EQUAL( child3Status.getTotalNumberOfCommands(), size_t(3) );
  BOOST_CHECK_EQUAL( child3Status.end() - child3Status.begin(), 3 );
  BOOST_CHECK_EQUAL( child3Status.getResults().size(), size_t(3) );
}


BOOST_FIXTURE_TEST_CASE(TestRunWarningTransitionWithoutAnalyser, SystemStateMachineTestSetup)
{
  testRunWarningTransition(false);
}

BOOST_FIXTURE_TEST_CASE(TestRunWarningTransitionWithAnalyser, SystemStateMachineTestSetup)
{
  testRunWarningTransition(true);
}


void testRunErrorTransition(bool aRegisterErrorAnalyser)
{
  SystemStateMachineTestSetup lSetup;
  typedef SystemStateMachineTestSetup TestSetup_t;

  // Preparation: Add child transitions & commands, engage FSM; then check state before starting unit tests
  lSetup.child1.trItoA->add(lSetup.child1.cmdNormal1);
  lSetup.child2.trItoA->add(lSetup.child2.cmdError);
  lSetup.child2.trItoA->add(lSetup.child2.cmdNormal2);
  lSetup.child3.trItoA->add(lSetup.child3.cmdNormal1);
  lSetup.child3.trItoA->add(lSetup.child3.cmdNormal2);
  lSetup.child3.trItoA->add(lSetup.child3.cmdNormal1);
  lSetup.sysItoA->add( std::vector<Transition*> {lSetup.child1.trItoA, lSetup.child2.trItoA});
  lSetup.sysItoA->add( std::vector<Transition*> {lSetup.child3.trItoA});
  if (aRegisterErrorAnalyser)
    lSetup.sysItoA->registerErrorAnalyser(TestSetup_t::analyseError);
  lSetup.fsm.engage(DummyGateKeeper());
  BOOST_REQUIRE_EQUAL(lSetup.sys->getStatus().getStateMachineId(), lSetup.fsm.getId());
  BOOST_REQUIRE_EQUAL(lSetup.sys->getStatus().getState(), TestSetup_t::kSysState0);
  BOOST_REQUIRE_EQUAL(lSetup.sysItoA->getStatus().getState(), Functionoid::State::kInitial);
  BOOST_REQUIRE_EQUAL(lSetup.child1.obj.getStatus().getStateMachineId(), lSetup.child1.fsm.getId() );
  BOOST_REQUIRE_EQUAL(lSetup.child2.obj.getStatus().getStateMachineId(), lSetup.child2.fsm.getId() );
  BOOST_REQUIRE_EQUAL(lSetup.child3.obj.getStatus().getStateMachineId(), lSetup.child3.fsm.getId() );
  BOOST_REQUIRE_EQUAL(lSetup.child1.obj.getStatus().getState(), TestSetup_t::kChildState0);
  BOOST_REQUIRE_EQUAL(lSetup.child2.obj.getStatus().getState(), TestSetup_t::kChildState0);
  BOOST_REQUIRE_EQUAL(lSetup.child3.obj.getStatus().getState(), TestSetup_t::kChildState0);

  // Running transition: Should move system & child2 into error state, child1 into A, and not affect child3
  BOOST_CHECK_NO_THROW( lSetup.sysItoA->exec(lSetup.gk, false) );
  BOOST_CHECK_EQUAL( lSetup.sys->getStatus().getStateMachineId(), lSetup.fsm.getId());
  BOOST_CHECK_EQUAL( lSetup.sys->getStatus().getState(), TestSetup_t::kSysStateErr );
  BOOST_CHECK_EQUAL( lSetup.child1.obj.getStatus().getStateMachineId(), lSetup.child1.fsm.getId()  );
  BOOST_CHECK_EQUAL( lSetup.child2.obj.getStatus().getStateMachineId(), lSetup.child2.fsm.getId()  );
  BOOST_CHECK_EQUAL( lSetup.child3.obj.getStatus().getStateMachineId(), lSetup.child3.fsm.getId()  );
  BOOST_CHECK_EQUAL( lSetup.child1.obj.getStatus().getState(), TestSetup_t::kChildStateA );
  BOOST_CHECK_EQUAL( lSetup.child2.obj.getStatus().getState(), TestSetup_t::kChildStateErr );
  BOOST_CHECK_EQUAL( lSetup.child3.obj.getStatus().getState(), TestSetup_t::kChildState0 );

  // Check the transition status object
  SystemTransitionSnapshot s = lSetup.sysItoA->getStatus();
  BOOST_CHECK_EQUAL( s.getActionPath(), lSetup.sysItoA->getPath() );
  BOOST_CHECK_EQUAL( s.getActionId(), lSetup.sysItoA->getId());
  BOOST_CHECK_EQUAL( s.getActionableId(), lSetup.sys->getId());
  BOOST_CHECK_EQUAL( s.getState(), Functionoid::State::kError );
  BOOST_CHECK_EQUAL( s.getProgress(), DummyErrorCommand::kFinalProgress / float(4) );
  BOOST_CHECK_GT( s.getRunningTime(), 0.0 );
  if (aRegisterErrorAnalyser)
    BOOST_CHECK_EQUAL( s.getMessage(), SystemStateMachineTestSetup::analyseError(s) );
  else
    BOOST_CHECK_EQUAL( s.getMessage(), "" );
  BOOST_CHECK_EQUAL( s.getNumberOfCompletedSteps(), size_t(1) );
  BOOST_CHECK_EQUAL( s.getTotalNumberOfSteps(), size_t(2) );
  std::set<std::string> lExpectedEnabled = {lSetup.child1.obj.getPath(), lSetup.child2.obj.getPath(), lSetup.child3.obj.getPath()};
  BOOST_CHECK_EQUAL_COLLECTIONS(s.getEnabledChildren().begin(), s.getEnabledChildren().end(), lExpectedEnabled.begin(), lExpectedEnabled.end());
  BOOST_REQUIRE( (s.begin()+1) == s.end());
  BOOST_REQUIRE_EQUAL( s.end() - s.begin(), 1 );
  BOOST_CHECK_THROW( s.at(1), std::out_of_range);

  // Check status of step 1
  BOOST_CHECK_EQUAL( &*s.begin(), &s.at(0));
  BOOST_REQUIRE_EQUAL( s.begin()->size(), size_t(2) );
  const TransitionSnapshot& child1Status = *s.begin()->at(0);
  BOOST_CHECK_EQUAL( child1Status.getActionPath(), lSetup.child1.trItoA->getPath() );
  BOOST_CHECK_EQUAL( child1Status.getActionId(), lSetup.child1.trItoA->getId());
  BOOST_CHECK_EQUAL( child1Status.getActionableId(), lSetup.child1.obj.getId());
  BOOST_CHECK_EQUAL( child1Status.getState(), Functionoid::State::kDone );
  BOOST_CHECK_EQUAL( child1Status.getProgress(), 1.0 );
  BOOST_CHECK_GT( child1Status.getRunningTime(), 0.0 );
  BOOST_CHECK_EQUAL( child1Status.getNumberOfCompletedCommands(), size_t(1) );
  BOOST_CHECK_EQUAL( child1Status.getTotalNumberOfCommands(), size_t(1) );
  BOOST_CHECK_EQUAL( child1Status.end() - child1Status.begin(), 1 );
  BOOST_CHECK_EQUAL( child1Status.getResults().size(), size_t(1) );
  const TransitionSnapshot& child2Status = *s.begin()->at(1);
  BOOST_CHECK_EQUAL( child2Status.getActionPath(), lSetup.child2.trItoA->getPath() );
  BOOST_CHECK_EQUAL( child2Status.getActionId(), lSetup.child2.trItoA->getId());
  BOOST_CHECK_EQUAL( child2Status.getActionableId(), lSetup.child2.obj.getId());
  BOOST_CHECK_EQUAL( child2Status.getState(), Functionoid::State::kError );
  BOOST_CHECK_EQUAL( child2Status.getProgress(), DummyErrorCommand::kFinalProgress / float(2) );
  BOOST_CHECK_GT( child2Status.getRunningTime(), 0.0 );
  BOOST_CHECK_EQUAL( child2Status.getNumberOfCompletedCommands(), size_t(1) );
  BOOST_CHECK_EQUAL( child2Status.getTotalNumberOfCommands(), size_t(2) );
  BOOST_CHECK_EQUAL( child2Status.end() - child2Status.begin(), 1 );
  BOOST_CHECK_EQUAL( child2Status.getResults().size(), size_t(1) );
  BOOST_CHECK_EQUAL( lSetup.child2.cmdNormal1.getStatus().getState(), Functionoid::State::kInitial );
  BOOST_CHECK_EQUAL( lSetup.child2.cmdNormal2.getStatus().getState(), Functionoid::State::kInitial );

  // Check that stopped before step 2 - i.e. none of child3's commands were run
  BOOST_CHECK_EQUAL( lSetup.child3.trItoA->getStatus().getState(), Functionoid::State::kInitial );
  BOOST_CHECK_EQUAL( lSetup.child3.cmdNormal1.getStatus().getState(), Functionoid::State::kInitial );
  BOOST_CHECK_EQUAL( lSetup.child3.cmdNormal2.getStatus().getState(), Functionoid::State::kInitial );
}


BOOST_AUTO_TEST_CASE(TestRunErrorTransitionWithoutAnalyser)
{
  testRunErrorTransition(false);
}

BOOST_AUTO_TEST_CASE(TestRunErrorTransitionWithAnalyser)
{
  testRunErrorTransition(true);
}


BOOST_FIXTURE_TEST_CASE(TestRunTransitionWrongSystemState, SystemStateMachineTestSetup)
{
  // Preparation: Engage FSM, check state before starting unit tests
  fsm.engage(DummyGateKeeper());
  BOOST_REQUIRE_EQUAL(sys->getStatus().getStateMachineId(), fsm.getId());
  BOOST_REQUIRE_EQUAL(sys->getStatus().getState(), kSysState0);
  BOOST_REQUIRE_EQUAL(sysItoA->getStatus().getState(), Functionoid::State::kInitial);
  BOOST_REQUIRE_EQUAL(sysAtoB->getStatus().getState(), Functionoid::State::kInitial);

  // Running transition A->B from initial state: Should throw, and change nothing
  BOOST_CHECK_THROW( sysAtoB->exec(DummyGateKeeper(), false), ResourceInWrongState );
  BOOST_CHECK_EQUAL(sysAtoB->getStatus().getState(), Functionoid::State::kInitial);
  BOOST_CHECK_EQUAL(sys->getStatus().getStateMachineId(), fsm.getId());
  BOOST_CHECK_EQUAL(sys->getStatus().getState(), kSysState0);

  // Run transition I->A to prepare for 2nd test
  sysItoA->exec(DummyGateKeeper(), false);
  BOOST_REQUIRE_EQUAL(sys->getStatus().getStateMachineId(), fsm.getId());
  BOOST_REQUIRE_EQUAL(sys->getStatus().getState(), kSysStateA);
  BOOST_REQUIRE_EQUAL(sysAtoB->getStatus().getState(), Functionoid::State::kInitial);

  // Running transition I->A from state A: Should throw, and change nothing
  BOOST_CHECK_THROW( sysItoA->exec(DummyGateKeeper(), false), ResourceInWrongState);
  BOOST_CHECK_EQUAL(sysAtoB->getStatus().getState(), Functionoid::State::kInitial);
  BOOST_CHECK_EQUAL(sys->getStatus().getStateMachineId(), fsm.getId());
  BOOST_CHECK_EQUAL(sys->getStatus().getState(), kSysStateA);
}


BOOST_FIXTURE_TEST_CASE(TestRunTransitionWrongChildState, SystemStateMachineTestSetup)
{
  // Preparation: Add children to transitions, engage FSM, run child1 I->A already, and check state before starting unit tests
  sysItoA->add(children.begin(), children.end(), child1.fsm.getId(), kChildState0, child1.trItoA->getId());
  sysItoA->add(std::vector<Transition*> {&child1.fsm.addTransition("tmp_AtoA", kChildStateA, kChildStateA)});
  fsm.engage(DummyGateKeeper());
  child1.trItoA->exec(gk, false);
  BOOST_REQUIRE_EQUAL(sys->getStatus().getStateMachineId(), fsm.getId());
  BOOST_REQUIRE_EQUAL(sys->getStatus().getState(), kSysState0);
  BOOST_REQUIRE_EQUAL(sysItoA->getStatus().getState(), Functionoid::State::kInitial);
  BOOST_REQUIRE_EQUAL(sysAtoB->getStatus().getState(), Functionoid::State::kInitial);
  BOOST_REQUIRE_EQUAL(child1.obj.getStatus().getStateMachineId(), child1.fsm.getId() );
  BOOST_REQUIRE_EQUAL(child2.obj.getStatus().getStateMachineId(), child2.fsm.getId() );
  BOOST_REQUIRE_EQUAL(child3.obj.getStatus().getStateMachineId(), child3.fsm.getId() );
  BOOST_REQUIRE_EQUAL(child1.obj.getStatus().getState(), kChildStateA);
  BOOST_REQUIRE_EQUAL(child2.obj.getStatus().getState(), kChildState0);
  BOOST_REQUIRE_EQUAL(child3.obj.getStatus().getState(), kChildState0);

  // Running transition I->A with child1 in state A: Should throw, and change nothing
  BOOST_CHECK_THROW( sysItoA->exec(DummyGateKeeper()), ResourceInWrongState );
  do {}
  while (sysItoA->getStatus().getState() == Functionoid::State::kScheduled || sysItoA->getStatus().getState() == Functionoid::State::kRunning);
  BOOST_CHECK_EQUAL(sys->getStatus().getStateMachineId(), fsm.getId());
  BOOST_CHECK_EQUAL(sys->getStatus().getState(), kSysState0);
  BOOST_CHECK_EQUAL(sysItoA->getStatus().getState(), Functionoid::State::kInitial);
  BOOST_CHECK_EQUAL(sysAtoB->getStatus().getState(), Functionoid::State::kInitial);
  BOOST_CHECK_EQUAL(child1.obj.getStatus().getStateMachineId(), child1.fsm.getId() );
  BOOST_CHECK_EQUAL(child2.obj.getStatus().getStateMachineId(), child2.fsm.getId() );
  BOOST_CHECK_EQUAL(child3.obj.getStatus().getStateMachineId(), child3.fsm.getId());
  BOOST_CHECK_EQUAL(child1.obj.getStatus().getState(), kChildStateA);
  BOOST_CHECK_EQUAL(child2.obj.getStatus().getState(), kChildState0);
  BOOST_CHECK_EQUAL(child3.obj.getStatus().getState(), kChildState0);

  // Reset child1 & run transition I->A to prepare for 2nd test
  child1.fsm.reset(gk);
  sysItoA->exec(gk, false);
  sysAtoB->add(std::vector<Transition*> {child1.trAtoB, child2.trAtoB});
  sysAtoB->add(std::vector<Transition*> {child3.trAtoB});
  child3.fsm.reset(gk);
  BOOST_REQUIRE_EQUAL(sys->getStatus().getStateMachineId(), fsm.getId());
  BOOST_REQUIRE_EQUAL(sys->getStatus().getState(), kSysStateA);
  BOOST_REQUIRE_EQUAL(sysItoA->getStatus().getState(), Functionoid::State::kDone);
  BOOST_REQUIRE_EQUAL(sysAtoB->getStatus().getState(), Functionoid::State::kInitial);
  BOOST_REQUIRE_EQUAL(child1.obj.getStatus().getStateMachineId(), child1.fsm.getId());
  BOOST_REQUIRE_EQUAL(child2.obj.getStatus().getStateMachineId(), child2.fsm.getId());
  BOOST_REQUIRE_EQUAL(child3.obj.getStatus().getStateMachineId(), child3.fsm.getId());
  BOOST_REQUIRE_EQUAL(child1.obj.getStatus().getState(), kChildStateA);
  BOOST_REQUIRE_EQUAL(child2.obj.getStatus().getState(), kChildStateA);
  BOOST_REQUIRE_EQUAL(child3.obj.getStatus().getState(), kChildState0);

  // Running transition A->B with child3 (2nd step) in state I: Should throw, and change nothing
  BOOST_CHECK_THROW( sysAtoB->exec(DummyGateKeeper()), ResourceInWrongState);
  do {}
  while (sysAtoB->getStatus().getState() == Functionoid::State::kScheduled || sysAtoB->getStatus().getState() == Functionoid::State::kRunning);
  BOOST_CHECK_EQUAL(sys->getStatus().getStateMachineId(), fsm.getId());
  BOOST_CHECK_EQUAL(sys->getStatus().getState(), kSysStateA);
  BOOST_CHECK_EQUAL(sysItoA->getStatus().getState(), Functionoid::State::kDone);
  BOOST_CHECK_EQUAL(sysAtoB->getStatus().getState(), Functionoid::State::kInitial);
  BOOST_CHECK_EQUAL(child1.obj.getStatus().getStateMachineId(), child1.fsm.getId());
  BOOST_CHECK_EQUAL(child2.obj.getStatus().getStateMachineId(), child2.fsm.getId());
  BOOST_CHECK_EQUAL(child3.obj.getStatus().getStateMachineId(), child3.fsm.getId());
  BOOST_CHECK_EQUAL(child1.obj.getStatus().getState(), kChildStateA);
  BOOST_CHECK_EQUAL(child2.obj.getStatus().getState(), kChildStateA);
  BOOST_CHECK_EQUAL(child3.obj.getStatus().getState(), kChildState0);
}


BOOST_FIXTURE_TEST_CASE(TestRunTransitionDisengagedChildFSM, SystemStateMachineTestSetup)
{
  // Preparation:
  //   * Define transitions: I->A = child1 & child2
  //                         A->B = all three children
  //   * Engage system FSM & then engage child1 in another FSM w/ same initial state
  //   * Require that state is correct before testing
  sysItoA->add(children.begin(), children.end()-1, child1.fsm.getId(), kChildState0, child1.trItoA->getId());
  sysAtoB->add(children.begin(), children.end(), child1.fsm.getId(), kChildStateA, child1.trAtoB->getId());
  fsm.engage(DummyGateKeeper());
  child1.fsm.disengage();
  StateMachine& child1OtherFSM = child1.obj.registerStateMachine("anotherFSM", kChildState0, kChildStateErr);
  child1OtherFSM.engage(DummyGateKeeper());
  BOOST_REQUIRE( fsm.getParticipants() == (std::set<const StateMachine*> {&child1.fsm, &child2.fsm, &child3.fsm}) );
  BOOST_REQUIRE_EQUAL(sys->getStatus().getStateMachineId(), fsm.getId());
  BOOST_REQUIRE_EQUAL(sys->getStatus().getState(), kSysState0);
  BOOST_REQUIRE_EQUAL(child1.obj.getStatus().getStateMachineId(), child1OtherFSM.getId());
  BOOST_REQUIRE_EQUAL(child2.obj.getStatus().getStateMachineId(), child2.fsm.getId());
  BOOST_REQUIRE_EQUAL(child3.obj.getStatus().getStateMachineId(), child3.fsm.getId());
  BOOST_REQUIRE_EQUAL(child1.obj.getStatus().getState(), kChildState0);
  BOOST_REQUIRE_EQUAL(child2.obj.getStatus().getState(), kChildState0);
  BOOST_REQUIRE_EQUAL(child3.obj.getStatus().getState(), kChildState0);

  // Running transition after child FSM has been disengaged: should throw, and leave state/TransitionStatus unchanged
  BOOST_CHECK_THROW( sysItoA->exec(gk, false), ResourceInWrongStateMachine);
  BOOST_CHECK_EQUAL( sysItoA->getStatus().getState(), Functionoid::State::kInitial);
  BOOST_CHECK_EQUAL( sys->getStatus().getStateMachineId(), fsm.getId());
  BOOST_CHECK_EQUAL( sys->getStatus().getState(), kSysState0);
  BOOST_CHECK_EQUAL( child1.obj.getStatus().getStateMachineId(), child1OtherFSM.getId());
  BOOST_CHECK_EQUAL( child1.obj.getStatus().getState(), kChildState0);
  BOOST_CHECK_EQUAL( child2.obj.getStatus().getStateMachineId(), child2.fsm.getId());
  BOOST_CHECK_EQUAL( child2.obj.getStatus().getState(), kChildState0);
  BOOST_CHECK_EQUAL( child3.obj.getStatus().getStateMachineId(), child3.fsm.getId());
  BOOST_CHECK_EQUAL( child3.obj.getStatus().getState(), kChildState0);

  // Running transition after child1 engaged in correct FSM again:
  // ... should work, even with child3 in wrong FSM
  child1OtherFSM.disengage();
  child1.fsm.engage(DummyGateKeeper());
  child3.fsm.disengage();
  BOOST_CHECK_NO_THROW( sysItoA->exec(gk, false));
  BOOST_CHECK_EQUAL( sys->getStatus().getStateMachineId(), fsm.getId() );
  BOOST_CHECK_EQUAL( sys->getStatus().getState(), kSysStateA);
  BOOST_CHECK_EQUAL( child1.obj.getStatus().getStateMachineId(), child1.fsm.getId());
  BOOST_CHECK_EQUAL( child1.obj.getStatus().getState(), kChildStateA);
  BOOST_CHECK_EQUAL( child2.obj.getStatus().getStateMachineId(), child2.fsm.getId());
  BOOST_CHECK_EQUAL( child2.obj.getStatus().getState(), kChildStateA);
  BOOST_CHECK_EQUAL( child3.obj.getStatus().getStateMachineId(), ActionableSnapshot::kNullStateMachineId);
  BOOST_CHECK_EQUAL( child3.obj.getStatus().getState(), ActionableSnapshot::kNullStateId);
  SystemTransitionSnapshot s = sysItoA->getStatus();
  BOOST_CHECK_EQUAL( s.getActionPath(), sysItoA->getPath());
  BOOST_CHECK_EQUAL( s.getActionId(), sysItoA->getId());
  BOOST_CHECK_EQUAL( s.getActionableId(), sys->getId());
  BOOST_CHECK_EQUAL( s.getState(), Functionoid::State::kDone);
  BOOST_CHECK_EQUAL( s.getProgress(), 1.0);
  BOOST_CHECK_GE( s.getRunningTime(), 0.0);
  BOOST_CHECK_EQUAL( s.getMessage(), "");
  BOOST_CHECK_EQUAL( s.getNumberOfCompletedSteps(), size_t(1));
  BOOST_CHECK_EQUAL( s.getTotalNumberOfSteps(), size_t(1));
  std::set<std::string> lExpectedEnabled = {child1.obj.getPath(), child2.obj.getPath()};
  BOOST_CHECK_EQUAL_COLLECTIONS(s.getEnabledChildren().begin(), s.getEnabledChildren().end(), lExpectedEnabled.begin(), lExpectedEnabled.end());
  BOOST_REQUIRE( (s.begin()+1) == s.end());
  BOOST_REQUIRE_EQUAL( s.end() - s.begin(), 1 );
  BOOST_CHECK_THROW( s.at(1), std::out_of_range);

  BOOST_CHECK_EQUAL( &*s.begin(), &s.at(0));
  BOOST_CHECK_EQUAL( s.begin()->size(), size_t(2));
}



/* ---------------------------------------------------------------- */
/*   PART 4: RESETTING                                              */
/* ---------------------------------------------------------------- */

BOOST_FIXTURE_TEST_CASE(TestResetFSM, SystemStateMachineTestSetup)
{
  // Add children to system FSM, engage, and confirm initial state to prepare for following tests
  sysItoA->add(children.begin(), children.end()-1, child1.fsm.getId(), kChildState0, child1.trItoA->getId());
  BOOST_REQUIRE( fsm.getParticipants() == (std::set<const StateMachine*> {&child1.fsm, &child2.fsm}));
  BOOST_REQUIRE_NO_THROW( fsm.engage(DummyGateKeeper()) );
  BOOST_REQUIRE_EQUAL( sys->getStatus().getStateMachineId(), fsm.getId() );
  BOOST_REQUIRE_EQUAL( sys->getStatus().getState(), kSysState0 );
  BOOST_REQUIRE_EQUAL( child1.obj.getStatus().getStateMachineId(), child1.fsm.getId() );
  BOOST_REQUIRE_EQUAL( child2.obj.getStatus().getStateMachineId(), child2.fsm.getId() );
  BOOST_REQUIRE_EQUAL( child3.obj.getStatus().getStateMachineId(), ActionableSnapshot::kNullStateMachineId );
  BOOST_REQUIRE_EQUAL( child1.obj.getStatus().getState(), kChildState0 );
  BOOST_REQUIRE_EQUAL( child2.obj.getStatus().getState(), kChildState0 );
  BOOST_REQUIRE_EQUAL( child3.obj.getStatus().getState(), ActionableSnapshot::kNullStateId );

  // Resetting state machine from initial state: should work, but do nothing
  BOOST_CHECK_NO_THROW( fsm.reset(DummyGateKeeper()) );
  BOOST_CHECK_EQUAL( sys->getStatus().getStateMachineId(), fsm.getId() );
  BOOST_CHECK_EQUAL( sys->getStatus().getState(), kSysState0 );
  BOOST_CHECK_EQUAL( child1.obj.getStatus().getStateMachineId(), child1.fsm.getId() );
  BOOST_CHECK_EQUAL( child2.obj.getStatus().getStateMachineId(), child2.fsm.getId() );
  BOOST_CHECK_EQUAL( child3.obj.getStatus().getStateMachineId(), ActionableSnapshot::kNullStateMachineId );
  BOOST_CHECK_EQUAL( child1.obj.getStatus().getState(), kChildState0 );
  BOOST_CHECK_EQUAL( child2.obj.getStatus().getState(), kChildState0 );
  BOOST_CHECK_EQUAL( child3.obj.getStatus().getState(), ActionableSnapshot::kNullStateId );

  // Run a transition to prepare for following test
  BOOST_REQUIRE_NO_THROW( sysItoA->exec(DummyGateKeeper(), false) );
  BOOST_REQUIRE_EQUAL( sysItoA->getStatus().getState(), Functionoid::State::kDone );
  BOOST_REQUIRE_EQUAL( sys->getStatus().getStateMachineId(), fsm.getId() );
  BOOST_REQUIRE_EQUAL( sys->getStatus().getState(), kSysStateA );
  BOOST_REQUIRE_EQUAL( child1.trItoA->getStatus().getState(), Functionoid::State::kDone );
  BOOST_REQUIRE_EQUAL( child2.trItoA->getStatus().getState(), Functionoid::State::kDone );
  BOOST_REQUIRE_EQUAL( child1.obj.getStatus().getStateMachineId(), child1.fsm.getId() );
  BOOST_REQUIRE_EQUAL( child2.obj.getStatus().getStateMachineId(), child2.fsm.getId() );
  BOOST_REQUIRE_EQUAL( child3.obj.getStatus().getStateMachineId(), ActionableSnapshot::kNullStateMachineId );
  BOOST_REQUIRE_EQUAL( child1.obj.getStatus().getState(), kChildStateA );
  BOOST_REQUIRE_EQUAL( child2.obj.getStatus().getState(), kChildStateA );
  BOOST_REQUIRE_EQUAL( child3.obj.getStatus().getState(), ActionableSnapshot::kNullStateId );

  // Resetting state machine: should reset system & children to initial state
  BOOST_CHECK_NO_THROW( fsm.reset(DummyGateKeeper()) );
  BOOST_CHECK_EQUAL( sys->getStatus().getStateMachineId(), fsm.getId() );
  BOOST_CHECK_EQUAL( sys->getStatus().getState(), kSysState0 );
  BOOST_CHECK_EQUAL( child1.obj.getStatus().getStateMachineId(), child1.fsm.getId() );
  BOOST_CHECK_EQUAL( child2.obj.getStatus().getStateMachineId(), child2.fsm.getId() );
  BOOST_CHECK_EQUAL( child3.obj.getStatus().getStateMachineId(), ActionableSnapshot::kNullStateMachineId );
  BOOST_CHECK_EQUAL( child1.obj.getStatus().getState(), kChildState0 );
  BOOST_CHECK_EQUAL( child2.obj.getStatus().getState(), kChildState0 );
  BOOST_CHECK_EQUAL( child3.obj.getStatus().getState(), ActionableSnapshot::kNullStateId );
}


BOOST_FIXTURE_TEST_CASE(TestResetDisengagedFSM, SystemStateMachineTestSetup)
{
  // Resetting FSM before engaged: Should throw (regardless of whether or not contains children), and leave state + maskables unchanged
  BOOST_CHECK_THROW( fsm.reset(DummyGateKeeper()), ResourceInWrongStateMachine);
  BOOST_CHECK_EQUAL( sys->getStatus().getStateMachineId(), ActionableSnapshot::kNullStateMachineId  );
  BOOST_CHECK_EQUAL( sys->getStatus().getState(), ActionableSnapshot::kNullStateId );

  sysItoA->add(children.begin(), children.end(), child1.fsm.getId(), kChildState0, child1.trItoA->getId());
  BOOST_CHECK_THROW( fsm.reset(DummyGateKeeper()), ResourceInWrongStateMachine);
  BOOST_CHECK_EQUAL( sys->getStatus().getStateMachineId(), ActionableSnapshot::kNullStateMachineId  );
  BOOST_CHECK_EQUAL( sys->getStatus().getState(), ActionableSnapshot::kNullStateId );

  typedef std::vector<Child*>::const_iterator ChildIt_t;
  const std::vector<Child*> lChildren = {&child1, &child2, &child3};
  for (ChildIt_t lIt=lChildren.begin(); lIt!=lChildren.end(); lIt++) {
    BOOST_CHECK_EQUAL((*lIt)->obj.getStatus().getStateMachineId(), ActionableSnapshot::kNullStateMachineId );
    BOOST_CHECK_EQUAL((*lIt)->obj.getStatus().getState(), ActionableSnapshot::kNullStateId );
    BOOST_CHECK_EQUAL((*lIt)->maskableA.isMasked(), false);
    BOOST_CHECK_EQUAL((*lIt)->maskableB.isMasked(), false);
    BOOST_CHECK_EQUAL((*lIt)->maskableC.isMasked(), false);
  }

}


BOOST_FIXTURE_TEST_CASE(TestResetFSMChildDisengaged, SystemStateMachineTestSetup)
{
  // Preparation: add children to system FSM, engage, run "I->A" transition & disengage 1 child
  sysItoA->add(children.begin(), children.end(), child1.fsm.getId(), kChildState0, child1.trItoA->getId());
  fsm.engage(DummyGateKeeper());
  sysItoA->exec(DummyGateKeeper(), false);
  child1.fsm.disengage();
  BOOST_REQUIRE_EQUAL(sysItoA->getStatus().getState(), Functionoid::State::kDone);
  BOOST_REQUIRE_EQUAL(sys->getStatus().getStateMachineId(), fsm.getId());
  BOOST_REQUIRE_EQUAL(sys->getStatus().getState(), kSysStateA);
  BOOST_REQUIRE_EQUAL(child1.trItoA->getStatus().getState(), Functionoid::State::kDone);
  BOOST_REQUIRE_EQUAL(child2.trItoA->getStatus().getState(), Functionoid::State::kDone);
  BOOST_REQUIRE_EQUAL(child3.trItoA->getStatus().getState(), Functionoid::State::kDone);
  BOOST_REQUIRE_EQUAL(child1.obj.getStatus().getStateMachineId(), ActionableSnapshot::kNullStateMachineId);
  BOOST_REQUIRE_EQUAL(child2.obj.getStatus().getStateMachineId(), child2.fsm.getId());
  BOOST_REQUIRE_EQUAL(child3.obj.getStatus().getStateMachineId(), child3.fsm.getId());
  BOOST_REQUIRE_EQUAL(child1.obj.getStatus().getState(), ActionableSnapshot::kNullStateId);
  BOOST_REQUIRE_EQUAL(child2.obj.getStatus().getState(), kChildStateA);
  BOOST_REQUIRE_EQUAL(child3.obj.getStatus().getState(), kChildStateA);
  typedef std::vector<Child*>::const_iterator ChildIt_t;
  const std::vector<Child*> lChildren = {&child1, &child2, &child3};
  for (ChildIt_t lIt=lChildren.begin(); lIt!=lChildren.end(); lIt++) {
    BOOST_REQUIRE_EQUAL((*lIt)->maskableA.isMasked(), false);
    BOOST_REQUIRE_EQUAL((*lIt)->maskableB.isMasked(), false);
    BOOST_REQUIRE_EQUAL((*lIt)->maskableC.isMasked(), false);
  }


  // Resetting FSM when child disengaged: Should throw, and leave state + maskables unchanged
  BOOST_CHECK_THROW( fsm.reset(gk), ResourceInWrongStateMachine);
  BOOST_REQUIRE_EQUAL(sys->getStatus().getStateMachineId(), fsm.getId());
  BOOST_REQUIRE_EQUAL(sys->getStatus().getState(), kSysStateA);
  BOOST_REQUIRE_EQUAL(child1.obj.getStatus().getStateMachineId(), ActionableSnapshot::kNullStateMachineId);
  BOOST_REQUIRE_EQUAL(child2.obj.getStatus().getStateMachineId(), child2.fsm.getId());
  BOOST_REQUIRE_EQUAL(child3.obj.getStatus().getStateMachineId(), child3.fsm.getId());
  BOOST_REQUIRE_EQUAL(child1.obj.getStatus().getState(), ActionableSnapshot::kNullStateId);
  BOOST_REQUIRE_EQUAL(child2.obj.getStatus().getState(), kChildStateA);
  BOOST_REQUIRE_EQUAL(child3.obj.getStatus().getState(), kChildStateA);

  for (ChildIt_t lIt=lChildren.begin(); lIt!=lChildren.end(); lIt++) {
    BOOST_CHECK_EQUAL((*lIt)->maskableA.isMasked(), false);
    BOOST_CHECK_EQUAL((*lIt)->maskableB.isMasked(), false);
    BOOST_CHECK_EQUAL((*lIt)->maskableC.isMasked(), false);
  }

}


BOOST_FIXTURE_TEST_CASE(TestMonitoringSettingsAppliedDuringReset, SystemStateMachineTestSetup)
{
  // Add children to state machine
  typedef std::vector<Child*>::const_iterator ChildIt_t;
  const std::vector<Child*> lChildren = {&child1, &child2, &child3};
  sysItoA->add(children.begin(), children.end(), child1.fsm.getId(), kChildState0, child1.trItoA->getId());
  BOOST_REQUIRE_NO_THROW(fsm.engage(DummyGateKeeper()));

  // GOAL: Check that monitoring settings are reset, and those for initial state applied, during reset
  //       ONLY for actionable objects that are enabled
  addMonSettingsToGateKeeper(kChildState0);
  gk.addDisabledId(child2.obj.getPath());

  // THE SETUP: Set to 'kDisabled', the monitorables that don't have settings in gatekeeper
  //            ... in order to ensure their settings are reset to 'kEnabled'
  for (ChildIt_t lIt=lChildren.begin(); lIt != lChildren.end(); lIt++) {
    (*lIt)->monGrandChild2.setMonitoringStatus(core::monitoring::kDisabled);
    (*lIt)->monGrandChild1.getMetric("dummyMetric").setMonitoringStatus(core::monitoring::kDisabled);
    (*lIt)->monGrandChild2.getMetric("dummyMetric").setMonitoringStatus(core::monitoring::kDisabled);

    // CHECK ASSUMPTIONS: Child monitorable objects and metrics have correct setting value before reset
    BOOST_REQUIRE_EQUAL((*lIt)->monChild.getMonitoringStatus(), core::monitoring::kEnabled);
    BOOST_REQUIRE_EQUAL((*lIt)->monGrandChild1.getMonitoringStatus(), core::monitoring::kEnabled);
    BOOST_REQUIRE_EQUAL((*lIt)->monGrandChild2.getMonitoringStatus(), core::monitoring::kDisabled);
    BOOST_REQUIRE_EQUAL((*lIt)->monChild.getMetric("dummyMetric").getStatus().second, core::monitoring::kEnabled);
    BOOST_REQUIRE_EQUAL((*lIt)->monGrandChild1.getMetric("dummyMetric").getStatus().second, core::monitoring::kDisabled);
    BOOST_REQUIRE_EQUAL((*lIt)->monGrandChild2.getMetric("dummyMetric").getStatus().second, core::monitoring::kDisabled);
  }

  // THE TEST: Gatekeeper's monitoring settings for initial state should be applied during state machine reset
  BOOST_REQUIRE_NO_THROW(fsm.reset(gk));

  std::vector<Child*> lEnabledChildren = {&child1, &child3};
  for (ChildIt_t lIt=lEnabledChildren.begin(); lIt != lEnabledChildren.end(); lIt++) {
    BOOST_CHECK_EQUAL((*lIt)->monChild.getMonitoringStatus(), core::monitoring::kNonCritical);
    BOOST_CHECK_EQUAL((*lIt)->monGrandChild1.getMonitoringStatus(), core::monitoring::kDisabled);
    // this one should fail, as paths need to be relative to obj and this setting has been made with ID 'grandChild2'
    // not 'child1.grandChild2'
    BOOST_CHECK_EQUAL((*lIt)->monGrandChild2.getMonitoringStatus(), core::monitoring::kEnabled);
    // metrics
    BOOST_CHECK_EQUAL((*lIt)->monChild.getMetric("dummyMetric").getStatus().second, core::monitoring::kDisabled);
    BOOST_CHECK_EQUAL((*lIt)->monGrandChild1.getMetric("dummyMetric").getStatus().second, core::monitoring::kEnabled);
    BOOST_CHECK_EQUAL((*lIt)->monGrandChild2.getMetric("dummyMetric").getStatus().second, core::monitoring::kEnabled);
  }
  BOOST_CHECK_EQUAL(child2.monChild.getMonitoringStatus(), core::monitoring::kEnabled);
  BOOST_CHECK_EQUAL(child2.monGrandChild1.getMonitoringStatus(), core::monitoring::kEnabled);
  BOOST_CHECK_EQUAL(child2.monGrandChild2.getMonitoringStatus(), core::monitoring::kDisabled);
  BOOST_CHECK_EQUAL(child2.monChild.getMetric("dummyMetric").getStatus().second, core::monitoring::kEnabled);
  BOOST_CHECK_EQUAL(child2.monGrandChild1.getMetric("dummyMetric").getStatus().second, core::monitoring::kDisabled);
  BOOST_CHECK_EQUAL(child2.monGrandChild2.getMetric("dummyMetric").getStatus().second, core::monitoring::kDisabled);
}


BOOST_FIXTURE_TEST_CASE(TestMaskablesMaskedDuringReset, SystemStateMachineTestSetup)
{
  typedef std::vector<Child*>::const_iterator ChildIt_t;
  const std::vector<Child*> lChildren = {&child1, &child2, &child3};

  // PART A :
  //  * Goal: Check that MaskableObjects we ask it to mask are being masked
  //  * Start off with everything unmasked

  // First must engage FSM. N.B. use empty gatekeeper so that maskable objects remain unmasked
  sysItoA->add(children.begin(), children.end(), child1.fsm.getId(), kChildState0, child1.trItoA->getId());
  fsm.engage(DummyGateKeeper());

  // A.1) Require that all are unmasked before tests start
  for (ChildIt_t lIt=lChildren.begin(); lIt!=lChildren.end(); lIt++) {
    BOOST_REQUIRE_EQUAL((*lIt)->maskableA.isMasked(), false);
    BOOST_REQUIRE_EQUAL((*lIt)->maskableB.isMasked(), false);
    BOOST_REQUIRE_EQUAL((*lIt)->maskableC.isMasked(), false);
  }
  // A.2) Engage FSM; check that A & B now masked, but C remains unmasked
  fsm.reset(gk);
  for (ChildIt_t lIt=lChildren.begin(); lIt!=lChildren.end(); lIt++) {
    BOOST_CHECK_EQUAL((*lIt)->maskableA.isMasked(), true);
    BOOST_CHECK_EQUAL((*lIt)->maskableB.isMasked(), true);
    BOOST_CHECK_EQUAL((*lIt)->maskableC.isMasked(), false);
  }

  // PART B :
  //   * Goal: Check that all MaskableObjects we don't ask it to mask are being reset to unmasked state
  //   * Start off with A & B unmasked, but C masked
  // B.1) Unmask A & B, mask C; require that starting assumptions are correct before testing
  for (ChildIt_t lIt=lChildren.begin(); lIt!=lChildren.end(); lIt++) {
    (*lIt)->maskableA.setMasked(false);
    (*lIt)->maskableB.setMasked(false);
    (*lIt)->maskableC.setMasked(true);
    BOOST_REQUIRE_EQUAL((*lIt)->maskableA.isMasked(), false);
    BOOST_REQUIRE_EQUAL((*lIt)->maskableB.isMasked(), false);
    BOOST_REQUIRE_EQUAL((*lIt)->maskableC.isMasked(), true);
  }

  // B.2) Engage FSM; check that A & B now masked, but C remains unmasked
  fsm.reset(gk);
  for (ChildIt_t lIt=lChildren.begin(); lIt!=lChildren.end(); lIt++) {
    BOOST_CHECK_EQUAL((*lIt)->maskableA.isMasked(), true);
    BOOST_CHECK_EQUAL((*lIt)->maskableB.isMasked(), true);
    BOOST_CHECK_EQUAL((*lIt)->maskableC.isMasked(), false);
  }
}


BOOST_FIXTURE_TEST_CASE(TestChildrenDisabledDuringReset, SystemStateMachineTestSetup)
{
  typedef std::vector<Child*>::const_iterator ChildIt_t;
  const std::vector<Child*> lChildren = {&child1, &child2, &child3};

  // Finish off setup: Add all children to state machine
  sysItoA->add(children.begin(), children.end(), child1.fsm.getId(), kChildState0, child1.trItoA->getId());

  // PURPOSE: Check that reset ...
  //    - Disables children specified in gatekeeper (child1)
  //    - Leaves all other children:
  //       - Enabled if they were already enabled at the start of `reset` method (child2)
  //       - Disabled if they were already disabled at the start of `reset` method (child3)
  //    - Doesn't care if disabled children are in other state machines
  //    - Only changes masks on children that are enabled (i.e. child2 + child3)

  // SETUP:
  //  * Engage system FSM with only child1 & child2 enabled
  DummyGateKeeper lEngageGK;
  lEngageGK.addDisabledId(child3.obj.getPath());
  fsm.engage(lEngageGK);
  BOOST_REQUIRE_EQUAL(sys->getStatus().getStateMachineId(), fsm.getId());
  BOOST_REQUIRE_EQUAL(sys->getStatus().getState(), fsm.getInitialState());

  //  * Engage child3 in main FSM (will be enabled in reset)
  child3.fsm.engage(DummyGateKeeper());

  //  * Engage child1 & child3 in another FSM (will be disabled in reset, hence reset shouldn't care)
  child1.fsm.disengage();
  child1.obj.registerStateMachine("anotherFSM", "anotherInitialState", "sE").engage(DummyGateKeeper());
  child3.fsm.disengage();
  child3.obj.registerStateMachine("anotherFSM", "anotherInitialState", "sE").engage(DummyGateKeeper());

  BOOST_REQUIRE_EQUAL(child1.obj.getStatus().isEnabled(), true);
  BOOST_REQUIRE_EQUAL(child1.obj.getStatus().getStateMachineId(), "anotherFSM");
  BOOST_REQUIRE_EQUAL(child1.obj.getStatus().getState(), "anotherInitialState");
  BOOST_REQUIRE_EQUAL(child1.obj.getMonitoringStatus(), core::monitoring::kEnabled);
  BOOST_REQUIRE_EQUAL(child2.obj.getStatus().isEnabled(), true);
  BOOST_REQUIRE_EQUAL(child2.obj.getStatus().getStateMachineId(), child2.fsm.getId());
  BOOST_REQUIRE_EQUAL(child2.obj.getStatus().getState(), kChildState0);
  BOOST_REQUIRE_EQUAL(child2.obj.getMonitoringStatus(), core::monitoring::kEnabled);
  BOOST_REQUIRE_EQUAL(child3.obj.getStatus().isEnabled(), false);
  BOOST_REQUIRE_EQUAL(child3.obj.getStatus().getStateMachineId(), "anotherFSM");
  BOOST_REQUIRE_EQUAL(child3.obj.getStatus().getState(), "anotherInitialState");
  child3.obj.setMonitoringStatus(core::monitoring::kDisabled);
  BOOST_REQUIRE_EQUAL(child3.obj.getMonitoringStatus(), core::monitoring::kDisabled);

  //  * Mask maskableC on each child (giving inverse of final result); require that starting assumptions are correct before testing
  for (ChildIt_t lIt=lChildren.begin(); lIt!=lChildren.end(); lIt++) {
    (*lIt)->maskableC.setMasked(true);
    BOOST_REQUIRE_EQUAL((*lIt)->maskableA.isMasked(), false);
    BOOST_REQUIRE_EQUAL((*lIt)->maskableB.isMasked(), false);
    BOOST_REQUIRE_EQUAL((*lIt)->maskableC.isMasked(), true);
  }


  // THE TEST: Reset system FSM, check that:
  //  * child1 has been disabled; it's maskable descendants unchanged.
  //  * child2 & child3 are enabled, maskable children: A & B now masked, but C unmasked
  gk.addDisabledId(child1.obj.getPath());
  BOOST_CHECK_NO_THROW(fsm.reset(gk));

  BOOST_CHECK_EQUAL(sys->getStatus().getStateMachineId(), fsm.getId());
  BOOST_CHECK_EQUAL(sys->getStatus().getState(), kSysState0);

  BOOST_CHECK_EQUAL(child1.obj.getStatus().isEnabled(), false);
  BOOST_CHECK_EQUAL(child1.obj.getMonitoringStatus(), core::monitoring::kNonCritical);
  BOOST_CHECK_EQUAL(child2.obj.getStatus().isEnabled(), true);
  BOOST_CHECK_EQUAL(child2.obj.getMonitoringStatus(), core::monitoring::kEnabled);
  BOOST_CHECK_EQUAL(child3.obj.getStatus().isEnabled(), false);
  BOOST_CHECK_EQUAL(child3.obj.getMonitoringStatus(), core::monitoring::kNonCritical);


  BOOST_CHECK_EQUAL(child2.obj.getStatus().getStateMachineId(), child2.fsm.getId());
  BOOST_CHECK_EQUAL(child2.obj.getStatus().getState(), kChildState0);
  BOOST_CHECK_EQUAL(child2.maskableA.isMasked(), true);
  BOOST_CHECK_EQUAL(child2.maskableB.isMasked(), true);
  BOOST_CHECK_EQUAL(child2.maskableC.isMasked(), false);

  for (ChildIt_t lIt=lChildren.begin(); lIt!=lChildren.end(); lIt++) {
    const Child& lChild = **lIt;

    if ( &lChild != &child2 ) {
      BOOST_CHECK_EQUAL(lChild.obj.getStatus().getStateMachineId(), "anotherFSM");
      BOOST_CHECK_EQUAL(lChild.obj.getStatus().getState(), "anotherInitialState");
      BOOST_CHECK_EQUAL(lChild.maskableA.isMasked(), false);
      BOOST_CHECK_EQUAL(lChild.maskableB.isMasked(), false);
      BOOST_CHECK_EQUAL(lChild.maskableC.isMasked(), true);
    }
  }
}


BOOST_AUTO_TEST_SUITE_END() // SystemStateMachineTestSuite


} /* namespace test */
} /* namespace action */
} /* namespace swatch */


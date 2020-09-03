#include <boost/test/unit_test.hpp>

// Boost Headers
#include <boost/assign.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/unordered/unordered_map.hpp>

// C++ Headers
#include <iomanip>
#include <vector>
#include <map>

// Swatch Headers
#include "swatch/core/Object.hpp"
#include "swatch/core/utilities.hpp"
#include "swatch/core/Factory.hpp"

#include "swatch/processor/Port.hpp"
#include "swatch/processor/ProcessorStub.hpp"
#include "swatch/processor/Processor.hpp"
#include "swatch/processor/test/DummyProcessor.hpp"

#include "swatch/dtm/DaqTTCStub.hpp"
#include "swatch/system/Crate.hpp"
#include "swatch/system/CrateStub.hpp"
#include "swatch/system/Link.hpp"
#include "swatch/system/System.hpp"
#include "swatch/system/test/DummyAMC13Manager.hpp"
#include "swatch/processor/PortCollection.hpp"


namespace swatch {
namespace system {
namespace test {


BOOST_AUTO_TEST_SUITE( SystemTestSuite )


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//   EMPTY STUB
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE(BuildEmptySystem)
{
  SystemStub lStub("mySystem");
  lStub.loggerName = "myCoolLogger";

  System lSystem(lStub);
  BOOST_CHECK_EQUAL(lSystem.getId(), "mySystem");
  BOOST_CHECK_EQUAL(lSystem.getAlias(), "");
  BOOST_CHECK_EQUAL(lSystem.getLogger().getName(), "myCoolLogger");
  BOOST_CHECK(lSystem.getStub() == lStub);

  BOOST_CHECK_EQUAL(lSystem.getActionableChildren().size(), size_t(0));
  BOOST_CHECK_EQUAL(lSystem.getProcessors().size(), size_t(0));
  BOOST_CHECK_EQUAL(lSystem.getDaqTTCs().size(), size_t(0));
  BOOST_CHECK_EQUAL(lSystem.getCrates().size(), size_t(0));
  BOOST_CHECK_EQUAL(lSystem.getLinks().size(), size_t(0));

  BOOST_CHECK_EQUAL(lSystem.getStateMachines().size(), size_t(1));
  BOOST_CHECK_EQUAL(*lSystem.getStateMachines().begin(), RunControlFSM::kId);

  lStub.alias = "system's useful alias";
  System lSystem2(lStub);
  BOOST_CHECK_EQUAL(lSystem2.getId(), "mySystem");
  BOOST_CHECK_EQUAL(lSystem2.getAlias(), "system's useful alias");
  BOOST_CHECK_EQUAL(lSystem2.getLogger().getName(), "myCoolLogger");
  BOOST_CHECK(lSystem2.getStub() == lStub);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//   STUB CONTAINING ONLY CRATES
/////////////////////////////////////////////////////////////////////////////////////////////////////////////


struct SystemCrateTestSetup {
  CrateStub crateStubX;
  CrateStub crateStubY;
  SystemStub systemStub;

  SystemCrateTestSetup() :
    crateStubX("crateX"),
    crateStubY("crateY"),
    systemStub("myCoolSystem")
  {
    crateStubX.location = "crateX's location";
    crateStubX.description = "An exciting description";

    crateStubY.location = "crateY's location";
    crateStubY.description = "A dull (but informative) description";
    crateStubY.alias = "A useful crate alias!";

    systemStub.crates.push_back(crateStubX);
    systemStub.crates.push_back(crateStubY);
  }

  ~SystemCrateTestSetup () {}
};


BOOST_FIXTURE_TEST_CASE(BuildSystemWithCrates_Successful, SystemCrateTestSetup)
{
  System lSystem(systemStub);

  BOOST_CHECK_EQUAL(lSystem.getId(), "myCoolSystem");
  BOOST_CHECK(lSystem.getStub() == systemStub);

  // 1) Check that system contains 2 crates & nothing else
  BOOST_CHECK_EQUAL(lSystem.getActionableChildren().size(), size_t(0));
  BOOST_CHECK_EQUAL(lSystem.getProcessors().size(), size_t(0));
  BOOST_CHECK_EQUAL(lSystem.getDaqTTCs().size(), size_t(0));
  BOOST_CHECK_EQUAL(lSystem.getCrates().size(), size_t(2));
  BOOST_CHECK_EQUAL(lSystem.getLinks().size(), size_t(0));

  BOOST_CHECK_EQUAL( lSystem.hasCrate(crateStubX.id), true );
  BOOST_CHECK_EQUAL( lSystem.hasCrate(crateStubY.id), true );
  BOOST_CHECK_EQUAL( lSystem.hasCrate("nonexistant_crate"), false );

  // 2) Check that the crates have correct IDs & stubs
  Crate* lCrateX = lSystem.getCrates().at(crateStubX.id);
  BOOST_REQUIRE_NE(lCrateX, (Crate*) NULL);
  BOOST_CHECK_EQUAL(lCrateX->getId(), crateStubX.id);
  BOOST_CHECK_EQUAL(lCrateX->getAlias(), crateStubX.alias);
  BOOST_CHECK(lCrateX->getStub() == crateStubX);

  Crate* lCrateY = lSystem.getCrates().at(crateStubY.id);
  BOOST_REQUIRE_NE(lCrateY, (Crate*) NULL);
  BOOST_CHECK_EQUAL(lCrateY->getId(), crateStubY.id);
  BOOST_REQUIRE(!crateStubY.alias.empty());
  BOOST_CHECK_EQUAL(lCrateY->getAlias(), crateStubY.alias);
  BOOST_CHECK(lCrateY->getStub() == crateStubY);

  // 3) Check that crates are accessible through normal object tree as well
  BOOST_CHECK_EQUAL(lCrateX, &lSystem.getObj<Crate>(crateStubX.id));
  BOOST_CHECK_EQUAL(lCrateY, &lSystem.getObj<Crate>(crateStubY.id));

  // 4) Check that crates don't have any populated slots
  const std::vector<const Crate*> crates = {lCrateX, lCrateY};
  for (auto lIt=crates.begin(); lIt != crates.end(); lIt++) {
    BOOST_CHECK_EQUAL((*lIt)->getPopulatedSlots().empty(), true);
    BOOST_CHECK_EQUAL((*lIt)->amc13(), (dtm::DaqTTCManager*) NULL);
    for (size_t i=1; i<=12; i++) {
      BOOST_CHECK_EQUAL((*lIt)->amc(i), (processor::Processor*) NULL);
      BOOST_CHECK_EQUAL((*lIt)->isAMCSlotTaken(i), false);
    }
  }
}


BOOST_FIXTURE_TEST_CASE(BuildSystemWithCrates_DuplicateId, SystemCrateTestSetup)
{
  // Check that original system stub is OK, before modifying it to perform the test
  BOOST_REQUIRE_NO_THROW( System lSystem(systemStub) );

  // Check that system construction fails after making both crates' ID strings identical
  systemStub.crates.at(1).id = systemStub.crates.at(0).id;
  BOOST_CHECK_THROW( System lSystem(systemStub), SystemConstructionFailed);
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//   STUB CONTAINING ONLY PROCESSORS & CRATES
/////////////////////////////////////////////////////////////////////////////////////////////////////////////


struct SystemProcessorTestSetup : public SystemCrateTestSetup {
  processor::ProcessorStub processorStubA;
  processor::ProcessorStub processorStubB;

  SystemProcessorTestSetup() :
    processorStubA("procA"),
    processorStubB("procB")
  {
    processorStubA.creator = "swatch::processor::test::DummyProcessor";
    processorStubA.hwtype = "type1";
    processorStubA.role = "roleA";
    processorStubA.uri = "uri://anEndpoint";
    processorStubA.addressTable = "file://anAddrTable.xml";
    processorStubA.crate = crateStubX.id;
    processorStubA.slot = 10;

    processorStubB.alias = "An insightful processor alias!";
    processorStubB.creator = "swatch::processor::test::DummyProcessor";
    processorStubB.hwtype = "type2";
    processorStubB.role = "roleB";
    processorStubB.uri = "uri://otherEndpoint";
    processorStubB.addressTable = "file://otherAddrTable.xml";
    processorStubB.crate = crateStubY.id;
    processorStubB.slot = 5;

    systemStub.processors.push_back(processorStubA);
    systemStub.processors.push_back(processorStubB);
  }

  ~SystemProcessorTestSetup () {}
};


BOOST_FIXTURE_TEST_CASE(BuildSystemWithProcessors_Successful, SystemProcessorTestSetup)
{
  System lSystem(systemStub);

  BOOST_CHECK_EQUAL(lSystem.getId(), "myCoolSystem");
  BOOST_CHECK(lSystem.getStub() == systemStub);

  // 1) Check that system contains 2 crates & nothing else
  BOOST_CHECK_EQUAL(lSystem.getActionableChildren().size(), size_t(2));
  BOOST_CHECK_EQUAL(lSystem.getProcessors().size(), size_t(2));
  BOOST_CHECK_EQUAL(lSystem.getDaqTTCs().size(), size_t(0));
  BOOST_CHECK_EQUAL(lSystem.getCrates().size(), size_t(2));
  BOOST_CHECK_EQUAL(lSystem.getLinks().size(), size_t(0));

  // 2) Check that the processors have correct ID, stub, and type
  processor::Processor* lProcA = lSystem.getProcessors().at(0);
  BOOST_REQUIRE_NE(lProcA, (processor::Processor*) NULL);
  BOOST_CHECK_EQUAL(lProcA->getId(), processorStubA.id);
  BOOST_CHECK_EQUAL(lProcA->getAlias(), "");
  BOOST_CHECK(lProcA->getStub() == processorStubA);
  BOOST_CHECK(typeid(*lProcA) == typeid(processor::test::DummyProcessor));

  processor::Processor* lProcB = lSystem.getProcessors().at(1);
  BOOST_REQUIRE_NE(lProcB, (processor::Processor*) NULL);
  BOOST_CHECK_EQUAL(lProcB->getId(), processorStubB.id);
  BOOST_CHECK_EQUAL(lProcB->getAlias(), "An insightful processor alias!");
  BOOST_CHECK(lProcB->getStub() == processorStubB);
  BOOST_CHECK(typeid(*lProcB) == typeid(processor::test::DummyProcessor));

  // 3) Check that processors are accessible through normal object tree as well
  BOOST_CHECK_EQUAL(lProcA, &lSystem.getObj<processor::test::DummyProcessor>(processorStubA.id));
  BOOST_CHECK_EQUAL(lProcA, lSystem.getActionableChildren().at(processorStubA.id));
  BOOST_CHECK_EQUAL(lProcB, &lSystem.getObj<processor::test::DummyProcessor>(processorStubB.id));
  BOOST_CHECK_EQUAL(lProcB, lSystem.getActionableChildren().at(processorStubB.id));

  // 4) Check correct slots are populated, in correct crates
  const Crate& lCrateX = *lSystem.getCrates().at(crateStubX.id);
  BOOST_CHECK_EQUAL(lCrateX.getPopulatedSlots().size(), size_t(1));
  BOOST_CHECK_EQUAL(lCrateX.getPopulatedSlots().at(0), processorStubA.slot);
  BOOST_CHECK_EQUAL(lCrateX.amc13(), (dtm::DaqTTCManager*) NULL);
  for (size_t i=1; i<=12; i++) {
    BOOST_CHECK_EQUAL(lCrateX.isAMCSlotTaken(i), (i == processorStubA.slot));
    if (i == processorStubA.slot) {
      BOOST_CHECK_EQUAL(lCrateX.amc(i), lProcA);
    }
    else {
      BOOST_CHECK_EQUAL(lCrateX.amc(i), (processor::Processor*) NULL);
    }
  }

  const Crate& lCrateY = *lSystem.getCrates().at(crateStubY.id);
  BOOST_CHECK_EQUAL(lCrateY.getPopulatedSlots().size(), size_t(1));
  BOOST_CHECK_EQUAL(lCrateY.getPopulatedSlots().at(0), processorStubB.slot);
  BOOST_CHECK_EQUAL(lCrateY.amc13(), (dtm::DaqTTCManager*) NULL);
  for (size_t i=1; i<=12; i++) {
    BOOST_CHECK_EQUAL(lCrateY.isAMCSlotTaken(i), (i == processorStubB.slot));
    if (i == processorStubB.slot) {
      BOOST_CHECK_EQUAL(lCrateY.amc(i), lProcB);
    }
    else {
      BOOST_CHECK_EQUAL(lCrateY.amc(i), (processor::Processor*) NULL);
    }
  }
}


BOOST_FIXTURE_TEST_CASE(BuildSystemWithProcessors_DuplicateId, SystemProcessorTestSetup)
{
  // Check that original system stub is OK, before modifying it to perform the test
  BOOST_REQUIRE_NO_THROW( System lSystem(systemStub) );

  // Check that system construction fails after making both processor's ID strings identical
  systemStub.processors.at(1).id = systemStub.processors.at(0).id;
  BOOST_CHECK_THROW( System lSystem(systemStub), SystemConstructionFailed);

  // Check that system construction fails if processor's ID string identical to that of a crate
  systemStub.processors.at(1).id = systemStub.crates.at(0).id;
  BOOST_CHECK_THROW( System lSystem(systemStub), SystemConstructionFailed);
}

BOOST_FIXTURE_TEST_CASE(BuildSystemWithProcessors_EmptyCrateId, SystemProcessorTestSetup)
{
  // Check that original system stub is OK, before modifying it to perform the test
  BOOST_REQUIRE_NO_THROW( System lSystem(systemStub) );

  // Check that system construction fails if processor stub's crate field is empty
  systemStub.processors.at(1).crate = "";
  BOOST_CHECK_THROW( System lSystem(systemStub), SystemConstructionFailed);
}


BOOST_FIXTURE_TEST_CASE(BuildSystemWithProcessors_InvalidCrateId, SystemProcessorTestSetup)
{
  // Check that original system stub is OK, before modifying it to perform the test
  BOOST_REQUIRE_NO_THROW( System lSystem(systemStub) );

  // Check that system construction fails if processor stub's crate field doesn't match any of the system's crates
  systemStub.processors.at(1).crate = "nonexistant_crate_id";
  BOOST_CHECK_THROW( System lSystem(systemStub), SystemConstructionFailed);
}


BOOST_FIXTURE_TEST_CASE(BuildSystemWithProcessors_InvalidSlotNumber, SystemProcessorTestSetup)
{
  // Check that original system stub is OK, before modifying it to perform the test
  BOOST_REQUIRE_NO_THROW( System lSystem(systemStub) );

  // Check that system construction fails if slot number is too low
  systemStub.processors.at(1).slot = 0;
  BOOST_CHECK_THROW( System lSystem(systemStub), SystemConstructionFailed);

  // Check that system construction fails if slot number is too high
  systemStub.processors.at(1).slot = 13;
  BOOST_CHECK_THROW( System lSystem(systemStub), SystemConstructionFailed);
}


BOOST_FIXTURE_TEST_CASE(BuildSystemWithProcessors_InvalidCreator, SystemProcessorTestSetup)
{
  // Check that original system stub is OK, before modifying it to perform the test
  BOOST_REQUIRE_NO_THROW( System lSystem(systemStub) );

  // Check that system construction fails if creator string doesn't match any registered class
  systemStub.processors.at(1).creator = "NonExistantClass";
  BOOST_CHECK_THROW( System lSystem(systemStub), SystemConstructionFailed);
}

BOOST_FIXTURE_TEST_CASE(BuildSystemWithProcessors_ProcConstructorThrows, SystemProcessorTestSetup)
{
  // Check that original system stub is OK, before modifying it to perform the test
  BOOST_REQUIRE_NO_THROW( System lSystem(systemStub) );

  // Check that system construction fails if processor constructor throws (Object CTOR will throw if ID invalid)
  systemStub.processors.at(1).id = "id.containing.dots";
  BOOST_CHECK_THROW( System lSystem(systemStub), SystemConstructionFailed);

  systemStub.processors.at(1) = processorStubB;
  BOOST_REQUIRE_NO_THROW(System lSystem(systemStub));
  systemStub.processors.at(1).uri = "throw"; // Will cause non-SWATCH exception to be thrown
  BOOST_CHECK_THROW( System lSystem(systemStub), SystemConstructionFailed);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//   STUB CONTAINING ONLY DAQ-TTC MANAGERS & CRATES
/////////////////////////////////////////////////////////////////////////////////////////////////////////////


struct SystemDaqTtcManagerTestSetup : public SystemCrateTestSetup {
  system::CrateStub crateStubZ;
  dtm::DaqTTCStub dtmStubA;
  dtm::DaqTTCStub dtmStubB;

  SystemDaqTtcManagerTestSetup() :
    crateStubZ("crateZ"),
    dtmStubA("dtmA"),
    dtmStubB("dtmB")
  {
    crateStubZ.location = "crateZ's location";
    crateStubZ.description = "A fascinating description for crate Z";

    dtmStubA.creator = "swatch::system::test::DummyAMC13Manager";
    dtmStubA.role = "roleA";
    dtmStubA.uriT1 = "uri://anEndpoint_T1";
    dtmStubA.uriT2 = "uri://anEndpoint_T2";
    dtmStubA.addressTableT1 = "file://anAddrTableT1.xml";
    dtmStubA.addressTableT2 = "file://anAddrTableT2.xml";
    dtmStubA.crate = crateStubX.id;
    dtmStubA.slot = 13;
    dtmStubA.fedId = 999;

    dtmStubB.alias = "A self-explanatory DTM alias!";
    dtmStubB.creator = "swatch::system::test::DummyAMC13Manager";
    dtmStubB.role = "roleB";
    dtmStubB.uriT1 = "uri://otherEndpointT1";
    dtmStubB.uriT2 = "uri://otherEndpointT2";
    dtmStubB.addressTableT1 = "file://otherAddrTableT1.xml";
    dtmStubB.addressTableT2 = "file://otherAddrTableT2.xml";
    dtmStubB.crate = crateStubZ.id;
    dtmStubB.slot = 13;
    dtmStubB.fedId = 1001;

    systemStub.crates.push_back(crateStubZ);
    systemStub.daqttcs.push_back(dtmStubA);
    systemStub.daqttcs.push_back(dtmStubB);
  }

  ~SystemDaqTtcManagerTestSetup () {}
};


BOOST_FIXTURE_TEST_CASE(BuildSystemWithDaqTtcManagers_Successful, SystemDaqTtcManagerTestSetup)
{
  System lSystem(systemStub);

  BOOST_CHECK_EQUAL(lSystem.getId(), "myCoolSystem");
  BOOST_CHECK(lSystem.getStub() == systemStub);

  // 1) Check that system contains 2 crates & nothing else
  BOOST_CHECK_EQUAL(lSystem.getActionableChildren().size(), size_t(2));
  BOOST_CHECK_EQUAL(lSystem.getProcessors().size(), size_t(0));
  BOOST_CHECK_EQUAL(lSystem.getDaqTTCs().size(), size_t(2));
  BOOST_CHECK_EQUAL(lSystem.getCrates().size(), size_t(3));
  BOOST_CHECK_EQUAL(lSystem.getLinks().size(), size_t(0));

  // 2) Check that the DAQ-TTC managers have correct ID, stub, and type
  dtm::DaqTTCManager* lDaqTtcMgrA = lSystem.getDaqTTCs().at(0);
  BOOST_REQUIRE_NE(lDaqTtcMgrA, (dtm::DaqTTCManager*) NULL);
  BOOST_CHECK_EQUAL(lDaqTtcMgrA->getId(), dtmStubA.id);
  BOOST_CHECK_EQUAL(lDaqTtcMgrA->getAlias(), "");
  BOOST_CHECK(lDaqTtcMgrA->getStub() == dtmStubA);
  BOOST_CHECK(typeid(*lDaqTtcMgrA) == typeid(system::test::DummyAMC13Manager));

  dtm::DaqTTCManager* lDaqTtcMgrB = lSystem.getDaqTTCs().at(1);
  BOOST_REQUIRE_NE(lDaqTtcMgrB, (dtm::DaqTTCManager*) NULL);
  BOOST_CHECK_EQUAL(lDaqTtcMgrB->getId(), dtmStubB.id);
  BOOST_CHECK_EQUAL(lDaqTtcMgrB->getAlias(), "A self-explanatory DTM alias!");
  BOOST_CHECK(lDaqTtcMgrB->getStub() == dtmStubB);
  BOOST_CHECK(typeid(*lDaqTtcMgrB) == typeid(system::test::DummyAMC13Manager));

  // 3) Check that DAQ-TTC managers are accessible through normal object tree as well
  BOOST_CHECK_EQUAL(lDaqTtcMgrA, &lSystem.getObj<system::test::DummyAMC13Manager>(dtmStubA.id));
  BOOST_CHECK_EQUAL(lDaqTtcMgrA, lSystem.getActionableChildren().at(dtmStubA.id));
  BOOST_CHECK_EQUAL(lDaqTtcMgrB, &lSystem.getObj<system::test::DummyAMC13Manager>(dtmStubB.id));
  BOOST_CHECK_EQUAL(lDaqTtcMgrB, lSystem.getActionableChildren().at(dtmStubB.id));

  // 4) Check correct slots are populated, in correct crates
  const Crate& lCrateX = *lSystem.getCrates().at(crateStubX.id);
  BOOST_CHECK_EQUAL(lCrateX.getAMCSlots().empty(), true);
  BOOST_CHECK_EQUAL(lCrateX.getPopulatedSlots().size(), size_t(1));
  BOOST_CHECK_EQUAL(lCrateX.getPopulatedSlots().at(0), uint32_t(13));
  BOOST_CHECK_EQUAL(lCrateX.amc13(), lDaqTtcMgrA);
  for (size_t i=1; i<=12; i++) {
    BOOST_CHECK_EQUAL(lCrateX.isAMCSlotTaken(i), false);
    BOOST_CHECK_EQUAL(lCrateX.amc(i), (processor::Processor*) NULL);
  }

  const Crate& lCrateY = *lSystem.getCrates().at(crateStubY.id);
  BOOST_CHECK_EQUAL(lCrateY.getAMCSlots().empty(), true);
  BOOST_CHECK_EQUAL(lCrateY.getPopulatedSlots().empty(), true);
  BOOST_CHECK_EQUAL(lCrateY.amc13(), (dtm::DaqTTCManager*) NULL);
  for (size_t i=1; i<=12; i++) {
    BOOST_CHECK_EQUAL(lCrateY.isAMCSlotTaken(i), false);
    BOOST_CHECK_EQUAL(lCrateY.amc(i), (processor::Processor*) NULL);
  }

  const Crate& lCrateZ = *lSystem.getCrates().at(crateStubZ.id);
  BOOST_CHECK_EQUAL(lCrateZ.getAMCSlots().empty(), true);
  BOOST_CHECK_EQUAL(lCrateZ.getPopulatedSlots().size(), size_t(1));
  BOOST_CHECK_EQUAL(lCrateZ.getPopulatedSlots().at(0), uint32_t(13));
  BOOST_CHECK_EQUAL(lCrateZ.amc13(), lDaqTtcMgrB);
  for (size_t i=1; i<=12; i++) {
    BOOST_CHECK_EQUAL(lCrateZ.isAMCSlotTaken(i), false);
    BOOST_CHECK_EQUAL(lCrateZ.amc(i), (processor::Processor*) NULL);
  }
}


BOOST_FIXTURE_TEST_CASE(BuildSystemWithDaqTtcManagers_DuplicateId, SystemDaqTtcManagerTestSetup)
{
  // Check that original system stub is OK, before modifying it to perform the test
  BOOST_REQUIRE_NO_THROW( System lSystem(systemStub) );

  // Check that system construction fails after making both DaqTTCManager's ID strings identical
  systemStub.daqttcs.at(1).id = systemStub.daqttcs.at(0).id;
  BOOST_CHECK_THROW( System lSystem(systemStub), SystemConstructionFailed);

  // Check that system construction fails if DaqTTCManager's ID string identical to that of a crate
  systemStub.daqttcs.at(1).id = systemStub.crates.at(0).id;
  BOOST_CHECK_THROW( System lSystem(systemStub), SystemConstructionFailed);
}

BOOST_FIXTURE_TEST_CASE(BuildSystemWithDaqTtcManagers_EmptyCrateId, SystemDaqTtcManagerTestSetup)
{
  // Check that original system stub is OK, before modifying it to perform the test
  BOOST_REQUIRE_NO_THROW( System lSystem(systemStub) );

  // Check that system construction fails if DaqTTCManager stub's crate field is empty
  systemStub.daqttcs.at(1).crate = "";
  BOOST_CHECK_THROW( System lSystem(systemStub), SystemConstructionFailed);
}


BOOST_FIXTURE_TEST_CASE(BuildSystemWithDaqTtcManagers_InvalidCrateId, SystemDaqTtcManagerTestSetup)
{
  // Check that original system stub is OK, before modifying it to perform the test
  BOOST_REQUIRE_NO_THROW( System lSystem(systemStub) );

  // Check that system construction fails if DaqTTCManager stub's crate field doesn't match any of the system's crates
  systemStub.daqttcs.at(1).crate = "nonexistant_crate_id";
  BOOST_CHECK_THROW( System lSystem(systemStub), SystemConstructionFailed);
}


BOOST_FIXTURE_TEST_CASE(BuildSystemWithDaqTtcManagers_InvalidSlotNumber, SystemDaqTtcManagerTestSetup)
{
  // Check that original system stub is OK, before modifying it to perform the test
  BOOST_REQUIRE_NO_THROW( System lSystem(systemStub) );

  // Check that system construction fails if slot number is invalid
  systemStub.daqttcs.at(1).slot = 5;
  BOOST_CHECK_THROW( System lSystem(systemStub), SystemConstructionFailed);
  systemStub.daqttcs.at(1).slot = 12;
  BOOST_CHECK_THROW( System lSystem(systemStub), SystemConstructionFailed);
}


BOOST_FIXTURE_TEST_CASE(BuildSystemWithDaqTtcManagers_InvalidCreator, SystemDaqTtcManagerTestSetup)
{
  // Check that original system stub is OK, before modifying it to perform the test
  BOOST_REQUIRE_NO_THROW( System lSystem(systemStub) );

  // Check that system construction fails if creator string doesn't match any registered class
  systemStub.daqttcs.at(1).creator = "NonExistantClass";
  BOOST_CHECK_THROW( System lSystem(systemStub), SystemConstructionFailed);
}

BOOST_FIXTURE_TEST_CASE(BuildSystemWithDaqTtcManagers_DaqTtcMgrConstructorThrows, SystemDaqTtcManagerTestSetup)
{
  // Check that original system stub is OK, before modifying it to perform the test
  BOOST_REQUIRE_NO_THROW( System lSystem(systemStub) );

  // Check that system construction fails if DaqTTCManager constructor throws (Object CTOR will throw if ID invalid)
  systemStub.daqttcs.at(1).id = "id.containing.dots";
  BOOST_CHECK_THROW( System lSystem(systemStub), SystemConstructionFailed);

  systemStub.daqttcs.at(1) = dtmStubB;
  BOOST_REQUIRE_NO_THROW(System lSystem(systemStub));
  systemStub.daqttcs.at(1).uriT1 = "throw"; // Will cause non-SWATCH exception to be thrown
  BOOST_CHECK_THROW( System lSystem(systemStub), SystemConstructionFailed);
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//   STUB CONTAINING ONLY PROCESSORS & CRATES
/////////////////////////////////////////////////////////////////////////////////////////////////////////////


struct SystemLinkTestSetup : public SystemProcessorTestSetup {
  LinkStub linkStubAB;
  LinkStub linkStubBA;

  SystemLinkTestSetup() :
    linkStubAB("linkAB"),
    linkStubBA("linkBA")
  {
    for (size_t i=0; i<3; i++) {
      std::string lIdSuffix = boost::lexical_cast<std::string>(i);
      processorStubA.rxPorts.push_back( processor::ProcessorPortStub("myRxPort" + lIdSuffix, i) );
      processorStubB.rxPorts.push_back( processor::ProcessorPortStub("myRxPort" + lIdSuffix, i) );
      processorStubA.txPorts.push_back( processor::ProcessorPortStub("myTxPort" + lIdSuffix, i) );
      processorStubB.txPorts.push_back( processor::ProcessorPortStub("myTxPort" + lIdSuffix, i) );
    }
    systemStub.processors = {processorStubA, processorStubB};

    linkStubAB.srcProcessor = processorStubA.id;
    linkStubAB.srcPort = "myTxPort2";
    linkStubAB.dstProcessor = processorStubB.id;
    linkStubAB.dstPort = "myRxPort2";

    linkStubBA.alias = "An informative link alias!";
    linkStubBA.srcProcessor = processorStubB.id;
    linkStubBA.srcPort = "myTxPort1";
    linkStubBA.dstProcessor = processorStubA.id;
    linkStubBA.dstPort = "myRxPort1";

    systemStub.links.push_back(linkStubAB);
    systemStub.links.push_back(linkStubBA);
  }

  ~SystemLinkTestSetup () {}
};


BOOST_FIXTURE_TEST_CASE(BuildSystemWithLinks_Successful, SystemLinkTestSetup)
{
  System lSystem(systemStub);

  BOOST_CHECK_EQUAL(lSystem.getId(), "myCoolSystem");
  BOOST_CHECK(lSystem.getStub() == systemStub);

  // 1) Check that system contains 2 crates & nothing else
  BOOST_CHECK_EQUAL(lSystem.getActionableChildren().size(), size_t(2));
  BOOST_CHECK_EQUAL(lSystem.getProcessors().size(), size_t(2));
  BOOST_CHECK_EQUAL(lSystem.getDaqTTCs().size(), size_t(0));
  BOOST_CHECK_EQUAL(lSystem.getCrates().size(), size_t(2));
  BOOST_CHECK_EQUAL(lSystem.getLinks().size(), size_t(2));

  // 2) Check that the links have correct ID, source and destination
  const processor::Processor& lProcA = *lSystem.getProcessors().at(0);
  const processor::Processor& lProcB = *lSystem.getProcessors().at(1);

  const Link* lLinkAB = lSystem.getLinks().at(0);
  BOOST_REQUIRE_NE(lLinkAB, (Link*) NULL);
  BOOST_CHECK_EQUAL(lLinkAB->getId(), linkStubAB.id);
  BOOST_CHECK_EQUAL(lLinkAB->getAlias(), "");
  BOOST_CHECK_EQUAL(lLinkAB->getSrcProcessor(), &lProcA);
  BOOST_CHECK_EQUAL(lLinkAB->getSrcPort(), &lProcA.getOutputPorts().getPort(linkStubAB.srcPort));
  BOOST_CHECK_EQUAL(lLinkAB->getDstProcessor(), &lProcB);
  BOOST_CHECK_EQUAL(lLinkAB->getDstPort(), &lProcB.getInputPorts().getPort(linkStubAB.dstPort));

  const Link* lLinkBA = lSystem.getLinks().at(1);
  BOOST_REQUIRE_NE(lLinkBA, (Link*) NULL);
  BOOST_CHECK_EQUAL(lLinkBA->getId(), linkStubBA.id);
  BOOST_CHECK_EQUAL(lLinkBA->getAlias(), "An informative link alias!");
  BOOST_CHECK_EQUAL(lLinkBA->getSrcProcessor(), &lProcB);
  BOOST_CHECK_EQUAL(lLinkBA->getSrcPort(), &lProcB.getOutputPorts().getPort(linkStubBA.srcPort));
  BOOST_CHECK_EQUAL(lLinkBA->getDstProcessor(), &lProcA);
  BOOST_CHECK_EQUAL(lLinkBA->getDstPort(), &lProcA.getInputPorts().getPort(linkStubBA.dstPort));

  // 3) Check that links are accessible through normal object tree as well
  BOOST_CHECK_EQUAL(lLinkAB, &lSystem.getObj<Link>(linkStubAB.id));
  BOOST_CHECK_EQUAL(lLinkBA, &lSystem.getObj<Link>(linkStubBA.id));
}


BOOST_FIXTURE_TEST_CASE(BuildSystemWithLinks_DuplicateId, SystemLinkTestSetup)
{
  // Check that original system stub is OK, before modifying it to perform the test
  BOOST_REQUIRE_NO_THROW( System lSystem(systemStub) );

  // Check that system construction fails after making both link's ID strings identical
  systemStub.links.at(1).id = systemStub.links.at(0).id;
  BOOST_CHECK_THROW( System lSystem(systemStub), SystemConstructionFailed);

  // Check that system construction fails if link's ID string identical to that of a crate
  systemStub.links.at(1).id = systemStub.crates.at(0).id;
  BOOST_CHECK_THROW( System lSystem(systemStub), SystemConstructionFailed);
}


BOOST_FIXTURE_TEST_CASE(BuildSystemWithLinks_InvalidSrcProcessorId, SystemLinkTestSetup)
{
  // Check that original system stub is OK, before modifying it to perform the test
  BOOST_REQUIRE_NO_THROW( System lSystem(systemStub) );

  // Check that system construction fails if link's srcProcessor field  is empty
  systemStub.links.at(1).srcProcessor = "";
  BOOST_CHECK_THROW( System lSystem(systemStub), SystemConstructionFailed);

  // Check that system construction fails if link's srcProcessor field doesn't match any of the processors
  systemStub.links.at(1).srcProcessor = "nonexistant_processor";
  BOOST_CHECK_THROW( System lSystem(systemStub), SystemConstructionFailed);
}


BOOST_FIXTURE_TEST_CASE(BuildSystemWithLinks_InvalidSrcPortId, SystemLinkTestSetup)
{
  // Check that original system stub is OK, before modifying it to perform the test
  BOOST_REQUIRE_NO_THROW( System lSystem(systemStub) );

  // Check that system construction fails if link's srcPort field is empty
  systemStub.links.at(1).srcPort = "";
  BOOST_CHECK_THROW( System lSystem(systemStub), SystemConstructionFailed);

  // Check that system construction fails if link's srcPort field doesn't match any of the processor's tx ports
  systemStub.links.at(1).srcPort = "nonexistant_txPort";
  BOOST_CHECK_THROW( System lSystem(systemStub), SystemConstructionFailed);
}


BOOST_FIXTURE_TEST_CASE(BuildSystemWithLinks_InvalidDstProcessorId, SystemLinkTestSetup)
{
  // Check that original system stub is OK, before modifying it to perform the test
  BOOST_REQUIRE_NO_THROW( System lSystem(systemStub) );

  // Check that system construction fails if link's dstProcessor field  is empty
  systemStub.links.at(1).dstProcessor = "";
  BOOST_CHECK_THROW( System lSystem(systemStub), SystemConstructionFailed);

  // Check that system construction fails if link's dstProcessor field doesn't match any of the processors
  systemStub.links.at(1).dstProcessor = "nonexistant_processor";
  BOOST_CHECK_THROW( System lSystem(systemStub), SystemConstructionFailed);
}


BOOST_FIXTURE_TEST_CASE(BuildSystemWithLinks_InvalidDstPortId, SystemLinkTestSetup)
{
  // Check that original system stub is OK, before modifying it to perform the test
  BOOST_REQUIRE_NO_THROW( System lSystem(systemStub) );

  // Check that system construction fails if link's dstPort field is empty
  systemStub.links.at(1).dstPort = "";
  BOOST_CHECK_THROW( System lSystem(systemStub), SystemConstructionFailed);

  // Check that system construction fails if link's dstPort field doesn't match any of the processor's rx ports
  systemStub.links.at(1).dstPort = "nonexistant_rxPort";
  BOOST_CHECK_THROW( System lSystem(systemStub), SystemConstructionFailed);
}




BOOST_AUTO_TEST_SUITE_END()


} // end ns: test
} // end ns: system
} // end ns: swatch

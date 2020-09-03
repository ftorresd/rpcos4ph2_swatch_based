// Boost Unit Test includes
#include <boost/test/unit_test.hpp>
#include "boost/test/test_tools.hpp"

// Swatch Headers
#include "swatch/core/Object.hpp"
#include "swatch/core/test/DummyObject.hpp"

// Boost Headers
#include <boost/assign.hpp>
#include <boost/foreach.hpp>


using namespace swatch::core::test;

namespace swatch {
namespace core {
namespace test {

//! Unused class that inherits from object; defined to test what happens when dynamic cast fails
class OtherObject : public Object {
};

struct TestFamily {
  TestFamily();
  ~TestFamily();

  DummyObject* granpa;
  DummyObject* parentA;
  DummyObject* kidA1;
  DummyObject* kidA2;
  DummyObject* parentB;
  DummyObject* kidB1;
  DummyObject* kidB2;
};

TestFamily::TestFamily() :
  granpa(new DummyObject("granpa")),
  parentA(new DummyObject("parentA")),
  kidA1(new DummyObject("kidA1")),
  kidA2(new DummyObject("kidA2")),
  parentB(new DummyObject("parentB")),
  kidB1(new DummyObject("kidB1")),
  kidB2(new DummyObject("kidB2"))
{
  granpa->add(parentA);
  parentA->add(kidA1);
  parentA->add(kidA2);
  granpa->add(parentB);
  parentB->add(kidB1);
  parentB->add(kidB2);
}

TestFamily::~TestFamily()
{
  delete granpa;
}



BOOST_AUTO_TEST_SUITE( ObjectTestSuite )


BOOST_AUTO_TEST_CASE(ObjectIdEmpty)
{
  // Should throw if ID string is empty
  BOOST_CHECK_THROW( Object(""), InvalidObjectId );
  BOOST_CHECK_THROW( DummyObject(""), InvalidObjectId );

  BOOST_CHECK_NO_THROW( Object("validId") );
  BOOST_CHECK_NO_THROW( DummyObject("validId") );
}


BOOST_AUTO_TEST_CASE(ObjectIdContainsDots)
{
  // Should throw if ID string contains one or more dots (regardless of location)
  BOOST_CHECK_THROW( Object("."), InvalidObjectId );
  BOOST_CHECK_THROW( Object(".anId"), InvalidObjectId );
  BOOST_CHECK_THROW( Object("an.Id"), InvalidObjectId );
  BOOST_CHECK_THROW( Object("anId."), InvalidObjectId );

  BOOST_CHECK_THROW( DummyObject("."), InvalidObjectId );
  BOOST_CHECK_THROW( DummyObject(".anId"), InvalidObjectId );
  BOOST_CHECK_THROW( DummyObject("an.Id"), InvalidObjectId );
  BOOST_CHECK_THROW( DummyObject("anId."), InvalidObjectId );
}


BOOST_AUTO_TEST_CASE(ObjectIdContainsInvalidCharacters)
{
  // Constructor should not throw if ID string only contains alphanumeric characters
  BOOST_CHECK_NO_THROW( Object("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_") );
  BOOST_CHECK_NO_THROW( DummyObject("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_") );

  // Constructor should throw if ID string contains any non-alphanumeric characters
  std::vector<char> lForbiddenCharacters = {'-', '#', '$', '%', '/', '\\', '+', ' ', '\'', '\"'};

  for (auto lIt = lForbiddenCharacters.begin(); lIt != lForbiddenCharacters.end(); lIt++) {
    const std::string lForbiddenChar = std::string(1, *lIt);
    BOOST_TEST_MESSAGE("  Trying forbidden character '" << lForbiddenChar << "'");

    BOOST_CHECK_THROW( Object(lForbiddenChar.data()), InvalidObjectId );
    BOOST_CHECK_THROW( Object(lForbiddenChar + "anId"), InvalidObjectId );
    BOOST_CHECK_THROW( Object("an" + lForbiddenChar + "Id"), InvalidObjectId );
    BOOST_CHECK_THROW( Object("anId" + lForbiddenChar), InvalidObjectId );

    BOOST_CHECK_THROW( DummyObject(lForbiddenChar.data()), InvalidObjectId );
    BOOST_CHECK_THROW( DummyObject(lForbiddenChar + "anId"), InvalidObjectId );
    BOOST_CHECK_THROW( DummyObject("an" + lForbiddenChar + "Id"), InvalidObjectId );
    BOOST_CHECK_THROW( DummyObject("anId" + lForbiddenChar), InvalidObjectId );
  }
}


BOOST_AUTO_TEST_CASE(ObjectAliasTests)
{
  DummyObject lObjWithoutAlias("myId");
  BOOST_CHECK_EQUAL(lObjWithoutAlias.getId(), "myId");
  BOOST_CHECK_EQUAL(lObjWithoutAlias.getAlias(), "");

  DummyObject lObjWithAlias("myId", "some useful alias");
  BOOST_CHECK_EQUAL(lObjWithAlias.getId(), "myId");
  BOOST_CHECK_EQUAL(lObjWithAlias.getAlias(), "some useful alias");
}


BOOST_AUTO_TEST_CASE(ObjectGetTests)
{
  using namespace swatch::core;
  using namespace swatch::core::test;

  TestFamily family;

  // 1) Check access to immediate children (via pointer comparison)
  BOOST_CHECK_EQUAL( & family.granpa->getObj("parentA"), family.parentA );
  BOOST_CHECK_EQUAL( & family.parentA->getObj("kidA1"), family.kidA1 );
  BOOST_CHECK_EQUAL( & family.parentA->getObj("kidA2"), family.kidA2 );
  BOOST_CHECK_EQUAL( & family.parentB->getObj("kidB1"), family.kidB1 );
  BOOST_CHECK_EQUAL( & family.parentB->getObj("kidB2"), family.kidB2 );

  // 2) Check access to children of children (via pointer comparison)
  BOOST_CHECK_EQUAL( & family.granpa->getObj("parentA.kidA1"), family.kidA1 );
  BOOST_CHECK_EQUAL( & family.granpa->getObj("parentA.kidA2"), family.kidA2 );
  BOOST_CHECK_EQUAL( & family.granpa->getObj("parentB.kidB1"), family.kidB1 );
  BOOST_CHECK_EQUAL( & family.granpa->getObj("parentB.kidB2"), family.kidB2 );
  BOOST_CHECK_EQUAL( & family.granpa->getObj<DummyObject>("parentA.kidA1"), family.kidA1 );
  BOOST_CHECK_EQUAL( & family.granpa->getObj<DummyObject>("parentA.kidA2"), family.kidA2 );
  BOOST_CHECK_EQUAL( & family.granpa->getObj<DummyObject>("parentB.kidB1"), family.kidB1 );
  BOOST_CHECK_EQUAL( & family.granpa->getObj<DummyObject>("parentB.kidB2"), family.kidB2 );

  // 3a) Check behaviour when ID is correct, but cast fails
  BOOST_CHECK_THROW( family.granpa->getObj<OtherObject>("parentA.kidA1"), ObjectFailedCast);
  BOOST_CHECK_EQUAL( family.granpa->getObjPtr<OtherObject>("parentA.kidA1"), (OtherObject*) NULL );

  // 3b) Check that throws correctly when invalid ID given
  BOOST_CHECK_THROW( family.granpa->getObj("invalid_object_id"), ObjectDoesNotExist);
  BOOST_CHECK_THROW( family.granpa->getObj("parentA.invalid_object_id"), ObjectDoesNotExist);
  BOOST_CHECK_THROW( family.granpa->getObj<DummyObject>("invalid_object_id"), ObjectDoesNotExist);
  BOOST_CHECK_THROW( family.granpa->getObj<DummyObject>("parentA.invalid_object_id"), ObjectDoesNotExist);
  BOOST_CHECK_THROW( family.granpa->getObjPtr<DummyObject>("invalid_object_id"), ObjectDoesNotExist);
  BOOST_CHECK_THROW( family.granpa->getObjPtr<DummyObject>("parentA.invalid_object_id"), ObjectDoesNotExist);

  // ...
  //TODO: Here, or in other test cases ... Check type, typeName, getPaths, template<T>getObj, getChildrenOfType
  //TODO: Update to BOOST_FIXTURE_TEST_CASE ???
}


BOOST_AUTO_TEST_CASE(ObjectGetByFullPathTests)
{
  using namespace swatch::core;
  using namespace swatch::core::test;

  TestFamily family;

  // 1) Check access to immediate children (via pointer comparison)
  BOOST_CHECK_EQUAL( & family.granpa->getObjByFullPath<DummyObject>("granpa.parentA"), family.parentA );
  BOOST_CHECK_EQUAL( & family.parentA->getObjByFullPath<DummyObject>("granpa.parentA.kidA1"), family.kidA1 );
  BOOST_CHECK_EQUAL( & family.parentA->getObjByFullPath<DummyObject>("granpa.parentA.kidA2"), family.kidA2 );
  BOOST_CHECK_EQUAL( & family.parentB->getObjByFullPath<DummyObject>("granpa.parentB.kidB1"), family.kidB1 );
  BOOST_CHECK_EQUAL( & family.parentB->getObjByFullPath<DummyObject>("granpa.parentB.kidB2"), family.kidB2 );

  // 2) Check access to children of children (via pointer comparison)
  BOOST_CHECK_EQUAL( & family.granpa->getObjByFullPath<DummyObject>("granpa.parentA.kidA1"), family.kidA1 );
  BOOST_CHECK_EQUAL( & family.granpa->getObjByFullPath<DummyObject>("granpa.parentA.kidA2"), family.kidA2 );
  BOOST_CHECK_EQUAL( & family.granpa->getObjByFullPath<DummyObject>("granpa.parentB.kidB1"), family.kidB1 );
  BOOST_CHECK_EQUAL( & family.granpa->getObjByFullPath<DummyObject>("granpa.parentB.kidB2"), family.kidB2 );

  // 3a) Check behaviour when ID is correct, but cast fails
  BOOST_CHECK_THROW( family.granpa->getObjByFullPath<OtherObject>("granpa.parentA.kidA1"), ObjectFailedCast);

  // 3b) Check that throws correctly when invalid ID given
  BOOST_CHECK_THROW( family.granpa->getObjByFullPath<DummyObject>("invalid_object_id"), ObjectDoesNotExist);
  BOOST_CHECK_THROW( family.granpa->getObjByFullPath<DummyObject>("granpa.invalid_object_id"), ObjectDoesNotExist);
  BOOST_CHECK_THROW( family.granpa->getObjByFullPath<DummyObject>(".granpa.parentB.kidB2"), ObjectDoesNotExist);

  // ...
  //TODO: Here, or in other test cases ... Check type, typeName, getPaths, template<T>getObj, getChildrenOfType
  //TODO: Update to BOOST_FIXTURE_TEST_CASE ???
}


BOOST_AUTO_TEST_CASE(ObjectAddTests_DuplicateId)
{
  DummyObject anObject("parent");

  // Setup: Add a dummy child
  DummyObject* child = new DummyObject("child");
  BOOST_CHECK_THROW( anObject.getObj("child"), ObjectDoesNotExist);
  anObject.add(child);
  BOOST_CHECK_EQUAL( & anObject.getObj("child"), child);

  // Check that Object::add throws when try to add another child with same ID
  DummyObject* child2 = new DummyObject("child");
  BOOST_CHECK_THROW(anObject.add(child2), ObjectOfSameIdAlreadyExists);
}


BOOST_AUTO_TEST_CASE(ObjectAddTests_DuplicateAlias)
{
  DummyObject anObject("parent");

  // Setup: Add a dummy child
  DummyObject* child = new DummyObject("child", "an alias!");
  BOOST_CHECK_THROW( anObject.getObj("child"), ObjectDoesNotExist);
  anObject.add(child);
  BOOST_CHECK_EQUAL( & anObject.getObj("child"), child);

  // Check that Object::add throws when try to add another child with same ID
  DummyObject* child2 = new DummyObject("child2", "an alias!");
  BOOST_CHECK_THROW(anObject.add(child2), ObjectOfSameAliasAlreadyExists);

  // Check that exception isn't thrown when add objects of different ID, both without alias
  BOOST_CHECK_NO_THROW(anObject.add(new DummyObject("child3")));
  BOOST_CHECK_NO_THROW(anObject.add(new DummyObject("child4")));
}


BOOST_AUTO_TEST_CASE(ObjectIdPathTests)
{
  using namespace swatch::core;
  using namespace swatch::core::test;

  TestFamily family;

  BOOST_CHECK_EQUAL( family.granpa->getId(), "granpa");
  BOOST_CHECK_EQUAL( family.granpa->getPath(), "granpa");

  BOOST_CHECK_EQUAL( family.parentA->getId(), "parentA");
  BOOST_CHECK_EQUAL( family.parentB->getId(), "parentB");
  BOOST_CHECK_EQUAL( family.parentA->getPath(), "granpa.parentA");
  BOOST_CHECK_EQUAL( family.parentB->getPath(), "granpa.parentB");

  BOOST_CHECK_EQUAL( family.kidA1->getId(), "kidA1");
  BOOST_CHECK_EQUAL( family.kidA2->getId(), "kidA2");
  BOOST_CHECK_EQUAL( family.kidB1->getId(), "kidB1");
  BOOST_CHECK_EQUAL( family.kidB2->getId(), "kidB2");
  BOOST_CHECK_EQUAL( family.kidA1->getPath(), "granpa.parentA.kidA1");
  BOOST_CHECK_EQUAL( family.kidA2->getPath(), "granpa.parentA.kidA2");
  BOOST_CHECK_EQUAL( family.kidB1->getPath(), "granpa.parentB.kidB1");
  BOOST_CHECK_EQUAL( family.kidB2->getPath(), "granpa.parentB.kidB2");
}



BOOST_AUTO_TEST_CASE(ObjectChildrenTests)
{
  using namespace swatch::core;
  using namespace swatch::core::test;

  TestFamily family;

  // 1) Check children returned from granpa are correct
  std::vector<std::string> lExpectedIds = {"parentA", "parentB"};
  std::vector<std::string> lReturnedIds = family.granpa->getChildren();

  std::sort(lExpectedIds.begin(), lExpectedIds.end());
  std::sort(lReturnedIds.begin(), lReturnedIds.end());
  BOOST_CHECK_EQUAL_COLLECTIONS( lReturnedIds.begin(), lReturnedIds.end(), lExpectedIds.begin(), lExpectedIds.end() );

  // 2) Check children returned from parentA are correct
  lExpectedIds = {"kidA1", "kidA2"};
  lReturnedIds = family.parentA->getChildren();

  std::sort(lExpectedIds.begin(), lExpectedIds.end());
  std::sort(lReturnedIds.begin(), lReturnedIds.end());
  BOOST_CHECK_EQUAL_COLLECTIONS( lReturnedIds.begin(), lReturnedIds.end(), lExpectedIds.begin(), lExpectedIds.end() );

  // 3) Check children returned from parentB are correct
  lExpectedIds = {"kidB1", "kidB2"};
  lReturnedIds = family.parentB->getChildren();

  std::sort(lExpectedIds.begin(), lExpectedIds.end());
  std::sort(lReturnedIds.begin(), lReturnedIds.end());
  BOOST_CHECK_EQUAL_COLLECTIONS( lReturnedIds.begin(), lReturnedIds.end(), lExpectedIds.begin(), lExpectedIds.end() );

  // 4) Check children returned from kids are correct
  BOOST_CHECK( family.kidA1->getChildren().empty() );
  BOOST_CHECK( family.kidA2->getChildren().empty() );
  BOOST_CHECK( family.kidB1->getChildren().empty() );
  BOOST_CHECK( family.kidB2->getChildren().empty() );

}

BOOST_AUTO_TEST_CASE(ObjectDescendantsTests)
{
  using namespace swatch::core;
  using namespace swatch::core::test;

  TestFamily family;

  // 1) Check descendants returned from granpa are correct
  std::vector<std::string> expectedIds = {"parentA", "parentA.kidA1", "parentA.kidA2", "parentB", "parentB.kidB1", "parentB.kidB2"};
  std::vector<std::string> returnedIds = family.granpa->getDescendants();

  std::sort(expectedIds.begin(), expectedIds.end());
  std::sort(returnedIds.begin(), returnedIds.end());
  BOOST_CHECK_EQUAL_COLLECTIONS( returnedIds.begin(), returnedIds.end(), expectedIds.begin(), expectedIds.end() );

  // 2) Check descendants returned from parentA are correct
  expectedIds = {"kidA1", "kidA2"};
  returnedIds = family.parentA->getDescendants();

  std::sort(expectedIds.begin(), expectedIds.end());
  std::sort(returnedIds.begin(), returnedIds.end());
  BOOST_CHECK_EQUAL_COLLECTIONS( returnedIds.begin(), returnedIds.end(), expectedIds.begin(), expectedIds.end() );

  // 3) Check children returned from parentB are correct
  expectedIds = {"kidB1", "kidB2"};
  returnedIds = family.parentB->getDescendants();

  std::sort(expectedIds.begin(), expectedIds.end());
  std::sort(returnedIds.begin(), returnedIds.end());
  BOOST_CHECK_EQUAL_COLLECTIONS( returnedIds.begin(), returnedIds.end(), expectedIds.begin(), expectedIds.end() );

  // 4) Check children returned from kids are correct
  BOOST_CHECK( family.kidA1->getDescendants().empty() );
  BOOST_CHECK( family.kidA2->getDescendants().empty() );
  BOOST_CHECK( family.kidB1->getDescendants().empty() );
  BOOST_CHECK( family.kidB2->getDescendants().empty() );

}

BOOST_FIXTURE_TEST_CASE(ObjectIsAncestor, TestFamily)
{
  BOOST_CHECK_EQUAL(granpa->isAncestorOf(*granpa), false);
  BOOST_CHECK_EQUAL(granpa->isAncestorOf(*parentA), true);
  BOOST_CHECK_EQUAL(granpa->isAncestorOf(*kidA1), true);
  BOOST_CHECK_EQUAL(granpa->isAncestorOf(*kidA2), true);
  BOOST_CHECK_EQUAL(granpa->isAncestorOf(*parentB), true);
  BOOST_CHECK_EQUAL(granpa->isAncestorOf(*kidB1), true);
  BOOST_CHECK_EQUAL(granpa->isAncestorOf(*kidB2), true);

  BOOST_CHECK_EQUAL(parentA->isAncestorOf(*granpa), false);
  BOOST_CHECK_EQUAL(parentA->isAncestorOf(*parentA), false);
  BOOST_CHECK_EQUAL(parentA->isAncestorOf(*kidA1), true);
  BOOST_CHECK_EQUAL(parentA->isAncestorOf(*kidA2), true);
  BOOST_CHECK_EQUAL(parentA->isAncestorOf(*parentB), false);
  BOOST_CHECK_EQUAL(parentA->isAncestorOf(*kidB1), false);
  BOOST_CHECK_EQUAL(parentA->isAncestorOf(*kidB2), false);

  BOOST_CHECK_EQUAL(kidA1->isAncestorOf(*granpa), false);
  BOOST_CHECK_EQUAL(kidA1->isAncestorOf(*parentA), false);
  BOOST_CHECK_EQUAL(kidA1->isAncestorOf(*kidA1), false);
  BOOST_CHECK_EQUAL(kidA1->isAncestorOf(*kidA2), false);
  BOOST_CHECK_EQUAL(kidA1->isAncestorOf(*parentB), false);
  BOOST_CHECK_EQUAL(kidA1->isAncestorOf(*kidB1), false);
  BOOST_CHECK_EQUAL(kidA1->isAncestorOf(*kidB2), false);

  // Check that comparison isn't just based on ID paths - instead isAncestor should check if actual object instances are part of same tree
  TestFamily family2;
  BOOST_CHECK_EQUAL(parentA->isAncestorOf(*family2.granpa), false);
  BOOST_CHECK_EQUAL(parentA->isAncestorOf(*family2.parentA), false);
  BOOST_CHECK_EQUAL(parentA->isAncestorOf(*family2.kidA1), false);
  BOOST_CHECK_EQUAL(parentA->isAncestorOf(*family2.kidA2), false);
  BOOST_CHECK_EQUAL(parentA->isAncestorOf(*family2.parentB), false);
  BOOST_CHECK_EQUAL(parentA->isAncestorOf(*family2.kidB1), false);
  BOOST_CHECK_EQUAL(parentA->isAncestorOf(*family2.kidB2), false);
}


BOOST_FIXTURE_TEST_CASE(ObjectGetNumberOfGenerationsTo, TestFamily)
{
  BOOST_CHECK_THROW(granpa->getNumberOfGenerationsTo(*granpa), ObjectIsNotDescendant);
  BOOST_CHECK_EQUAL(granpa->getNumberOfGenerationsTo(*parentA), size_t(1));
  BOOST_CHECK_EQUAL(granpa->getNumberOfGenerationsTo(*kidA1), size_t(2));
  BOOST_CHECK_EQUAL(granpa->getNumberOfGenerationsTo(*kidA2), size_t(2));
  BOOST_CHECK_EQUAL(granpa->getNumberOfGenerationsTo(*parentB), size_t(1));
  BOOST_CHECK_EQUAL(granpa->getNumberOfGenerationsTo(*kidB1), size_t(2));
  BOOST_CHECK_EQUAL(granpa->getNumberOfGenerationsTo(*kidB2), size_t(2));

  BOOST_CHECK_THROW(parentA->getNumberOfGenerationsTo(*granpa), ObjectIsNotDescendant);
  BOOST_CHECK_THROW(parentA->getNumberOfGenerationsTo(*parentA), ObjectIsNotDescendant);
  BOOST_CHECK_EQUAL(parentA->getNumberOfGenerationsTo(*kidA1), size_t(1));
  BOOST_CHECK_EQUAL(parentA->getNumberOfGenerationsTo(*kidA2), size_t(1));
  BOOST_CHECK_THROW(parentA->getNumberOfGenerationsTo(*parentB), ObjectIsNotDescendant);
  BOOST_CHECK_THROW(parentA->getNumberOfGenerationsTo(*kidB1), ObjectIsNotDescendant);
  BOOST_CHECK_THROW(parentA->getNumberOfGenerationsTo(*kidB2), ObjectIsNotDescendant);

  BOOST_CHECK_THROW(kidA1->getNumberOfGenerationsTo(*granpa), ObjectIsNotDescendant);
  BOOST_CHECK_THROW(kidA1->getNumberOfGenerationsTo(*parentA), ObjectIsNotDescendant);
  BOOST_CHECK_THROW(kidA1->getNumberOfGenerationsTo(*kidA1), ObjectIsNotDescendant);
  BOOST_CHECK_THROW(kidA1->getNumberOfGenerationsTo(*kidA2), ObjectIsNotDescendant);
  BOOST_CHECK_THROW(kidA1->getNumberOfGenerationsTo(*parentB), ObjectIsNotDescendant);
  BOOST_CHECK_THROW(kidA1->getNumberOfGenerationsTo(*kidB1), ObjectIsNotDescendant);
  BOOST_CHECK_THROW(kidA1->getNumberOfGenerationsTo(*kidB2), ObjectIsNotDescendant);

  // Check that comparison isn't just based on ID paths - instead isAncestor should check if actual object instances are part of same tree
  TestFamily family2;
  BOOST_CHECK_THROW(parentA->getNumberOfGenerationsTo(*family2.granpa), ObjectIsNotDescendant);
  BOOST_CHECK_THROW(parentA->getNumberOfGenerationsTo(*family2.parentA), ObjectIsNotDescendant);
  BOOST_CHECK_THROW(parentA->getNumberOfGenerationsTo(*family2.kidA1), ObjectIsNotDescendant);
  BOOST_CHECK_THROW(parentA->getNumberOfGenerationsTo(*family2.kidA2), ObjectIsNotDescendant);
  BOOST_CHECK_THROW(parentA->getNumberOfGenerationsTo(*family2.parentB), ObjectIsNotDescendant);
  BOOST_CHECK_THROW(parentA->getNumberOfGenerationsTo(*family2.kidB1), ObjectIsNotDescendant);
  BOOST_CHECK_THROW(parentA->getNumberOfGenerationsTo(*family2.kidB2), ObjectIsNotDescendant);
}


//TODO
/*
BOOST_AUTO_TEST_CASE(ObjectViewTest) {

    SimpleView* uncle = new SimpleView("uncle");

    uncle->add(kidA1, "nA1");
    uncle->add(kidA2, "nA2");

    uncle->add(kidB1, "nB1");
    uncle->add(kidB2, "nB2");


    nephews->add(kidA)

    granpa->add(nephews);



}
*/


BOOST_AUTO_TEST_SUITE_END() // ObjectTestSuite


} //namespace test
} //namespace core
} //namespace swatch


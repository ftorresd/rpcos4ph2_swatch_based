// Boost Unit Test includes
#include <boost/test/unit_test.hpp>

// Classes under test ...
#include "swatch/core/ReadOnlyXParameterSet.hpp"
#include "swatch/core/ReadWriteXParameterSet.hpp"

// boost headers
#include "boost/lexical_cast.hpp"
#include "boost/scoped_ptr.hpp"
#include <boost/test/test_tools.hpp>

// XDAQ headers
#include "xdata/Boolean.h"
#include "xdata/Integer.h"
#include "xdata/Float.h"
#include "xdata/String.h"


namespace swatch {
namespace core {
namespace test {


BOOST_AUTO_TEST_SUITE( XParSetsTestSuite )

BOOST_AUTO_TEST_CASE ( EmptySetTest )
{
  ReadOnlyXParameterSet lPSet1;

  BOOST_CHECK_EQUAL(lPSet1.size(), size_t(0));
  BOOST_CHECK_EQUAL(lPSet1.keys().size(), size_t(0));
  BOOST_CHECK_THROW(lPSet1["aKey"], XParameterNotFound);
  BOOST_CHECK_THROW(lPSet1.get("aKey"), XParameterNotFound);
  BOOST_CHECK_THROW(lPSet1.get<xdata::String>("aKey"), XParameterNotFound);
  BOOST_CHECK_EQUAL(lPSet1.has("aKey"), false);


  ReadWriteXParameterSet lPSet2;

  BOOST_CHECK_EQUAL(lPSet2.size(), size_t(0));
  BOOST_CHECK_EQUAL(lPSet2.keys().size(), size_t(0));
  BOOST_CHECK_THROW(lPSet2["aKey"], XParameterNotFound);
  BOOST_CHECK_THROW(lPSet2.get("aKey"), XParameterNotFound);
  BOOST_CHECK_THROW(lPSet2.get<xdata::String>("aKey"), XParameterNotFound);
  BOOST_CHECK_EQUAL(lPSet2.has("aKey"), false);
}


struct XParTestSetup {
  XParTestSetup();
  ~XParTestSetup();

  boost::shared_ptr<xdata::String> str;
  boost::shared_ptr<xdata::Integer> intA;
  boost::shared_ptr<xdata::Integer> intB;
};

XParTestSetup::XParTestSetup() :
  str(new xdata::String("aStringValue")),
  intA(new xdata::Integer(42)),
  intB(new xdata::Integer(-1))
{
}

XParTestSetup::~XParTestSetup() {}


BOOST_FIXTURE_TEST_CASE ( SimpleAdoptGetEraseTest, XParTestSetup)
{
  /*  ---  ReadOnlyXParameterSet  ---  */
  {
    ReadOnlyXParameterSet psetReadOnly;

    psetReadOnly.adopt("aString", str);
    psetReadOnly.adopt("intA", intA);
    psetReadOnly.adopt("intB", intB);

    BOOST_CHECK_EQUAL(psetReadOnly.size(), size_t(3));
    std::set<std::string> expectedKeys = {"aString", "intA", "intB"};
    std::set<std::string> returnedKeys = psetReadOnly.keys();
    BOOST_CHECK_EQUAL_COLLECTIONS(returnedKeys.begin(), returnedKeys.end(), expectedKeys.begin(), expectedKeys.end() );

    BOOST_CHECK_EQUAL(psetReadOnly.has("aString"), true);
    BOOST_CHECK_EQUAL(psetReadOnly.has("intA"), true);
    BOOST_CHECK_EQUAL(psetReadOnly.has("intB"), true);
    BOOST_CHECK_EQUAL(psetReadOnly.has("someOtherItem"), false);

    // Check that parameter values are correct
    BOOST_CHECK_EQUAL( psetReadOnly.get<xdata::String>("aString").value_, str->value_);
    BOOST_CHECK_EQUAL( psetReadOnly.get<xdata::Integer>("intA").value_, intA->value_);
    BOOST_CHECK_EQUAL( psetReadOnly.get<xdata::Integer>("intB").value_, intB->value_);

    // Check that values converted to string correctly
    BOOST_CHECK_EQUAL( psetReadOnly.get("aString").toString(), str->value_);
    BOOST_CHECK_EQUAL( psetReadOnly.get("intA").toString(), boost::lexical_cast<std::string>(intA->value_) );
    BOOST_CHECK_EQUAL( psetReadOnly.get("intB").toString(), boost::lexical_cast<std::string>(intB->value_) );

    // Check that parameter values weren't copied
    BOOST_CHECK_EQUAL( & psetReadOnly.get("aString") , str.get() );
    BOOST_CHECK_EQUAL( & psetReadOnly.get("intA") , intA.get() );
    BOOST_CHECK_EQUAL( & psetReadOnly.get("intB") , intB.get() );
    BOOST_CHECK_EQUAL( & psetReadOnly["aString"] , str.get() );
    BOOST_CHECK_EQUAL( & psetReadOnly["intA"] , intA.get() );
    BOOST_CHECK_EQUAL( & psetReadOnly["intB"] , intB.get() );

    // Check that exception thrown when cast to incorrect type
    BOOST_CHECK_THROW( psetReadOnly.XParameterSet::get<xdata::Boolean>("aString") , XParameterFailedCast);
    BOOST_CHECK_THROW( psetReadOnly.get<xdata::Boolean>("aString") , XParameterFailedCast);
    BOOST_CHECK_THROW( psetReadOnly.XParameterSet::get<xdata::Boolean>("intA") , XParameterFailedCast);
    BOOST_CHECK_THROW( psetReadOnly.get<xdata::Boolean>("intA") , XParameterFailedCast);
    BOOST_CHECK_THROW( psetReadOnly.XParameterSet::get<xdata::String>("intB") , XParameterFailedCast);
    BOOST_CHECK_THROW( psetReadOnly.get<xdata::String>("intB") , XParameterFailedCast);
  }


  /*  ---  ReadWriteXParameterSet : adopt method  ---  */
  {
    ReadWriteXParameterSet psetReadWrite;
    psetReadWrite.adopt("aString", str);
    psetReadWrite.adopt("intA", intA);
    psetReadWrite.adopt("intB", intB);

    BOOST_CHECK_EQUAL(psetReadWrite.size(), size_t(3));
    std::set<std::string> expectedKeys = {"aString", "intA", "intB"};
    std::set<std::string> returnedKeys = psetReadWrite.keys();
    BOOST_CHECK_EQUAL_COLLECTIONS(returnedKeys.begin(), returnedKeys.end(), expectedKeys.begin(), expectedKeys.end() );

    BOOST_CHECK_EQUAL(psetReadWrite.has("aString"), true);
    BOOST_CHECK_EQUAL(psetReadWrite.has("intA"), true);
    BOOST_CHECK_EQUAL(psetReadWrite.has("intB"), true);
    BOOST_CHECK_EQUAL(psetReadWrite.has("someOtherItem"), false);

    // Check that parameter values are correct
    BOOST_CHECK_EQUAL( psetReadWrite.get<xdata::String>("aString").value_, str->value_);
    BOOST_CHECK_EQUAL( psetReadWrite.get<xdata::Integer>("intA").value_, intA->value_);
    BOOST_CHECK_EQUAL( psetReadWrite.get<xdata::Integer>("intB").value_, intB->value_);

    // Check that values converted to string correctly
    BOOST_CHECK_EQUAL( psetReadWrite.get("aString").toString(), str->value_);
    BOOST_CHECK_EQUAL( psetReadWrite.get("intA").toString(), boost::lexical_cast<std::string>(intA->value_) );
    BOOST_CHECK_EQUAL( psetReadWrite.get("intB").toString(), boost::lexical_cast<std::string>(intB->value_) );

    // Check that parameter values weren't copied
    BOOST_CHECK_EQUAL( & psetReadWrite.get("aString") , str.get() );
    BOOST_CHECK_EQUAL( & psetReadWrite.get("intA") , intA.get() );
    BOOST_CHECK_EQUAL( & psetReadWrite.get("intB") , intB.get() );
    BOOST_CHECK_EQUAL( & psetReadWrite["aString"] , str.get() );
    BOOST_CHECK_EQUAL( & psetReadWrite["intA"] , intA.get() );
    BOOST_CHECK_EQUAL( & psetReadWrite["intB"] , intB.get() );

    // Check that exception thrown when cast to incorrect type
    BOOST_CHECK_THROW( psetReadWrite.XParameterSet::get<xdata::Boolean>("aString") , XParameterFailedCast);
    BOOST_CHECK_THROW( psetReadWrite.get<xdata::Boolean>("aString") , XParameterFailedCast);
    BOOST_CHECK_THROW( psetReadWrite.XParameterSet::get<xdata::Boolean>("intA") , XParameterFailedCast);
    BOOST_CHECK_THROW( psetReadWrite.get<xdata::Boolean>("intA") , XParameterFailedCast);
    BOOST_CHECK_THROW( psetReadWrite.XParameterSet::get<xdata::String>("intB") , XParameterFailedCast);
    BOOST_CHECK_THROW( psetReadWrite.get<xdata::String>("intB") , XParameterFailedCast);
  }


  /*  ---  ReadWriteXParameterSet : add method ---  */
  {
    ReadWriteXParameterSet psetReadWrite;
    psetReadWrite.add("aString", xdata::String(*str));
    psetReadWrite.add("intA", xdata::Integer(*intA));
    psetReadWrite.add("intB", xdata::Integer(*intB));

    BOOST_CHECK_EQUAL(psetReadWrite.size(), size_t(3));
    std::set<std::string> expectedKeys = {"aString", "intA", "intB"};
    std::set<std::string> returnedKeys = psetReadWrite.keys();
    BOOST_CHECK_EQUAL_COLLECTIONS(returnedKeys.begin(), returnedKeys.end(), expectedKeys.begin(), expectedKeys.end() );

    BOOST_CHECK_EQUAL(psetReadWrite.has("aString"), true);
    BOOST_CHECK_EQUAL(psetReadWrite.has("intA"), true);
    BOOST_CHECK_EQUAL(psetReadWrite.has("intB"), true);
    BOOST_CHECK_EQUAL(psetReadWrite.has("someOtherItem"), false);
    
    // Check that cannot overwrite existing entries using 'add' method
    BOOST_CHECK_THROW(psetReadWrite.add("aString", xdata::String("A string that should not enter the set")), XParameterExists);
    BOOST_CHECK_THROW(psetReadWrite.add("intA", xdata::Integer(*intA+1)), XParameterExists);
    BOOST_CHECK_THROW(psetReadWrite.add("intB", xdata::Integer(*intB+1)), XParameterExists);

    // Check that parameter values are correct
    BOOST_CHECK_EQUAL( psetReadWrite.get<xdata::String>("aString").value_, str->value_);
    BOOST_CHECK_EQUAL( psetReadWrite.get<xdata::Integer>("intA").value_, intA->value_);
    BOOST_CHECK_EQUAL( psetReadWrite.get<xdata::Integer>("intB").value_, intB->value_);

    // Check that parameter values converted to string correctly
    BOOST_CHECK_EQUAL( psetReadWrite.get("aString").toString(), str->value_);
    BOOST_CHECK_EQUAL( psetReadWrite.get("intA").toString(), boost::lexical_cast<std::string>(intA->value_) );
    BOOST_CHECK_EQUAL( psetReadWrite.get("intB").toString(), boost::lexical_cast<std::string>(intB->value_) );

    // Check that parameter values WERE copied
    BOOST_CHECK_NE ( & psetReadWrite.get("aString") , str.get() );
    BOOST_CHECK_NE ( & psetReadWrite.get("intA") , intA.get() );
    BOOST_CHECK_NE ( & psetReadWrite.get("intB") , intB.get() );
    BOOST_CHECK_NE ( & psetReadWrite["aString"] , str.get() );
    BOOST_CHECK_NE ( & psetReadWrite["intA"] , intA.get() );
    BOOST_CHECK_NE ( & psetReadWrite["intB"] , intB.get() );

    // Check that exception thrown when cast to incorrect type
    BOOST_CHECK_THROW( psetReadWrite.XParameterSet::get<xdata::Boolean>("aString") , XParameterFailedCast);
    BOOST_CHECK_THROW( psetReadWrite.get<xdata::Boolean>("aString") , XParameterFailedCast);
    BOOST_CHECK_THROW( psetReadWrite.XParameterSet::get<xdata::Boolean>("intA") , XParameterFailedCast);
    BOOST_CHECK_THROW( psetReadWrite.get<xdata::Boolean>("intA") , XParameterFailedCast);
    BOOST_CHECK_THROW( psetReadWrite.XParameterSet::get<xdata::String>("intB") , XParameterFailedCast);
    BOOST_CHECK_THROW( psetReadWrite.get<xdata::String>("intB") , XParameterFailedCast);
  }
}



BOOST_FIXTURE_TEST_CASE ( ReadOnlyShallowCopyTest, XParTestSetup)
{
  // Create the "source" set
  boost::scoped_ptr<ReadOnlyXParameterSet> originalPSet( new ReadOnlyXParameterSet() );
  originalPSet->adopt("aString", str);
  originalPSet->adopt("intA", intA);
  originalPSet->adopt("intB", intB);

  // Copy the "source" set - creating instance that will be tested
  ReadOnlyXParameterSet pset(*originalPSet);

  // Delete the original set before testing ...
  originalPSet.reset(NULL);

  // Check the basics
  BOOST_CHECK_EQUAL(pset.size(), size_t(3));
  std::set<std::string> expectedKeys = {"aString", "intA", "intB"};
  std::set<std::string> returnedKeys = pset.keys();
  BOOST_CHECK_EQUAL_COLLECTIONS(returnedKeys.begin(), returnedKeys.end(), expectedKeys.begin(), expectedKeys.end() );

  BOOST_CHECK_EQUAL(pset.has("aString"), true);
  BOOST_CHECK_EQUAL(pset.has("intA"), true);
  BOOST_CHECK_EQUAL(pset.has("intB"), true);
  BOOST_CHECK_EQUAL(pset.has("someOtherItem"), false);

  // Check that parameter values are correct
  BOOST_CHECK_EQUAL( pset.get<xdata::String>("aString").value_, str->value_);
  BOOST_CHECK_EQUAL( pset.get<xdata::Integer>("intA").value_, intA->value_);
  BOOST_CHECK_EQUAL( pset.get<xdata::Integer>("intB").value_, intB->value_);

  // Check that values converted to string correctly
  BOOST_CHECK_EQUAL( pset.get("aString").toString(), str->value_);
  BOOST_CHECK_EQUAL( pset.get("intA").toString(), boost::lexical_cast<std::string>(intA->value_) );
  BOOST_CHECK_EQUAL( pset.get("intB").toString(), boost::lexical_cast<std::string>(intB->value_) );

  // Check that parameter values weren't copied
  BOOST_CHECK_EQUAL( & pset.get("aString") , str.get() );
  BOOST_CHECK_EQUAL( & pset.get("intA") , intA.get() );
  BOOST_CHECK_EQUAL( & pset.get("intB") , intB.get() );
  BOOST_CHECK_EQUAL( & pset["aString"] , str.get() );
  BOOST_CHECK_EQUAL( & pset["intA"] , intA.get() );
  BOOST_CHECK_EQUAL( & pset["intB"] , intB.get() );

  // Check that exception thrown when cast to incorrect type
  BOOST_CHECK_THROW( pset.XParameterSet::get<xdata::Boolean>("aString") , XParameterFailedCast);
  BOOST_CHECK_THROW( pset.get<xdata::Boolean>("aString") , XParameterFailedCast);
  BOOST_CHECK_THROW( pset.XParameterSet::get<xdata::Boolean>("intA") , XParameterFailedCast);
  BOOST_CHECK_THROW( pset.get<xdata::Boolean>("intA") , XParameterFailedCast);
  BOOST_CHECK_THROW( pset.XParameterSet::get<xdata::String>("intB") , XParameterFailedCast);
  BOOST_CHECK_THROW( pset.get<xdata::String>("intB") , XParameterFailedCast);
}



BOOST_FIXTURE_TEST_CASE ( ReadWriteToReadOnlyShallowCopyTest, XParTestSetup)
{
  // Create the "source" set
  boost::scoped_ptr<ReadWriteXParameterSet> originalPSet( new ReadWriteXParameterSet() );
  originalPSet->adopt("aString", str);
  originalPSet->adopt("intA", intA);
  originalPSet->adopt("intB", intB);

  // Copy the "source" set - creating instance that will be tested
  ReadOnlyXParameterSet pset(*originalPSet);

  // Delete the original set before testing ...
  originalPSet.reset(NULL);

  // Check the basics
  BOOST_CHECK_EQUAL(pset.size(), size_t(3));
  std::set<std::string> expectedKeys = {"aString", "intA", "intB"};
  std::set<std::string> returnedKeys = pset.keys();
  BOOST_CHECK_EQUAL_COLLECTIONS(returnedKeys.begin(), returnedKeys.end(), expectedKeys.begin(), expectedKeys.end() );

  BOOST_CHECK_EQUAL(pset.has("aString"), true);
  BOOST_CHECK_EQUAL(pset.has("intA"), true);
  BOOST_CHECK_EQUAL(pset.has("intB"), true);
  BOOST_CHECK_EQUAL(pset.has("someOtherItem"), false);

  // Check that parameter values are correct
  BOOST_CHECK_EQUAL( pset.get<xdata::String>("aString").value_, str->value_);
  BOOST_CHECK_EQUAL( pset.get<xdata::Integer>("intA").value_, intA->value_);
  BOOST_CHECK_EQUAL( pset.get<xdata::Integer>("intB").value_, intB->value_);

  // Check that values converted to string correctly
  BOOST_CHECK_EQUAL( pset.get("aString").toString(), str->value_);
  BOOST_CHECK_EQUAL( pset.get("intA").toString(), boost::lexical_cast<std::string>(intA->value_) );
  BOOST_CHECK_EQUAL( pset.get("intB").toString(), boost::lexical_cast<std::string>(intB->value_) );

  // Check that parameter values weren't copied
  BOOST_CHECK_EQUAL( & pset.get("aString") , str.get() );
  BOOST_CHECK_EQUAL( & pset.get("intA") , intA.get() );
  BOOST_CHECK_EQUAL( & pset.get("intB") , intB.get() );
  BOOST_CHECK_EQUAL( & pset["aString"] , str.get() );
  BOOST_CHECK_EQUAL( & pset["intA"] , intA.get() );
  BOOST_CHECK_EQUAL( & pset["intB"] , intB.get() );

  // Check that exception thrown when cast to incorrect type
  BOOST_CHECK_THROW( pset.XParameterSet::get<xdata::Boolean>("aString") , XParameterFailedCast);
  BOOST_CHECK_THROW( pset.get<xdata::Boolean>("aString") , XParameterFailedCast);
  BOOST_CHECK_THROW( pset.XParameterSet::get<xdata::Boolean>("intA") , XParameterFailedCast);
  BOOST_CHECK_THROW( pset.get<xdata::Boolean>("intA") , XParameterFailedCast);
  BOOST_CHECK_THROW( pset.XParameterSet::get<xdata::String>("intB") , XParameterFailedCast);
  BOOST_CHECK_THROW( pset.get<xdata::String>("intB") , XParameterFailedCast);
}



BOOST_FIXTURE_TEST_CASE ( ReadWriteDeepCopyTest, XParTestSetup)
{
  // Create the "source" set
  boost::scoped_ptr<ReadWriteXParameterSet> originalPSet( new ReadWriteXParameterSet() );
  originalPSet->adopt("aString", str);
  originalPSet->adopt("intA", intA);
  originalPSet->adopt("intB", intB);

  // Copy the "source" set - creating instance that will be tested
  ReadWriteXParameterSet pset;
  pset.deepCopyFrom(*originalPSet);

  // Delete the original set before testing ...
  originalPSet.reset(NULL);

  // Check the basics
  BOOST_CHECK_EQUAL(pset.size(), size_t(3));
  std::set<std::string> expectedKeys = {"aString", "intA", "intB"};
  std::set<std::string> returnedKeys = pset.keys();
  BOOST_CHECK_EQUAL_COLLECTIONS(returnedKeys.begin(), returnedKeys.end(), expectedKeys.begin(), expectedKeys.end() );

  BOOST_CHECK_EQUAL(pset.has("aString"), true);
  BOOST_CHECK_EQUAL(pset.has("intA"), true);
  BOOST_CHECK_EQUAL(pset.has("intB"), true);
  BOOST_CHECK_EQUAL(pset.has("someOtherItem"), false);

  // Check that parameter values are correct
  BOOST_CHECK_EQUAL( pset.get<xdata::String>("aString").value_, str->value_);
  BOOST_CHECK_EQUAL( pset.get<xdata::Integer>("intA").value_, intA->value_);
  BOOST_CHECK_EQUAL( pset.get<xdata::Integer>("intB").value_, intB->value_);

  // Check that values converted to string correctly
  BOOST_CHECK_EQUAL( pset.get("aString").toString(), str->value_);
  BOOST_CHECK_EQUAL( pset.get("intA").toString(), boost::lexical_cast<std::string>(intA->value_) );
  BOOST_CHECK_EQUAL( pset.get("intB").toString(), boost::lexical_cast<std::string>(intB->value_) );

  // Check that parameter values WERE copied
  BOOST_CHECK_NE( & pset.get("aString") , str.get() );
  BOOST_CHECK_NE( & pset.get("intA") , intA.get() );
  BOOST_CHECK_NE( & pset.get("intB") , intB.get() );
  BOOST_CHECK_NE( & pset["aString"] , str.get() );
  BOOST_CHECK_NE( & pset["intA"] , intA.get() );
  BOOST_CHECK_NE( & pset["intB"] , intB.get() );

  // Check that exception thrown when cast to incorrect type
  BOOST_CHECK_THROW( pset.XParameterSet::get<xdata::Boolean>("aString") , XParameterFailedCast);
  BOOST_CHECK_THROW( pset.get<xdata::Boolean>("aString") , XParameterFailedCast);
  BOOST_CHECK_THROW( pset.XParameterSet::get<xdata::Boolean>("intA") , XParameterFailedCast);
  BOOST_CHECK_THROW( pset.get<xdata::Boolean>("intA") , XParameterFailedCast);
  BOOST_CHECK_THROW( pset.XParameterSet::get<xdata::String>("intB") , XParameterFailedCast);
  BOOST_CHECK_THROW( pset.get<xdata::String>("intB") , XParameterFailedCast);
}



BOOST_AUTO_TEST_SUITE_END() // XParsTestSuite


} //ns: test
} //ns: core
} //ns: swatch

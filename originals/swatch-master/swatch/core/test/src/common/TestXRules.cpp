#include <boost/test/unit_test.hpp>


// boost headers
#include "boost/lexical_cast.hpp"
#include <boost/filesystem.hpp>

// XDAQ headers
#include "xdata/Table.h"
#include "xdata/TableIterator.h"

// SWATCH headers
#include "swatch/xsimpletypedefs.hpp"
#include "swatch/xvectortypedefs.hpp"
#include "swatch/core/rules/None.hpp"
#include "swatch/core/rules/FiniteNumber.hpp"
#include "swatch/core/rules/FiniteVector.hpp"
#include "swatch/core/rules/IsAmong.hpp"
#include "swatch/core/rules/NoEmptyCells.hpp"
#include "swatch/core/rules/GreaterThan.hpp"
#include "swatch/core/rules/LesserThan.hpp"
#include "swatch/core/rules/InRange.hpp"
#include "swatch/core/rules/OutOfRange.hpp"
#include "swatch/core/rules/OfSize.hpp"
#include "swatch/core/rules/And.hpp"
#include "swatch/core/rules/Or.hpp"
#include "swatch/core/rules/Not.hpp"
#include "swatch/core/rules/PathExists.hpp"


namespace swatch {
namespace core {
namespace test {

struct RuleTestSetup {

  XInt_t intNaN;
  XInt_t intOK;
  XUInt_t uintNaN;
  XUInt_t uintOK;

  XFloat_t floatNaN;
  XFloat_t floatOK;
  XDouble_t doubleNaN;
  XDouble_t doubleOK;

  //---
  XVectorInt_t vIntEmpty;
  XVectorInt_t vIntNaN;
  XVectorInt_t vIntOK;

  XVectorUInt_t vUintEmpty;
  XVectorUInt_t vUintNaN;
  XVectorUInt_t vUintOK;

  XVectorFloat_t vFloatEmpty;
  XVectorFloat_t vFloatNaN;
  XVectorFloat_t vFloatOK;

  XVectorDouble_t vDoubleEmpty;
  XVectorDouble_t vDoubleNaN;
  XVectorDouble_t vDoubleOK;

  XString_t strEmpty;
  XString_t strFull;
  XString_t strOptA;
  XString_t strOptD;

  xdata::Table tableEmpty;
  xdata::Table tableEmptyCell;
  xdata::Table tableEmptyCellBig;
  xdata::Table tableUnsupColumn;
  xdata::Table tableUnsupColumnAndEmpty;
  xdata::Table tableOK;

  RuleTestSetup() : 
    intNaN(),
    intOK(-123),
    uintNaN(),
    uintOK(56.),
    floatNaN(),
    floatOK(-5.),
    doubleNaN(),
    doubleOK(90.),

    vIntEmpty(),
    vUintEmpty(),
    vFloatEmpty(),
    vDoubleEmpty(),

    strEmpty(),
    strFull("I am a string"),
    strOptA("A"),
    strOptD("D"),

    tableEmpty()
  {
    for ( int i : { 1, -2, 3 } ) {
       vIntNaN.push_back(XInt_t(i));
       vIntOK.push_back(XInt_t(i));
    }
    vIntNaN.push_back(XInt_t());

    for ( int u : { 10, 20, 30 } ) {
       vUintNaN.push_back(XUInt_t(u));
       vUintOK.push_back(XUInt_t(u));
    }
    vUintNaN.push_back(XUInt_t());

    for ( int f : { 5., 6., -7. } ) {
       vFloatNaN.push_back(XFloat_t(f));
       vFloatOK.push_back(XFloat_t(f));
    }
    vFloatNaN.push_back(XFloat_t());

    for ( int d : { 50., 60., -70. } ) {
       vDoubleNaN.push_back(XDouble_t(d));
       vDoubleOK.push_back(XDouble_t(d));
    }
    vDoubleNaN.push_back(XDouble_t());

    auto initTable = [] ( xdata::Table& t ) {
      t.addColumn("col_float", "float");
      t.addColumn("col_int", "int");
      t.addColumn("col_uint", "unsigned int");
      t.addColumn("col_str", "string");

      t.append();

      *(t.getValueAt(0, "col_float")) = XFloat_t(5.);
      *(t.getValueAt(0, "col_int")) = XInt_t(-87);
      *(t.getValueAt(0, "col_uint")) = XUInt_t(1900);
      *(t.getValueAt(0, "col_str")) = XString_t("Lorem ipsum");

      t.append();
      *(t.getValueAt(1, "col_float")) = XFloat_t(10.);
      *(t.getValueAt(1, "col_int")) = XInt_t(-847);
      *(t.getValueAt(1, "col_uint")) = XUInt_t(3900);
      *(t.getValueAt(1, "col_str")) = XString_t("Ipsum lorem");

      t.append();
      *(t.getValueAt(2, "col_float")) = XFloat_t(13.);
      *(t.getValueAt(2, "col_int")) = XInt_t(-27);
      *(t.getValueAt(2, "col_uint")) = XUInt_t(40);
      *(t.getValueAt(2, "col_str")) = XString_t("Dixit");
    };

    initTable(tableOK);
    initTable(tableEmptyCell);
    initTable(tableUnsupColumn);
    initTable(tableEmptyCellBig);
    initTable(tableUnsupColumnAndEmpty);

    // Insert an empty cell
    *(tableEmptyCell.getValueAt(1, "col_float")) = XFloat_t();
    // Add a extra, empty row
    tableEmptyCell.append();

    // Add many more columns
    tableEmptyCellBig.addColumn("col_float_B", "float");
    tableEmptyCellBig.addColumn("col_int_B", "int");
    tableEmptyCellBig.addColumn("col_uint_B", "unsigned int");
    tableEmptyCellBig.addColumn("col_str_B", "string");
    // Add a extra, empty row
    tableEmptyCellBig.append();

    // And a complex column type
    tableUnsupColumn.addColumn("col_unk_A", "table");
    tableUnsupColumn.addColumn("col_unk_B", "table");
    tableUnsupColumn.addColumn("col_unk_C", "table");
    tableUnsupColumn.addColumn("col_unk_D", "table");
    tableUnsupColumn.addColumn("col_unk_E", "table");
    tableUnsupColumn.addColumn("col_unk_F", "table");

    // tableUnknownColumn.addColumn("col_unk3", "table");
    tableUnsupColumnAndEmpty.addColumn("col_unk_A", "table");
    tableUnsupColumnAndEmpty.append();

  }

  //---


};


BOOST_AUTO_TEST_SUITE(XRuleTestSuite)

BOOST_FIXTURE_TEST_CASE(SimpleNumericTest, RuleTestSetup)
{
  rules::FiniteNumber<XUInt_t> lUintRule;
  BOOST_CHECK(lUintRule.type() == typeid(XUInt_t));
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(lUintRule), "isFinite(x)");

  BOOST_CHECK_EQUAL(lUintRule(uintNaN), false);
  BOOST_CHECK_EQUAL(lUintRule(uintOK), true);

  BOOST_CHECK_THROW(lUintRule(floatNaN), XRuleTypeMismatch);
  BOOST_CHECK_THROW(lUintRule(floatOK), XRuleTypeMismatch);
  BOOST_CHECK_THROW(lUintRule(doubleNaN), XRuleTypeMismatch);
  BOOST_CHECK_THROW(lUintRule(doubleOK), XRuleTypeMismatch);
  BOOST_CHECK_THROW(lUintRule(strFull), XRuleTypeMismatch);
  BOOST_CHECK_THROW(lUintRule(vIntEmpty), XRuleTypeMismatch);
}


BOOST_FIXTURE_TEST_CASE(SimpleVectorTest, RuleTestSetup)
{
  rules::FiniteVector<XVectorUInt_t> lUintVecRule;
  BOOST_CHECK(lUintVecRule.type() == typeid(XVectorUInt_t));
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(lUintVecRule), "all(x,isFinite)");

  BOOST_CHECK_EQUAL(lUintVecRule(vUintEmpty), true);
  BOOST_CHECK_EQUAL(lUintVecRule(vUintNaN), false);
  BOOST_CHECK_EQUAL(lUintVecRule(vUintOK), true);

  BOOST_CHECK_THROW(lUintVecRule(floatNaN), XRuleTypeMismatch);
  BOOST_CHECK_THROW(lUintVecRule(floatOK), XRuleTypeMismatch);
  BOOST_CHECK_THROW(lUintVecRule(doubleNaN), XRuleTypeMismatch);
  BOOST_CHECK_THROW(lUintVecRule(doubleOK), XRuleTypeMismatch);
  BOOST_CHECK_THROW(lUintVecRule(strFull), XRuleTypeMismatch);
  BOOST_CHECK_THROW(lUintVecRule(vIntEmpty), XRuleTypeMismatch);

  rules::OfSize<XVectorUInt_t> lUintVecOsSizeRule(3);


  // vUintOK has size 3
  BOOST_CHECK_EQUAL(lUintVecOsSizeRule(vUintOK), true);
  // vUintNaN has size 4
  BOOST_CHECK_EQUAL(lUintVecOsSizeRule(vUintNaN), false);

}


BOOST_FIXTURE_TEST_CASE(NoneTest, RuleTestSetup)
{
  rules::None<XDouble_t> lNoDoubleRule;
  BOOST_CHECK(lNoDoubleRule.type() == typeid(XDouble_t));
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(lNoDoubleRule), "true");

  BOOST_CHECK_EQUAL(lNoDoubleRule(doubleNaN), true);
  BOOST_CHECK_EQUAL(lNoDoubleRule(doubleOK), true);

  BOOST_CHECK_THROW(lNoDoubleRule(floatNaN), XRuleTypeMismatch);
  BOOST_CHECK_THROW(lNoDoubleRule(floatOK), XRuleTypeMismatch);
  BOOST_CHECK_THROW(lNoDoubleRule(strFull), XRuleTypeMismatch);
  BOOST_CHECK_THROW(lNoDoubleRule(vIntEmpty), XRuleTypeMismatch);
  BOOST_CHECK_THROW(lNoDoubleRule(vDoubleEmpty), XRuleTypeMismatch);
  BOOST_CHECK_THROW(lNoDoubleRule(tableOK), XRuleTypeMismatch);
}


BOOST_FIXTURE_TEST_CASE(IsAmongTest, RuleTestSetup)
{
  rules::IsAmong lRule({"A", "B", "C"});
  BOOST_CHECK(lRule.type() == typeid(XString_t));
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(lRule), "x in {A, B, C}");  

  BOOST_CHECK_EQUAL(lRule(strOptA), true);
  BOOST_CHECK_EQUAL(lRule(strOptD), false);

}


// BOOST_AUTO_TEST_CASE_EXPECTED_FAILURES( NoEmptyCellsTest, 1)
BOOST_FIXTURE_TEST_CASE(NoEmptyCellsTest, RuleTestSetup)
{
  rules::NoEmptyCells lRule;
  BOOST_CHECK(lRule.type() == typeid(xdata::Table));
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(lRule), "!any(x,isEmpty)");  

  BOOST_CHECK_EQUAL(lRule(tableEmpty), true);
  BOOST_CHECK_EQUAL(lRule(tableEmptyCell), XMatch(false, "5 empty cell(s) found in 4 columns. 'col_float': [1,3], 'col_int': [3], 'col_str': [3], 'col_uint': [3]."));
  BOOST_CHECK_EQUAL(lRule(tableEmptyCellBig), XMatch(false, "20 empty cell(s) found in 8 columns (only first 5 columns shown). 'col_float': [3], 'col_float_B': [0-3], 'col_int': [3], 'col_int_B': [0-3], 'col_str': [3], ..."));
  BOOST_CHECK_EQUAL(lRule(tableOK), true);
  BOOST_CHECK_EQUAL(lRule(tableUnsupColumn), XMatch(false, "6 column(s) have unsupported types (only first 5 columns shown). 'col_unk_A': table, 'col_unk_B': table, 'col_unk_C': table, 'col_unk_D': table, 'col_unk_E': table, ..."));
  BOOST_CHECK_EQUAL(lRule(tableUnsupColumnAndEmpty), XMatch(false, "1 column(s) have unsupported types. 'col_unk_A': table. 4 empty cell(s) found in 4 columns. 'col_float': [3], 'col_int': [3], 'col_str': [3], 'col_uint': [3]."));

  BOOST_CHECK_THROW(lRule(floatNaN), XRuleTypeMismatch);
  BOOST_CHECK_THROW(lRule(floatOK), XRuleTypeMismatch);
  BOOST_CHECK_THROW(lRule(doubleNaN), XRuleTypeMismatch);
  BOOST_CHECK_THROW(lRule(doubleOK), XRuleTypeMismatch);
  BOOST_CHECK_THROW(lRule(strFull), XRuleTypeMismatch);
  BOOST_CHECK_THROW(lRule(vIntEmpty), XRuleTypeMismatch);
  BOOST_CHECK_THROW(lRule(vDoubleEmpty), XRuleTypeMismatch);
}


BOOST_FIXTURE_TEST_CASE(GreaterThanTest, RuleTestSetup)
{

  rules::GreaterThan<XUInt_t> lUintGT(100);
  
  BOOST_CHECK(lUintGT.type() == typeid(XUInt_t));
  BOOST_CHECK_EQUAL(lUintGT(XUInt_t(120)), true);
  BOOST_CHECK_EQUAL(lUintGT(XUInt_t(80)), false);

  rules::GreaterThan<XInt_t> lIntGT(-100);

  BOOST_CHECK(lIntGT.type() == typeid(XInt_t));
  BOOST_CHECK_EQUAL(lIntGT(XInt_t(-80)), true);
  BOOST_CHECK_EQUAL(lIntGT(XInt_t(-120)), false);

  rules::GreaterThan<XFloat_t> lFloatGT(-100);

  BOOST_CHECK(lFloatGT.type() == typeid(XFloat_t));
  BOOST_CHECK_EQUAL(lFloatGT(XFloat_t(-80)), true);
  BOOST_CHECK_EQUAL(lFloatGT(XFloat_t(-120)), false);

  rules::GreaterThan<XDouble_t> lDoubleGT(-100);

  BOOST_CHECK(lDoubleGT.type() == typeid(XDouble_t));
  BOOST_CHECK_EQUAL(lDoubleGT(XDouble_t(-80)), true);
  BOOST_CHECK_EQUAL(lDoubleGT(XDouble_t(-120)), false);
}


BOOST_FIXTURE_TEST_CASE(LesserThanTest, RuleTestSetup)
{

  rules::LesserThan<XUInt_t> lUintLT(100);
  
  BOOST_CHECK(lUintLT.type() == typeid(XUInt_t));
  BOOST_CHECK_EQUAL(lUintLT(XUInt_t(120)), false);
  BOOST_CHECK_EQUAL(lUintLT(XUInt_t(80)), true);

  rules::LesserThan<XInt_t> lIntLT(-100);

  BOOST_CHECK(lIntLT.type() == typeid(XInt_t));
  BOOST_CHECK_EQUAL(lIntLT(XInt_t(-80)), false);
  BOOST_CHECK_EQUAL(lIntLT(XInt_t(-120)), true);

  rules::LesserThan<XFloat_t> lFloatLT(-100);

  BOOST_CHECK(lFloatLT.type() == typeid(XFloat_t));
  BOOST_CHECK_EQUAL(lFloatLT(XFloat_t(-80)), false);
  BOOST_CHECK_EQUAL(lFloatLT(XFloat_t(-120)), true);

  rules::LesserThan<XDouble_t> lDoubleLT(-100);

  BOOST_CHECK(lDoubleLT.type() == typeid(XDouble_t));
  BOOST_CHECK_EQUAL(lDoubleLT(XDouble_t(-80)), false);
  BOOST_CHECK_EQUAL(lDoubleLT(XDouble_t(-120)), true);
}

BOOST_FIXTURE_TEST_CASE(InRangeTest, RuleTestSetup)
{

  rules::InRange<XUInt_t> lUintRng(100,200);
  
  BOOST_CHECK(lUintRng.type() == typeid(XUInt_t));
  BOOST_CHECK_EQUAL(lUintRng(XUInt_t(120)), true);
  BOOST_CHECK_EQUAL(lUintRng(XUInt_t(80)), false);

  rules::InRange<XInt_t> lIntRng(-200, -100);

  BOOST_CHECK(lIntRng.type() == typeid(XInt_t));
  BOOST_CHECK_EQUAL(lIntRng(XInt_t(-80)), false);
  BOOST_CHECK_EQUAL(lIntRng(XInt_t(-120)), true);

  rules::InRange<XFloat_t> lFloatRng(-200, -100);

  BOOST_CHECK(lFloatRng.type() == typeid(XFloat_t));
  BOOST_CHECK_EQUAL(lFloatRng(XFloat_t(-80)), false);
  BOOST_CHECK_EQUAL(lFloatRng(XFloat_t(-120)), true);

  rules::InRange<XDouble_t> lDoubleRng(-200, -100);

  BOOST_CHECK(lDoubleRng.type() == typeid(XDouble_t));
  BOOST_CHECK_EQUAL(lDoubleRng(XDouble_t(-80)), false);
  BOOST_CHECK_EQUAL(lDoubleRng(XDouble_t(-120)), true);
}


// ----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(OutOfRangeTest, RuleTestSetup)
{

  rules::OutOfRange<XUInt_t> lUintRng(100,200);
  
  BOOST_CHECK(lUintRng.type() == typeid(XUInt_t));
  BOOST_CHECK_EQUAL(lUintRng(XUInt_t(120)), false);
  BOOST_CHECK_EQUAL(lUintRng(XUInt_t(80)), true);

  rules::OutOfRange<XInt_t> lIntRng(-200, -100);

  BOOST_CHECK(lIntRng.type() == typeid(XInt_t));
  BOOST_CHECK_EQUAL(lIntRng(XInt_t(-80)), true);
  BOOST_CHECK_EQUAL(lIntRng(XInt_t(-120)), false);

  rules::OutOfRange<XFloat_t> lFloatRng(-200, -100);

  BOOST_CHECK(lFloatRng.type() == typeid(XFloat_t));
  BOOST_CHECK_EQUAL(lFloatRng(XFloat_t(-80)), true);
  BOOST_CHECK_EQUAL(lFloatRng(XFloat_t(-120)), false);

  rules::OutOfRange<XDouble_t> lDoubleRng(-200, -100);

  BOOST_CHECK(lDoubleRng.type() == typeid(XDouble_t));
  BOOST_CHECK_EQUAL(lDoubleRng(XDouble_t(-80)), true);
  BOOST_CHECK_EQUAL(lDoubleRng(XDouble_t(-120)), false);
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(AndTest, RuleTestSetup)
{
  rules::GreaterThan<XUInt_t> lUintGT100(100);
  rules::LesserThan<XUInt_t> lUintLT200(200);

  rules::And<XUInt_t> lAnd(lUintGT100,lUintLT200);
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(lAnd), "(x > 100 && x < 200)");

  BOOST_CHECK_EQUAL(lAnd(XUInt_t(120)), true);
  BOOST_CHECK_EQUAL(lAnd(XUInt_t(80)), false);
  BOOST_CHECK_EQUAL(lAnd(XUInt_t(250)), false);

  // TODO: Improve this test
  rules::And<XUInt_t> lAnd2(lAnd, lAnd);
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(lAnd2), "((x > 100 && x < 200) && (x > 100 && x < 200))");

}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(OrTest, RuleTestSetup)
{
  rules::GreaterThan<XUInt_t> lUintGT100(100);
  rules::LesserThan<XUInt_t> lUintLT200(200);

  rules::Or<XUInt_t> lOr(lUintGT100,lUintLT200);
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(lOr), "(x > 100 || x < 200)");

  BOOST_CHECK_EQUAL(lOr(XUInt_t(120)), true);
  BOOST_CHECK_EQUAL(lOr(XUInt_t(80)), true);
  BOOST_CHECK_EQUAL(lOr(XUInt_t(250)), true);

  // TODO: Improve this test
  rules::Or<XUInt_t> lOr2(lOr, lOr);
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(lOr2), "((x > 100 || x < 200) || (x > 100 || x < 200))");

}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(NotTest, RuleTestSetup)
{
  rules::GreaterThan<XUInt_t> lUintGT100(100);
  rules::Not<XUInt_t> lNotUintGT100(lUintGT100);

  BOOST_CHECK_EQUAL(lNotUintGT100(XUInt_t(120)), false);

}
// ----------------------------------------------------------------------------

 
// ----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(PathExistsTest, RuleTestSetup)
{
  // char* lEnvVar = std::getenv("SWATCH_ROOT");
  std::string lSwatchRoot = std::getenv("SWATCH_ROOT");
  boost::filesystem::path lBasePath(lSwatchRoot);
  lBasePath /= "test";

  std::cout << "base path " << lBasePath << std::endl;
  rules::PathExists lExistsPlain, lExistsInTest(lBasePath.string()), lShExistsInTest(lBasePath.string(), "sh");

  // Existsing paths
  for ( const auto& lPath : {lBasePath, lBasePath/"env.sh"} ) {
    BOOST_CHECK_EQUAL(lExistsPlain(XString_t(lPath.string())), true);
  }

  // Non-existing paths
  for ( const auto& lPath : { lBasePath/"nowhere/", lBasePath/"nothing"} ) {
    BOOST_CHECK_EQUAL(lExistsPlain(XString_t(lPath.string())), false);
  }
  
  // Existing in test
  for ( const auto& lPath : {"env.sh"} ) {
    BOOST_CHECK_EQUAL(lExistsInTest(XString_t(lPath)), true);
  }

  // Non existing in test
  for ( const auto& lPath : {"nowhere/", "nothing"} ) {
    BOOST_CHECK_EQUAL(lExistsInTest(XString_t(lPath)), false);
  }

  // Existing in test with extension sh
  for ( const auto& lPath : {"env", "envHw"} ) {
    BOOST_CHECK_EQUAL(lShExistsInTest(XString_t(lPath)), true);
  }
}
// ----------------------------------------------------------------------------


BOOST_AUTO_TEST_SUITE_END() // XRuleTestSuite
// ----------------------------------------------------------------------------

} /* namespace test */
} /* namespace core */
} /* namespace swatch */

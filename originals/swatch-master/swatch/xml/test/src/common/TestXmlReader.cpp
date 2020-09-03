// Boost Unit Test includes
#include <boost/test/unit_test.hpp>

#include <algorithm>
#include <string>

// SWATCH headers
#include "swatch/xml/XmlReader.hpp"

// external headers
#include "pugixml.hpp"

namespace swatch {
namespace xml {
namespace test {

struct TestXmlReaderSetup {

  TestXmlReaderSetup() :
    mainConfigStr(""),
    invalidMainConfigStr(""),
    subConfigStr(""),
    invalidSubConfigStr(""),
    mergedConfigStr(""),
    mainConfig(),
    invalidMainConfig(),
    subConfig(),
    invalidSubConfig()
  {
    mainConfigStr = "<db>"
                    "<key id=\"MyDummyKey\">"
                    "<load module=\"file://xml/test/etc/swatch/test/sub1.xml\" />"
                    "</key>"
                    "</db>";
    invalidMainConfigStr = "<db>"
                           "<key id=\"MyDummyKey\">"
                           "<load module=\"file://xml/test/etc/swatch/test/sub1.xml\" />"
                           "<context id=\"system.processors\">"
                           "<param cmd=\"resetBoard\" id=\"clockSource\" type=\"string\">external</param>"
                           "</context>"
                           "</key>"
                           "</db>";
    subConfigStr = "<run-settings id=\"system\">"
                   "<context id=\"processor1\">"
                   "<param id=\"aParam\" type=\"string\">aValue</param>"
                   "<state id=\"Halted\">"
                   "<mon-obj id=\"ports.Rx00\" status=\"non-critical\" />"
                   "</state>"
                   "<mask id=\"ports.Rx00\" />"
                   "</context>"
                   "<disable id=\"brokenProcessor\" />"
                   "</run-settings>";
    invalidSubConfigStr = "<run-settings>"
                          "<load module=\"file://xml/test/etc/swatch/test/sub1.xml\" />"
                          "</run-settings>";
    // main config + sub config
    mergedConfigStr = "<db>"
                      "<key id=\"MyDummyKey\">"
                      "<context id=\"system.processor1\">"
                      "<state id=\"Halted\">"
                      "<mon-obj id=\"ports.Rx00\" status=\"non-critical\" />"
                      "</state>"
                      "<mask id=\"ports.Rx00\"/>"
                      "</context>"
                      "<disable id=\"system.brokenProcessor\" />"
                      "</key>"
                      "</db>";

    // make sure we the strings are valid configs
    BOOST_REQUIRE_EQUAL(mainConfig.load(mainConfigStr.c_str()), true);
    BOOST_REQUIRE_EQUAL(invalidMainConfig.load(invalidMainConfigStr.c_str()), true);
    BOOST_REQUIRE_EQUAL(subConfig.load(subConfigStr.c_str()), true);
    BOOST_REQUIRE_EQUAL(invalidSubConfig.load(invalidSubConfigStr.c_str()), true);
  }

  std::string mainConfigStr, invalidMainConfigStr, subConfigStr, invalidSubConfigStr;
  std::string mergedConfigStr;

  pugi::xml_document mainConfig, invalidMainConfig, subConfig, invalidSubConfig;
};

BOOST_AUTO_TEST_SUITE( TestXmlReader )

BOOST_FIXTURE_TEST_CASE ( VerifyMainConfig, TestXmlReaderSetup )
{
  XmlReader lReader;
  std::string lErrorMsg("");
  BOOST_REQUIRE_EQUAL(lReader.checkMainConfig(mainConfig, lErrorMsg), true);
  BOOST_REQUIRE_EQUAL(lErrorMsg, "");
  BOOST_REQUIRE_EQUAL(lReader.checkMainConfig(invalidMainConfig, lErrorMsg), false);
  BOOST_REQUIRE_NE(lErrorMsg, "");
}

BOOST_FIXTURE_TEST_CASE ( VerifySubConfig, TestXmlReaderSetup )
{
  XmlReader lReader;
  std::string lErrorMsg("");
  std::cout << "About to check valid config module" << std::endl;
  BOOST_REQUIRE_EQUAL(lReader.checkSubConfig(subConfig, lErrorMsg), true);
  BOOST_REQUIRE_EQUAL(lErrorMsg, "");
  std::cout << "About to check invalid config module" << std::endl;
  BOOST_REQUIRE_EQUAL(lReader.checkSubConfig(invalidSubConfig, lErrorMsg), false);
  BOOST_REQUIRE_NE(lErrorMsg, "");
}

BOOST_FIXTURE_TEST_CASE ( TestToString, TestXmlReaderSetup )
{
  XmlReader lReader;
  std::string lTestString(lReader.docToString(subConfig));
  // trim all tabs and new lines (mSubConfigStr is a one-liner without any tabs or new lines)
  lTestString.erase(std::remove(lTestString.begin(), lTestString.end(), '\t'), lTestString.end());
  lTestString.erase(std::remove(lTestString.begin(), lTestString.end(), '\n'), lTestString.end());
  // but pugi also adds '<?xml version="1.0"?>' at the beginning
  std::string lTestAgainst("<?xml version=\"1.0\"?>" + subConfigStr);
  // pugi also corrects missing spaces, e.g. <mask id=\"ports.Rx00\"/> vs <mask id=\"ports.Rx00\" />
  // so keep that in mind when comparing
  BOOST_REQUIRE_EQUAL(lTestString, lTestAgainst);
}

BOOST_AUTO_TEST_CASE( TestReadXmlConfig )
{
  std::string lTestFile("xml/test/etc/swatch/test/mainConfig.xml");
  XmlReader lReader;
  pugi::xml_document lMergedDoc;
  lMergedDoc.load(lReader.readXmlConfig(lTestFile).c_str());

  // the file itself will pass the check
  pugi::xml_document lMainConfig;
  lMainConfig.load_file(lTestFile.c_str());
  std::string lErrorMsg("");
  BOOST_REQUIRE_EQUAL(lReader.checkMainConfig(lMainConfig, lErrorMsg), true);
  // but the merged config, where <load> has been replaced with contexts and 'disable' tag, will fail
  BOOST_REQUIRE_EQUAL(lReader.checkMainConfig(lMergedDoc, lErrorMsg), false);
  // the merged config should now have 2 contexts (merged from 3) and one <disable> tag
  pugi::xml_node lKey(lMergedDoc.child("db").find_child_by_attribute("key", "id", "MyDummyKey"));

  int lResult = std::distance(lKey.children("context").begin(), lKey.children("context").end());
  BOOST_REQUIRE_EQUAL(lResult, 2);
  lResult = std::distance(lKey.children("disable").begin(), lKey.children("disable").end());
  BOOST_REQUIRE_EQUAL(lResult, 1);

  // added to describe the 'missing state' bug
  pugi::xml_node lProcessors = lKey.find_child_by_attribute("context", "id", "system.processor1");
  pugi::xml_node lState = lProcessors.find_child_by_attribute("state", "id", "Halted");
  BOOST_REQUIRE(lState);

  lProcessors = lKey.find_child_by_attribute("context", "id", "system.processors");
  lState = lProcessors.find_child_by_attribute("state", "id", "Halted");
  BOOST_REQUIRE(lState);

}

BOOST_AUTO_TEST_SUITE_END() // TestXmlReader

}//ns: test
}//ns: xml
} //ns: swatch

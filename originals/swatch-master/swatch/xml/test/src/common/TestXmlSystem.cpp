// Boost Unit Test includes
#include <boost/test/unit_test.hpp>

#include <algorithm>
#include <string>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

// SWATCH headers
#include "swatch/core/utilities.hpp"
#include "swatch/xml/XmlSystem.hpp"

// external headers
#include "pugixml.hpp"

namespace swatch {
namespace xml {
namespace system {
namespace test {

struct TestXmlSystemSetup {

  TestXmlSystemSetup() :
    crateXmlStr(""),
    proc1XmlStr(""),
    proc2XmlStr(""),
    faultyProcStr(""),
    daqTTCXmlStr(""),
    linkXmlStr1(""),
    linkXmlStr2(""),
    fedXmlStr(""),
    systemXmlStr(""),
    systemXml()
  {
    crateXmlStr = ""
                  "  <crate id=\"S2D11-29\">"
                  "    <location>Point5, S2D11-29</location>"
                  "    <description>Calorimeter trigger layer 2 main processor crate</description>"
                  "  </crate>";
    proc1XmlStr = ""
                  "  <processor id=\"MP8\">"
                  "    <creator>calol2::MainProcessor</creator>"
                  "    <hw-type>MP7-XE</hw-type>"
                  "    <role>MainProcessor</role>"
                  "    <uri>chtcp-2.0://ctrl-calol2:10203?target=amc-s2d11-29-09:50001</uri>"
                  "    <address-table>file:///opt/cactus/etc/mp7/addrtab/xe_v2_0_0/mp7xe_infra.xml</address-table>"
                  "    <crate>S2D11-29</crate>"
                  "    <slot>9</slot>"
                  "    <rx-port name=\"MyCoolRx[0:10]\" alias=\"useful rx [00:10] alias!\" pid=\"[0:10]\"/>"
                  "    <rx-port name=\"MyOtherRx[60:70:2]\" pid=\"[60:70:2]\"/>"
                  "    <tx-port name=\"MyTx[60:63]\" alias=\"snappy tx [60:63] alias!\" pid=\"[60:63]\"/>"
                  "    <tx-port name=\"MyOtherTx[63:66]\" pid=\"[63:66]\"/>"
                  "  </processor>";
    proc2XmlStr = ""
                  "  <processor id=\"DEMUX\">"
                  "    <creator>calol2::Demux</creator>"
                  "    <hw-type>MP7-XE</hw-type>"
                  "    <role>Demux</role>"
                  "    <uri>chtcp-2.0://ctrl-calol2:10203?target=amc-s2d11-29-12:50001</uri>"
                  "    <address-table>file:///opt/cactus/etc/mp7/addrtab/xe_v2_0_0/mp7xe_infra.xml</address-table>"
                  "    <crate>S2D11-29</crate>"
                  "    <slot>12</slot>"
                  "    <rx-port name=\"Rx[0:72]\" pid=\"[0:72]\"/>"
                  "    <rx-port name=\"Rx[72:99]\" pid=\"[72:99]\"/>"
                  "    <tx-port name=\"Tx[00:07]\" pid=\"[00:07]\"/>"
                  "    <tx-port name=\"Tx[07:10]\" pid=\"[07:10]\"/>"
                  "    </processor>";
    faultyProcStr = ""
                    "  <processor id=\"FaultyProcessor\">"
                    "    <creator>calol2::MainProcessor</creator>"
                    "    <hw-type>MP7-XE</hw-type>"
                    "    <role>Defunct</role>"
                    "    <uri>chtcp-2.0://ctrl-calol2:10203?target=amc-s2d11-29-12:50001</uri>"
                    "    <address-table>file:///opt/cactus/etc/mp7/addrtab/xe_v2_0_0/mp7xe_infra.xml</address-table>"
                    "    <crate>S2D11-29</crate>"
                    "    <slot>12</slot>"
                    "    <rx-port name=\"Rx[73:76]\" pid=\"[73:76]\"/>"
                    "    <tx-port name=\"Tx[04:11]\" pid=\"[04:11]\"/>"
                    "    </processor>";
    daqTTCXmlStr = ""
                   "  <daqttc-mgr id=\"AMC13\">                                                   "
                   "    <creator>swatch::amc13::AMC13Manager</creator>                            "
                   "    <role>daqttc</role>                                                       "
                   "    <crate>S2D11-29</crate>                                                   "
                   "    <slot>13</slot>                                                           "
                   "    <uri id=\"t2\">chtcp-2.0://ctrl-calol2:10203?target=amc-s2d11-29-13-t2:50001</uri>  "
                   "    <uri id=\"t1\">chtcp-2.0://ctrl-calol2:10203?target=amc-s2d11-29-13-t1:50001</uri>  "
                   "    <address-table id=\"t1\">file:///opt/cactus/etc/amc13/AMC13XG_T1.xml</address-table>"
                   "    <address-table id=\"t2\">file:///opt/cactus/etc/amc13/AMC13XG_T2.xml</address-table>"
                   "    <fed-id>1360</fed-id>                                                     "
                   "  </daqttc-mgr>                                                               ";
    linkXmlStr1 = ""
                  "  <link id=\"MP8_Tx_[60:63]\">                                                "
                  "    <alias>From MP8 tx [60:63]</alias>                                        "
                  "    <from>MP8</from>                                                          "
                  "    <tx-port>MyTx[60:63]</tx-port>                                            "
                  "    <to>DEMUX</to>                                                            "
                  "    <rx-port>Rx[39:57:6]</rx-port>                                            "
                  "  </link>                                                                     ";
    linkXmlStr2 = ""
                  "  <link id=\"MP8_Tx_[63:66]\">                                                "
                  "    <from>MP8</from>                                                          "
                  "    <to>DEMUX</to>                                                            "
                  "    <tx-port>MyOtherTx[63:66]</tx-port>                                              "
                  "    <rx-port>Rx[57:72:6]</rx-port>                                            "
                  "  </link>                                                                     ";
    fedXmlStr = ""
                "  <connected-fed id=\"1356\">                                                 "
                "    <port id=\"MP8.inputPorts.Rx[00:05]\"/>                                   "
                "    <port id=\"MP8.inputPorts.Rx[60:72:2]\"/>                                 "
                "    <port id=\"FaultyProcessor.inputPorts.Rx[73:76]\" />                      "
                "  </connected-fed>                                                            "
                "                                                                              ";
    systemXmlStr = ""
                   "<system id=\"calol2\">"
                   "  <creator>calol2::System</creator>"
                   "  <excluded-boards>"
                   "    <exclude id=\"FaultyProcessor\" />"
                   "  </excluded-boards>"
                   "  <crates>" + //
                   crateXmlStr + //
                   "  </crates>"
                   "  <processors>" + //
                   proc1XmlStr + proc2XmlStr + //
                   faultyProcStr + //
                   "  </processors>"
                   "  <daqttc-mgrs>" + //
                   daqTTCXmlStr + //
                   "  </daqttc-mgrs>"
                   "  <links>" + //
                   linkXmlStr1 + linkXmlStr2 + //
                   "  </links>"
                   "  <connected-feds>" + //
                   fedXmlStr + //
                   "  </connected-feds>"
                   "</system>";
    // make sure we the strings are valid configs
    assert(crateXml.load(crateXmlStr.c_str()) == true);
    assert(proc1Xml.load(proc1XmlStr.c_str()) == true);
    assert(proc2Xml.load(proc2XmlStr.c_str()) == true);
    assert(daqTTCXml.load(daqTTCXmlStr.c_str()) == true);
    assert(linkXml1.load(linkXmlStr1.c_str()) == true);
    assert(linkXml2.load(linkXmlStr2.c_str()) == true);
    assert(fedXml.load(fedXmlStr.c_str()) == true);
    assert(systemXml.load(systemXmlStr.c_str()) == true);
  }

  std::string crateXmlStr, proc1XmlStr, proc2XmlStr, faultyProcStr;
  std::string daqTTCXmlStr, linkXmlStr1, linkXmlStr2, fedXmlStr;
  std::string systemXmlStr;

  pugi::xml_document crateXml, proc1Xml, proc2Xml;
  pugi::xml_document daqTTCXml, linkXml1, linkXml2, fedXml;
  pugi::xml_document systemXml;
};

BOOST_AUTO_TEST_SUITE( TestXmlSystem )

BOOST_FIXTURE_TEST_CASE ( TestInvalidSystem, TestXmlSystemSetup )
{
  //TODO: set logger to ignore ERRORS
  pugi::xml_document lInvalid;
  lInvalid.load("<system></system>");
  std::string lErrorMsg("");
  BOOST_CHECK_EQUAL(detail::validateSystemXml(lInvalid, lErrorMsg), false);
  BOOST_CHECK_THROW(xmlToSystemStub(lInvalid), InvalidSystemDescription);

  // test for tag duplication
  const std::vector<std::string> lTags = { "creator", "crates", "processors", "daqttc-mgrs", "links", "connected-feds",
                                           "excluded-boards"
                                         };
  BOOST_FOREACH(std::string lTag, lTags) {
    lInvalid.reset();
    lInvalid.load(systemXmlStr.c_str());
    BOOST_REQUIRE_EQUAL(detail::validateSystemXml(lInvalid, lErrorMsg), true);
    pugi::xml_node lSystem = lInvalid.child("system");
    lInvalid.child("system").append_copy(lSystem.child(lTag.c_str()));
    BOOST_CHECK_EQUAL(detail::validateSystemXml(lInvalid, lErrorMsg), false);
  }

  // test for invalid tags within collection blocks
  const std::vector<std::string> lCollectionTags = { "crates", "processors", "daqttc-mgrs", "links", "connected-feds",
                                                     "excluded-boards"
                                                   };
  const std::vector<std::string> lSTags = { "excluded-boards", "crates", "processors", "daqttc-mgrs", "links",
                                            "connected-feds",
                                          };
  unsigned int lIndex(0);
  BOOST_FOREACH(std::string lTag, lCollectionTags) {
    lInvalid.reset();
    lInvalid.load(systemXmlStr.c_str());
    BOOST_REQUIRE_EQUAL(detail::validateSystemXml(lInvalid, lErrorMsg), true);
    std::string lSTag = lSTags.at(lIndex);
    pugi::xml_node lSystem = lInvalid.child("system");
    lInvalid.child("system").child(lTag.c_str()).append_copy( lSystem.child(lSTag.c_str()).first_child() );
//    detail::XmlToStringWriter lWriter;
//    lInvalid.save(lWriter);
//    std::cerr << lWriter.mResult << std::endl;
    BOOST_CHECK_EQUAL(detail::validateSystemXml(lInvalid, lErrorMsg), false);
    ++lIndex;
  }
}

BOOST_AUTO_TEST_CASE ( TestMinimalSystem )
{
  pugi::xml_document lDoc;
  lDoc.load("<system id=\"mySystem\"><creator>MySystemClass</creator></system>");

  std::string lErrorMsg;
  BOOST_CHECK(detail::validateSystemXml(lDoc, lErrorMsg));

  swatch::system::SystemStub lStub = xmlToSystemStub(lDoc);
  BOOST_CHECK_EQUAL(lStub.id, "mySystem");
  BOOST_CHECK_EQUAL(lStub.alias, "");
  BOOST_CHECK_EQUAL(lStub.loggerName, "mySystem");
  BOOST_CHECK_EQUAL(lStub.creator, "MySystemClass");
  BOOST_CHECK_EQUAL(lStub.excludedBoards.size(), size_t(0));
  BOOST_CHECK_EQUAL(lStub.crates.size(), size_t(0));
  BOOST_CHECK_EQUAL(lStub.processors.size(), size_t(0));
  BOOST_CHECK_EQUAL(lStub.daqttcs.size(), size_t(0));
  BOOST_CHECK_EQUAL(lStub.links.size(), size_t(0));
  BOOST_CHECK_EQUAL(lStub.connectedFEDs.size(), size_t(0));

  lDoc.child("system").append_child("alias").append_child(pugi::node_pcdata).set_value("An alias!");

  lStub = xmlToSystemStub(lDoc);
  BOOST_CHECK_EQUAL(lStub.id, "mySystem");
  BOOST_CHECK_EQUAL(lStub.alias, "An alias!");

  lDoc.child("system").append_child("alias").append_child(pugi::node_pcdata).set_value("One alias too many!");
  BOOST_CHECK_THROW(xmlToSystemStub(lDoc), InvalidSystemDescription);
}

BOOST_FIXTURE_TEST_CASE ( TestPopulatedSystem, TestXmlSystemSetup )
{
  std::string lErrorMsg("");
  BOOST_REQUIRE_EQUAL(detail::validateSystemXml(systemXml, lErrorMsg), true);
  swatch::system::SystemStub lStub = xmlToSystemStub(systemXml);
  BOOST_CHECK_EQUAL(lStub.id, "calol2");
  BOOST_CHECK_EQUAL(lStub.loggerName, "calol2");
  BOOST_CHECK_EQUAL(lStub.creator, "calol2::System");

  for (unsigned int i = 0; i < lStub.processors.size(); ++i) {
    BOOST_CHECK_EQUAL(lStub.processors.at(i).loggerName, lStub.id + "." + lStub.processors.at(i).id);
  }
  for (unsigned int i = 0; i < lStub.daqttcs.size(); ++i) {
    BOOST_CHECK_EQUAL(lStub.daqttcs.at(i).loggerName, lStub.id + "." + lStub.daqttcs.at(i).id);
  }

  BOOST_CHECK_EQUAL(lStub.processors.size(), size_t(3));
  BOOST_CHECK_EQUAL(lStub.daqttcs.size(), size_t(1));
  BOOST_CHECK_EQUAL(lStub.crates.size(), size_t(1));
  BOOST_CHECK_EQUAL(lStub.links.size(), size_t(6));

  swatch::system::SystemStub::FEDInputPortsMap_t lFedMap = lStub.connectedFEDs;

  BOOST_REQUIRE_EQUAL(lFedMap.count(1356), size_t(1));
  BOOST_CHECK_EQUAL(lFedMap[1356].size(), size_t(5 + 6));

  BOOST_CHECK_EQUAL(lStub.excludedBoards.size(), size_t(1));
  BOOST_CHECK_EQUAL(lStub.excludedBoards.at(0), "FaultyProcessor");
}

// ---------------------------------------------------------------------
//    CHILD TAG: CRATE
// ---------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE (TestCrate, TestXmlSystemSetup )
{
  pugi::xml_node lNode = crateXml.child("crate");
  swatch::system::CrateStub lStub = detail::xmlToCrateStub(lNode);

  BOOST_CHECK_EQUAL(lStub.id, "S2D11-29");
  BOOST_CHECK_EQUAL(lStub.alias, "");
  BOOST_CHECK_EQUAL(lStub.location, "Point5, S2D11-29");
  BOOST_CHECK_EQUAL(lStub.description, "Calorimeter trigger layer 2 main processor crate");
}

BOOST_FIXTURE_TEST_CASE (TestCrateWithAlias, TestXmlSystemSetup )
{
  pugi::xml_document lDoc;
  pugi::xml_node lCrateNode = lDoc.append_copy(crateXml.child("crate"));
  lCrateNode.append_child("alias").append_child(pugi::node_pcdata).set_value("A useful crate alias!");

  swatch::system::CrateStub lStub = detail::xmlToCrateStub(lCrateNode);

  BOOST_CHECK_EQUAL(lStub.id, "S2D11-29");
  BOOST_CHECK_EQUAL(lStub.alias, "A useful crate alias!");
  BOOST_CHECK_EQUAL(lStub.location, "Point5, S2D11-29");
  BOOST_CHECK_EQUAL(lStub.description, "Calorimeter trigger layer 2 main processor crate");

  lCrateNode.append_child("alias").append_child(pugi::node_pcdata).set_value("One alias too many!");
  BOOST_CHECK_THROW(detail::xmlToCrateStub(lCrateNode), InvalidSystemDescription);
}

BOOST_FIXTURE_TEST_CASE ( TestInvalidCrateXml_MissingId, TestXmlSystemSetup)
{
  std::string lErrorMsg("");
// Check behaviour when 'id' attribute is not specified
  pugi::xml_document lDoc;
  pugi::xml_node lCrateNode = lDoc.append_copy(crateXml.child("crate"));
  BOOST_REQUIRE(detail::validateCrateXml(lCrateNode, lErrorMsg));
  BOOST_REQUIRE_NO_THROW(detail::xmlToCrateStub(lCrateNode));
  lCrateNode.remove_attribute("id");
  BOOST_CHECK(!detail::validateCrateXml(lCrateNode, lErrorMsg));
  BOOST_CHECK_THROW(detail::xmlToCrateStub(lCrateNode), InvalidSystemDescription);
}

BOOST_FIXTURE_TEST_CASE (TestInvalidCrateXml_MissingChildTag, TestXmlSystemSetup)
{
  std::string lErrorMsg("");
// Check behaviour when any of the mandatory child tags is missing
  std::vector<std::string> lRequiredTags = { "location", "description" };
  for (auto lIt = lRequiredTags.begin(); lIt != lRequiredTags.end(); lIt++) {
    BOOST_TEST_MESSAGE("crate tag '" + *lIt + "'");
    pugi::xml_document lDoc;
    pugi::xml_node lCrateNode = lDoc.append_copy(crateXml.child("crate"));

    // Check that original XML is valid before starting actual tests
    BOOST_REQUIRE(detail::validateCrateXml(lCrateNode, lErrorMsg));
    BOOST_REQUIRE_NO_THROW(detail::xmlToCrateStub(lCrateNode));

    // Remove this mandatory tag, and check that validation fails
    lCrateNode.remove_child(lIt->c_str());
    BOOST_CHECK(!detail::validateCrateXml(lCrateNode, lErrorMsg));
    BOOST_CHECK_THROW(detail::xmlToCrateStub(lCrateNode), InvalidSystemDescription);
  }
}

BOOST_FIXTURE_TEST_CASE (TestInvalidCrateXml_DuplicatedChildTag, TestXmlSystemSetup)
{
  std::string lErrorMsg("");
// Check behaviour when any of the mandatory child tags is duplicated
  std::vector<std::string> lRequiredTags = { "location", "description" };
  for (auto lIt = lRequiredTags.begin(); lIt != lRequiredTags.end(); lIt++) {
    // Check original valid XML is valid our test
    BOOST_TEST_MESSAGE("crate tag '" + *lIt + "'");
    pugi::xml_document lDoc;
    pugi::xml_node lCrateNode = lDoc.append_copy(crateXml.child("crate"));
    BOOST_REQUIRE(detail::validateCrateXml(lCrateNode, lErrorMsg));
    BOOST_REQUIRE_NO_THROW(detail::xmlToCrateStub(lCrateNode));

    // Add extra copy of this mandatory tag, and check that validation fails
    lCrateNode.append_child(lIt->c_str()).append_child(pugi::node_pcdata).set_value("42");
    BOOST_CHECK(!detail::validateCrateXml(lCrateNode, lErrorMsg));
    BOOST_CHECK_THROW(detail::xmlToCrateStub(lCrateNode), InvalidSystemDescription);
  }
}

BOOST_FIXTURE_TEST_CASE (TestInvalidCrateXml_UnknownChildTag, TestXmlSystemSetup)
{
  std::string lErrorMsg("");
// Check behaviour when there's an extra unknown tag
  pugi::xml_document lDoc;
  pugi::xml_node lCrateNode = lDoc.append_copy(crateXml.child("crate"));
  BOOST_REQUIRE(detail::validateCrateXml(lCrateNode, lErrorMsg));
  BOOST_REQUIRE_NO_THROW(detail::xmlToCrateStub(lCrateNode));

  lCrateNode.append_child("unknown_tag").append_child(pugi::node_pcdata).set_value("42");
  BOOST_CHECK(!detail::validateCrateXml(lCrateNode, lErrorMsg));
  BOOST_CHECK_THROW(detail::xmlToCrateStub(lCrateNode), InvalidSystemDescription);
}

// ---------------------------------------------------------------------
//    CHILD TAG: PROCESSOR
// ---------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE ( TestProcessor, TestXmlSystemSetup )
{
  pugi::xml_node lNode = proc1Xml.child("processor");
  swatch::processor::ProcessorStub lStub = detail::xmlToProcessorStub(lNode);

  BOOST_CHECK_EQUAL(lStub.id, "MP8");
  BOOST_CHECK_EQUAL(lStub.alias, "");
  BOOST_CHECK_EQUAL(lStub.creator, "calol2::MainProcessor");
  BOOST_CHECK_EQUAL(lStub.hwtype, "MP7-XE");
  BOOST_CHECK_EQUAL(lStub.role, "MainProcessor");
  BOOST_CHECK_EQUAL(lStub.crate, "S2D11-29");
  BOOST_CHECK_EQUAL(lStub.slot, uint32_t(9));
  BOOST_CHECK_EQUAL(lStub.uri, "chtcp-2.0://ctrl-calol2:10203?target=amc-s2d11-29-09:50001");
// system id + . + processor id is only valid when reading the whole system
// otherwise it is just the processor id
  BOOST_CHECK_EQUAL(lStub.loggerName, lStub.id);

  BOOST_CHECK_EQUAL(lStub.rxPorts.size(), size_t(15));
  for (size_t i=0; i<10; i++) {
    BOOST_CHECK_EQUAL(lStub.rxPorts.at(i).id, "MyCoolRx0" + boost::lexical_cast<std::string>(i));
    BOOST_CHECK_EQUAL(lStub.rxPorts.at(i).alias, "useful rx 0" + boost::lexical_cast<std::string>(i) + " alias!");
    BOOST_CHECK_EQUAL(lStub.rxPorts.at(i).number, i);
  }
  for (size_t i=0; i<5; i++) {
    const size_t lPortIdx = 60 + 2 * i;
    BOOST_CHECK_EQUAL(lStub.rxPorts.at(10+i).id, "MyOtherRx" + boost::lexical_cast<std::string>(lPortIdx));
    BOOST_CHECK_EQUAL(lStub.rxPorts.at(10+i).alias, "");
    BOOST_CHECK_EQUAL(lStub.rxPorts.at(10+i).number, lPortIdx);
  }
  BOOST_CHECK_EQUAL(lStub.txPorts.size(), size_t(6));
  for (size_t i=0; i<6; i++) {
    size_t lPortIdx = 60 + i;
    const std::string lIdPrefix = (i < 3) ? "MyTx" : "MyOtherTx";

    BOOST_CHECK_EQUAL(lStub.txPorts.at(i).id, lIdPrefix + boost::lexical_cast<std::string>(lPortIdx));
    if (i < 3)
      BOOST_CHECK_EQUAL(lStub.txPorts.at(i).alias, "snappy tx " + boost::lexical_cast<std::string>(lPortIdx) + " alias!");
    else
      BOOST_CHECK_EQUAL(lStub.txPorts.at(i).alias, "");
    BOOST_CHECK_EQUAL(lStub.txPorts.at(i).number, lPortIdx);
  }

  lNode = proc2Xml.child("processor");
  lStub = detail::xmlToProcessorStub(lNode);
  BOOST_CHECK_EQUAL(lStub.rxPorts.size(), size_t(99));
  BOOST_CHECK_EQUAL(lStub.txPorts.size(), size_t(10));
}

BOOST_FIXTURE_TEST_CASE (TestProcessorWithAlias, TestXmlSystemSetup )
{
  pugi::xml_document lDoc;
  pugi::xml_node lProcNode = lDoc.append_copy(proc1Xml.child("processor"));
  lProcNode.append_child("alias").append_child(pugi::node_pcdata).set_value("An insightful processor alias!");

  swatch::processor::ProcessorStub lStub = detail::xmlToProcessorStub(lProcNode);

  BOOST_CHECK_EQUAL(lStub.id, "MP8");
  BOOST_CHECK_EQUAL(lStub.alias, "An insightful processor alias!");

  lProcNode.append_child("alias").append_child(pugi::node_pcdata).set_value("One alias too many!");
  BOOST_CHECK_THROW(detail::xmlToProcessorStub(lProcNode), InvalidSystemDescription);
}

BOOST_FIXTURE_TEST_CASE ( TestValidProcessorXml_MissingOptionalChildTag, TestXmlSystemSetup)
{
  std::vector<std::string> lOptionalTags = { "rx-port", "tx-port" };
  std::string lErrorMsg("");
  for (auto lIt = lOptionalTags.begin(); lIt != lOptionalTags.end(); lIt++) {
    BOOST_TEST_MESSAGE("processor tag '" + *lIt + "'");
    pugi::xml_document lDoc;
    pugi::xml_node lProcessorNode = lDoc.append_copy(proc1Xml.child("processor"));

    // Check original XML is valid before starting actual tests
    BOOST_REQUIRE_MESSAGE(detail::validateProcessorXml(lProcessorNode, lErrorMsg),
                          "Original XML is invalid. Won't proceed with test");
    BOOST_REQUIRE_NO_THROW(detail::xmlToProcessorStub(lProcessorNode));

    // Remove all instances of this optional tag, and check that validation still passes
    while (lProcessorNode.child(lIt->c_str())) {
      lProcessorNode.remove_child(lIt->c_str());
    };
    BOOST_CHECK(detail::validateProcessorXml(lProcessorNode, lErrorMsg));
    BOOST_CHECK_NO_THROW(detail::xmlToProcessorStub(lProcessorNode));
  }
}

BOOST_FIXTURE_TEST_CASE ( TestInvalidProcessorXml_MissingId, TestXmlSystemSetup)
{
// Check behaviour when 'id' attribute is not specified
  pugi::xml_document lDoc;
  pugi::xml_node lProcessorNode = lDoc.append_copy(proc1Xml.child("processor"));
  std::string lErrorMsg("");
  BOOST_REQUIRE(detail::validateProcessorXml(lProcessorNode, lErrorMsg));
  BOOST_REQUIRE_NO_THROW(detail::xmlToProcessorStub(lProcessorNode));
  lProcessorNode.remove_attribute("id");
  BOOST_CHECK(!detail::validateProcessorXml(lProcessorNode, lErrorMsg));
  BOOST_CHECK_THROW(detail::xmlToProcessorStub(lProcessorNode), InvalidSystemDescription);
}

BOOST_FIXTURE_TEST_CASE ( TestInvalidProcessorXml_MissingPortAttributes, TestXmlSystemSetup)
{
  std::vector<std::string> lPortTags = { "rx-port", "tx-port" };
  std::vector<std::string> lRequiredAttributes = { "name", "pid" };

  for (auto lTagIt = lPortTags.begin(); lTagIt != lPortTags.end(); lTagIt++) {
    for (auto lAttrIt = lRequiredAttributes.begin(); lAttrIt != lRequiredAttributes.end(); lAttrIt++) {
      BOOST_TEST_MESSAGE("processor tag '" + *lTagIt + "', attribute '" + *lAttrIt + "'");
      pugi::xml_document lDoc;
      pugi::xml_node lProcessorNode = lDoc.append_copy(proc1Xml.child("processor"));

      // Check original XML is valid before starting actual tests
      std::string lErrorMsg;
      BOOST_REQUIRE_MESSAGE(detail::validateProcessorXml(lProcessorNode, lErrorMsg),
                            "Original XML is invalid. Won't proceed with test");
      BOOST_REQUIRE_NO_THROW(detail::xmlToProcessorStub(lProcessorNode));

      // Remove this mandatory attribute, and check that validation fails
      lProcessorNode.child(lTagIt->c_str()).remove_attribute(lAttrIt->c_str());
      BOOST_CHECK(!detail::validateProcessorXml(lProcessorNode, lErrorMsg));
      BOOST_CHECK_THROW(detail::xmlToProcessorStub(lProcessorNode), InvalidSystemDescription);
    }
  }
}

BOOST_FIXTURE_TEST_CASE ( TestInvalidProcessorXml_EmptyPortAttributes, TestXmlSystemSetup)
{
  std::vector<std::string> lPortTags = { "rx-port", "tx-port" };
  std::vector<std::string> lRequiredAttributes = { "name", "pid" };

  for (auto lTagIt = lPortTags.begin(); lTagIt != lPortTags.end(); lTagIt++) {
    for (auto lAttrIt = lRequiredAttributes.begin(); lAttrIt != lRequiredAttributes.end(); lAttrIt++) {
      BOOST_TEST_MESSAGE("port tag '" + *lTagIt + "', attribute '" + *lAttrIt + "'");
      pugi::xml_document lDoc;
      pugi::xml_node lProcessorNode = lDoc.append_copy(proc1Xml.child("processor"));

      // Check original XML is valid before starting actual tests
      std::string lErrorMsg;
      BOOST_REQUIRE_MESSAGE(detail::validateProcessorXml(lProcessorNode, lErrorMsg),
                            "Original XML is invalid. Won't proceed with test");
      BOOST_REQUIRE_NO_THROW(detail::xmlToProcessorStub(lProcessorNode));

      // Change attribute's value to empty string, and check that validation fails
      lProcessorNode.child(lTagIt->c_str()).attribute(lAttrIt->c_str()).set_value("");
      BOOST_CHECK(!detail::validateProcessorXml(lProcessorNode, lErrorMsg));
      BOOST_CHECK_THROW(detail::xmlToProcessorStub(lProcessorNode), InvalidSystemDescription);
    }
  }
}


BOOST_FIXTURE_TEST_CASE ( TestInvalidProcessorXml_PortMultiplicityMismatchAttributes, TestXmlSystemSetup)
{
  std::vector<std::string> lPortTags = { "rx-port", "tx-port" };
  std::vector<std::string> lRequiredAttributes = { "name", "pid" };

  for (auto lTagIt = lPortTags.begin(); lTagIt != lPortTags.end(); lTagIt++) {
    BOOST_TEST_MESSAGE("port tag '" + *lTagIt + "'");
    pugi::xml_document lDoc;
    pugi::xml_node lProcessorNode = lDoc.append_copy(proc1Xml.child("processor"));

    // Check original XML is valid before starting actual tests
    std::string lErrorMsg;
    BOOST_REQUIRE_MESSAGE(detail::validateProcessorXml(lProcessorNode, lErrorMsg),
                          "Original XML is invalid. Won't proceed with test");
    BOOST_REQUIRE_NO_THROW(detail::xmlToProcessorStub(lProcessorNode));

    // Change attribute's value to empty string, and check that validation fails
    lProcessorNode.child(lTagIt->c_str()).attribute("pid").set_value("[800:842]");
    BOOST_CHECK(!detail::validateProcessorXml(lProcessorNode, lErrorMsg));
    BOOST_CHECK_THROW(detail::xmlToProcessorStub(lProcessorNode), InvalidSystemDescription);
  }
}


BOOST_FIXTURE_TEST_CASE ( TestInvalidProcessorXml_DuplicatedChildTag, TestXmlSystemSetup)
{
// Check behaviour when any of the mandatory child tags is duplicated
  std::vector<std::string> lRequiredTags = { "creator", "hw-type", "role", "uri", "address-table", "crate", "slot" };
  std::string lErrorMsg("");
  for (auto lIt = lRequiredTags.begin(); lIt != lRequiredTags.end(); lIt++) {
    BOOST_TEST_MESSAGE("processor tag '" + *lIt + "'");
    pugi::xml_document lDoc;
    pugi::xml_node lProcessorNode = lDoc.append_copy(proc1Xml.child("processor"));

    // Check original XML is valid before starting actual tests
    BOOST_REQUIRE_MESSAGE(detail::validateProcessorXml(lProcessorNode, lErrorMsg),
                          "Original XML is invalid. Won't proceed with test");
    BOOST_REQUIRE_NO_THROW(detail::xmlToProcessorStub(lProcessorNode));

    // Add extra copy of this mandatory tag, and check that validation fails
    lProcessorNode.append_child(lIt->c_str()).append_child(pugi::node_pcdata).set_value("42");
    BOOST_CHECK(!detail::validateProcessorXml(lProcessorNode, lErrorMsg));
    BOOST_CHECK_THROW(detail::xmlToProcessorStub(lProcessorNode), InvalidSystemDescription);
  }
}

BOOST_FIXTURE_TEST_CASE ( TestInvalidProcessorXml_MissingChildTag, TestXmlSystemSetup)
{
  std::vector<std::string> lRequiredTags = { "creator", "hw-type", "role", "uri", "address-table", "crate", "slot" };
  std::string lErrorMsg("");
  for (auto lIt = lRequiredTags.begin(); lIt != lRequiredTags.end(); lIt++) {
    BOOST_TEST_MESSAGE("processor tag '" + *lIt + "'");
    pugi::xml_document lDoc;
    pugi::xml_node lProcessorNode = lDoc.append_copy(proc1Xml.child("processor"));

    // Check original XML is valid before starting actual tests
    BOOST_REQUIRE_MESSAGE(detail::validateProcessorXml(lProcessorNode, lErrorMsg),
                          "Original XML is invalid. Won't proceed with test");
    BOOST_REQUIRE_NO_THROW(detail::xmlToProcessorStub(lProcessorNode));

    // Remove this mandatory tag, and check that validation fails
    lProcessorNode.remove_child(lIt->c_str());
    BOOST_CHECK(!detail::validateProcessorXml(lProcessorNode, lErrorMsg));
    BOOST_CHECK_THROW(detail::xmlToProcessorStub(lProcessorNode), InvalidSystemDescription);
  }
}

BOOST_FIXTURE_TEST_CASE ( TestInvalidProcessorXml_InvalidChildTagContent, TestXmlSystemSetup)
{
  pugi::xml_document lDoc;
  pugi::xml_node lProcessorNode = lDoc.append_copy(proc1Xml.child("processor"));

// Check original XML is valid before starting actual test
  std::string lErrorMsg("");
  BOOST_REQUIRE(detail::validateProcessorXml(lProcessorNode, lErrorMsg));
  BOOST_REQUIRE_NO_THROW(detail::xmlToProcessorStub(lProcessorNode));

// Check behaviour when 'slot' is invalid string (not a number)
  lProcessorNode.child("slot").last_child().set_value("not a number");
  BOOST_CHECK(!detail::validateProcessorXml(lProcessorNode, lErrorMsg));
  BOOST_CHECK_THROW(detail::xmlToProcessorStub(lProcessorNode), InvalidSystemDescription);
}

BOOST_FIXTURE_TEST_CASE ( TestInvalidProcessorXml_UnknownChildTag, TestXmlSystemSetup)
{

  pugi::xml_document lDoc;
  pugi::xml_node lProcessorNode = lDoc.append_copy(proc1Xml.child("processor"));

// Check original XML is valid before starting actual test
  std::string lErrorMsg("");
  BOOST_REQUIRE(detail::validateProcessorXml(lProcessorNode, lErrorMsg));
  BOOST_REQUIRE_NO_THROW(detail::xmlToProcessorStub(lProcessorNode));

// Check behaviour when there's an extra unknown tag
  lProcessorNode.append_child("unknown_tag").append_child(pugi::node_pcdata).set_value("42");
  BOOST_CHECK(!detail::validateProcessorXml(lProcessorNode, lErrorMsg));
  BOOST_CHECK_THROW(detail::xmlToProcessorStub(lProcessorNode), InvalidSystemDescription);
}

// ---------------------------------------------------------------------
//    CHILD TAG: DAQTTC-MGR
// ---------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE ( TestDaqTTC, TestXmlSystemSetup )
{
  pugi::xml_node lNode = daqTTCXml.child("daqttc-mgr");
  swatch::dtm::DaqTTCStub lStub = detail::xmlToDaqTTCStub(lNode);

  BOOST_CHECK_EQUAL(lStub.id, "AMC13");
  BOOST_CHECK_EQUAL(lStub.role, "daqttc");
  BOOST_CHECK_EQUAL(lStub.crate, "S2D11-29");
  BOOST_CHECK_EQUAL(lStub.slot, uint32_t(13));
  BOOST_CHECK_EQUAL(lStub.creator, "swatch::amc13::AMC13Manager");
  BOOST_CHECK_EQUAL(lStub.fedId, uint16_t(1360));

  BOOST_CHECK_EQUAL(lStub.uriT1, "chtcp-2.0://ctrl-calol2:10203?target=amc-s2d11-29-13-t1:50001");
  BOOST_CHECK_EQUAL(lStub.uriT2, "chtcp-2.0://ctrl-calol2:10203?target=amc-s2d11-29-13-t2:50001");
  BOOST_CHECK_EQUAL(lStub.addressTableT1, "file:///opt/cactus/etc/amc13/AMC13XG_T1.xml");
  BOOST_CHECK_EQUAL(lStub.addressTableT2, "file:///opt/cactus/etc/amc13/AMC13XG_T2.xml");
// system id + . + processor id is only valid when reading the whole system
// otherwise it is just the processor id
  BOOST_CHECK_EQUAL(lStub.loggerName, lStub.id);
}

BOOST_FIXTURE_TEST_CASE (TestDaqTTCMgrWithAlias, TestXmlSystemSetup )
{
  pugi::xml_document lDoc;
  pugi::xml_node lDTMNode = lDoc.append_copy(daqTTCXml.child("daqttc-mgr"));
  lDTMNode.append_child("alias").append_child(pugi::node_pcdata).set_value("An interesting DTM alias!");

  swatch::dtm::DaqTTCStub lStub = detail::xmlToDaqTTCStub(lDTMNode);

  BOOST_CHECK_EQUAL(lStub.id, "AMC13");
  BOOST_CHECK_EQUAL(lStub.alias, "An interesting DTM alias!");

  lDTMNode.append_child("alias").append_child(pugi::node_pcdata).set_value("One alias too many!");
  BOOST_CHECK_THROW(detail::xmlToDaqTTCStub(lDTMNode), InvalidSystemDescription);
}

BOOST_FIXTURE_TEST_CASE ( TestInvalidDaqTTCXml_MissingId, TestXmlSystemSetup)
{
// Check behaviour when 'id' attribute is not specified
  pugi::xml_document lDoc;
  pugi::xml_node lDaqTtcNode = lDoc.append_copy(daqTTCXml.child("daqttc-mgr"));
  std::string lErrorMsg("");
  BOOST_REQUIRE(detail::validateDAQTTCXml(lDaqTtcNode, lErrorMsg));
  BOOST_REQUIRE_NO_THROW(detail::xmlToDaqTTCStub(lDaqTtcNode));
  lDaqTtcNode.remove_attribute("id");
  BOOST_CHECK(!detail::validateDAQTTCXml(lDaqTtcNode, lErrorMsg));
  BOOST_CHECK_THROW(detail::xmlToDaqTTCStub(lDaqTtcNode), InvalidSystemDescription);
}

BOOST_FIXTURE_TEST_CASE ( TestInvalidDaqTTCXml_DuplicatedChildTag, TestXmlSystemSetup)
{
// Check behaviour when any of the mandatory child tags is duplicated
  std::vector<std::string> lRequiredTags = { "creator", "role", "uri", "address-table", "crate", "slot", "fed-id" };
  std::string lErrorMsg("");
  for (auto lIt = lRequiredTags.begin(); lIt != lRequiredTags.end(); lIt++) {
    BOOST_TEST_MESSAGE("daqttc-mgr tag '" + *lIt + "'");
    pugi::xml_document lDoc;
    pugi::xml_node lDaqTtcNode = lDoc.append_copy(daqTTCXml.child("daqttc-mgr"));

    // Check original XML is valid before starting actual tests
    BOOST_REQUIRE_MESSAGE(detail::validateDAQTTCXml(lDaqTtcNode, lErrorMsg),
                          "Original XML is invalid. Won't proceed with test");
    BOOST_REQUIRE_NO_THROW(detail::xmlToDaqTTCStub(lDaqTtcNode));

    // Add extra copy of this mandatory tag, and check that validation fails
    lDaqTtcNode.append_child(lIt->c_str()).append_child(pugi::node_pcdata).set_value("42");
    BOOST_CHECK(!detail::validateDAQTTCXml(lDaqTtcNode, lErrorMsg));
    BOOST_CHECK_THROW(detail::xmlToDaqTTCStub(lDaqTtcNode), InvalidSystemDescription);
  }
}

BOOST_FIXTURE_TEST_CASE ( TestInvalidDaqTTCXml_MissingChildTag, TestXmlSystemSetup)
{
  std::vector<std::string> lRequiredTags = { "creator", "role", "uri", "address-table", "crate", "slot", "fed-id" };
  std::string lErrorMsg("");
  for (auto lIt = lRequiredTags.begin(); lIt != lRequiredTags.end(); lIt++) {
    BOOST_TEST_MESSAGE("daqttc-mgr tag '" + *lIt + "'");
    pugi::xml_document lDoc;
    pugi::xml_node lDaqTtcNode = lDoc.append_copy(daqTTCXml.child("daqttc-mgr"));

    // Check original XML is valid before starting actual tests
    BOOST_REQUIRE_MESSAGE(detail::validateDAQTTCXml(lDaqTtcNode, lErrorMsg),
                          "Original XML is invalid. Won't proceed with test");
    BOOST_REQUIRE_NO_THROW(detail::xmlToDaqTTCStub(lDaqTtcNode));

    // Remove this mandatory tag, and check that validation fails
    lDaqTtcNode.remove_child(lIt->c_str());
    BOOST_CHECK(!detail::validateDAQTTCXml(lDaqTtcNode, lErrorMsg));
    BOOST_CHECK_THROW(detail::xmlToDaqTTCStub(lDaqTtcNode), InvalidSystemDescription);
  }
}

BOOST_FIXTURE_TEST_CASE ( TestInvalidDaqTTCXml_InvalidChildTagContent, TestXmlSystemSetup)
{
  std::string lErrorMsg("");
// Check behaviour when 'fed-id' is invalid string (not a number)
  pugi::xml_document lDoc;
  pugi::xml_node lDaqTtcNode = lDoc.append_copy(daqTTCXml.child("daqttc-mgr"));
  lDaqTtcNode.child("fed-id").last_child().set_value("not a number");
  BOOST_CHECK(!detail::validateDAQTTCXml(lDaqTtcNode, lErrorMsg));
  BOOST_CHECK_THROW(detail::xmlToDaqTTCStub(lDaqTtcNode), InvalidSystemDescription);

// Check behaviour when 'slot' is invalid string (not a number)
  lDoc.reset();
  lDaqTtcNode = lDoc.append_copy(daqTTCXml.child("daqttc-mgr"));
  lDaqTtcNode.child("slot").last_child().set_value("not a number");
  BOOST_CHECK(!detail::validateDAQTTCXml(lDaqTtcNode, lErrorMsg));
  BOOST_CHECK_THROW(detail::xmlToDaqTTCStub(lDaqTtcNode), InvalidSystemDescription);
}

BOOST_FIXTURE_TEST_CASE ( TestInvalidDaqTTCXml_InvalidUriId, TestXmlSystemSetup)
{
  std::string lErrorMsg("");
  pugi::xml_document lDoc;
  pugi::xml_node lDaqTtcNode = lDoc.append_copy(daqTTCXml.child("daqttc-mgr"));

  // Check that XML is valid before modifying it for test
  BOOST_REQUIRE(detail::validateDAQTTCXml(lDaqTtcNode, lErrorMsg));
  BOOST_REQUIRE_NO_THROW(detail::xmlToDaqTTCStub(lDaqTtcNode));

  // Check behaviour when uri tag's "id" attribute has invalid value (neither 't1' or 't2')
  lDaqTtcNode.child("uri").attribute("id").set_value("invalid_id_value");
  BOOST_CHECK_EQUAL(detail::validateDAQTTCXml(lDaqTtcNode, lErrorMsg), false);
  BOOST_CHECK_THROW(detail::xmlToDaqTTCStub(lDaqTtcNode), InvalidSystemDescription);
}


BOOST_FIXTURE_TEST_CASE ( TestInvalidDaqTTCXml_InvalidAddressTableId, TestXmlSystemSetup)
{
  std::string lErrorMsg("");
  pugi::xml_document lDoc;
  pugi::xml_node lDaqTtcNode = lDoc.append_copy(daqTTCXml.child("daqttc-mgr"));

  // Check that XML is valid before modifying it for test
  BOOST_REQUIRE(detail::validateDAQTTCXml(lDaqTtcNode, lErrorMsg));
  BOOST_REQUIRE_NO_THROW(detail::xmlToDaqTTCStub(lDaqTtcNode));

  // Check behaviour when address-table tag's "id" attribute has invalid value (neither 't1' or 't2')
  lDaqTtcNode.child("address-table").attribute("id").set_value("invalid_id_value");
  BOOST_CHECK_EQUAL(detail::validateDAQTTCXml(lDaqTtcNode, lErrorMsg), false);
  BOOST_CHECK_THROW(detail::xmlToDaqTTCStub(lDaqTtcNode), InvalidSystemDescription);
}


BOOST_FIXTURE_TEST_CASE ( TestInvalidDaqTTCXml_UnknownChildTag, TestXmlSystemSetup)
{
  std::string lErrorMsg("");
// Check behaviour when there's an extra unknown tag
  pugi::xml_document lDoc;
  pugi::xml_node lDaqTtcNode = lDoc.append_copy(daqTTCXml.child("daqttc-mgr"));
  BOOST_REQUIRE(detail::validateDAQTTCXml(lDaqTtcNode, lErrorMsg));
  BOOST_REQUIRE_NO_THROW(detail::xmlToDaqTTCStub(lDaqTtcNode));
  lDaqTtcNode.append_child("unknown_tag").append_child(pugi::node_pcdata).set_value("42");
  BOOST_CHECK_EQUAL(detail::validateDAQTTCXml(lDaqTtcNode, lErrorMsg), false);
  BOOST_CHECK_THROW(detail::xmlToDaqTTCStub(lDaqTtcNode), InvalidSystemDescription);
}

// ---------------------------------------------------------------------
//    CHILD TAG: LINK
// ---------------------------------------------------------------------


BOOST_FIXTURE_TEST_CASE ( TestLinks, TestXmlSystemSetup )
{
  pugi::xml_node lNode = linkXml1.child("link");
  swatch::system::SystemStub::Links_t lLinks = detail::xmlToLinkStubs(lNode);

  BOOST_CHECK_EQUAL(lLinks.size(), size_t(3));

  for (size_t i = 0; i < lLinks.size(); i++) {
    BOOST_CHECK_EQUAL(lLinks.at(i).srcProcessor, "MP8");
    BOOST_CHECK_EQUAL(lLinks.at(i).dstProcessor, "DEMUX");
  }

  BOOST_CHECK_EQUAL(lLinks.at(0).alias, "From MP8 tx 60");
  BOOST_CHECK_EQUAL(lLinks.at(0).srcPort, "MyTx60");
  BOOST_CHECK_EQUAL(lLinks.at(0).dstPort, "Rx39");

  BOOST_CHECK_EQUAL(lLinks.at(1).alias, "From MP8 tx 61");
  BOOST_CHECK_EQUAL(lLinks.at(1).srcPort, "MyTx61");
  BOOST_CHECK_EQUAL(lLinks.at(1).dstPort, "Rx45");

  BOOST_CHECK_EQUAL(lLinks.at(2).alias, "From MP8 tx 62");
  BOOST_CHECK_EQUAL(lLinks.at(2).srcPort, "MyTx62");
  BOOST_CHECK_EQUAL(lLinks.at(2).dstPort, "Rx51");


  pugi::xml_node lNode2 = linkXml2.child("link");
  lLinks = detail::xmlToLinkStubs(lNode2);

  BOOST_CHECK_EQUAL(lLinks.size(), size_t(3));
  for (size_t i = 0; i < lLinks.size(); i++) {
    BOOST_CHECK_EQUAL(lLinks.at(i).alias, "");
    BOOST_CHECK_EQUAL(lLinks.at(i).srcProcessor, "MP8");
    BOOST_CHECK_EQUAL(lLinks.at(i).dstProcessor, "DEMUX");
  }

  BOOST_CHECK_EQUAL(lLinks.at(0).srcPort, "MyOtherTx63");
  BOOST_CHECK_EQUAL(lLinks.at(0).dstPort, "Rx57");

  BOOST_CHECK_EQUAL(lLinks.at(1).srcPort, "MyOtherTx64");
  BOOST_CHECK_EQUAL(lLinks.at(1).dstPort, "Rx63");

  BOOST_CHECK_EQUAL(lLinks.at(2).srcPort, "MyOtherTx65");
  BOOST_CHECK_EQUAL(lLinks.at(2).dstPort, "Rx69");
}

BOOST_FIXTURE_TEST_CASE ( TestInvalidLinkXml_MissingId, TestXmlSystemSetup)
{
// Check behaviour when 'id' attribute is not specified
  pugi::xml_document lDoc;
  pugi::xml_node lLinkNode = lDoc.append_copy(linkXml1.child("link"));
  std::string lErrorMsg("");
  BOOST_REQUIRE(detail::validateLinkXml(lLinkNode, lErrorMsg));
  BOOST_REQUIRE_NO_THROW(detail::xmlToLinkStubs(lLinkNode));
  lLinkNode.remove_attribute("id");
  BOOST_CHECK_EQUAL(detail::validateLinkXml(lLinkNode, lErrorMsg), false);
  BOOST_CHECK_THROW(detail::xmlToLinkStubs(lLinkNode), InvalidSystemDescription);
}

BOOST_FIXTURE_TEST_CASE ( TestInvalidLinkXml_DuplicatedChildTag, TestXmlSystemSetup)
{
// Check behaviour when any of the child tags is duplicated
  std::vector<std::string> lChildTags = { "alias", "from", "to", "tx-port", "rx-port" };
  std::string lErrorMsg("");
  for (auto lIt = lChildTags.begin(); lIt != lChildTags.end(); lIt++) {
    BOOST_TEST_MESSAGE("link tag '" + *lIt + "'");
    pugi::xml_document lDoc;
    pugi::xml_node lLinkNode = lDoc.append_copy(linkXml1.child("link"));

    // Check original XML is valid before starting actual tests
    BOOST_REQUIRE_MESSAGE(detail::validateLinkXml(lLinkNode, lErrorMsg),
                          "Original XML is invalid. Won't proceed with test");
    BOOST_REQUIRE_NO_THROW(detail::xmlToLinkStubs(lLinkNode));

    // Add extra copy of this tag, and check that validation fails
    lLinkNode.append_child(lIt->c_str()).append_child(pugi::node_pcdata).set_value("42");
    BOOST_CHECK(!detail::validateLinkXml(lLinkNode, lErrorMsg));
    BOOST_CHECK_THROW(detail::xmlToLinkStubs(lLinkNode), InvalidSystemDescription);
  }
}

BOOST_FIXTURE_TEST_CASE ( TestInvalidLinkXml_MissingChildTag, TestXmlSystemSetup)
{
  std::vector<std::string> lRequiredTags = { "from", "to", "tx-port", "rx-port" };
  std::string lErrorMsg("");
  for (auto lIt = lRequiredTags.begin(); lIt != lRequiredTags.end(); lIt++) {
    BOOST_TEST_MESSAGE("link tag '" + *lIt + "'");
    pugi::xml_document lDoc;
    pugi::xml_node lLinkNode = lDoc.append_copy(linkXml1.child("link"));

    // Check original XML is valid before starting actual tests
    BOOST_REQUIRE_MESSAGE(detail::validateLinkXml(lLinkNode, lErrorMsg),
                          "Original XML is invalid. Won't proceed with test");
    BOOST_REQUIRE_NO_THROW(detail::xmlToLinkStubs(lLinkNode));

    // Remove this mandatory tag, and check that validation fails
    lLinkNode.remove_child(lIt->c_str());
    BOOST_CHECK(!detail::validateLinkXml(lLinkNode, lErrorMsg));
    BOOST_CHECK_THROW(detail::xmlToLinkStubs(lLinkNode), InvalidSystemDescription);
  }
}

//BOOST_FIXTURE_TEST_CASE ( TestInvalidLinkXml_InvalidChildTagContent, TestXmlSystemSetup)
//{
//  std::string lErrorMsg("");
//  // Check behaviour when 'tx-port' is invalid string (mistake in slice syntax)
//  pugi::xml_document lDoc;
//  pugi::xml_node lLinkNode = lDoc.append_copy(mLinkXml1.child("link"));
//  lLinkNode.child("tx-port").last_child().set_value("invalid_slice_[42:3:]");
//  BOOST_CHECK(!detail::validateLinkXml(lLinkNode, lErrorMsg));
//  BOOST_CHECK_THROW(detail::xmlToLinkStubs(lLinkNode), InvalidSystemDescription);
//
//  // Check behaviour when 'rx-port' is invalid string (mistake in slice syntax)
//  lDoc.reset();
//  lLinkNode = lDoc.append_copy(mLinkXml1.child("link"));
//  lLinkNode.child("rx-port").last_child().set_value("invalid_slice_[42:3:]");
//  BOOST_CHECK(!detail::validateLinkXml(lLinkNode, lErrorMsg));
//  BOOST_CHECK_THROW(detail::xmlToLinkStubs(lLinkNode), InvalidSystemDescription);
//}

BOOST_FIXTURE_TEST_CASE ( TestInvalidLinkXml_UnknownChildTag, TestXmlSystemSetup)
{
  std::string lErrorMsg("");
// Check behaviour when there's an extra unknown tag
  pugi::xml_document lDoc;
  pugi::xml_node lLinkNode = lDoc.append_copy(linkXml1.child("link"));
  BOOST_REQUIRE(detail::validateLinkXml(lLinkNode, lErrorMsg));
  BOOST_REQUIRE_NO_THROW(detail::xmlToLinkStubs(lLinkNode));

  lLinkNode.append_child("unknown_tag").append_child(pugi::node_pcdata).set_value("42");
  BOOST_CHECK(!detail::validateLinkXml(lLinkNode, lErrorMsg));
  BOOST_CHECK_THROW(detail::xmlToLinkStubs(lLinkNode), InvalidSystemDescription);
}

// ---------------------------------------------------------------------
//    CHILD TAG: CONNECTED FEDS
// ---------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE ( TestConnectedFeds, TestXmlSystemSetup )
{
  pugi::xml_node lNode = fedXml.child("connected-fed");
  swatch::system::SystemStub::FEDInputPortsMap_t lFedMap;
  std::vector<std::string> lExcludedBoards;
  lExcludedBoards.push_back("FaultyProcessor");
  detail::xmlToConnectedFeds(lNode, lFedMap, lExcludedBoards);

  BOOST_CHECK_EQUAL(lFedMap.size(), size_t(1));

  BOOST_REQUIRE_EQUAL(lFedMap.count(1356), size_t(1));
  BOOST_CHECK_EQUAL(lFedMap[1356].size(), size_t(5 + 6));
  BOOST_CHECK_EQUAL(lFedMap[1356].at(0), "MP8.inputPorts.Rx00");
  BOOST_CHECK_EQUAL(lFedMap[1356].at(1), "MP8.inputPorts.Rx01");
  BOOST_CHECK_EQUAL(lFedMap[1356].at(2), "MP8.inputPorts.Rx02");
  BOOST_CHECK_EQUAL(lFedMap[1356].at(3), "MP8.inputPorts.Rx03");
  BOOST_CHECK_EQUAL(lFedMap[1356].at(4), "MP8.inputPorts.Rx04");
  BOOST_CHECK_EQUAL(lFedMap[1356].at(5), "MP8.inputPorts.Rx60");
  BOOST_CHECK_EQUAL(lFedMap[1356].at(6), "MP8.inputPorts.Rx62");
  BOOST_CHECK_EQUAL(lFedMap[1356].at(7), "MP8.inputPorts.Rx64");
  BOOST_CHECK_EQUAL(lFedMap[1356].at(8), "MP8.inputPorts.Rx66");
  BOOST_CHECK_EQUAL(lFedMap[1356].at(9), "MP8.inputPorts.Rx68");
  BOOST_CHECK_EQUAL(lFedMap[1356].at(10), "MP8.inputPorts.Rx70");
}

BOOST_FIXTURE_TEST_CASE ( TestInvalidConnectedFedXml_MissingId, TestXmlSystemSetup)
{
// Check behaviour when 'id' attribute is not specified
  pugi::xml_document lDoc;
  pugi::xml_node lFedNode = lDoc.append_copy(fedXml.child("connected-fed"));
  std::string lErrorMsg("");
  BOOST_REQUIRE(detail::validateConnectedFedXml(lFedNode, lErrorMsg));
  swatch::system::SystemStub::FEDInputPortsMap_t lFedMap;
  BOOST_REQUIRE_NO_THROW(detail::xmlToConnectedFeds(lFedNode, lFedMap, std::vector<std::string>()));

  lFedNode.remove_attribute("id");
  BOOST_CHECK(!detail::validateConnectedFedXml(lFedNode, lErrorMsg));
  lFedMap.clear();
  BOOST_CHECK_THROW(detail::xmlToConnectedFeds(lFedNode, lFedMap, std::vector<std::string>()), InvalidSystemDescription);
}

BOOST_FIXTURE_TEST_CASE ( TestInvalidConnectedFedXml_MissingPortId, TestXmlSystemSetup)
{
  pugi::xml_document lDoc;
  pugi::xml_node lFedNode = lDoc.append_copy(fedXml.child("connected-fed"));

// Check original XML is valid before starting actual tests
  std::string lErrorMsg;
  BOOST_REQUIRE_MESSAGE(detail::validateConnectedFedXml(lFedNode, lErrorMsg),
                        "Original XML is invalid. Won't proceed with test");
  swatch::system::SystemStub::FEDInputPortsMap_t lFedMap;
  BOOST_REQUIRE_NO_THROW(detail::xmlToConnectedFeds(lFedNode, lFedMap, std::vector<std::string>()));

// Remove this mandatory attribute, and check that validation fails
  lFedNode.child("port").remove_attribute("id");
  BOOST_CHECK(!detail::validateConnectedFedXml(lFedNode, lErrorMsg));
  lFedMap.clear();
  BOOST_CHECK_THROW(detail::xmlToConnectedFeds(lFedNode, lFedMap, std::vector<std::string>()), InvalidSystemDescription);
}

BOOST_FIXTURE_TEST_CASE ( TestInvalidConnectedFedXml_EmptyPortId, TestXmlSystemSetup)
{
  pugi::xml_document lDoc;
  pugi::xml_node lFedNode = lDoc.append_copy(fedXml.child("connected-fed"));

// Check original XML is valid before starting actual tests
  std::string lErrorMsg;
  BOOST_REQUIRE_MESSAGE(detail::validateConnectedFedXml(lFedNode, lErrorMsg),
                        "Original XML is invalid. Won't proceed with test");
  swatch::system::SystemStub::FEDInputPortsMap_t lFedMap;
  BOOST_REQUIRE_NO_THROW(detail::xmlToConnectedFeds(lFedNode, lFedMap, std::vector<std::string>()));

// Change attribute's value to empty string, and check that validation fails
  lFedNode.child("port").attribute("id").set_value("");
  BOOST_CHECK(!detail::validateConnectedFedXml(lFedNode, lErrorMsg));
  lFedMap.clear();
  BOOST_CHECK_THROW(detail::xmlToConnectedFeds(lFedNode, lFedMap, std::vector<std::string>()), InvalidSystemDescription);
}

BOOST_FIXTURE_TEST_CASE ( TestInvalidConnectedFedXml_UnknownChildTag, TestXmlSystemSetup)
{
  pugi::xml_document lDoc;
  pugi::xml_node lFedNode = lDoc.append_copy(fedXml.child("connected-fed"));

// Check original XML is valid before starting actual test
  std::string lErrorMsg("");
  swatch::system::SystemStub::FEDInputPortsMap_t lFedMap;
  BOOST_REQUIRE(detail::validateConnectedFedXml(lFedNode, lErrorMsg));
  BOOST_REQUIRE_NO_THROW(detail::xmlToConnectedFeds(lFedNode, lFedMap, std::vector<std::string>()));

// Check behaviour when there's an extra unknown tag
  lFedNode.append_child("unknown_tag").append_child(pugi::node_pcdata).set_value("42");
  BOOST_CHECK(!detail::validateConnectedFedXml(lFedNode, lErrorMsg));
  lFedMap.clear();
  BOOST_CHECK_THROW(detail::xmlToConnectedFeds(lFedNode, lFedMap, std::vector<std::string>()), InvalidSystemDescription);
}


BOOST_AUTO_TEST_CASE ( TestInvalidConnectedFeds_DuplicateEntries)
{
  const std::string lValidSystemXml =
    "<system id='mySystem'>"
    "  <creator>MySystemClass</creator>"
    "  <connected-feds>"
    "    <connected-fed id='1'>"
    "      <port id='someProc.inputPorts.Rx01'/>"
    "    </connected-fed>"
    "    <connected-fed id='2'>"
    "      <port id='someProc.inputPorts.Rx02'/>"
    "    </connected-fed>"
    "  </connected-feds>"
    "</system>";


  // Load valid XML (and check that validation passes before tests)
  pugi::xml_document lDoc;
  BOOST_REQUIRE_EQUAL(lDoc.load(lValidSystemXml.c_str()), true);
  std::string lErrorMsg;
  BOOST_REQUIRE_EQUAL(detail::validateSystemXml(lDoc, lErrorMsg), true);
  BOOST_REQUIRE_NO_THROW(xmlToSystemStub(lDoc));

  // Case 1: Duplicate FED ID - change id of 2nd connected-fed tag to match that of first one.
  lDoc.child("system").child("connected-feds").last_child().attribute("id").set_value("1");
  BOOST_CHECK_EQUAL(detail::validateSystemXml(lDoc, lErrorMsg), true);
  BOOST_CHECK_THROW(xmlToSystemStub(lDoc), InvalidSystemDescription);

  // Reset lDoc to valid XMl before modifying the XML to test case 2
  BOOST_REQUIRE_EQUAL(lDoc.load(lValidSystemXml.c_str()), true);
  BOOST_REQUIRE_EQUAL(detail::validateSystemXml(lDoc, lErrorMsg), true);
  BOOST_REQUIRE_NO_THROW(xmlToSystemStub(lDoc));

  // Case 2: Duplicate port within same connected-fed node
  pugi::xml_node lConnFed1 =  lDoc.child("system").child("connected-feds").first_child();
  lConnFed1.append_copy(lConnFed1.first_child());
  BOOST_CHECK_EQUAL(detail::validateSystemXml(lDoc, lErrorMsg), true);
  BOOST_CHECK_THROW(xmlToSystemStub(lDoc), InvalidSystemDescription);

  // Reset lDoc to valid XMl before modifying the XML to test case 3
  BOOST_REQUIRE_EQUAL(lDoc.load(lValidSystemXml.c_str()), true);
  BOOST_REQUIRE_EQUAL(detail::validateSystemXml(lDoc, lErrorMsg), true);
  BOOST_REQUIRE_NO_THROW(xmlToSystemStub(lDoc));

  // Case 3: Duplicate port across different connected-fed nodes
  lConnFed1 = lDoc.child("system").child("connected-feds").child("connected-fed");
  pugi::xml_node lConnFed2 =  lConnFed1.next_sibling("connected-fed");
  lConnFed2.append_copy(lConnFed1.child("port"));
  BOOST_CHECK_EQUAL(detail::validateSystemXml(lDoc, lErrorMsg), true);
  BOOST_CHECK_THROW(xmlToSystemStub(lDoc), InvalidSystemDescription);
}


// ---------------------------------------------------------------------
//    CHILD TAG: EXCLUDE
// ---------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE ( TestExcludedBoards, TestXmlSystemSetup )
{
  pugi::xml_node lNode = fedXml.child("connected-fed");
  swatch::system::SystemStub::FEDInputPortsMap_t lFedMap;
  std::vector<std::string> lExcludedBoards;
  lExcludedBoards.push_back("FaultyProcessor");
  detail::xmlToConnectedFeds(lNode, lFedMap, lExcludedBoards);

  BOOST_REQUIRE_EQUAL(lFedMap.count(1356), size_t(1));
  BOOST_CHECK_EQUAL(lFedMap[1356].size(), size_t(5 + 6));

  lExcludedBoards.push_back("MP8");
  lFedMap.clear();
  detail::xmlToConnectedFeds(lNode, lFedMap, lExcludedBoards);
  BOOST_REQUIRE_EQUAL(lFedMap.count(1356), size_t(1));
  BOOST_CHECK_EQUAL(lFedMap[1356].size(), size_t(0));
}

BOOST_FIXTURE_TEST_CASE ( TestUnkownExcludedBoard, TestXmlSystemSetup )
{
  // Load valid XML (and check that validation passes before tests)
  std::string lErrorMsg("");
  BOOST_REQUIRE_EQUAL(detail::validateSystemXml(systemXml, lErrorMsg), true);
  BOOST_REQUIRE_NO_THROW(xmlToSystemStub(systemXml));

  // Check that validation fails if exclude tag's 'id' attribute doesn't match id string of any proc / daqttc-mgr
  systemXml.child("system").child("excluded-boards").child("exclude").attribute("id").set_value("unknown_id_string");
  BOOST_CHECK_EQUAL(detail::validateSystemXml(systemXml, lErrorMsg), true);
  BOOST_CHECK_THROW(xmlToSystemStub(systemXml), InvalidSystemDescription);
}


BOOST_AUTO_TEST_SUITE_END() // TestXmlSystem

}//ns: test
}//ns: system
} //ns: xml
} //ns: swatch

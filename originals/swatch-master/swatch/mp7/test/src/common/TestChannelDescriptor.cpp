#include <boost/test/unit_test.hpp>

#include "swatch/mp7/ChannelDescriptor.hpp"
#include "swatch/mp7/ChannelDescriptorCollection.hpp"

// Boost Headers
#include <boost/assign/std/vector.hpp>
#include "boost/function.hpp"

namespace swatch {
namespace mp7 {
namespace test {

struct DescriptorSetup {
  DescriptorSetup() :
    descriptors()
  {

    using namespace boost::assign; // bring 'operator+=()' into scope

    descriptors.insert(ChannelDescriptor("aa", 5, false, false, ::mp7::kTDRFormatter) );
    descriptors.insert(ChannelDescriptor("cc", 1, false, false, ::mp7::kNoFormatter) );
    descriptors.insert(ChannelDescriptor("ddd", 15, true, true, ::mp7::kNoFormatter) );

    // Known ids (sorted))
    ids += "aa", "cc", "ddd";
    // Known channel ids (sorted)
    channelIds += 1,5,15;
  }

  ChannelDescriptorCollection descriptors;
  std::vector<std::string> ids;
  std::vector<uint32_t> channelIds;

};

BOOST_AUTO_TEST_SUITE( MP7TestSuite )

BOOST_FIXTURE_TEST_CASE(InsertDuplicateDescriptor, DescriptorSetup)
{

  ChannelDescriptor lD("aa", 5, false, false, ::mp7::kNoFormatter);
  BOOST_CHECK_THROW(descriptors.insert(lD), swatch::mp7::ChannelDescriptorInsertionError);

}


BOOST_FIXTURE_TEST_CASE(InsertDuplicateId, DescriptorSetup)
{

  ChannelDescriptor lD("bb", 5, false, false, ::mp7::kNoFormatter);
  BOOST_CHECK_THROW(descriptors.insert(lD), swatch::mp7::ChannelDescriptorInsertionError);

}

BOOST_FIXTURE_TEST_CASE(InsertDuplicateChannel, DescriptorSetup)
{

  ChannelDescriptor lD("bb", 5, false, false, ::mp7::kNoFormatter);
  BOOST_CHECK_THROW(descriptors.insert(lD), swatch::mp7::ChannelDescriptorInsertionError);

}

BOOST_FIXTURE_TEST_CASE(GetByIdAndChannel, DescriptorSetup)
{
  descriptors.getByChannelId(5);
  descriptors.getById("aa");
}

BOOST_FIXTURE_TEST_CASE(DescriptorNotFound, DescriptorSetup)
{
  BOOST_CHECK_THROW(descriptors.getById("bb"), swatch::mp7::ChannelDescriptorNotFound);
  BOOST_CHECK_THROW(descriptors.getByChannelId(7), swatch::mp7::ChannelDescriptorNotFound);

}

BOOST_FIXTURE_TEST_CASE(GetListOfIds, DescriptorSetup)
{

  auto lIds = descriptors.ids();
  BOOST_CHECK_EQUAL_COLLECTIONS(lIds.begin(), lIds.end(), ids.begin(), ids.end());

  auto lChannels = descriptors.channels();
  BOOST_CHECK_EQUAL_COLLECTIONS(lChannels.begin(), lChannels.end(), channelIds.begin(), channelIds.end());

}

BOOST_FIXTURE_TEST_CASE(ChannelToIdMapping, DescriptorSetup)
{
  std::vector<uint32_t> lChansA = {5,1}, lChansB = {1,5};
  std::vector<std::string> lIdsA = {"aa","cc"}, lIdsB = {"cc","aa"};


  auto lChansAToIds = descriptors.chansToIds(lChansA);

  BOOST_CHECK_EQUAL_COLLECTIONS(lChansAToIds.begin(), lChansAToIds.end(), lIdsA.begin(), lIdsA.end());

  auto lChansBToIds = descriptors.chansToIds(lChansB);

  BOOST_CHECK_EQUAL_COLLECTIONS(lChansBToIds.begin(), lChansBToIds.end(), lIdsB.begin(), lIdsB.end());

  auto lIdsAToChans = descriptors.idsToChans(lIdsA);

  BOOST_CHECK_EQUAL_COLLECTIONS(lIdsAToChans.begin(), lIdsAToChans.end(), lChansA.begin(), lChansA.end());

  auto lIdsBToChans = descriptors.idsToChans(lIdsB);

  BOOST_CHECK_EQUAL_COLLECTIONS(lIdsBToChans.begin(), lIdsBToChans.end(), lChansB.begin(), lChansB.end());

}

BOOST_FIXTURE_TEST_CASE(MappingDescriptorNotFound, DescriptorSetup)
{
  BOOST_CHECK_THROW(descriptors.chansToIds({3,1}), swatch::mp7::DescriptorIdsNotFound);
}

BOOST_FIXTURE_TEST_CASE(FindDescriptors, DescriptorSetup)
{
  ChannelDescriptorRule_t lRuleTDRFmt = (boost::bind(&ChannelDescriptor::getFormatterKind, _1) == ::mp7::kTDRFormatter);
  ChannelDescriptorRule_t lRuleNoFmt = (boost::bind(&ChannelDescriptor::getFormatterKind, _1) == ::mp7::kNoFormatter);
  ChannelDescriptorRule_t lRuleBuffer = (boost::bind(&ChannelDescriptor::hasBuffer, _1));

  {
    // TDR Fmt scope
    std::vector<std::string> lExpectedIds = {"aa"};
    auto lFoundIds = descriptors.findIds(lRuleTDRFmt);

    BOOST_CHECK_EQUAL_COLLECTIONS(lExpectedIds.begin(), lExpectedIds.end(), lFoundIds.begin(), lFoundIds.end());
  }

  {
    // No formatter
    std::vector<std::string> lExpectedIds = {"cc","ddd"};
    auto lFoundIds = descriptors.findIds(lRuleNoFmt);

    BOOST_CHECK_EQUAL_COLLECTIONS(lExpectedIds.begin(), lExpectedIds.end(), lFoundIds.begin(), lFoundIds.end());
  }

  {
    // Has buffer
    std::vector<std::string> lExpectedIds = {"ddd"};

    auto lFoundIds = descriptors.findIds(lRuleBuffer);
    BOOST_CHECK_EQUAL_COLLECTIONS(lExpectedIds.begin(), lExpectedIds.end(), lFoundIds.begin(), lFoundIds.end());
  }




}

BOOST_AUTO_TEST_SUITE_END()


}
}
}

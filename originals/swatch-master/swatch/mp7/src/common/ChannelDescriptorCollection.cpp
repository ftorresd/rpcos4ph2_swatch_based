/*
 * File:   ChannelDescriptorMap.cpp
 * Author: ale
 *
 * Created on April 8, 2016, 7:27 PM
 */

#include "swatch/mp7/ChannelDescriptorCollection.hpp"


#include <algorithm>                    // for set_difference, sort
#include <cstdint>                      // for uint32_t
#include <iterator>                     // for back_insert_iterator, etc
#include <ostream>                      // for operator<<, basic_ostream, etc
#include <stdexcept>                    // for out_of_range
#include <utility>                      // for pair

// boost headers
#include "boost/function.hpp"
#include <boost/range/algorithm/copy.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/adaptor/filtered.hpp>
#include <boost/foreach.hpp>

// MP7 headers
#include "swatch/core/utilities.hpp"
#include "swatch/mp7/PointValidator.hpp"


namespace swatch {
namespace mp7 {

ChannelDescriptorCollection::ChannelDescriptorCollection():
  mContainer()
{
}


ChannelDescriptorCollection::~ChannelDescriptorCollection()
{
}


// ---
void
ChannelDescriptorCollection::insert(const ChannelDescriptor& aDescriptor)
{

  std::pair<Container_t::iterator,bool> lItDesc = mContainer.insert(aDescriptor);

  if ( !lItDesc.second ) {
    std::ostringstream lMsg;
    lMsg << "Failed to insert descriptor Id='" << aDescriptor.mId << "', ChId=" << aDescriptor.mChannelId << ". ";
    lMsg << "Descriptor with same Id/ChannelId already exists Id='" << lItDesc.first->mId << "', ChId=" <<  lItDesc.first->mChannelId;
    XCEPT_RAISE(ChannelDescriptorInsertionError,lMsg.str());
  }
}


// ---
const ChannelDescriptor&
ChannelDescriptorCollection::getById(const std::string& aId) const
{
  Container_t::index<byId>::type::const_iterator lIt = mContainer.get<byId>().find(aId);
  if ( lIt == mContainer.get<byId>().end() ) {
    // Throw here
    std::ostringstream lMsg;
    lMsg << "Descriptor with Id='" << aId << "' not found";
    XCEPT_RAISE(ChannelDescriptorNotFound,lMsg.str());
  }

  return *lIt;
}


// ---
const ChannelDescriptor&
ChannelDescriptorCollection::getByChannelId(uint32_t aChannelId) const
{
  Container_t::index<byChannel>::type::const_iterator lIt = mContainer.get<byChannel>().find(aChannelId);
  if ( lIt == mContainer.get<byChannel>().end() ) {
    // Throw here
    std::ostringstream lMsg;
    lMsg << "Descriptor with ChId=" << aChannelId << " not found";
    XCEPT_RAISE(ChannelDescriptorNotFound,lMsg.str());
  }

  return *lIt;
}

// ---
std::vector<std::string>
ChannelDescriptorCollection::ids() const
{
  std::vector<std::string> lIds;
  lIds.reserve(mContainer.size());

  boost::copy(mContainer.get<byId>()
              | boost::adaptors::transformed(boost::bind(&ChannelDescriptor::mId,_1)),
              std::back_inserter(lIds));

  return lIds;
}


// ---
std::vector<uint32_t>
ChannelDescriptorCollection::channels() const
{
  std::vector<uint32_t> lChanIds;
  lChanIds.reserve(mContainer.size());

  boost::copy(mContainer.get<byChannel>()
              | boost::adaptors::transformed(boost::bind(&ChannelDescriptor::mChannelId,_1)),
              std::back_inserter(lChanIds));

  return lChanIds;

}

//---
std::vector<std::string>
ChannelDescriptorCollection::findIds(const ChannelDescriptorRule_t& aPred) const
{
  std::vector<std::string> lIds;

  boost::copy(mContainer
              | boost::adaptors::filtered(aPred)
              | boost::adaptors::transformed(boost::bind(&ChannelDescriptor::mId,_1)),
              std::back_inserter(lIds));

  return lIds;
}


// ---


std::vector<std::string>
ChannelDescriptorCollection::filterIds(const std::vector<std::string>& aIds, const ChannelDescriptorRule_t& aPred) const
{
  std::vector<std::string> lFiltered;
  std::vector<std::string> lNotFound;
  BOOST_FOREACH( const std::string& lId, aIds ) {
    try {
      const ChannelDescriptor& lDesc = getById(lId);

      if ( !aPred(lDesc) ) continue;

      lFiltered.push_back(lId);

    }
    catch ( const std::out_of_range& lExc ) {
      lNotFound.push_back(lId);
    }
  }

  if ( !lNotFound.empty() ) {
    XCEPT_RAISE(DescriptorIdsNotFound,"IDs : "+core::joinAny(lNotFound));
  }

  return lFiltered;
}


// ---
std::vector<uint32_t>
ChannelDescriptorCollection::idsToChans(const std::vector<std::string>& aIds) const
{

  // Check for missing ids
  ensureIdsExist( aIds );

  // Get the id index
  const Container_t::index<byId>::type& idx = mContainer.get<byId>();

  std::vector<uint32_t> lChanIds;
  lChanIds.reserve(aIds.size());

  BOOST_FOREACH( const std::string& lId, aIds ) {
    lChanIds.push_back(idx.find(lId)->mChannelId);
  }

  return lChanIds;

}

// ---
std::vector<std::string>
ChannelDescriptorCollection::chansToIds(const std::vector<uint32_t>& aChannels) const
{

  // Check for missing channels
  ensureChannelsExist( aChannels );

  // Get the id index
  const Container_t::index<byChannel>::type& idx = mContainer.get<byChannel>();

  std::vector<std::string> lIds;
  lIds.reserve(aChannels.size());

  BOOST_FOREACH( uint32_t lChan, aChannels ) {
    lIds.push_back(idx.find(lChan)->mId);
  }
  return lIds;
}

// ---
void ChannelDescriptorCollection::ensureIdsExist( const std::vector<std::string>& aIdList ) const
{
  // Get a copy of all available ids
  const std::vector<std::string>& lIds = ids();

  // Copy and sort input
  std::vector<std::string> lSorted(aIdList);
  std::sort(lSorted.begin(), lSorted.end());


  std::vector<std::string> lNotFound;
  std::set_difference ( lSorted.begin(), lSorted.end(), lIds.begin(), lIds.end(), std::back_inserter(lNotFound) );

  if ( !lNotFound.empty() ) {
    XCEPT_RAISE(DescriptorIdsNotFound,"Missing IDs: "+core::joinAny(lNotFound) );
  }
}

// ---
void ChannelDescriptorCollection::ensureChannelsExist( const std::vector<uint32_t>& aChanList ) const
{
  // Get a copy of all available ids
  const std::vector<uint32_t>& lChanIds = channels();

  // Copy and sort input
  std::vector<uint32_t> lSorted(aChanList);
  std::sort(lSorted.begin(), lSorted.end());

  std::vector<uint32_t> lNotFound;
  std::set_difference ( lSorted.begin(), lSorted.end(), lChanIds.begin(), lChanIds.end(), std::back_inserter(lNotFound) );

  if ( !lNotFound.empty() ) {
    XCEPT_RAISE(DescriptorIdsNotFound,"Missing Chans: "+core::joinAny(lNotFound) );
  }
}


void ChannelDescriptorCollection::ensureIdsExistAndMatchRule(const std::vector<std::string>& aIds, const ChannelDescriptorRule_t& aPred) const
{
  std::vector<std::string> lNotMatching;
  std::vector<std::string> lNotFound;
  BOOST_FOREACH( const std::string& lId, aIds ) {
    try {
      const ChannelDescriptor& lDesc = getById(lId);

      // Continue if descriptor passes the rule
      if ( aPred(lDesc) ) continue;

      // This descriptor fails rule matching
      lNotMatching.push_back(lId);

    }
    catch ( const std::out_of_range& lExc ) {
      lNotFound.push_back(lId);
    }
  }

  if ( !lNotFound.empty() ) {
    XCEPT_RAISE(DescriptorIdsNotFound,"IDs : "+core::joinAny(lNotFound));
  }
  if ( !lNotMatching.empty() ) {
    XCEPT_RAISE(ChannelDescriptorFailsPrerequisites,"IDs don't satisfy Command prerequisites : "+core::joinAny(lNotMatching));
  }
}





} // namespace mp7
} // namespace swatch

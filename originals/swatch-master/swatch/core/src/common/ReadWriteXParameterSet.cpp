#include "swatch/core/ReadWriteXParameterSet.hpp"


// Standard headers
#include <algorithm>                    // for max, transform
#include <iterator>                     // for insert_iterator, inserter

// boost headers
#include "boost/bind.hpp"

// XDAQ headers
#include "xdata/Serializable.h"


namespace swatch {
namespace core {


//---
ReadWriteXParameterSet::ReadWriteXParameterSet() :
  mEntries()
{
}


//---
ReadWriteXParameterSet::~ReadWriteXParameterSet()
{
}


//---
bool ReadWriteXParameterSet::operator==(const ReadWriteXParameterSet& aOther) const
{
  return mEntries == aOther.mEntries;
}


//---
size_t ReadWriteXParameterSet::size() const
{
  return mEntries.size();
}


//---
std::set<std::string> ReadWriteXParameterSet::keys() const
{
  std::set<std::string> lNames;
  std::transform(mEntries.begin(), mEntries.end(), std::inserter(lNames, lNames.end()), boost::bind(&EntryMap_t::value_type::first, _1));
  return lNames;
}


//---
bool ReadWriteXParameterSet::has(const std::string& aName) const
{
  return mEntries.count(aName);
}


//---
const xdata::Serializable&
ReadWriteXParameterSet::get( const std::string& aName ) const
{
  EntryMap_t::const_iterator lIt = mEntries.find(aName);
  if ( lIt == mEntries.end() ) {
    XCEPT_RAISE(XParameterNotFound,aName +" not found");
  }

  return *(lIt->second.object);
}


//---
xdata::Serializable&
ReadWriteXParameterSet::get( const std::string& aName )
{
  EntryMap_t::iterator lIt = mEntries.find(aName);
  if ( lIt == mEntries.end() ) {
    XCEPT_RAISE(XParameterNotFound,"Parameter '"+aName +"' not found");
  }

  return *(lIt->second.object);
}


//---
xdata::Serializable&
ReadWriteXParameterSet::operator[](const std::string& aName)
{
  return get(aName);
}


//---
const xdata::Serializable&
ReadWriteXParameterSet::operator[](const std::string& aName) const
{
  return get(aName);
}


//---
void ReadWriteXParameterSet::deepCopyFrom(const ReadWriteXParameterSet& aOtherSet)
{
  for ( EntryMap_t::const_iterator lIt = aOtherSet.mEntries.begin(); lIt != aOtherSet.mEntries.end(); lIt++) {
    boost::shared_ptr<xdata::Serializable> lClonedData( lIt->second.cloner(lIt->second.object.get()) );

    XEntry lDeepCopy(lIt->second.typeinfo, lIt->second.cloner, lClonedData);
    emplace(lIt->first, lDeepCopy);
  }
}


//---
void ReadWriteXParameterSet::erase(const std::string& aName)
{
  mEntries.erase(aName);
}


//---
ReadWriteXParameterSet::XEntry::XEntry(const std::type_info* aType, XCloner_t aCloner, const boost::shared_ptr<xdata::Serializable>& aData) :
  typeinfo(aType),
  cloner(aCloner),
  object(aData)
{
}


//---
ReadWriteXParameterSet::XEntry::XEntry(const XEntry& aOther) :
  typeinfo(aOther.typeinfo),
  cloner( aOther.cloner),
  object( boost::shared_ptr<xdata::Serializable>( aOther.object ) )
{
}


//---
bool
ReadWriteXParameterSet::XEntry::operator==(const XEntry& aOther) const
{
  if ( this->typeinfo != aOther.typeinfo ) {
    // Different type
    return false;
  }
  else if ( (this->object == 0x0 ) != (aOther.object == 0x0 )  ) {
    // One of the 2 is null
    return false;
  }
  else if ( (this->object == 0x0 ) && (aOther.object == 0x0 ) ) {
    // Both are null
    return true;
  }
  else {
    return this->object->equals(*aOther.object);
  }
}


//---
std::pair<ReadWriteXParameterSet::EntryMap_t::iterator, bool>
ReadWriteXParameterSet::emplace(const std::string& aName, const std::type_info* aType, XCloner_t aCloner, const boost::shared_ptr<xdata::Serializable>& aData)
{
  return mEntries.emplace(aName, XEntry(aType, aCloner, aData));
}


//---
std::pair<ReadWriteXParameterSet::EntryMap_t::iterator, bool>
ReadWriteXParameterSet::emplace(const std::string& aName, const XEntry& aEntry)
{
  return mEntries.emplace(aName, aEntry);
}


} // core
} // swatch


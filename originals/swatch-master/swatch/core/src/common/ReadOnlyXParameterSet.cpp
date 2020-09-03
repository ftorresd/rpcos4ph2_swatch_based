#include "swatch/core/ReadOnlyXParameterSet.hpp"


#include <iterator>                     // for insert_iterator, inserter
#include <stdexcept>                    // for runtime_error
#include <typeinfo>                     // for type_info

// boost headers
#include "boost/bind.hpp"

// XDAQ headers
#include "swatch/core/ReadWriteXParameterSet.hpp"
#include "swatch/core/utilities.hpp"


namespace swatch {
namespace core {


//---
ReadOnlyXParameterSet::ReadOnlyXParameterSet() :
  mEntries()
{
}


//---
ReadOnlyXParameterSet::ReadOnlyXParameterSet(const ReadOnlyXParameterSet& aOther) :
  mEntries(aOther.mEntries)
{
}


//---
ReadOnlyXParameterSet::ReadOnlyXParameterSet(const XParameterSet& aOrig)
{
  // FIXME: Is this the best solution for long term ?

  if (const ReadOnlyXParameterSet* const origPtr = dynamic_cast<ReadOnlyXParameterSet const* const>(&aOrig) ) {
    *this = (*origPtr);
  }
  else if (const ReadWriteXParameterSet* const origPtr = dynamic_cast<ReadWriteXParameterSet const* const> (&aOrig) ) {
    typedef ReadWriteXParameterSet::EntryMap_t::const_iterator ConstIt_t;
    for (ConstIt_t lIt = origPtr->mEntries.begin(); lIt != origPtr->mEntries.end(); lIt++)
      mEntries.emplace(lIt->first, lIt->second.object);
  }
  else
    XCEPT_RAISE(XParameterUnknownType,"Cannot create ReadOnlyXParameterSet from unknown XParameterSet type: " + demangleName(typeid(aOrig).name()));
}


//---
ReadOnlyXParameterSet::~ReadOnlyXParameterSet()
{
}


//---
bool ReadOnlyXParameterSet::operator==(const ReadOnlyXParameterSet& aOther) const
{
  return mEntries == aOther.mEntries;
}


//---
size_t ReadOnlyXParameterSet::size() const
{
  return mEntries.size();
}


//---
std::set<std::string> ReadOnlyXParameterSet::keys() const
{
  std::set<std::string> lNames;
  std::transform(mEntries.begin(), mEntries.end(), std::inserter(lNames, lNames.end()), boost::bind(&EntryMap_t::value_type::first, _1));
  return lNames;
}


//---
bool ReadOnlyXParameterSet::has(const std::string& aName) const
{
  return mEntries.count(aName);
}


//---
const xdata::Serializable&
ReadOnlyXParameterSet::get( const std::string& aName ) const
{
  EntryMap_t::const_iterator lIt = mEntries.find(aName);
  if ( lIt == mEntries.end() ) {
    XCEPT_RAISE(XParameterNotFound,"Parameter '"+aName +"' not found");
  }

  return *(lIt->second);
}


//---
const xdata::Serializable&
ReadOnlyXParameterSet::operator[](const std::string& aName) const
{
  return get(aName);
}


//---
void ReadOnlyXParameterSet::adopt(const std::string& aName , const boost::shared_ptr<const xdata::Serializable>& aData )
{
  std::pair<EntryMap_t::iterator, bool> lIt = mEntries.emplace(aName, aData);

  // If failed to emplace, then throw
  if ( !lIt.second )
    XCEPT_RAISE(XParameterExists,"Parameter '" + aName + "' is already defined");
}


//---
void ReadOnlyXParameterSet::adopt(const std::string& aName , const ReadOnlyXParameterSet& aOtherSet)
{
  EntryMap_t::const_iterator lIt = aOtherSet.mEntries.find(aName);

  if ( lIt == aOtherSet.mEntries.end() )
    XCEPT_RAISE(XParameterNotFound,"Parameter '" + aName + "' does not exist in this set");

  adopt(aName, lIt->second);
}


//---
void ReadOnlyXParameterSet::adopt(const std::string& aName , const ReadWriteXParameterSet& aOtherSet)
{
  ReadWriteXParameterSet::EntryMap_t::const_iterator lIt = aOtherSet.mEntries.find(aName);

  if ( lIt == aOtherSet.mEntries.end() )
    XCEPT_RAISE(XParameterNotFound,"Parameter '" + aName + "' does not exist in this set");

  adopt(aName, lIt->second.object);
}


//---
void ReadOnlyXParameterSet::erase(const std::string& aName)
{
  mEntries.erase(aName);
}


} // core
} // swatch


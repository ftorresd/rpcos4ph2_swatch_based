/*
 * File:   Crate.cpp
 * Author: ale
 * Date: July 2014
 */

#include "swatch/system/Crate.hpp"


// C++ headers
#include <sstream>
#include <stddef.h>                     // for NULL
#include <string>                       // for operator<<

// boost headers
#include <boost/foreach.hpp>

// SWATCH headers
#include "swatch/core/AbstractStub.hpp"
#include "swatch/core/utilities.hpp"
#include "swatch/processor/Processor.hpp"
#include "swatch/dtm/DaqTTCManager.hpp"


// Namespace resolution
using namespace std;

namespace swatch {
namespace system {


Crate::Crate(const swatch::core::AbstractStub& aStub) :
  ObjectView(aStub.id, aStub.alias),
  mStub(dynamic_cast<const CrateStub&>(aStub)),
  mMCH(NULL),
  mAMC13(NULL),
  mAMCs(12, NULL),
  mMinSlot(1),
  mMaxSlot(12),
  mAMC13Slot(13)
{
}


Crate::~Crate()
{
}


void
Crate::add(dtm::DaqTTCManager* aAMC13)
{
  // Check the slot number
  if (aAMC13->getSlot() != mAMC13Slot) {
    ostringstream ss;
    ss << "Crate '"<< getId() << "': Slot " << mAMC13Slot << " for DaqTTCManager '" << aAMC13->getId() << "' does not match expected value (" << mAMC13Slot << ")" ;
    XCEPT_RAISE(CrateSlotOutOfRange,ss.str());
  }

  mAMC13 = aAMC13;
  addObj( aAMC13, "amc13" );
}


void
Crate::add(processor::Processor* aProcessor)
{

  // Check the slot number range
  uint32_t slot = aProcessor->getSlot();
  if (slot < mMinSlot || slot > mMaxSlot) {
    stringstream ss;
    ss << "Crate '"<< getId() << "': Slot " << slot << " for processor '" << aProcessor->getId() << "' is out of range";
    XCEPT_RAISE(CrateSlotOutOfRange,ss.str());
  }

  // Check if the slot is available
  if ( isAMCSlotTaken(slot) ) {
    stringstream ss;
    ss << this->getId() << ": Cannot add card " << aProcessor->getId() << " to slot " << slot
       << ". Slot already assigned to card " << this->mAMCs[slot-mMinSlot]->getId();
    XCEPT_RAISE(CrateSlotTaken,ss.str());
  }

  // All clear, add the processor
  mAMCs[slot-mMinSlot] = aProcessor;
  addObj(aProcessor, core::strPrintf("amc%02d", slot));
}


processor::Processor*
Crate::amc(uint32_t aSlot)
{
  if (aSlot < mMinSlot || aSlot > mMaxSlot) {
    stringstream ss;
    ss << "Crate '"<< getId() << "': Slot " << aSlot << " out of range";

    XCEPT_RAISE(CrateSlotOutOfRange,ss.str());
  }
  return mAMCs[aSlot-mMinSlot];
}


const processor::Processor* Crate::amc(uint32_t aSlot) const
{
  if (aSlot < mMinSlot || aSlot > mMaxSlot) {
    stringstream ss;
    ss << "Crate '"<< getId() << "': Slot " << aSlot << " out of range";

    XCEPT_RAISE(CrateSlotOutOfRange,ss.str());
  }
  return mAMCs[aSlot-mMinSlot];
}


std::vector<uint32_t>
Crate::getPopulatedSlots() const
{
  std::vector<uint32_t> slots = getAMCSlots();

  if ( mAMC13 ) slots.push_back(mAMC13->getSlot());
  return slots;
}


std::vector<uint32_t>
Crate::getAMCSlots() const
{
  std::vector<uint32_t> slots;
  BOOST_FOREACH( processor::Processor* p, mAMCs ) {
    if (p) slots.push_back(p->getSlot());
  }

  return slots;
}


bool Crate::isAMCSlotTaken( uint32_t aSlot ) const
{
  return mAMCs[aSlot - mMinSlot] != NULL;
}


const CrateStub& Crate::getStub() const
{
  return mStub;
}


std::ostream&
operator<<(std::ostream& aStream, const swatch::system::Crate& aCrate)
{
  aStream << "mch(" << aCrate.mMCH << ") amc13(" << aCrate.mAMC13 << ")";
  for ( uint8_t i = aCrate.mMinSlot; i < aCrate.mMaxSlot; ++i ) {
    aStream << " amc[" << (int)i << "](" << aCrate.mAMCs[i] << ")";
  }
  return aStream;
}


}
}

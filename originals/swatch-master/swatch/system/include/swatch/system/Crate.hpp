/*
 * File:   Crate.hpp
 * Author: ale
 */

#ifndef __SWATCH_SYSTEM_CRATE_HPP__
#define __SWATCH_SYSTEM_CRATE_HPP__


// Standard headers
#include <stdint.h>                     // for uint32_t
#include <iosfwd>                       // for ostream
#include <vector>                       // for vector

// SWATCH headers
#include "swatch/core/exception.hpp"
#include "swatch/core/Object.hpp"
#include "swatch/system/CrateStub.hpp"


// forward declarations
namespace swatch {
namespace core {
class AbstractStub;
}
namespace processor {
class Processor;
}
}


namespace swatch {

namespace dtm {
class DaqTTCManager;
} // namespace dtm

namespace system {

class Crate;
class Service;

std::ostream& operator<<(std::ostream& aStream, const swatch::system::Crate& aCrate);

//! Provides a view on the SWATCH objects in a crate. It doesn't own the objects it points to.
class Crate : public core::ObjectView {
public:
  Crate( const swatch::core::AbstractStub& aStub );
  virtual ~Crate();

  void add( dtm::DaqTTCManager* aAMC13 );
  void add( processor::Processor* aProcessor );

  processor::Processor* amc( uint32_t aSlot );

  const processor::Processor* amc(uint32_t aSlot) const;

  dtm::DaqTTCManager* amc13()
  {
    return mAMC13;
  }

  const dtm::DaqTTCManager* amc13() const
  {
    return mAMC13;
  }

  std::vector<uint32_t> getPopulatedSlots() const;

  std::vector<uint32_t> getAMCSlots() const;

  bool isAMCSlotTaken( uint32_t aSlot ) const;

  const CrateStub& getStub() const;

private:
  CrateStub mStub;
  Service* mMCH;
  dtm::DaqTTCManager* mAMC13;
  std::vector<processor::Processor*> mAMCs;

  //! Lowest possible slot number for a processor
  const uint32_t mMinSlot;
  //! Highest possible slot number for a processor
  const uint32_t mMaxSlot;
  //! Slot number for AMC13
  const uint32_t mAMC13Slot;

  friend class System;
  friend std::ostream& (operator<<) (std::ostream& aStream, const swatch::system::Crate& aCrate);
};

SWATCH_DEFINE_EXCEPTION(CrateSlotTaken)
SWATCH_DEFINE_EXCEPTION(CrateSlotOutOfRange)

}
}

#endif  /* __SWATCH_SYSTEM_CRATE_HPP__ */


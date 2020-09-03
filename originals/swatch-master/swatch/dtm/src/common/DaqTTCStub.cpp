/**
 * @file    AMC13ServiceDescriptor.cpp
 * @author  Alessandro Thea
 * @date    11/11/14
 */

#include "swatch/dtm/DaqTTCStub.hpp"


#include <ostream>


namespace swatch {
namespace dtm {


DaqTTCStub::DaqTTCStub(const std::string& aId) :
  AbstractStub(aId),
  slot(13),
  fedId(0)
{
}

DaqTTCStub::~DaqTTCStub()
{
}


bool operator==(const DaqTTCStub& aStub1, const DaqTTCStub& aStub2)
{
  bool lResult = (aStub1.id == aStub2.id);
  lResult = lResult && (aStub1.loggerName == aStub2.loggerName);

  lResult = lResult && (aStub1.creator == aStub2.creator);
  lResult = lResult && (aStub1.role == aStub2.role);
  lResult = lResult && (aStub1.uriT1 == aStub2.uriT1);
  lResult = lResult && (aStub1.uriT2 == aStub2.uriT2);
  lResult = lResult && (aStub1.addressTableT1 == aStub2.addressTableT1);
  lResult = lResult && (aStub1.addressTableT2 == aStub2.addressTableT2);
  lResult = lResult && (aStub1.crate == aStub2.crate);
  lResult = lResult && (aStub1.slot == aStub2.slot);
  lResult = lResult && (aStub1.fedId == aStub2.fedId);

  return lResult;
}


std::ostream& operator<<(std::ostream& aStream, const swatch::dtm::DaqTTCStub& aStub )
{
  aStream << "DaqTTCStub[" << aStub.id << "]:\n"
          << "   - Uses creator = " << aStub.creator << "\n"
          << "   - Uri [T1] = " << aStub.uriT1 << "\n"
          << "   - Address Table [T1] = " << aStub.addressTableT1 << "\n"
          << "   - Uri [T2] = " << aStub.uriT2 << "\n"
          << "   - Address Table [T2] = " << aStub.addressTableT2 << "\n"
          << "   - Crate ID = " << aStub.crate << "\n"
          << "   - Crate Slot = " << aStub.slot
          << "   - FED ID = " << aStub.fedId << "\n"
          << std::flush;

  return aStream;
}


} // namespace dtm
} // namespace swatch

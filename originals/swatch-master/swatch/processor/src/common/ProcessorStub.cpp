
#include "swatch/processor/ProcessorStub.hpp"


// Standard headers
#include <ostream>

// SWATCH headers
#include "swatch/processor/Processor.hpp"


namespace swatch {
namespace processor {

ProcessorPortStub::ProcessorPortStub(const std::string& aId):
  AbstractStub(aId),
  number(0)
{
}

ProcessorPortStub::ProcessorPortStub(const std::string& aId, uint32_t aNumber):
  AbstractStub(aId),
  number(aNumber)
{
}

ProcessorPortStub::~ProcessorPortStub()
{
}

bool operator==(const ProcessorPortStub& aStub1, const ProcessorPortStub& aStub2)
{
  bool lResult = (aStub1.id == aStub2.id);
  lResult = lResult && (aStub1.loggerName == aStub2.loggerName);

  lResult = lResult && (aStub1.number == aStub2.number);

  return lResult;
}


ProcessorStub::ProcessorStub(const std::string& aId) :
  AbstractStub(aId),
  slot(Processor::kNoSlot)
{
}

ProcessorStub::~ProcessorStub()
{
}

bool operator==(const ProcessorStub& aStub1, const ProcessorStub& aStub2)
{
  bool lResult = (aStub1.id == aStub2.id);
  lResult = lResult && (aStub1.loggerName == aStub2.loggerName);

  lResult = lResult && (aStub1.creator == aStub2.creator);
  lResult = lResult && (aStub1.hwtype == aStub2.hwtype);
  lResult = lResult && (aStub1.role == aStub2.role);
  lResult = lResult && (aStub1.uri == aStub2.uri);
  lResult = lResult && (aStub1.addressTable == aStub2.addressTable);
  lResult = lResult && (aStub1.crate == aStub2.crate);
  lResult = lResult && (aStub1.slot == aStub2.slot);
  lResult = lResult && (aStub1.rxPorts == aStub2.rxPorts);
  lResult = lResult && (aStub1.txPorts == aStub2.txPorts);

  return lResult;
}


std::ostream& operator<<(std::ostream& aStream, const ProcessorStub& aStub )
{
  aStream << "ProcessorDescriptor[" << aStub.id << "]:\n"
          << "   - User creator = " << aStub.creator << "\n"
          << "   - Hardware type = " << aStub.hwtype << "\n"
          << "   - Role = " << aStub.role << "\n"
          << "   - Uri = " << aStub.uri << "\n"
          << "   - Address table = " << aStub.addressTable << "\n"
          << "   - Crate ID = " << aStub.crate << "\n"
          << "   - Crate Slot = " << aStub.slot
          << std::flush;

  return aStream;
}

} // namespace processor
} // namespace swatch

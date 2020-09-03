/**
 * @file    SystemStub.cpp
 * @author  Alessandro Thea
 * @date    11/11/14
 */

#include "swatch/system/SystemStub.hpp"

#include <ostream>

namespace swatch {
namespace system {
SystemStub::SystemStub(const std::string& aId) :
  AbstractStub(aId)
{
}

SystemStub::SystemStub(const std::string& aId, const std::string& aCreator) :
  AbstractStub(aId), creator(aCreator)
{
}

SystemStub::~SystemStub()
{
}


bool operator==(const SystemStub& aStub1, const SystemStub& aStub2)
{
  bool lResult = (aStub1.id == aStub2.id);
  lResult = lResult && (aStub1.loggerName == aStub2.loggerName);

  lResult = lResult && (aStub1.creator == aStub2.creator);
  lResult = lResult && (aStub1.excludedBoards == aStub2.excludedBoards);
  lResult = lResult && (aStub1.crates == aStub2.crates);
  lResult = lResult && (aStub1.processors == aStub2.processors);
  lResult = lResult && (aStub1.daqttcs == aStub2.daqttcs);
  lResult = lResult && (aStub1.links == aStub2.links);
  lResult = lResult && (aStub1.connectedFEDs == aStub2.connectedFEDs);

  return lResult;
}


std::ostream& operator<<(std::ostream& aStream, const swatch::system::SystemStub& aStub)
{

  aStream << "SystemStub[" << aStub.id << "]:\n"
//       << "   - Location = " << sd.location << "\n"
//       << "   - Description = " << sd.description << "\n"
          << std::flush;

  return aStream;
}

} // namespace system
} // namespace swatch


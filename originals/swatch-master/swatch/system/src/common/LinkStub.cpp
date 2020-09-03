/**
 * @file    LinkStub.cpp
 * @author  Tom Williams
 * @date    June 2015
 */

#include "swatch/system/LinkStub.hpp"


#include <ostream>


namespace swatch {
namespace system {


bool operator==(const LinkStub& aStub1, const LinkStub& aStub2)
{
  bool lResult = (aStub1.id == aStub2.id);
  lResult = lResult && (aStub1.loggerName == aStub2.loggerName);

  lResult = lResult && (aStub1.srcProcessor == aStub2.srcProcessor);
  lResult = lResult && (aStub1.srcPort == aStub2.srcPort);
  lResult = lResult && (aStub1.dstProcessor == aStub2.dstProcessor);
  lResult = lResult && (aStub1.dstPort == aStub2.dstPort);

  return lResult;
}


std::ostream& operator<<(std::ostream& aStream, const swatch::system::LinkStub& aStub)
{
  aStream << "LinkStub[" << aStub.id << "]:\n"
          << "   - source processor = " << aStub.srcProcessor << ", port = " << aStub.srcPort << "\n"
          << "   - destination processor = " << aStub.dstProcessor << ", port = " << aStub.dstPort << "\n"
          << std::flush;

  return aStream;
}


} // namespace processor
} // namespace swatch

/**
 * @file    CrateStub.cpp
 * @author  Alessandro Thea
 * @date    11/11/14
 */

#include "swatch/system/CrateStub.hpp"


#include <ostream>


namespace swatch {
namespace system {


bool operator==(const CrateStub& aStub1, const CrateStub& aStub2)
{
  bool lResult = (aStub1.id == aStub2.id);
  lResult = lResult && (aStub1.loggerName == aStub2.loggerName);

  lResult = lResult && (aStub1.description == aStub2.description);
  lResult = lResult && (aStub1.location == aStub2.location);

  return lResult;
}


std::ostream& operator<<(std::ostream& aStream, const swatch::system::CrateStub& aStub)
{
  aStream << "CrateStub[" << aStub.id << "]:\n"
          << "   - Location = " << aStub.location << "\n"
          << "   - Description = " << aStub.description << "\n"
          << std::flush;

  return aStream;
}


} // namespace system
} // namespace swatch


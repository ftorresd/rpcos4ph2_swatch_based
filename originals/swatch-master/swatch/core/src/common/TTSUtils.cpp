#include "swatch/core/TTSUtils.hpp"


#include <ostream>
#include <stdint.h>                     // for uint32_t


namespace swatch {
namespace core {
namespace tts {

std::ostream&
operator<<(std::ostream& aStream, const State& aState)
{
  switch (aState) {
    case kWarning:
      return (aStream << "Warning Overflow (0x" << std::hex <<  static_cast<uint32_t>(aState) << ")");
    case kOutOfSync:
      return (aStream << "Out of Sync (0x" << std::hex <<  static_cast<uint32_t>(aState) << ")");
    case kBusy:
      return (aStream << "Busy (0x" << std::hex <<  static_cast<uint32_t>(aState) << ")");
    case kReady:
      return (aStream << "Ready (0x" << std::hex <<  static_cast<uint32_t>(aState) << ")");
    case kError:
      return (aStream << "Error (0x" << std::hex <<  static_cast<uint32_t>(aState) << ")");
    case kDisconnected:
      return (aStream << "Disconnected (0x" << std::hex <<  static_cast<uint32_t>(aState) << ")");
    default:
      return (aStream << "Unknown");
  }
}


} // namespace tts
} // namespace core
} // namespace swatch

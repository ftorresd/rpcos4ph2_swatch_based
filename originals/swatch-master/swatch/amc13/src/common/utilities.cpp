#include "swatch/amc13/utilities.hpp"


namespace swatch {
namespace amc13 {

uint64_t
read64bCounter(::amc13::AMC13& aDriver, ::amc13::AMC13::Board aBoard, const std::string& aName )
{
  return
    ((uint64_t)aDriver.read(aBoard,aName+"_HI") << 32) +
    aDriver.read(aBoard,aName+"_LO");
}

} // namespace amc13
} // namespace swatch


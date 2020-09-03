/*
 * File:   DummySystem.hpp
 * Author: tom
 *
 * Created on 18 September 2015, 14:05
 */

#ifndef __SWATCH_DUMMY_DUMMYSYSTEM_HPP__
#define __SWATCH_DUMMY_DUMMYSYSTEM_HPP__


#include "swatch/system/System.hpp"


namespace swatch {
namespace action {
class SystemTransitionSnapshot;
}

namespace dummy {

class DummySystem : public swatch::system::System {
public:
  DummySystem(const swatch::core::AbstractStub& aStub);
  ~DummySystem();

private:
  static std::string analyseSourceOfWarning(const action::SystemTransitionSnapshot& );
  static std::string analyseSourceOfError(const action::SystemTransitionSnapshot& );
};


} // namespace dummy
} // namespace swatch


#endif  /* __SWATCH_DUMMY_DUMMYSYSTEM_HPP__ */


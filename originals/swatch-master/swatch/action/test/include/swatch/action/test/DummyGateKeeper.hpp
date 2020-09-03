/**
 * @file    DummyGateKeeper.hpp
 * @author  arose
 * @brief   Emulate a GateKeeper
 * @date    May 2015
 *
 */

#ifndef __SWATCH_ACTION_TEST_DUMMYGATEKEEPER_HPP__
#define __SWATCH_ACTION_TEST_DUMMYGATEKEEPER_HPP__

// Swatch Headers
#include "swatch/action/GateKeeper.hpp"


namespace swatch {
namespace action {
namespace test {

class DummyGateKeeper : public GateKeeper {
public:

  DummyGateKeeper();

  virtual ~DummyGateKeeper();

  // Expose gatekeeper "add" methods as public for unit testing
  void addContext( const std::string& aId, const ParametersContext_t&);
  void addSettingsContext(const std::string& aContextId, const MonitoringSettings_t& aSettingsMap);
  void addMasksContext(const std::string& aContextId, const Masks_t& aMasksSet);
  void addDisabledId(const std::string& aId);
};

} /* namespace test */
} /* namespace action */
} /* namespace swatch */

#endif /* __SWATCH_ACTION_TEST_DUMMYGATEKEEPER_HPP__ */

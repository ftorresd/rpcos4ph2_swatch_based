/**
 * @file    GateKeeper.hpp
 * @author  arose
 * @brief   Guardian to the underworld that is the online database
 * @date    May 2015
 *
 */

#ifndef __SWATCH_ACTION_GATEKEEPER_HPP__
#define __SWATCH_ACTION_GATEKEEPER_HPP__

// C++ Headers
#include <string>
#include <iosfwd>
#include <vector>

// SWATCH headers
#include "swatch/core/exception.hpp"
#include "swatch/action/MonitoringSetting.hpp"

// boost headers
#include "boost/date_time/posix_time/ptime.hpp"  // for ptime
#include "boost/smart_ptr/shared_ptr.hpp"  // for shared_ptr
#include "boost/unordered/unordered_map.hpp"  // for unordered_map
#include "boost/unordered/unordered_set.hpp"  // for unordered_set


// forward declarations
namespace xdata {
class Serializable;
}


namespace swatch {
namespace action {

class GateKeeperView;

class GateKeeper;
std::ostream& operator<<(std::ostream& aStr, const swatch::action::GateKeeper& aGateKeeper);

class GateKeeper {
  friend std::ostream& operator<<(std::ostream& aStr, const swatch::action::GateKeeper& aGateKeeper);

  /**
   * allow GateKeeperView to read private members for easier extraction
   * all access is handled via a const GateKeeper reference, so no changes are
   * are going to happen in this class
   */
  friend class GateKeeperView;

public:
  typedef boost::shared_ptr<const xdata::Serializable> Parameter_t;
  // each parameter is mapped against a path, usually
  // 'aSequenceId.aCommandId.aParameterId'
  typedef boost::unordered_map<std::string, Parameter_t> Parameters_t;
  typedef boost::shared_ptr<const Parameters_t> ParametersContext_t;
  // these pairs are then stored in different contexts, e.g. 'system.processors'
  typedef boost::unordered_map<std::string, ParametersContext_t> ParametersContextCache_t;

  // for monitoring settings
  typedef MonitoringSettingPtr_t MonitoringSetting_t;
  // monitoring settings are mapped against a state, e.g. 'HALTED'
  typedef boost::unordered_map<std::string, MonitoringSetting_t> MonitoringSettings_t;
  typedef boost::shared_ptr<const MonitoringSettings_t> SettingsContext_t;
  // these pairs are then stored in different contexts, e.g. 'system.processors'
  typedef boost::unordered_map<std::string, SettingsContext_t> SettingsContextCache_t;

  typedef boost::unordered_set<std::string> Masks_t;
  typedef boost::shared_ptr<const Masks_t> MasksContext_t;
  typedef boost::unordered_map<std::string, MasksContext_t> MasksContextCache_t;

  typedef boost::unordered_set<std::string> DisabledSet_t;

  /**
   * Constructor
   * @param aKey A global run-identifier
   */
  GateKeeper(const std::string& aKey);

  /// Destructor
  virtual ~GateKeeper();

  /**
   * Method to retreive configuration data from a specified path
   * @param aParameterId A key used to identify the configuration data being requested of the gatekeeper
   * @param aContexts A list of context identifiers (which may or may not exist) to look in for the requested parameters
   * @return the requested data, or throw if the key is not found in any context
   */
  Parameter_t get(const std::string& aSequenceId, const std::string& aCommandId, const std::string& aParameterId,
                  const std::vector<std::string>& aContextsToLookIn) const;
  MonitoringSetting_t getMonitoringSetting(const std::string& aState, const std::string& aMetricId,
      const std::vector<std::string>& aContextsToLookIn) const;
  bool getMask(const std::string& aObjId, const std::vector<std::string>& aContextsToLookIn) const;

  bool isEnabled(const std::string& aObjId) const;

  const boost::posix_time::ptime& lastUpdated();

  ParametersContextCache_t::const_iterator parametersBegin() const;
  ParametersContextCache_t::const_iterator parametersEnd() const;

  SettingsContextCache_t::const_iterator monitoringSettingsBegin() const;
  SettingsContextCache_t::const_iterator monitoringSettingsEnd() const;

  MasksContextCache_t::const_iterator masksBegin() const;
  MasksContextCache_t::const_iterator masksEnd() const;

  DisabledSet_t::const_iterator disabledFlagsBegin() const;
  DisabledSet_t::const_iterator disabledFlagsEnd() const;

protected:
  /**
   * Add a named context to the map of cached contexts
   * @param aId the name of the context
   * @param aContext a new xdata context, of which the Gatekeeper will take ownership
   */
  void add(const std::string& aId, ParametersContext_t aContext);
  void add(const std::string& aId, SettingsContext_t aContext);
  void add(const std::string& aId, MasksContext_t aContext);

  //! Add specified ID string to the set of disabled object IDs
  void addToDisabledSet(const std::string& aId);

private:

  /**
   * Method to retreive configuration data from a specified path
   * @param aParam A key used to identify the configuration data being requested of the gatekeeper
   * @param aContext A context identifier (which may or may not exist) to look in for the requested parameters
   * @return the requested data, or throw if the key is not found in any context
   */
  Parameter_t get(const std::string& aParam, const std::string& aContext) const;

  Parameter_t get(const std::string& aNamespace, const std::string& aCommandPath, const std::string& aParameterId,
                  const std::string& aContext) const;

  MonitoringSetting_t getMonitoringSetting(const std::string& aStatePath, const std::string& aMetricId,
      const std::string& aContextsToLookIn) const;
  MonitoringSetting_t getMonitoringSetting(const std::string& aMetricId, const std::string& aContextsToLookIn) const;
  bool getMask(const std::string& aObjId, const std::string& aContextsToLookIn) const;

  /// The global run-identifier
  std::string mKey;

  /// The cache of contexts
  ParametersContextCache_t mParameters;
  SettingsContextCache_t mSettings;
  MasksContextCache_t mMasks;
  DisabledSet_t mDisabledObjs;

  /// The last time a context was modified
  boost::posix_time::ptime mUpdateTime;

};


SWATCH_DEFINE_EXCEPTION ( ContextWithIdAlreadyExists )
SWATCH_DEFINE_EXCEPTION ( ParameterWithGivenIdAlreadyExistsInContext )
SWATCH_DEFINE_EXCEPTION ( MSettingWithGivenIdAlreadyExistsInContext )
SWATCH_DEFINE_EXCEPTION ( NonExistentKey )


} // namespace action
} // namespace swatch

#endif /* __SWATCH_ACTION_GATEKEEPER_HPP__ */

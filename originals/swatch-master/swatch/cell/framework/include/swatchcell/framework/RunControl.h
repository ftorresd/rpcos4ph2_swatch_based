/** **********************************************************************
 * Trigger Supervisor SWATCH framework component
 *
 * @author: Christos Lazaridis, Tom Williams
 *************************************************************************/

#ifndef __SWATCHCELL_FRAMEWORK_RUNCONTROL_H__
#define __SWATCHCELL_FRAMEWORK_RUNCONTROL_H__


#include "boost/chrono/duration.hpp"

#include "xdata/Boolean.h"
#include "xdata/String.h"
#include "xdata/UnsignedLong.h"

#include "ts/framework/CellOperation.h"
#include "ts/toolbox/Timer.h"

#include "swatchcell/framework/CellContext.h"


// Forward declarations
namespace swatch {
namespace system {
class System;
}
}


namespace swatchcellframework {


class RunControl : public tsframework::CellOperation {
public:
  static const std::string kStateHalted;
  static const std::string kStateEngaged;
  static const std::string kStateSynchronized;
  static const std::string kStateConfigured;
  static const std::string kStateAligned;
  static const std::string kStateRunning;
  static const std::string kStatePaused;

  static const std::string kTrEngage;
  static const std::string kTrColdReset;
  static const std::string kTrSetup;
  static const std::string kTrConfigure;
  static const std::string kTrAlign;
  static const std::string kTrStart;
  static const std::string kTrStop;
  static const std::string kTrPause;
  static const std::string kTrResume;

  static const std::string kConfigKey;
  static const std::string kRSKey;
  static const std::string kRunNumber;
  static const std::string kTtcMap;
  static const std::string kFedMap;
  static const std::string kTestModeFlag;

  static const std::string kConfigKeyDefaultFileValue;

  RunControl(log4cplus::Logger& aLogger, tsframework::CellAbstractContext* aContext);
  virtual ~RunControl();

  //! Returns current value of 'run number' parameter  
  xdata::UnsignedLong getRunNumber();

  //! Returns current value of subsystem configuration key
  xdata::String getConfigurationKey();

  //! Returns current value of subsystem "run settings" key
  xdata::String getRunSettingsKey();

  //! Returns current value of TTC map
  xdata::String getTtcMap();

  //! Returns current value of FED map
  xdata::String getFedMap();
  
  //! Returns flag indicating if operation is currently being run in 'test' mode
  xdata::Boolean getTestModeFlag();


protected:
  swatch::system::System& getSystem(const CellContext::SharedGuard_t& aGuard);
  
  virtual void execPostStart();
  virtual void execPreStop();

  //! Writes an error message to both the warning box and the log
  virtual void publishError ( const std::string& message );

  //! Writes a warning message to both the warning box and the log
  virtual void publishWarning ( const std::string& message );

  //TODO: Maybe remove 2nd argument of getParam
  //! Returns the parameter as a trimmed string
  std::string getParam ( const std::string& name,bool mustBeAvailable = true );

  //! Returns the bool representation of the parameter
  bool getBoolParam ( const std::string& name );

  /*!
   *  Checks that enough memory is available; issues a warning if memory is limited. Default argument values are empirical levels
   * @returns true if enough memory is available; otherwise returns false.
   */
  bool memCheck( size_t errorLimit = 200, size_t warnLimit = 500 );

  
private:

  static const boost::chrono::seconds kExecTransitionMaxWaitTime;
  static const boost::chrono::milliseconds kExecTransitionSleepDuration;

  /*!
   * @brief Returns specified parameter, dynamically casted to type T
   * @throw If parameter of supplied does not exist, or dynamic cast to type T fails
   */
  template <class T>
  T getParam(const std::string& aName);

  bool checkEngage();
  bool checkColdReset();
  bool checkSetup();
  bool checkConfigure();
  bool checkAlign();
  bool checkStart();
  bool checkStop();
  bool checkPause();
  bool checkResume();

  void execEngage();
  void execColdReset();
  void execSetup();
  void execConfigure();
  void execAlign();
  void execStart();
  void execStop();
  void execPause();
  void execResume();

  void resetting();

  bool checkGateKeeperContextIds(const CellContext::WriteGuard_t& aGuard);

  /*!
   * @brief Generic implementation of "check" method for transitions
   * @param aTransitionId ID string for the transition of the SWATCH state machine
   */
  bool checkTransition(const std::string& aFromState, const std::string& aTransitionId);

  /*!
   * @brief Generic implementation of "exec" method for transitions
   * @param aTransitionId ID string for the transition of the SWATCH state machine
   */
  void execTransition(const std::string& aFromState, const std::string& aTransitionId);
  
  /*!
   * @brief Generic implementation of "exec" method for transitions
   * @param aTransitionId ID string for the transition of the SWATCH state machine
   */
  void execTransition(const std::string& aFromState, const std::string& aTransitionId, const CellContext::SharedGuard_t& aGuard);
  
  static void disableMonitoringOnMaskedObjects(swatch::system::System& aSystem, log4cplus::Logger& aLogger);

protected:
  CellContext& mContext;
  
private:
  tstoolbox::Timer mTimer;
};


template <class T>
T RunControl::getParam(const std::string& aName)
{
  BOOST_STATIC_ASSERT_MSG( (boost::is_base_of<xdata::Serializable, T>::value) , "class T must be a descendant of xdata::Serializable" );

  std::map<std::string, xdata::Serializable*>::const_iterator lIt = getParamList().find(aName);
  if ( lIt == getParamList().end() )
    XCEPT_RAISE(tsexception::CellException, "Run control operation does not have any parameter of name '" + aName + "'");
  else if ( const T* lValue = dynamic_cast<const T*>(lIt->second) )
    return T(*lValue);
  else
    XCEPT_RAISE(tsexception::CellException, "Could not cast run control operation parameter '" + aName + "' to type " + swatch::core::demangleName(typeid(T).name()));
}


} // end ns swatchcellframework


#endif  /* __SWATCHCELL_FRAMEWORK_RUNCONTROL_H__ */


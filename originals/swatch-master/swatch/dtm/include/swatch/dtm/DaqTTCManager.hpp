/*
 * @file    DaqTTCManager.hpp
 * @author  Alessandro Thea
 * @date    July 2014
 */

#ifndef __SWATCH_DTM_DAQTTCMANAGER_HPP__
#define __SWATCH_DTM_DAQTTCMANAGER_HPP__


// C++ headers
#include <stdint.h>                     // for uint16_t, uint32_t
#include <string>
#include <vector>                       // for vector

// boost headers
#include "boost/noncopyable.hpp"

// SWATCH headers
#include "swatch/action/ActionableObject.hpp"
#include "swatch/core/exception.hpp"
#include "swatch/dtm/DaqTTCStub.hpp"


namespace swatch {

namespace core {
class AbstractStub;
template <typename DataType> class SimpleMetric;
}
namespace action {
class Transition;
class StateMachine;
}

namespace dtm {

class AMCPortCollection;
class EVBInterface;
class SLinkExpress;
class TTCInterface;


struct RunControlFSM : public boost::noncopyable {
  //! ID string of the FSM
  static const std::string kId;
  //! Initial state (i.e. halted)
  static const std::string kStateInitial;
  //! Error state
  static const std::string kStateError;
  //! 'Clock OK' state
  static const std::string kStateClockOK;
  //! Configured state
  static const std::string kStateConfigured;
  //! Running state
  static const std::string kStateRunning;
  //! Paused state
  static const std::string kStatePaused;

  //! ID string for the 'cold reset' transition (initial state to initial state)
  static const std::string kTrColdReset;
  //! ID string for the 'clock setup' transition (initial state to 'clock OK' state)
  static const std::string kTrClockSetup;
  //! ID string for the 'configure DAQ' transition ('clock OK' state to configured state)
  static const std::string kTrCfgDaq;
  //! ID string for the start transition (configred state to running state)
  static const std::string kTrStart;
  //! ID string for the pause transition (running state to paused state)
  static const std::string kTrPause;
  //! ID string for the resume transition (paused state to running state)
  static const std::string kTrResume;
  //! ID string for the stop transition (running/paused state to configured state)
  static const std::string kTrStop;

  //! The run control FSM object
  action::StateMachine& fsm;
  //! The 'cold reset' transition (initial state to initial state)
  action::Transition& coldReset;
  //! The 'clock setup' transition (initial state to 'clock OK' state)
  action::Transition& clockSetup;
  //! The 'configure DAQ' transition ('clock OK' state to configured state)
  action::Transition& cfgDaq;
  //! The 'start' transition (configured state to running state)
  action::Transition& start;
  //! The 'pause' transition (running state to paused state)
  action::Transition& pause;
  //! The 'resume' transition (paused state to running state)
  action::Transition& resume;
  //! The 'stop' transition, from paused state to configured state
  action::Transition& stopFromPaused;
  //! The 'stop' transition, from running state to configured state
  action::Transition& stopFromRunning;

  RunControlFSM(action::StateMachine& aFSM);

private:
  static action::StateMachine& addStates(action::StateMachine& aFSM);
};


class DaqTTCManager : public action::ActionableObject {
protected:
  explicit DaqTTCManager( const swatch::core::AbstractStub& aStub );

public:
  virtual ~DaqTTCManager();

  const DaqTTCStub& getStub() const;

  uint32_t getSlot() const;

  const std::string& getCrateId() const;

  uint16_t getFedId() const;

  static const uint32_t kNumAMCPorts;

  static const std::vector<std::string> kDefaultMetrics;

  static const std::vector<std::string> kDefaultMonitorableObjects;

  virtual const std::vector<std::string>& getGateKeeperContexts() const;

  //! Returns this amc13's TTC interface
  TTCInterface& getTTC();

  //! Returns this amc13's link interface
  const dtm::AMCPortCollection& getAMCPorts() const;

  //! Returns this amc13's link interface
  dtm::AMCPortCollection& getAMCPorts();

  //! Returns this amc13's SLinkExpress
  const dtm::SLinkExpress& getSLinkExpress() const;

  //! Returns this amc13's SLinkExpress
  dtm::SLinkExpress& getSLinkExpress();

protected:
  //! Register the supplied (heap-allocated) TTC interface; the child base class takes ownership of the TTC interface instance.
  TTCInterface& registerInterface( TTCInterface* aTTCInterface );

  //! Register the supplied (heap-allocated) SLink interface; the child base class takes ownership of the SLink interface instance.
  SLinkExpress& registerInterface( SLinkExpress* aSLink );

  //! Register the supplied (heap-allocated) amc ports interface; the child base class takes ownership of the link interface instance.
  AMCPortCollection& registerInterface( dtm::AMCPortCollection* aPortCollection );

  //! Register the supplied (heap-allocated) EvB interface; the child base class takes ownership of the EvB interface instance.
  EVBInterface& registerInterface(EVBInterface* aEventBuilder);

  RunControlFSM& getRunControlFSM();


private:
  DaqTTCStub mStub;

  mutable std::vector<std::string> mGateKeeperTables;

  //!
  TTCInterface* mTTC;

  //!!
  SLinkExpress* mSLink;

  //! AMC Backplane links collection
  AMCPortCollection* mAMCPorts;

  //! Event Builder interface
  EVBInterface* mEvb;

  //! Run control State Machine
  RunControlFSM mRunControlFSM;

protected:
  //! Metric for FED ID
  core::SimpleMetric<uint16_t>& mDaqMetricFedId;

private:
  DaqTTCManager( const DaqTTCManager& ); // non copyable
  DaqTTCManager& operator=( const DaqTTCManager& ); // non copyable

};

SWATCH_DEFINE_EXCEPTION(DaqTTCManagerInterfaceAlreadyDefined)

} // namespace dtm
} // namespace swatch

#endif  /* __SWATCH_DTM_DAQTTCMANAGER_HPP__ */



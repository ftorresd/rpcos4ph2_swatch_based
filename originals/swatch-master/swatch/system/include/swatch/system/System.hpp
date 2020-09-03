/*
 * File:   System.hpp
 * Author: ale
 * Date:   July 2014
 * Created on July 13, 2014, 11:20 AM
 */

#ifndef __SWATCH_SYSTEM_SYSTEM_HPP__
#define __SWATCH_SYSTEM_SYSTEM_HPP__


// C++ headers
#include <string>

// boost headers
#include "boost/noncopyable.hpp"
#include "boost/unordered_map.hpp"

// SWATCH headers
#include "swatch/action/ActionableSystem.hpp"
#include "swatch/system/SystemStub.hpp"


// Forward declarations
namespace swatch {

namespace action {
class SystemTransition;
class SystemStateMachine;
}

namespace processor {
class Processor;
}

namespace dtm {
class DaqTTCManager;
}

namespace system {

class Link;
class Crate;
class Service;

struct RunControlFSM : boost::noncopyable {
  //! ID string of the swatch FSM object
  static const std::string kId;
  //! Initial state (i.e. halted)
  static const std::string kStateInitial;
  //! Error state
  static const std::string kStateError;
  //! Synchronised state
  static const std::string kStateSync;
  //! Configured state
  static const std::string kStateConfigured;
  //! Aligned state
  static const std::string kStateAligned;
  //! Running state
  static const std::string kStateRunning;
  //! Paused state
  static const std::string kStatePaused;

  //! ID string for the 'cold reset' transition (initial state to initial state)
  static const std::string kTrColdReset;
  //! ID string for the 'setup' transition (initial state to synchronised state)
  static const std::string kTrSetup;
  //! ID string for the 'configure' transition (synchronised state to configured state)
  static const std::string kTrConfigure;
  //! ID string for the 'align' transition (configured state to aligned state)
  static const std::string kTrAlign;
  //! ID string for the 'start' transition (aligned state to running state)
  static const std::string kTrStart;
  //! ID string for the 'pause' transition (running state to paused state)
  static const std::string kTrPause;
  //! ID string for the 'resume' transition (paused state to running state)
  static const std::string kTrResume;
  //! ID string for the 'stop' transitions (running/paused state to configured state)
  static const std::string kTrStop;

  //! The run control swatch FSM object
  action::SystemStateMachine& fsm;
  //! The 'cold reset' transition (initial state to initial state)
  action::SystemTransition& coldReset;
  //! The 'setup' transition (initial state to synchronised state)
  action::SystemTransition& setup;
  //! The 'configure' transition (synchronised state to configured state)
  action::SystemTransition& configure;
  //! The 'align' transition (configured state to aligned state)
  action::SystemTransition& align;
  //! The 'start' transition (aligned state to running state)
  action::SystemTransition& start;
  //! The 'pause' transition (running state to paused state)
  action::SystemTransition& pause;
  //! The 'resume' transition (paused state to running state)
  action::SystemTransition& resume;
  //! The 'stop' transition from aligned state, to configured state
  action::SystemTransition& stopFromAligned;
  //! The 'stop' transition from running state, to configured state
  action::SystemTransition& stopFromRunning;
  //! The 'stop' transition from paused state, to configured state
  action::SystemTransition& stopFromPaused;

  RunControlFSM(action::SystemStateMachine& aFSM);

private:
  static action::SystemStateMachine& addStates(action::SystemStateMachine& aFSM);
};


//! Generic class representing a system of one or mores processors
class System : public action::ActionableSystem {
public:

  typedef boost::unordered_map<std::string, Crate*> CratesMap_t;

  System( const swatch::core::AbstractStub& aStub );
  virtual ~System();

  const SystemStub& getStub() const;

  const std::deque<processor::Processor*>& getProcessors() ;
  const std::deque<Service*>& getServices();
  const std::deque<dtm::DaqTTCManager*>& getDaqTTCs();
  const std::deque<system::Link*>& getLinks();
  const CratesMap_t& getCrates();

  bool hasCrate(const std::string& aCrateId) const;

protected:
  virtual void retrieveMetricValues() {}

  RunControlFSM& getRunControlFSM();

private:

  void add( processor::Processor* aProcessor );
  void add( dtm::DaqTTCManager* aAMC13 );
  void add( system::Link* aLink );
  void add( system::Service* aService );
  void add( system::Crate* aCrate );

  void addCrates();
  void addProcessors();
  void addDaqTTCs();
  void addLinks();

  void validateConnectedFEDs();

  const SystemStub mStub;

  //! List of processors
  std::deque<processor::Processor*> mProcessors;

  //! List of AMC13s
  std::deque<dtm::DaqTTCManager*> mDaqTtc;

  //! List of services
  std::deque<Service*> mServices;

  //! List of internal links
  std::deque<Link*> mLinks;

  //! Map of crates
  CratesMap_t mCratesMap;

  RunControlFSM mRunControlFSM;
};

SWATCH_DEFINE_EXCEPTION(SystemConstructionFailed)

}
}

#endif  /* __SWATCH_SYSTEM_SYSTEM_HPP__ */


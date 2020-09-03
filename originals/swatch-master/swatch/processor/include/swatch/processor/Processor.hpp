/**
 * @file    Processor.hpp
 * @author  Alessandro Thea
 * @brief   Processor abstract interface
 * @date    1 July 2014
 */

#ifndef __SWATCH_PROCESSOR_PROCESSOR_HPP__
#define __SWATCH_PROCESSOR_PROCESSOR_HPP__


// Standard headers
#include <stdint.h>                     // for uint32_t, uint64_t
#include <string>                       // for string
#include <vector>                       // for vector

// boost headers
#include "boost/noncopyable.hpp"

// SWATCH headers
#include "swatch/action/ActionableObject.hpp"
#include "swatch/core/exception.hpp"
#include "swatch/processor/ProcessorStub.hpp"


namespace swatch {

namespace core {
class AbstractStub;
template <typename DataType> class SimpleMetric;
}
namespace action {
class Transition;
class StateMachine;
}

namespace processor {

// Forward declarations
class TTCInterface;
class ReadoutInterface;
class AlgoInterface;
class InputPortCollection;
class OutputPortCollection;


struct RunControlFSM : public boost::noncopyable {
  //! FSM ID string
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

  //! Cold reset transition (initial state to initial state)
  static const std::string kTrColdReset;
  //! ID string for the 'setup' transition (initial state to synchronised state)
  static const std::string kTrSetup;
  //! ID string for the 'configure' transition (synchronised state to configured state)
  static const std::string kTrConfigure;
  //! ID string for the 'align' transition (configured state to aligned state)
  static const std::string kTrAlign;
  //! ID string for the 'start' transition (aligned state to running state)
  static const std::string kTrStart;
  //! ID string for the 'stop' transition (aligned/running state to configured state)
  static const std::string kTrStop;

  //! The run control FSM object
  action::StateMachine& fsm;
  //! The 'cold reset' transition (initial state to initial state)
  action::Transition& coldReset;
  //! The 'setup' transition (initial state to synchronised state)
  action::Transition& setup;
  //! The 'configure' transition (synchronised state to configured state)
  action::Transition& configure;
  //! The 'align' transition (configured state to aligned state)
  action::Transition& align;
  //! The 'start' transition (aligned state to running state)
  action::Transition& start;
  //! The 'stop' transition from aligned state, to configured state
  action::Transition& stopFromAligned;
  //! The 'stop' transition from running state to configured state
  action::Transition& stopFromRunning;

  RunControlFSM(action::StateMachine& aFSM);

private:
  static action::StateMachine& addStates(action::StateMachine& aFSM);
};



class Processor : public action::ActionableObject {
protected:
  Processor(const swatch::core::AbstractStub& );
public:

  virtual ~Processor();

  const ProcessorStub& getStub() const;

  /**
   * Returns the board slot number
   * @return the board's slot number in the crate
   */
  uint32_t getSlot() const;

  /**
   * Processor crate ID getter
   * @return ID of the crate this processor that this processor is in
   */
  const std::string& getCrateId() const;

  //! Constant corresponding to no slot being assigned
  static const uint32_t kNoSlot;

//    /**
//     * Additional firmware information.
//     * The string is meant to be informative for the user
//     * @details [long description]
//     * @return String containing additional firmware informations
//     */
//    virtual std::string firmwareInfo() const = 0;

  //! Returns this processor's TTC interface
  const TTCInterface& getTTC() const;

  //! Returns this processor's TTC interface
  TTCInterface& getTTC();

  //! Returns this processor's readout interface
  const ReadoutInterface& getReadout() const;

  //! Returns this processor's readout interface
  ReadoutInterface& getReadout();

  //! Returns this processor's algo interface
  const AlgoInterface& getAlgo() const;

  //! Returns this processor's algo interface
  AlgoInterface& getAlgo();

  //! Returns this processor's input port collection
  const InputPortCollection& getInputPorts() const;

  //! Returns this processor's input port collection
  InputPortCollection& getInputPorts();

  //! Returns this processor's output port collection
  const OutputPortCollection& getOutputPorts() const;

  //! Returns this processor's output port collection
  OutputPortCollection& getOutputPorts();


  static const std::vector<std::string> kDefaultMetrics;

  static const std::vector<std::string> kDefaultMonitorableObjects;

  virtual const std::vector<std::string>& getGateKeeperContexts() const;

protected:

  //! Register the supplied (heap-allocated) TTC interface in this processor; the processor base class takes ownership of the TTC interface instance.
  TTCInterface& registerInterface( TTCInterface* aTTCInterface );

  //! Register the supplied (heap-allocated) readout interface in this processor; the processor base class takes ownership of the readout interface instance.
  ReadoutInterface& registerInterface( ReadoutInterface* aReadoutInterface );

  //! Register the supplied (heap-allocated) algo interface in this processor; the processor base class takes ownership of the algo interface instance.
  AlgoInterface& registerInterface( AlgoInterface* aAlgoInterface );

  //! Register the supplied (heap-allocated) input port collection in this processor; the processor base class takes ownership of the supplied port collection.
  InputPortCollection& registerInterface( InputPortCollection* aPortCollection );

  //! Register the supplied (heap-allocated) output port collection in this processor; the processor base class takes ownership of the supplied port collection.
  OutputPortCollection& registerInterface( OutputPortCollection* aPortCollection );

  //! Firmware version metric
  core::SimpleMetric<uint64_t>& mMetricFirmwareVersion;

  RunControlFSM& getRunControlFSM();

private:
  const ProcessorStub mStub;

  //! TTC control interface
  TTCInterface* mTTC;

  //! Readout control interface
  ReadoutInterface* mReadout;

  //! Algorithm control interface
  AlgoInterface* mAlgo;

  //! Optical input ports
  InputPortCollection* mInputPorts;

  //! Collection of optical output ports
  OutputPortCollection* mOutputPorts;

  mutable std::vector<std::string> mGateKeeperTables;

  RunControlFSM mRunControlFSM;

private:
  Processor( const Processor& ); // non copyable
  Processor& operator=( const Processor& ); // non copyable
};


SWATCH_DEFINE_EXCEPTION(InterfaceAlreadyDefined)
SWATCH_DEFINE_EXCEPTION(InterfaceNotDefined)


}
}

#endif  /* __SWATCH_PROCESSOR_PROCESSOR_HPP__ */


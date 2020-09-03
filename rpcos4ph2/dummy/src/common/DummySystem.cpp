
#include "swatch/dummy/DummySystem.hpp"


#include "swatch/core/Factory.hpp"
#include "swatch/action/SystemStateMachine.hpp"
#include "swatch/processor/Processor.hpp"
#include "swatch/processor/PortCollection.hpp"
#include "swatch/processor/Port.hpp"
#include "swatch/dtm/DaqTTCManager.hpp"
#include "swatch/dummy/utilities.hpp"


SWATCH_REGISTER_CLASS(swatch::dummy::DummySystem)


namespace swatch {
namespace dummy {

DummySystem::DummySystem(const swatch::core::AbstractStub& aStub) :
  swatch::system::System(aStub)
{
  // 1) Add system-level metrics 
  std::vector<core::AbstractMetric*> lCRCErrorMetrics;
  std::vector<core::AbstractMetric*> lPortCountMetrics;
  for (auto lProcIt=getProcessors().begin(); lProcIt != getProcessors().end(); lProcIt++) {
    processor::Processor& lProc = **lProcIt;
    lPortCountMetrics.push_back(&lProc.getMetric("portsInError"));
    lCRCErrorMetrics.push_back(&lProc.getMetric("totalCRCErrors"));
  }

  registerComplexMetric<uint32_t>("totalCRCErrors", lCRCErrorMetrics.begin(), lCRCErrorMetrics.end(), core::ComplexMetric<uint32_t>::CalculateFunction_t(&sumUpCRCErrors), filterOutDisabledActionables);
  registerComplexMetric<uint32_t>("portsInError", lPortCountMetrics.begin(), lPortCountMetrics.end(), core::ComplexMetric<uint32_t>::CalculateFunction_t(&sumUpCRCErrors), filterOutDisabledActionables);

  // 2) Define 'run control' FSM transitions 
  typedef processor::RunControlFSM ProcFSM_t;
  typedef dtm::RunControlFSM DaqTTCFSM_t;

  system::RunControlFSM& fsm = getRunControlFSM();
  fsm.coldReset.add(getDaqTTCs(), DaqTTCFSM_t::kStateInitial, DaqTTCFSM_t::kTrColdReset)
               .add(getProcessors(), ProcFSM_t::kStateInitial, ProcFSM_t::kTrColdReset);

  fsm.setup.add(getDaqTTCs(), DaqTTCFSM_t::kStateInitial, DaqTTCFSM_t::kTrClockSetup)
           .add(getProcessors(), ProcFSM_t::kStateInitial, ProcFSM_t::kTrSetup)
           .add(getDaqTTCs(), DaqTTCFSM_t::kStateClockOK, DaqTTCFSM_t::kTrCfgDaq);

  fsm.configure.add(getProcessors(), ProcFSM_t::kStateSync, ProcFSM_t::kTrConfigure);

  fsm.align.add(getProcessors(), ProcFSM_t::kStateConfigured, ProcFSM_t::kTrAlign);

  fsm.start.add(getProcessors(), ProcFSM_t::kStateAligned, ProcFSM_t::kTrStart)
           .add(getDaqTTCs(), DaqTTCFSM_t::kStateConfigured, DaqTTCFSM_t::kTrStart);

  fsm.pause.add(getDaqTTCs(), DaqTTCFSM_t::kStateRunning, DaqTTCFSM_t::kTrPause);

  fsm.resume.add(getDaqTTCs(), DaqTTCFSM_t::kStatePaused, DaqTTCFSM_t::kTrResume);

  fsm.stopFromAligned.add(getProcessors(), ProcFSM_t::kStateAligned, ProcFSM_t::kTrStop);

  fsm.stopFromRunning.add(getDaqTTCs(), DaqTTCFSM_t::kStateRunning, DaqTTCFSM_t::kTrStop)
                     .add(getProcessors(), ProcFSM_t::kStateRunning, ProcFSM_t::kTrStop);

  fsm.stopFromPaused.add(getDaqTTCs(), DaqTTCFSM_t::kStatePaused, DaqTTCFSM_t::kTrStop)
                    .add(getProcessors(), ProcFSM_t::kStateRunning, ProcFSM_t::kTrStop);

  fsm.setup.registerWarningAnalyser(&analyseSourceOfWarning);
  fsm.configure.registerWarningAnalyser(&analyseSourceOfWarning);
  fsm.align.registerWarningAnalyser(&analyseSourceOfWarning);

  fsm.setup.registerErrorAnalyser(&analyseSourceOfError);
  fsm.configure.registerErrorAnalyser(&analyseSourceOfError);
  fsm.align.registerErrorAnalyser(&analyseSourceOfError);
}


DummySystem::~DummySystem()
{
}


std::string DummySystem::analyseSourceOfWarning(const action::SystemTransitionSnapshot& aSystemSnapshot)
{
  std::vector<std::pair<std::string,std::string> > lOffendingIds;
  // Loop over steps ...
  action::SystemTransitionSnapshot::const_iterator lStepIt;
  for (lStepIt = aSystemSnapshot.begin(); lStepIt !=aSystemSnapshot.end(); lStepIt++) {

    // Loop over single-object transitions within a step ...
    action::SystemTransitionSnapshot::Step_t::const_iterator lObjectTransitionIt;
    for (lObjectTransitionIt = lStepIt->begin(); lObjectTransitionIt != lStepIt->end(); lObjectTransitionIt++) {

      // Check that this single-object transition snapshot was executed (i.e. that the board/crate wasn't excluded from run)
      if (*lObjectTransitionIt) {
        // Loop over commands within a single-object transition ...
        action::TransitionSnapshot::const_iterator lCommandIt;
        for (lCommandIt = (*lObjectTransitionIt)->begin(); lCommandIt != (*lObjectTransitionIt)->end(); lCommandIt++) {
          const action::CommandSnapshot& lCommand = *lCommandIt;

          // Basic analysis: Add names of offending commands & boards to the set
          if (lCommand.getState() == action::Functionoid::State::kWarning)
            lOffendingIds.push_back( std::make_pair(lCommand.getActionableId(), lCommand.getActionId()) );
        }
      }
    }
  }
  std::ostringstream lOutput;
  lOutput << "Warning is coming from " << lOffendingIds.size() << " boards/commands ... ";
  for (auto lIt=lOffendingIds.begin(); lIt != lOffendingIds.end(); lIt++) {
    lOutput << lIt->first << ":" << lIt->second;
    if (lIt != lOffendingIds.end())
      lOutput << ", ";
  }
  return lOutput.str();
}


std::string DummySystem::analyseSourceOfError(const action::SystemTransitionSnapshot& aSystemSnapshot)
{
  std::vector<std::pair<std::string,std::string> > lOffendingIds;
  // Loop over steps ...
  action::SystemTransitionSnapshot::const_iterator lStepIt;
  for (lStepIt = aSystemSnapshot.begin(); lStepIt !=aSystemSnapshot.end(); lStepIt++) {

    // Loop over single-object transitions within a step ...
    action::SystemTransitionSnapshot::Step_t::const_iterator lObjectTransitionIt;
    for (lObjectTransitionIt = lStepIt->begin(); lObjectTransitionIt != lStepIt->end(); lObjectTransitionIt++) {

      // Check that this single-object transition snapshot was executed (i.e. that the board/crate wasn't excluded from run)
      if (*lObjectTransitionIt) {
        // Loop over commands within a single-object transition ...
        action::TransitionSnapshot::const_iterator lCommandIt;
        for (lCommandIt = (*lObjectTransitionIt)->begin(); lCommandIt != (*lObjectTransitionIt)->end(); lCommandIt++) {
          const action::CommandSnapshot& lCommand = *lCommandIt;

          // Basic analysis: Add names of offending commands & boards to the set
          if (lCommand.getState() == action::Functionoid::State::kError)
            lOffendingIds.push_back( std::make_pair(lCommand.getActionableId(), lCommand.getActionId()) );
        }
      }
    }
  }
  std::ostringstream lOutput;
  lOutput << "Error is coming from " << lOffendingIds.size() << " boards/commands ... ";
  for (auto lIt=lOffendingIds.begin(); lIt != lOffendingIds.end(); lIt++) {
    lOutput << lIt->first << ":" << lIt->second;
    if (lIt != lOffendingIds.end())
      lOutput << ", ";
  }
  return lOutput.str();
}


} // namespace dummy
} // namespace swatch

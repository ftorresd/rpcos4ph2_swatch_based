/*
 * File:   TestMP7Processor.cpp
 * Author: ale
 *
 * Created on December 8, 2015, 2:19 PM
 */


#include "swatch/mp7/test/TestMP7Processor.hpp"

// XDAQ Headers
// #include <xdata/UnsignedInteger.h>
// #include <xdata/String.h>
// #include <xdata/Vector.h>
#include "swatch/xsimpletypedefs.hpp"

// SWATCH Headers
#include "swatch/core/Factory.hpp"
#include "swatch/core/toolbox/IdSliceParser.hpp"
#include "swatch/core/utilities.hpp"
#include "swatch/processor/PortCollection.hpp"
#include "swatch/mp7/MP7Ports.hpp"
#include "swatch/mp7/PointValidator.hpp"
#include "swatch/mp7/cmds/AlignRxsTo.hpp"
#include "swatch/mp7/cmds/OrbitConstraint.hpp"
#include "swatch/core/ReadWriteXParameterSet.hpp"
#include "swatch/action/CommandSequence.hpp"

// MP7 Headers
#include "mp7/definitions.hpp"
#include "mp7/ChannelManager.hpp"
#include "mp7/MP7Controller.hpp"

// Boost Headers
#include <boost/foreach.hpp>
#include <boost/range/adaptor/map.hpp>
#include <boost/range/adaptor/filtered.hpp>
#include <boost/range/algorithm/copy.hpp>

SWATCH_REGISTER_CLASS(swatch::mp7::test::TestMP7Processor);

namespace swatch {
namespace mp7 {
namespace test {


// ----------------------------------------------------------------------------
TestMP7Processor::TestMP7Processor(const swatch::core::AbstractStub& aStub) :
  MP7NullAlgoProcessor(aStub)
{
  // registerCommand<PrintRxDescriptors>("printRxDescriptors");
  registerCommand<TMTAlign>("tmtAlign");

  registerSequence("mgtLoopbackTest", CmdIds::kReset)
      .then(CmdIds::kCfgTxMGTs)
      .then(CmdIds::kCfgTxBuffers)
      .then(CmdIds::kCfgRxMGTs)
      .then(CmdIds::kAutoAlignMGTs)
      ;
  
  registerSequence("zsConfiguration", CmdIds::kSetupZS).then(CmdIds::kLoadZSMenu);

}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
TestMP7Processor::~TestMP7Processor()
{
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
PrintRxDescriptors::PrintRxDescriptors(const std::string& aId, action::ActionableObject& aResource) :
  swatch::action::Command(aId, aResource, xdata::String()),
  mCore(*this)
{
  mCore.registerParameters();
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
swatch::action::Command::State
PrintRxDescriptors::code(const core::XParameterSet& aParams)
{
  ::mp7::ChannelManager mgr = mCore.manager(aParams);

  auto channels = mgr.getDescriptor().pickRxBufferIDs(::mp7::kBuffer).channels();
  setResult(xdata::String( std::string("Selected channels : "+core::joinAny(channels)) ));

  return State::kWarning;
}
// ----------------------------------------------------------------------------

const std::string TmtParameterMapper::kBxParam = "bx";
const std::string TmtParameterMapper::kCycleParam = "cycle";
const std::string TmtParameterMapper::kTmtBxParam = "tmtBx";
const std::string TmtParameterMapper::kTmtCycleParam = "tmtCycle";
const std::string TmtParameterMapper::kTmtIdParam = "tmtId";
const std::string TmtParameterMapper::kOrbitConstr = "pointCheck";

// ----------------------------------------------------------------------------
void TmtParameterMapper::replaceParameters(swatch::action::Command& aCommand)
{
  
  aCommand.removeConstraint(kOrbitConstr);
  aCommand.unregisterParameter(kBxParam);
  aCommand.unregisterParameter(kCycleParam);

  aCommand.registerParameter(kTmtBxParam, XUInt_t(0x0));
  aCommand.registerParameter(kTmtCycleParam, XUInt_t(0x0));
  aCommand.registerParameter(kTmtIdParam, XUInt_t(0x0));

  // Query the MP7 driver
  ::mp7::MP7MiniController& lDriver = aCommand.getActionable<MP7AbstractProcessor>().driver();
  // to access the metric
  ::mp7::orbit::Metric lMetric = lDriver.getMetric();

  aCommand.addConstraint(kOrbitConstr, cmds::OrbitConstraint(kTmtBxParam, kTmtCycleParam, lMetric));

}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
core::ReadOnlyXParameterSet
TmtParameterMapper::rebuild(const core::ReadOnlyXParameterSet& aParams, const ::mp7::orbit::Metric& aMetric)
{
  XUInt_t tmtBx = aParams.get<XUInt_t>(kTmtBxParam);
  XUInt_t tmtCycle = aParams.get<XUInt_t>(kTmtCycleParam);
  XUInt_t tmtSlot = aParams.get<XUInt_t>(kTmtIdParam);

  // Copy the parameter set
  swatch::core::ReadOnlyXParameterSet lParams(aParams);

  // Strip parameters
  lParams.erase(kTmtBxParam);
  lParams.erase(kTmtCycleParam);
  lParams.erase(kTmtIdParam);

  orbit::PointValidator lTmtVal(tmtBx, tmtCycle, aMetric);

  // Ensure that tmt point is a valid point
  lTmtVal.throwIfInvalid();

  ::mp7::orbit::Point lTmtPoint = aMetric.addBXs(lTmtVal, tmtSlot);

  boost::shared_ptr<XUInt_t> lBx( new XUInt_t(lTmtPoint.bx) );
  boost::shared_ptr<XUInt_t> lCycle( new XUInt_t(lTmtPoint.cycle) );

  lParams.adopt(kBxParam, lBx);
  lParams.adopt(kCycleParam, lCycle);

  return lParams;
}
// ----------------------------------------------------------------------------


//
// Parameter override tests
//
// ----------------------------------------------------------------------------
TMTAlign::TMTAlign(const std::string& aId, swatch::action::ActionableObject& aActionable):
  cmds::AlignRxsTo(aId, aActionable)
{
  TmtParameterMapper::replaceParameters(*this);
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
action::Functionoid::State
TMTAlign::code(const ::swatch::core::XParameterSet& aParams)
{
  // FIXME: Embed this check somewhere
  ::mp7::MP7MiniController& driver = getActionable<MP7AbstractProcessor>().driver();
  ::mp7::orbit::Metric lMetric = driver.getMetric();

  swatch::core::ReadOnlyXParameterSet lParams = TmtParameterMapper::rebuild(aParams, lMetric);

  return cmds::AlignRxsTo::code(lParams);
}
// ----------------------------------------------------------------------------

} // namespace test
} // namespace mp7
} // namespace swatch

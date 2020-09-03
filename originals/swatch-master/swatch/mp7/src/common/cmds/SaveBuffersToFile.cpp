#include "swatch/mp7/cmds/SaveBuffersToFile.hpp"

// SWATCH headers
#include "swatch/mp7/cmds/BufferTraits.hpp"
#include "swatch/mp7/MP7AbstractProcessor.hpp"
#include "swatch/mp7/PointValidator.hpp"
#include "swatch/core/utilities.hpp"

// MP7 Core Headers
#include "mp7/MP7MiniController.hpp"    // for MP7MiniController
#include "mp7/CtrlNode.hpp"             // for CtrlNode

// boost headers
#include <boost/filesystem.hpp>


namespace swatch {
namespace mp7 {
namespace cmds {


// ----------------------------------------------------------------------------
template<class Selector>
SaveBuffersToFile<Selector>::SaveBuffersToFile(const std::string& aId, swatch::action::ActionableObject& aActionable) :
  swatch::action::Command(aId, aActionable, xdata::String()),
  mBufferSelector(*this)
{

  mBufferSelector.registerParameters();

  registerParameter("filename", xdata::String(""));
}
// ----------------------------------------------------------------------------



// ----------------------------------------------------------------------------
template<class Selector>
action::Command::State SaveBuffersToFile<Selector>::code(const ::swatch::core::XParameterSet& params)
{

  // Get the corresponding buffer kind
  const ::mp7::RxTxSelector bKind = BufferTraits<Selector>::kRxTxSelector;

  std::string filename = params.get<xdata::String>("filename").value_;

  ::mp7::MP7MiniController& driver = getActionable< MP7AbstractProcessor>().driver();

  ::mp7::CtrlNode ctrl = driver.getCtrl();
  ::mp7::ChannelManager cm = mBufferSelector.manager(params);

  // TOFIX: Output file should be compulsory
  if (filename == "") {
    std::ostringstream fn;
    fn << bKind << "_summary.txt";
    filename = fn.str();
  }
  else {
    // Expand variables contained in the filename
    filename = swatch::core::shellExpandPath(filename);

    boost::filesystem::path p(filename);
    if (!boost::filesystem::exists(p.parent_path())) {
      try {
        boost::filesystem::create_directories(p.parent_path());
      }
      catch (std::exception& e) {
        std::ostringstream err;
        err << "Exception caught. Cannot create dir " << p.parent_path() << ". Please provide a valid path for captures.";
        setStatusMsg(err.str());
        return State::kError;
      }
    }
  }

  setProgress(0.1, "Reading buffers...");

  ::mp7::BoardData data = cm.readBuffers(bKind);

  setProgress(0.5, "Saving data...");

  ::mp7::BoardDataFactory::saveToFile(data, filename);

  setStatusMsg("Data save to file "+filename+" for buffers: "+core::joinAny(mBufferSelector.extractIds(cm)) );
  return State::kDone;
}
// ----------------------------------------------------------------------------

// Template Instance
template class SaveBuffersToFile<RxBufferSelector>;
template class SaveBuffersToFile<TxBufferSelector>;



} // namespace cmds
} // namespace mp7
} // namespace swatch


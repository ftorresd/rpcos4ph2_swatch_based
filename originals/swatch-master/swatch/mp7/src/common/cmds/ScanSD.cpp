#include "swatch/mp7/cmds/ScanSD.hpp"

// log4cplus headers
#include "log4cplus/loggingmacros.h"

// SWATCH headers
#include "swatch/mp7/MP7Processor.hpp"

#include "mp7/MmcController.hpp"


namespace swatch {
namespace mp7 {
namespace cmds {

// ----------------------------------------------------------------------------
ScanSD::ScanSD(const std::string& aId, swatch::action::ActionableObject& aActionable) :
  Command(aId, aActionable, xdata::String())
{
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
::swatch::action::Command::State
ScanSD::code(const ::swatch::core::XParameterSet& aParams)
{
  MP7AbstractProcessor& p = getActionable<MP7AbstractProcessor>();

  setProgress(0., "Scanning SD card...");

  ::mp7::MmcPipeInterface mmcNode(p.driver().hw().getNode< ::mp7::MmcPipeInterface>("uc"));
  std::vector<std::string> fileNames = mmcNode.ListFilesOnSD();

  std::string fileNameString = boost::algorithm::join(fileNames, ",  ");
  //for(int i=0;i<fileNames.size();++i){

  setStatusMsg("Scan SD complete!");
  setResult(xdata::String(fileNameString));

  return State::kDone;
}
// ----------------------------------------------------------------------------



} // namespace cmds
} // namespace mp7
} // namespace swatch


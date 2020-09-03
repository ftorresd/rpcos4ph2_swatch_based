#include "swatch/mp7/cmds/HeaderFormatter.hpp"

#include "swatch/xsimpletypedefs.hpp"

namespace swatch {
namespace mp7 {
namespace cmds {

// ----------------------------------------------------------------------------
HeaderFormatter::HeaderFormatter(const std::string& aId, swatch::action::ActionableObject& aActionable) :
  swatch::action::Command(aId, aActionable, xdata::String()),
  mFmtSelector(*this,
               (boost::bind(&ChannelDescriptor::getFormatterKind, _1) == ::mp7::kTDRFormatter) ||
               (boost::bind(&ChannelDescriptor::getFormatterKind, _1) == ::mp7::kDemuxFormatter)
              )
{

  // add default parameters
  mFmtSelector.registerParameters();

  registerParameter("strip", XBool_t(true));
  registerParameter("insert", XBool_t(true));
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
action::Command::State HeaderFormatter::code(const swatch::core::XParameterSet& params)
{

  bool strip = (bool)params.get<XBool_t>("strip");
  bool insert = (bool)params.get<XBool_t>("insert");

  ::mp7::ChannelManager cm = mFmtSelector.manager(params);
  //  IOChannelManager cm = mFmtSelector.manager(params);

  setProgress(0.0, "Configuring header formatting...");

  //
  cm.configureHdrFormatters(::mp7::kTDRFormatter, strip, insert);
  cm.configureHdrFormatters(::mp7::kDemuxFormatter, strip, insert);

  setStatusMsg("Configure TDR Formatting complete");

  return State::kDone;
}
// ----------------------------------------------------------------------------



} // namespace cmds
} // namespace mp7
} // namespace swatch


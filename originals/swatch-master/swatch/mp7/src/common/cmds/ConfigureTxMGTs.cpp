#include "swatch/mp7/cmds/ConfigureTxMGTs.hpp"

#include "swatch/xsimpletypedefs.hpp"
#include "swatch/core/utilities.hpp"


namespace swatch {
namespace mp7 {
namespace cmds {

// ----------------------------------------------------------------------------
ConfigureTxMGTs::ConfigureTxMGTs(const std::string& aId, swatch::action::ActionableObject& aActionable) :
  swatch::action::Command(aId, aActionable, xdata::String()),
  mTxMGTSelector(*this)
{

  mTxMGTSelector.registerParameters();

  registerParameter("orbitTag", XBool_t(false));
  registerParameter("loopback", XBool_t(false));
  registerParameter("polarity", XBool_t(true));

}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
swatch::action::Command::State
ConfigureTxMGTs::code(const swatch::core::XParameterSet& params)
{
  const XBool_t& orbitTag = params.get<XBool_t>("orbitTag");
  const XBool_t& loopback = params.get<XBool_t>("loopback");
  const XBool_t& polarity = params.get<XBool_t>("polarity");

  ::mp7::ChannelManager cm = mTxMGTSelector.manager(params);

  setProgress(0.0, "Configuring Tx MGTs");

  if (loopback) {
    setProgress(0.1, "Generating loopback pattern");
    if (orbitTag)
      cm.setupTx2RxOrbitPattern();
    else
      cm.setupTx2RxPattern();
  }

  setProgress(0.3, "Configuring links...");

  try {
    cm.configureTxMGTs(loopback, polarity);
  }
  catch (std::exception& e) {
    std::ostringstream err;
    err << "Exception caught while trying to configure Tx MGTs: " << e.what();
    setStatusMsg(err.str());
    return State::kError;
  }

  setStatusMsg("Configure Tx MGTs completed: "+core::joinAny(mTxMGTSelector.extractIds(cm)));
  return State::kDone;
}
// ----------------------------------------------------------------------------



} // namespace cmds
} // namespace mp7
} // namespace swatch


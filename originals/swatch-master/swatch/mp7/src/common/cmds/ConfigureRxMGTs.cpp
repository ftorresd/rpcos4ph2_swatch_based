#include "swatch/mp7/cmds/ConfigureRxMGTs.hpp"

#include "swatch/xsimpletypedefs.hpp"
#include "swatch/core/utilities.hpp"


namespace swatch {
namespace mp7 {
namespace cmds {

// ----------------------------------------------------------------------------
ConfigureRxMGTs::ConfigureRxMGTs(const std::string& aId, swatch::action::ActionableObject& aActionable) :
  swatch::action::Command(aId, aActionable, xdata::String()),
  mRxMGTSelector(*this)
{

  mRxMGTSelector.registerParameters();
  registerParameter("orbitTag", XBool_t(false));
  registerParameter("polarity", XBool_t(true));

}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
swatch::action::Command::State
ConfigureRxMGTs::code(const swatch::core::XParameterSet& params)
{
  const XBool_t&  orbitTag = params.get<XBool_t>("orbitTag");
  const XBool_t& polarity = params.get<XBool_t>("polarity");

//  IOChannelManager cm = mRxMGTSelector.manager(params);
  ::mp7::ChannelManager cm = mRxMGTSelector.manager(params);

  setProgress(0.0, "Configuring Rx MGTs...");

  try {
    cm.configureRxMGTs(orbitTag, polarity);
  }
  catch (const ::mp7::exception& aExc ) {

    std::ostringstream lErr;
    lErr << "Exception caught while trying to configure Rx MGTs: " << aExc.what();
    setStatusMsg(lErr.str());
    return State::kError;

  }
  catch (const std::exception& aExc) {

    std::ostringstream lErr;
    lErr << "Exception caught while trying to configure Rx MGTs: " << aExc.what();
    setStatusMsg(lErr.str());
    return State::kError;

  }

  setStatusMsg("Configure Rx MGTs completed: "+core::joinAny(mRxMGTSelector.extractIds(cm)));
  return State::kDone;
}
// ----------------------------------------------------------------------------

} // namespace cmds
} // namespace mp7
} // namespace swatch

/*************************************************************************
 * Trigger Supervisor SWATCH worker                                      *
 *                                                                       *
 * Authors: Carlos Ghabrous Larrea         				         *
 *************************************************************************/

#ifndef __SWATCHCELL_FRAMEWORK_SYSTEMSTATEMACHINEPANEL_H__
#define __SWATCHCELL_FRAMEWORK_SYSTEMSTATEMACHINEPANEL_H__


#include "swatch/action/ActionableObject.hpp"
#include "swatch/action/SystemStateMachine.hpp"

#include "ts/framework/CellPanel.h"

#include "swatchcell/framework/CellContext.h"

#include "jsoncpp/json/json.h"

#include <string>


namespace swatch {
namespace action {
class Transition;
}
}


namespace swatchcellframework
{

class SystemStateMachinePanel : public tsframework::CellPanel
{
public:

    SystemStateMachinePanel(tsframework::CellAbstractContext* context, log4cplus::Logger& logger);

    ~SystemStateMachinePanel();

    void layout(cgicc::Cgicc& cgi);

private:

    SystemStateMachinePanel( const SystemStateMachinePanel& );

    log4cplus::Logger& getLogger(){ return logger_;}

    swatch::system::System& getSystem(const CellContext::SharedGuard_t& aGuard) { return cellContext_.getSystem(aGuard); }

    void getStateMachineArray(cgicc::Cgicc& cgi,std::ostream& out);
    void getStateMachineInfo(cgicc::Cgicc& cgi,std::ostream& out);
    void actStateMachine(cgicc::Cgicc& cgi,std::ostream& out);
    void getTransitionStatus(cgicc::Cgicc& cgi,std::ostream& out);
    void serializeSystemTransition(const swatch::action::SystemTransition& lSystemTransition , Json::Value& lSystemTransitionJson);
    const swatch::action::GateKeeper* getGateKeeper(const CellContext::SharedGuard_t& aGuard) const { return cellContext_.getGateKeeper(aGuard); }

    log4cplus::Logger logger_;
    CellContext& cellContext_;

    const swatch::action::Transition* runningTransition_;

    size_t timerCounter_;

};

} // end ns swatchcellframework


#endif  /* __SWATCHCELL_FRAMEWORK_SYSTEMSTATEMACHINEPANEL_H__ */


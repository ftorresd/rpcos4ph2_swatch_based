/*************************************************************************
 * Trigger Supervisor SWATCH worker                                      *
 *                                                                       *
 * Authors: Carlos Ghabrous Larrea         				         *
 *************************************************************************/

#ifndef __SWATCHCELL_FRAMEWORK_STATEMACHINEPANEL_H__
#define __SWATCHCELL_FRAMEWORK_STATEMACHINEPANEL_H__


#include "swatch/action/ActionableObject.hpp"
#include "swatch/action/StateMachine.hpp"

#include "ts/framework/CellPanel.h"

#include "swatchcell/framework/CellContext.h"

#include <string>


namespace swatch {
namespace action {
class Transition;
}
}


namespace swatchcellframework
{

class StateMachinePanel : public tsframework::CellPanel
{
public:

    StateMachinePanel(tsframework::CellAbstractContext* context, log4cplus::Logger& logger);

    ~StateMachinePanel();

    void layout(cgicc::Cgicc& cgi);

private:

    StateMachinePanel( const StateMachinePanel& );

    log4cplus::Logger& getLogger(){ return logger_;}

    swatch::system::System& getSystem(const CellContext::SharedGuard_t& aGuard) { return cellContext_.getSystem(aGuard); }

    void getDeviceArray(cgicc::Cgicc& cgi,std::ostream& out);
    void getStateMachineArray(cgicc::Cgicc& cgi,std::ostream& out);
    void getStateMachineInfo(cgicc::Cgicc& cgi,std::ostream& out);
    void actStateMachine(cgicc::Cgicc& cgi,std::ostream& out);
    void getTransitionStatus(cgicc::Cgicc& cgi,std::ostream& out);

    const swatch::action::GateKeeper* getGateKeeper(const CellContext::SharedGuard_t& aGuard) const { return cellContext_.getGateKeeper(aGuard); }

    log4cplus::Logger logger_;
    CellContext& cellContext_;

    const swatch::action::Transition* runningTransition_;

    size_t timerCounter_;

};

} // end ns swatchcellframework


#endif  /* __SWATCHCELL_FRAMEWORK_STATEMACHINEPANEL_H__ */


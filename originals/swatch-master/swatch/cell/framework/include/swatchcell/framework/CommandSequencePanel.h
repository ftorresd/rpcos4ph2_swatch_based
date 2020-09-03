/*************************************************************************
 * Trigger Supervisor SWATCH cell framework                              *
 *                                                                       *
 * Authors: Simone Bologna                                               *
 *************************************************************************/

#ifndef __SWATCHCELL_FRAMEWORK_COMMANDSEQUENCEPANEL_H__
#define __SWATCHCELL_FRAMEWORK_COMMANDSEQUENCEPANEL_H__


#include "ts/framework/CellPanel.h"

#include "swatchcell/framework/CellContext.h"

#include <string>

namespace swatchcellframework
{

  class CommandSequencePanel : public tsframework::CellPanel
  {
    public:

    CommandSequencePanel(tsframework::CellAbstractContext* context, log4cplus::Logger& logger);

    log4cplus::Logger logger_;
    CellContext& cellContext_;

    //! Main result box (located below the "select panel type" form )
    ajax::ResultBox* resultBox_;

    ~CommandSequencePanel();

    //! Layout method; called when panel is created
    void layout(cgicc::Cgicc& cgi);

    //! Events to populate the ObjectType Device and Commands dropdown
    void getObjectTypeArray(cgicc::Cgicc& cgi,std::ostream& out);
    void getDeviceArray(cgicc::Cgicc& cgi,std::ostream& out);
    void getCommandSequenceArray(cgicc::Cgicc& cgi,std::ostream& out);
    //! Event to run a command
    void runCommandSequence(cgicc::Cgicc& cgi,std::ostream& out);
    //! Event to get the command status
    void getCommandSequenceStatus(cgicc::Cgicc& cgi,std::ostream& out);
    //! Event to get the list of subcommands
    void getCommandsList(cgicc::Cgicc& cgi,std::ostream& out);
    //! Event to check if the GateKeeper is ready
    void isGateKeeperReady(cgicc::Cgicc& cgi,std::ostream& out);

    const swatch::action::GateKeeper* getGateKeeper(const CellContext::SharedGuard_t& aGuard) const { return cellContext_.getGateKeeper(aGuard); }

    private:

    //! Contains all the Object types
    typedef std::map<std::string, std::set< std::string> > SequenceMap ;
    SequenceMap objTypeSequenceMap_;
    typedef std::map<std::string, std::map<std::string, std::string> > ActionableMap ;
    ActionableMap objTypeActionableMap_;
  };
} // end ns swatchcellframework


#endif  /* __SWATCHCELL_FRAMEWORK_COMMANDSEQUENCEPANEL_H__ */


/*************************************************************************
 * Trigger Supervisor SWATCH cell framework                              *
 *                                                                       *
 * Authors: Simone Bologna                                               *
 *************************************************************************/

#ifndef __SWATCHCELL_FRAMEWORK_SETUPPANEL_H__
#define __SWATCHCELL_FRAMEWORK_SETUPPANEL_H__


#include "ts/framework/CellPanel.h"

#include "swatchcell/framework/CellContext.h"
#include "jsoncpp/json/json.h"

#include <string>

namespace swatchcellframework
{

  class SetupPanel : public tsframework::CellPanel
  {
    public:

    SetupPanel(tsframework::CellAbstractContext* context, log4cplus::Logger& logger);

    ~SetupPanel();

    //! Layout method; called when panel is created
    void layout(cgicc::Cgicc& cgi);

    //! Event called to switch views
    void switchView(cgicc::Cgicc& cgi,std::ostream& out);
    //! Event to load the system
    void loadSystemFromFile(cgicc::Cgicc& cgi,std::ostream& out);
    void loadSystemFromDB(cgicc::Cgicc& cgi,std::ostream& out);
    //! Event which returns the system tree
    void getSystemTreeJSON(cgicc::Cgicc& cgi,std::ostream& out);
    //!Event which returns the object details
    void getObjectDetails(cgicc::Cgicc& cgi,std::ostream& out);
    //!Event which sets up the monitoring thread
    void monitoringThreadRunCommand(cgicc::Cgicc& cgi, std::ostream& out);
    //!Event which gets the mon thread status
    void getMonitoringThreadStatus(cgicc::Cgicc& cgi, std::ostream& out);
    //!Event which gets the GK from XML file
    void loadGateKeeperFromXML(cgicc::Cgicc& cgi, std::ostream& out);
    //!Event which loads the GK from DB
    void loadGateKeeperFromDB(cgicc::Cgicc& cgi, std::ostream& out);

    const swatch::action::GateKeeper* getGateKeeper(const CellContext::SharedGuard_t& aGuard) const { return mCellContext.getGateKeeper(aGuard); }
    const swatch::action::GateKeeper* getGateKeeper(const CellContext::WriteGuard_t& aGuard) const { return mCellContext.getGateKeeper(aGuard); }

    enum Mode_t{
      kSystemSetup,
      kGateKeeper,
      kMonitoringThread
    };

    //Tab layout functions
    void respondSystemSetupPage();
    void respondGateKeeperPage();
    void respondMonitoringThreadPage();

    //Tree element detail functions
    void returnProcessorDetails(const std::string& procId, std::ostream& out);
    void returnDAQTTCDetails(const std::string& dtmId, std::ostream& out);
    void returnCrateDetails(const std::string& crateId, std::ostream& out);
    void returnProcessorsDetails(std::ostream& out);
    void returnDAQTTCsDetails(std::ostream& out);
    void returnCratesDetails(std::ostream& out);
    void returnLinksDetails(std::ostream& out);
    void returnConnectedFEDsDetails(std::ostream& out);
    void returnSystemDetails(std::ostream& out);

    private:

    Mode_t mMode;

    log4cplus::Logger mLogger;
    CellContext& mCellContext;

    //! Main result box (located below the "select panel type" form )
    ajax::ResultBox* mResultBox;

  };
} // end ns swatchcellframework


#endif  /* __SWATCHCELL_FRAMEWORK_SETUPPANEL_H__ */


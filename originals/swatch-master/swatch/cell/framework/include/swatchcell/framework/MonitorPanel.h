/*************************************************************************
 * Trigger Supervisor SWATCH cell framework                              *
 *                                                                       *
 * Authors: Tom Williams         				         *
 *************************************************************************/

#ifndef __SWATCHCELL_FRAMEWORK_MONITORPANEL_H__
#define __SWATCHCELL_FRAMEWORK_MONITORPANEL_H__


#include "swatch/core/StatusFlag.hpp"

#include "ts/framework/CellPanel.h"

#include "swatchcell/framework/CellContext.h"

#include "jsoncpp/json/json.h"

#include <string>

namespace ajax
{
  class Form;
  class ResultBox;
  class Table;
}

namespace swatch {
  namespace core {
    class AbstractMetric;
    class MetricView;
  }
}

namespace swatchcellframework
{

  class MonitorPanel : public tsframework::CellPanel
  {
  public:

    MonitorPanel(tsframework::CellAbstractContext* context, log4cplus::Logger& logger);

    ~MonitorPanel();

    //! Layout method; called when panel is created
    void layout(cgicc::Cgicc& cgi);

    /// tabs
    void respondObjectPage();
    void respondProcessorsPage();
    void respondPortsPage();
    void respondSystemPage();

  ///JSON output: these functions only produce the JSON output and they are called every time the metrics are refreshed

  void respondProcessorJSON(cgicc::Cgicc& cgi,std::ostream& out);
  void respondPortsJSON(cgicc::Cgicc& cgi,std::ostream& out);
  void respondSystemJSON(cgicc::Cgicc& cgi,std::ostream& out);
  void respondMonitoringTitleJSON(cgicc::Cgicc& cgi,std::ostream& out);

    /// tabs switch
    void switchView(cgicc::Cgicc& cgi,std::ostream& out);

    /// change object in ObjectDetails panel and auto-refresh
    void switchComponent(cgicc::Cgicc& cgi,std::ostream& out);

    //! Enum values correspond to the different types of monitoring panel contents that can be chosen by the user
    enum Mode{
      kSystem,
      kProcessors,
      kObjectDetails,
      kPorts
    };

  private:

    MonitorPanel( const MonitorPanel& );

    log4cplus::Logger& getLogger(){ return logger_;}

    swatch::system::System& getSystem(const CellContext::SharedGuard_t& aGuard) { return cellContext_.getSystem(aGuard); }

    log4cplus::Logger logger_;
    CellContext& cellContext_;

    //! Main result box (located below the "select panel type" form )
    ajax::ResultBox* resultBox_;
    // ajax::ResultBox* objectResultBox_;

    //! Sub-panel selector
    Mode mode_;


  };


  std::ostream& operator<<(std::ostream& aOut, MonitorPanel::Mode aMode);

  std::istream& operator>>(std::istream& aIn, MonitorPanel::Mode& aMode);

} // end ns swatchcellframework


#endif /*  __SWATCHCELL_FRAMEWORK_MONITORPANEL_H__ */


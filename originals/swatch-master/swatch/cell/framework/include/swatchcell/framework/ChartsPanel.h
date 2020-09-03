/*************************************************************************
 * Trigger Supervisor SWATCH cell framework                              *
 *                                                                       *
 * Authors: Simone Bologna                                               *
 *************************************************************************/

#ifndef __SWATCHCELL_FRAMEWORK_CHARTSPANEL_H__
#define __SWATCHCELL_FRAMEWORK_CHARTSPANEL_H__


#include "ts/framework/CellPanel.h"

#include "swatchcell/framework/CellContext.h"
#include "jsoncpp/json/json.h"

#include <string>

namespace swatchcellframework
{

  class ChartsPanel : public tsframework::CellPanel
  {
    public:

    ChartsPanel(tsframework::CellAbstractContext* context, log4cplus::Logger& logger);

    void getMetricsList(cgicc::Cgicc& cgi, std::ostream& out);
    void getMetricsValues(cgicc::Cgicc& cgi, std::ostream& out);

    log4cplus::Logger logger_;
    CellContext& cellContext_;


    ~ChartsPanel();

    enum Mode{
      kCharts,
      kMetricView
    };
    
    //! Layout method; called when panel is created
    void layout(cgicc::Cgicc& cgi);

    private:
    Mode mode_;
    ajax::ResultBox* resultBox_;

  };
} // end ns swatchcellframework


#endif  /* __SWATCHCELL_FRAMEWORK_CHARTSPANEL_H__ */


/*************************************************************************;
 * Trigger Supervisor SWATCH cell framework                              *
 *                                                                       *
 * Authors: Simone Bologna                                               *
 *************************************************************************/

#ifndef __SWATCHCELL_FRAMEWORK_EXPLOREPANEL_H__
#define __SWATCHCELL_FRAMEWORK_EXPLOREPANEL_H__


#include "ts/framework/CellPanel.h"

#include "swatchcell/framework/CellContext.h"
#include "jsoncpp/json/json.h"

#include <string>

namespace swatchcellframework
{

  class LogPanel : public tsframework::CellPanel
  {
    public:

    LogPanel(tsframework::CellAbstractContext* context, log4cplus::Logger& logger);

    log4cplus::Logger mLogger;
    CellContext& mCellContext;

    void getLoggers(cgicc::Cgicc& cgi, std::ostream& out);
    void getLog(cgicc::Cgicc& cgi, std::ostream& out);
    std::string localLogLevel(log4cplus::LogLevel aLevel );

    ~LogPanel();

    //! Layout method; called when panel is created
    void layout(cgicc::Cgicc& cgi);


  };
} // end ns swatchcellframework


#endif  /* __SWATCHCELL_FRAMEWORK_REDIRECTPANEL_H__ */


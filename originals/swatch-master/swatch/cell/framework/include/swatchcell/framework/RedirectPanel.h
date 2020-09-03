/*************************************************************************;
 * Trigger Supervisor SWATCH cell framework                              *
 *                                                                       *
 * Authors: Simone Bologna                                               *
 *************************************************************************/

#ifndef __SWATCHCELL_FRAMEWORK_REDIRECTPANEL_H__
#define __SWATCHCELL_FRAMEWORK_REDIRECTPANEL_H__


#include "ts/framework/CellPanel.h"

#include "swatchcell/framework/CellContext.h"

#include <string>

namespace swatchcellframework
{

  class RedirectPanel : public tsframework::CellPanel
  {
    public:

    RedirectPanel(tsframework::CellAbstractContext* context, log4cplus::Logger& logger);

    log4cplus::Logger logger_;
    CellContext& cellContext_;

    static std::string kRedirectPath;

    ~RedirectPanel();

    //! Layout method; called when panel is created
    void layout(cgicc::Cgicc& cgi);


  };
} // end ns swatchcellframework


#endif  /* __SWATCHCELL_FRAMEWORK_REDIRECTPANEL_H__ */


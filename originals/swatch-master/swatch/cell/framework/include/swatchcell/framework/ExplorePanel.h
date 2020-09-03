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

  class ExplorePanel : public tsframework::CellPanel
  {
    public:

    ExplorePanel(tsframework::CellAbstractContext* context, log4cplus::Logger& logger);
    
    void getExploreTree( cgicc::Cgicc& cgi, std::ostream& out );
    
    void serializeTreeNode(const swatch::core::Object& aObj, Json::Value& aJsonNode);

    log4cplus::Logger mLogger;
    CellContext& mCellContext;

    ~ExplorePanel();

    //! Layout method; called when panel is created
    void layout(cgicc::Cgicc& cgi);


  };
} // end ns swatchcellframework


#endif  /* __SWATCHCELL_FRAMEWORK_REDIRECTPANEL_H__ */


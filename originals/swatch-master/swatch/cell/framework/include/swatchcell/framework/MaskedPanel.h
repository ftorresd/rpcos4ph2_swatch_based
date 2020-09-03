
#ifndef __SWATCHCELL_FRAMEWORK_MASKEDPANEL_H__
#define __SWATCHCELL_FRAMEWORK_MASKEDPANEL_H__


#include "ts/framework/CellPanel.h"

#include "swatchcell/framework/CellContext.h"
#include "jsoncpp/json/json.h"

#include <string>

/*
namespace ajax
{
    class ResultBox;
    class Timer;
    class Dialog;
}
 */


//A panel showing the list of the masked MaskableObject and disabled ActionableObject

namespace swatchcellframework
{

  class MaskedPanel : public tsframework::CellPanel
  {
    public:

    MaskedPanel(tsframework::CellAbstractContext* context, log4cplus::Logger& logger);

    log4cplus::Logger mLogger;
    CellContext& mCellContext;

    void applyDynamicMasks ( cgicc::Cgicc& cgi, std::ostream& out );
    void applyStaticMasks ( cgicc::Cgicc& cgi, std::ostream& out );
    void clearMasks ( cgicc::Cgicc& cgi, std::ostream& out );
    void getMaskedObjects ( cgicc::Cgicc& cgi, std::ostream& out );

    void serializeMaskedObjects ( Json::Value & aJsonOutput );

    ~MaskedPanel();

    //! Layout method; called when panel is created
    void layout(cgicc::Cgicc& cgi);


  };
} // end ns swatchcellframework


#endif  /* __SWATCHCELL_FRAMEWORK_MASKEDPANEL_H__ */

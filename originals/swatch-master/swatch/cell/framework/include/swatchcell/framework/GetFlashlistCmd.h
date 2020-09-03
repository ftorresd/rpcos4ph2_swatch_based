/*************************************************************************;
 * Trigger Supervisor SWATCH cell framework                              *
 *                                                                       *
 * Authors:                                                              *
 *************************************************************************/

#ifndef __SWATCHCELL_FRAMEWORK_GETFLASHLISTCMD_H__
#define __SWATCHCELL_FRAMEWORK_GETFLASHLISTCMD_H__


#include "ts/framework/CellCommand.h"

namespace swatchcellframework
{

  class GetFlashlistCmd : public tsframework::CellCommand
  {
    public:

    GetFlashlistCmd( log4cplus::Logger& aLogger, tsframework::CellAbstractContext* aContext );

    virtual void code();

  };
} // end ns swatchcellframework


#endif  /* __SWATCHCELL_FRAMEWORK_GETFLASHLISTCMD_H__ */


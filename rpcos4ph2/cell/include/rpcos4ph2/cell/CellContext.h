
#ifndef __RPCOS4PH2_CELL_CELLCONTEXT_H__
#define __RPCOS4PH2_CELL_CELLCONTEXT_H__


#include "swatchcell/framework/CellContext.h"


namespace rpcos4ph2
{
    namespace cell
    {

class CellContext : public swatchcellframework::CellContext {
public:
  CellContext(log4cplus::Logger& log, tsframework::CellAbstract* cell);

  ~CellContext();

private:

};
  
} // end ns: rpcos4ph2
} // end ns: cell


#endif /* __RPCOS4PH2_CELL_CELLCONTEXT_H__ */


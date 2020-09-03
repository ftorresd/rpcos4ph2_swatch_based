
#ifndef __SWATCHCELL_EXAMPLE_CELLCONTEXT_H__
#define __SWATCHCELL_EXAMPLE_CELLCONTEXT_H__


#include "swatchcell/framework/CellContext.h"


namespace swatchcellexample {

class CellContext : public swatchcellframework::CellContext {
public:
  CellContext(log4cplus::Logger& log, tsframework::CellAbstract* cell);

  ~CellContext();

private:

};
  
} // end ns: swatchcellexample


#endif /* __SWATCHCELL_EXAMPLE_CELLCONTEXT_H__ */


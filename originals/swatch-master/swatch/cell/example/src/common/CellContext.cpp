
#include "swatchcell/example/CellContext.h"


#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"


namespace swatchcellexample {


CellContext::CellContext(log4cplus::Logger& aLogger, tsframework::CellAbstract* aCell) :
  swatchcellframework::CellContext(aLogger, aCell)
{
  LOG4CPLUS_INFO(aLogger, "swatchcellexample::CellContext : In constructor");
}


CellContext::~CellContext()
{
}


} // end ns: swatchcellexample

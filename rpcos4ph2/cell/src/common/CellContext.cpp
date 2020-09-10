
#include "rpcos4ph2/cell/CellContext.h"


#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"

namespace rpcos4ph2 {
namespace cell {


CellContext::CellContext(log4cplus::Logger& aLogger, tsframework::CellAbstract* aCell) :
  swatchcellframework::CellContext(aLogger, aCell)
{
  LOG4CPLUS_INFO(aLogger, "swatchcellexample::CellContext : In constructor");
}


CellContext::~CellContext()
{
}


} // end ns: cell
} // end ns: rpcos4ph2


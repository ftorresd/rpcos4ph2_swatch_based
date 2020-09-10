
#include "rpcos4ph2/cell/RunControl.h"


#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"



namespace rpcos4ph2 {
namespace cell {


RunControl::RunControl(log4cplus::Logger& log, tsframework::CellAbstractContext* context) :
  swatchcellframework::RunControl(log, context)
{
  LOG4CPLUS_INFO(getLogger(), "swatchcellexample::RunControl : In constructor");
}


RunControl::~RunControl()
{
  LOG4CPLUS_INFO(getLogger(), "swatchcellexample::RunControl : In destructor");
}


void RunControl::execPostStart()
{
  LOG4CPLUS_INFO(getLogger(), "swatchcellexample::RunControl : execPostStart");
}


void RunControl::execPreStop()
{
  LOG4CPLUS_INFO(getLogger(), "swatchcellexample::RunControl : execPreStop");
}


} // end ns: cell
} // end ns: rpcos4ph2

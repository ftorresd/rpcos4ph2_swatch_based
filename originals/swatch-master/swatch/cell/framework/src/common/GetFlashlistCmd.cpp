#include "swatchcell/framework/CellContext.h"
#include "swatchcell/framework/GetFlashlistCmd.h"

// log4cplus headers
#include "log4cplus/logger.h"

#include <boost/algorithm/string.hpp>

namespace swatchcellframework
{

  GetFlashlistCmd::GetFlashlistCmd( log4cplus::Logger& aLogger, tsframework::CellAbstractContext* aContext )
    :
    CellCommand( aLogger, aContext )
  {
    // logger_ = log4cplus::Logger::getInstance(log.getName() +".GetFlashlistCmd");
    // getParamList()["VIEW"] = new xdata::String("urn:tstore-view-SQL:subsystem");
  }


  void GetFlashlistCmd::code()									
  {
    swatchcellframework::CellContext* lContext = dynamic_cast<swatchcellframework::CellContext*>( getContext() ) ;
    swatchcellframework::MonitoringThread & lMonyThread = lContext->getMonitoringThread();

    std::ostringstream lResultBuffer;
    lMonyThread.getFlashlist( lResultBuffer );

    payload_->fromString( lResultBuffer.str() );
  }


} // end ms swatchframework

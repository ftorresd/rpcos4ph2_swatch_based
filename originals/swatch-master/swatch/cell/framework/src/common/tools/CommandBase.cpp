#include "swatchcell/framework/tools/CommandBase.h"

#include "swatchcell/framework/CellContext.h"

namespace swatchcellframework
{

CommandBase::CommandBase(log4cplus::Logger& logger, tsframework::CellAbstractContext* context)
:
tsframework::CellCommand(logger, context),
cellContext_( dynamic_cast<CellContext*>(context) )
{
}

CommandBase::~CommandBase()
{
}


CellContext*
CommandBase::getCellContext()
{
    return cellContext_;
}

} // end ns swatchcellframework

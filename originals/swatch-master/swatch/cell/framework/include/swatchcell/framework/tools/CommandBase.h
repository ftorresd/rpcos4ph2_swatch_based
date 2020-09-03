#ifndef __SWATCHCELL_FRAMEWORK_TOOLS_COMMANDBASE_H__
#define __SWATCHCELL_FRAMEWORK_TOOLS_COMMANDBASE_H__

#include "ts/framework/CellCommand.h"

namespace tsframework
{
    class CellAbstractContext; 
}

namespace swatchcellframework
{
    class CellContext; 
    
    class CommandBase : public tsframework::CellCommand
    {
    public:
        
        CommandBase(log4cplus::Logger& logger, tsframework::CellAbstractContext* context);
        
        virtual ~CommandBase();
        
        
    protected:
        
        CellContext* getCellContext();
        
    private:
        
        CellContext* cellContext_;
    };
    
} // end ns swatchcellframework

#endif  /* __SWATCHCELL_FRAMEWORK_TOOLS_COMMANDBASE_H__ */


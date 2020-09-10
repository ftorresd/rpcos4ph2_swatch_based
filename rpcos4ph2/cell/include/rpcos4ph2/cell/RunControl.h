
#ifndef __RPCOS4PH2_CELL_RUNCONTROL_H__
#define __RPCOS4PH2_CELL_RUNCONTROL_H__

#include "swatchcell/framework/RunControl.h"

namespace rpcos4ph2
{
    namespace cell
    {
        class RunControl : public swatchcellframework::RunControl
        {
        public:
            RunControl(log4cplus::Logger &aLogger, tsframework::CellAbstractContext *aContext);

            ~RunControl();

        private:
            void execPostStart();

            void execPreStop();
        };

    } // namespace cell
} // namespace rpcos4ph2

#endif /* __RPCOS4PH2_CELL_RUNCONTROL_H__ */

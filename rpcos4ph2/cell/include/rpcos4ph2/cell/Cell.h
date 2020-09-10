/*************************************************************************
 * Minimal example of a SWATCH-based Trigger Supervisor cell             *
 *                                                                       *
 * Authors: Tom Williams                                                 *
 *************************************************************************/

#ifndef __RPCOS4PH2_CELL_CELL_H__
#define __RPCOS4PH2_CELL_CELL_H__

#include "ts/framework/CellAbstract.h"

#include "swatchcell/framework/CellAbstract.h"

#include <string>

namespace rpcos4ph2
{
    namespace cell
    {

        class Cell : public swatchcellframework::CellAbstract
        {
        public:
            XDAQ_INSTANTIATOR();

            Cell(xdaq::ApplicationStub *s);

            ~Cell();

            //!This method should be filled with addCommand, addOperation and addPanel that corresponds to that Cell
            void init();

        private:
            Cell(const Cell &);
        };

    } // namespace cell
} // namespace rpcos4ph2

#endif /* __RPCOS4PH2_CELL_CELL_H__ */

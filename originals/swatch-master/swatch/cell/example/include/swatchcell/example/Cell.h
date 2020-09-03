/*************************************************************************
 * Minimal example of a SWATCH-based Trigger Supervisor cell             *
 *                                                                       *
 * Authors: Tom Williams                                                 *
 *************************************************************************/

#ifndef __SWATCHCELL_EXAMPLE_CELL_H__
#define __SWATCHCELL_EXAMPLE_CELL_H__

#include "ts/framework/CellAbstract.h"

#include "swatchcell/framework/CellAbstract.h"

#include <string>

namespace swatchcellexample
{

class Cell : public swatchcellframework::CellAbstract
{
public:

    XDAQ_INSTANTIATOR();

    Cell(xdaq::ApplicationStub * s);

    ~Cell();

    //!This method should be filled with addCommand, addOperation and addPanel that corresponds to that Cell
    void init();

private:
    Cell( const Cell& );

};

} // namespace swatchexample

#endif /* __SWATCHCELL_EXAMPLE_CELL_H__ */

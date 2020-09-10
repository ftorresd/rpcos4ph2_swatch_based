#include "rpcos4ph2/cell/Cell.h"

#include "swatch/action/ThreadPool.hpp"

#include "ts/framework/CellPanelFactory.h"

#include "swatchcell/framework/ExplorePanel.h"
#include "swatchcell/framework/RedirectPanel.h"
#include "swatchcell/framework/RunControl.h"

#include "rpcos4ph2/cell/CellContext.h"
#include "rpcos4ph2/cell/RunControl.h"

XDAQ_INSTANTIATOR_IMPL(rpcos4ph2::cell::Cell)
namespace rpcos4ph2
{
    namespace cell
    {

        Cell::Cell(xdaq::ApplicationStub *s) : swatchcellframework::CellAbstract(s, TypeCarrier<RunControl>())
        {
            LOG4CPLUS_INFO(getLogger(), "rpcos4ph2::cell::Cell : In constructor");
        }

        Cell::~Cell()
        {
        }

        void Cell::init()
        {
            addGenericSwatchComponents();

            tsframework::CellPanelFactory *lPanelFactory = getContext()->getPanelFactory();
            lPanelFactory->add<swatchcellframework::ExplorePanel>("SWATCH Explorer");
            lPanelFactory->add<swatchcellframework::RedirectPanel>("Home");
        }

    } // namespace cell
} // namespace rpcos4ph2

#include "swatchcell/example/Cell.h"


#include "swatch/action/ThreadPool.hpp"

#include "ts/framework/CellPanelFactory.h"

#include "swatchcell/framework/ExplorePanel.h"
#include "swatchcell/framework/RedirectPanel.h"
#include "swatchcell/framework/RunControl.h"

#include "swatchcell/example/CellContext.h"
#include "swatchcell/example/RunControl.h"


XDAQ_INSTANTIATOR_IMPL(swatchcellexample::Cell)

namespace swatchcellexample
{

Cell::Cell(xdaq::ApplicationStub * s) :
  swatchcellframework::CellAbstract(s, TypeCarrier<RunControl>())
{
  LOG4CPLUS_INFO(getLogger(), "swatchcellexample::Cell : In constructor");
}


Cell::~Cell()
{
}


void Cell::init()
{
  addGenericSwatchComponents();

  tsframework::CellPanelFactory* lPanelFactory = getContext()->getPanelFactory();
  lPanelFactory->add<swatchcellframework::ExplorePanel>("SWATCH Explorer");
  lPanelFactory->add<swatchcellframework::RedirectPanel>("Home");
}

} // end

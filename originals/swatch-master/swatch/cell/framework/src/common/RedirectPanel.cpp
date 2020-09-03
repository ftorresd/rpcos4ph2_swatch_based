#include "swatchcell/framework/RedirectPanel.h"

#include "boost/foreach.hpp"
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "ajax/PolymerElement.h"
#include "ajax/toolbox.h"

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"

#include "swatch/action/Command.hpp"
#include "swatch/core/ReadWriteXParameterSet.hpp"
#include "swatch/system/Crate.hpp"
#include "swatch/dtm/DaqTTCManager.hpp"
#include "swatch/system/System.hpp"
#include "swatch/processor/Processor.hpp"
#include "swatch/action/ActionableSystem.hpp"
#include "swatch/action/GateKeeper.hpp"

namespace swatchcellframework
{

  std::string RedirectPanel::kRedirectPath = "/Control Panels/1. Summary";

  RedirectPanel::RedirectPanel(tsframework::CellAbstractContext* context, log4cplus::Logger& logger)
  :
    CellPanel(context, logger),
    logger_(logger),
    cellContext_(dynamic_cast<swatchcellframework::CellContext&>(*context))
  {


  }

  RedirectPanel::~RedirectPanel()
  {

  }

  void RedirectPanel::layout(cgicc::Cgicc& cgi)
  {
    this->remove();

    ajax::PolymerElement* redirectPanel = new ajax::PolymerElement("redirect-panel");
    redirectPanel -> set ("redirect-path", this -> kRedirectPath);

    this -> add(redirectPanel);

    return;

  }

} // end ms swatchframework

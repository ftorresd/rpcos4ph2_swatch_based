#include "swatchcell/framework/CellAbstract.h"


#include "xcept/tools.h"

#include "uhal/log/log.hpp"

#include "ts/framework/CellFactory.h"
#include "ts/framework/CellPanelFactory.h"
#include "ts/framework/CellOperationFactory.h"

#include "swatchcell/framework/RunControl.h"

#include "swatchcell/framework/MonitorPanel.h"
#include "swatchcell/framework/StateMachinePanel.h"
#include "swatchcell/framework/SystemStateMachinePanel.h"
#include "swatchcell/framework/MaskedPanel.h"
#include "swatchcell/framework/CommandPanel.h"
#include "swatchcell/framework/CommandSequencePanel.h"
#include "swatchcell/framework/SetupPanel.h"
#include "swatchcell/framework/ChartsPanel.h"
#include "swatchcell/framework/LogPanel.h"
#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"

#include "swatchcell/framework/GetFlashlistCmd.h"

namespace swatchcellframework {


CellAbstract::CellAbstract(xdaq::ApplicationStub * s) :
  tsframework::CellAbstract(s)
{
  createContext<CellContext>();

  uhal::setLogLevelTo(uhal::Notice());

  registerRunControlOperation<RunControl>();
}


CellAbstract::~CellAbstract()
{
}


void CellAbstract::addGenericSwatchComponents()
{

  // Add this package to the import statements in header section of webpages
  getContext()->addImport("/swatchcell/framework/html/elements/elements.html");

  // ... add the commands
  tsframework::CellFactory* cmdF = getContext()->getCommandFactory();
  cmdF->add<swatchcellframework::GetFlashlistCmd>("Default", "GetFlashlist");

  // ... and add the panels themselves
  tsframework::CellPanelFactory* lFactory = getContext()->getPanelFactory();

  lFactory->add<swatchcellframework::MonitorPanel>("1. Summary");

  lFactory->add<swatchcellframework::LogPanel>("2. Logging");
  lFactory->add<swatchcellframework::SetupPanel>("3. System setup");
  lFactory->add<swatchcellframework::MaskedPanel>("4. Masking");
  lFactory->add<swatchcellframework::ChartsPanel>("5. Monitoring");

  lFactory->add<swatchcellframework::CommandPanel>("6. Commands");
  lFactory->add<swatchcellframework::CommandSequencePanel>("7. Sequences");
  lFactory->add<swatchcellframework::StateMachinePanel>("8. State machines");
  lFactory->add<swatchcellframework::SystemStateMachinePanel>("9. System state machines");
}


} // end

#include "swatchcell/framework/MaskedPanel.h"
#include "swatchcell/framework/tools/panelFunctions.h"

// TS Headers
#include "ajax/PolymerElement.h"
#include "ajax/toolbox.h"

//Json library
#include "jsoncpp/json/json.h"

// SWATCH Headers
#include "swatch/action/MaskableObject.hpp"
#include "swatch/action/ActionableObject.hpp"
#include "swatch/processor/Processor.hpp"
#include "swatch/processor/Port.hpp"
#include "swatch/processor/PortCollection.hpp"
#include "swatch/system/Crate.hpp"
#include "swatch/system/Link.hpp"
#include "swatch/system/System.hpp"
#include "swatch/dtm/DaqTTCManager.hpp"
#include "swatch/dtm/AMCPortCollection.hpp"
#include "swatch/dtm/AMCPort.hpp"
#include "swatch/processor/Processor.hpp"
#include "swatch/system/MaskingConfigurator.hpp"

// Log4CPlus Headers
#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"

// C++ Headers
#include <sstream>

// Boost Headers
#include <boost/foreach.hpp>
#include <boost/lambda/core.hpp>
#include <boost/range/algorithm/copy.hpp>

namespace swatchcellframework
{

  MaskedPanel::MaskedPanel(tsframework::CellAbstractContext* context, log4cplus::Logger& logger)
  :
    CellPanel(context, logger),
    mLogger(logger),
    mCellContext(dynamic_cast<swatchcellframework::CellContext&>(*context))
  {


  }

  MaskedPanel::~MaskedPanel()
  {

  }

  /*
  ██       █████  ██    ██  ██████  ██    ██ ████████
  ██      ██   ██  ██  ██  ██    ██ ██    ██    ██
  ██      ███████   ████   ██    ██ ██    ██    ██
  ██      ██   ██    ██    ██    ██ ██    ██    ██
  ███████ ██   ██    ██     ██████   ██████     ██
  */


  void MaskedPanel::layout(cgicc::Cgicc& cgi)
  {
    this->remove();

    setEvent("MaskedPanel::applyDynamicMasks", ajax::Eventable::OnClick, this, &MaskedPanel::applyDynamicMasks);
    setEvent("MaskedPanel::applyStaticMasks", ajax::Eventable::OnClick, this, &MaskedPanel::applyStaticMasks);
    setEvent("MaskedPanel::clearMasks", ajax::Eventable::OnClick, this, &MaskedPanel::clearMasks);
    setEvent("MaskedPanel::getMaskedObjects", ajax::Eventable::OnClick, this, &MaskedPanel::getMaskedObjects);

    ajax::PolymerElement* maskedPanel = new ajax::PolymerElement("masked-panel");

    CellContext::SharedGuard_t lGuard(mCellContext);
    const swatch::action::GateKeeper* lGk = mCellContext.getGateKeeper(lGuard);

    // No GK, no party
    if (lGk) {
      maskedPanel -> set("is-gatekeeper-ready", ajax::toolbox::escapeHTML("true"));
    }

    this -> add(maskedPanel);

    return;

  }

  /*
   █████  ██████  ██████  ██   ██    ██     ██████  ██    ██ ███    ██
  ██   ██ ██   ██ ██   ██ ██    ██  ██      ██   ██  ██  ██  ████   ██
  ███████ ██████  ██████  ██     ████       ██   ██   ████   ██ ██  ██
  ██   ██ ██      ██      ██      ██        ██   ██    ██    ██  ██ ██
  ██   ██ ██      ██      ███████ ██        ██████     ██    ██   ████
  */

  void MaskedPanel::applyDynamicMasks( cgicc::Cgicc& cgi, std::ostream& out ){
    
    std::string lFEDMapInput = ajax::toolbox::getSubmittedValue(cgi, "fedmaps");
    swatch::system::MaskingConfigurator::FEDEnableMap_t lFedMap;

    LOG4CPLUS_INFO(mLogger, "[MaskedPanel] Applying masks for FED map: " << lFEDMapInput);
    
    Json::Value lJsonOutput(Json::objectValue);

    try {
      lFedMap = CellContext::parseFEDEnableMap(lFEDMapInput);
    } catch ( const std::exception& lException ) {
      
      lJsonOutput["status"] = "Error";
      std::ostringstream lOss;
      lOss << "Failed to parse FEDMap. Exception: "<< lException.what();
      lJsonOutput["message"] = lOss.str();
      out << lJsonOutput;
      return;

    }

    CellContext::SharedGuard_t lGuard(mCellContext);
    const swatch::action::GateKeeper* lGk = mCellContext.getGateKeeper(lGuard);

    // No GK, no party
    if (!lGk) {
      
      lJsonOutput["status"] = "Error";
      lJsonOutput["message"] = "GateKeeper is not available. Please load one before proceeding.";
      out << lJsonOutput;
      return;
    }

    swatch::system::MaskingConfigurator masks(mCellContext.getSystem(lGuard));

    // Clear enable/disable flags
    masks.clear();

    // Apply gatekeeper settings
    masks.applyStatic(*lGk);

    // Disable crates
    masks.disableCrates(lFedMap);

    // Apply crates
    masks.applyDynamicMasks(lFedMap);

    //Success!
    
    lJsonOutput["status"] = "Success";
    lJsonOutput["message"] = "Dynamic and static masks applied.";
    
    out << lJsonOutput;
    
    return;
  }

  /*
   █████  ██████  ██████  ██   ██    ██     ███████ ████████  █████  ████████ ██  ██████
  ██   ██ ██   ██ ██   ██ ██    ██  ██      ██         ██    ██   ██    ██    ██ ██
  ███████ ██████  ██████  ██     ████       ███████    ██    ███████    ██    ██ ██
  ██   ██ ██      ██      ██      ██             ██    ██    ██   ██    ██    ██ ██
  ██   ██ ██      ██      ███████ ██        ███████    ██    ██   ██    ██    ██  ██████
  */

  void MaskedPanel::applyStaticMasks( cgicc::Cgicc& cgi, std::ostream& out ){
    
    CellContext::SharedGuard_t lGuard(mCellContext);
    swatch::system::System& lSystem = mCellContext.getSystem(lGuard);
    const swatch::action::GateKeeper* lGk = mCellContext.getGateKeeper(lGuard);
    
    Json::Value lJsonOutput(Json::objectValue);
    
    // No GK, no party
    if (!lGk) {
      lJsonOutput["status"] = "Error";
      lJsonOutput["message"] = "GateKeeper is not available. Please load one before proceeding.";
      out << lJsonOutput;
      return;
    }

    swatch::system::MaskingConfigurator lMasks(lSystem);

    // Clear enable/disable flags
    lMasks.clear();

    // Apply gatekeeper settings
    lMasks.applyStatic(*lGk);

    //Success
    
    lJsonOutput["status"] = "Success";
    lJsonOutput["message"] = "Static masks applied.";
    
    out << lJsonOutput;
    
    return;
  }

  /*
   ██████ ██      ███████  █████  ██████
  ██      ██      ██      ██   ██ ██   ██
  ██      ██      █████   ███████ ██████
  ██      ██      ██      ██   ██ ██   ██
   ██████ ███████ ███████ ██   ██ ██   ██
  */


  void MaskedPanel::clearMasks( cgicc::Cgicc& cgi, std::ostream& out ) {

    CellContext::SharedGuard_t lGuard(mCellContext);
    const swatch::action::GateKeeper* lGk = mCellContext.getGateKeeper(lGuard);
    
    Json::Value lJsonOutput(Json::objectValue);

    // No GK, no party
    if (!lGk) {
      lJsonOutput["status"] = "Error";
      lJsonOutput["message"] = "GateKeeper is not available. Please load one before proceeding.";
      out << lJsonOutput;
      return;
    }

    swatch::system::MaskingConfigurator lMasks(mCellContext.getSystem(lGuard));

    // Clear enable/disable flags
    lMasks.clear();

    lJsonOutput["status"] = "Success";
    lJsonOutput["message"] = "Masks cleared.";
    out << lJsonOutput;
    return;
  }

  /*
  ██      ██ ███████ ████████
  ██      ██ ██         ██
  ██      ██ ███████    ██
  ██      ██      ██    ██
  ███████ ██ ███████    ██
  */

  void MaskedPanel::getMaskedObjects( cgicc::Cgicc& cgi, std::ostream& out ) {
    Json::Value lJsonOutput(Json::objectValue);
    this -> serializeMaskedObjects(lJsonOutput);
    out << lJsonOutput;
    return;
  }

  /*
  ███    ███  █████  ███████ ██   ██          ██ ███████  ██████  ███    ██
  ████  ████ ██   ██ ██      ██  ██           ██ ██      ██    ██ ████   ██
  ██ ████ ██ ███████ ███████ █████            ██ ███████ ██    ██ ██ ██  ██
  ██  ██  ██ ██   ██      ██ ██  ██      ██   ██      ██ ██    ██ ██  ██ ██
  ██      ██ ██   ██ ███████ ██   ██      █████  ███████  ██████  ██   ████
  */

  void MaskedPanel::serializeMaskedObjects(Json::Value& aJsonOutput) {

    CellContext::SharedGuard_t lGuard(mCellContext);
    swatch::system::System& lSystem = mCellContext.getSystem(lGuard);

    Json::Value lMaskedObjects(Json::arrayValue);
    Json::Value lDisabledObjects(Json::arrayValue);

    Json::Value lDisabledAMCs(Json::arrayValue);
    Json::Value lDisabledDTMs(Json::arrayValue);
    Json::Value lMaskedLinks(Json::arrayValue);
    Json::Value lMaskedAMCPorts(Json::arrayValue);
    Json::Value lMaskedInputs(Json::arrayValue);

    swatch::system::MaskingConfigurator masks(mCellContext.getSystem(lGuard));


    for (swatch::core::Object::iterator lIt = lSystem.begin(); lIt != lSystem.end(); ++lIt) {
      //dynamic casting the objects

      //AMC/Processors
      if (swatch::processor::Processor* lProcessor = dynamic_cast<swatch::processor::Processor*> (&(*lIt))){
        if (!lProcessor -> getStatus().isEnabled()) lDisabledAMCs.append(lIt -> getPath());
        continue;
      }

      //AMC13/DTM
      if (swatch::dtm::DaqTTCManager* lDtm = dynamic_cast<swatch::dtm::DaqTTCManager*> (&(*lIt))){
        if (!lDtm -> getStatus().isEnabled()) lDisabledDTMs.append(lIt -> getPath());
        continue;
      }

      //AMC13 ports
      if (swatch::dtm::AMCPort* lAMCPort = dynamic_cast<swatch::dtm::AMCPort*> (&(*lIt))){
        if (lAMCPort -> isMasked()) lMaskedAMCPorts.append(lIt -> getPath());
        continue;
      }

      //AMC/Processor input ports
      if (swatch::processor::InputPort* lInputPort = dynamic_cast<swatch::processor::InputPort*> (&(*lIt))){
        if (lInputPort -> isMasked()) lMaskedInputs.append(lIt -> getPath());
        continue;
      }
      
      //Serializing masked links
      if (swatch::system::Link* lLink = dynamic_cast<swatch::system::Link*> (&(*lIt))){
        if (lLink -> getSrcProcessor() -> getStatus().isEnabled() ) continue;
        if (lLink -> getDstPort() -> isMasked()) lMaskedLinks.append(lIt -> getPath());
        continue;
      }

      //I don't know if this can happen, but if something does not enter in the previous conditions it will be serialized here in a different list
      if (swatch::action::MaskableObject* lMaskableObject = dynamic_cast<swatch::action::MaskableObject*> (&(*lIt))){
        if (lMaskableObject -> isMasked()) lMaskedObjects.append(lIt -> getPath());
      }
      if (swatch::action::ActionableObject* lActionableObject = dynamic_cast<swatch::action::ActionableObject*> (&(*lIt))) {
        if (!lActionableObject -> getStatus().isEnabled()) lDisabledObjects.append(lIt -> getPath());
      }
      
    }
    
    aJsonOutput["disabledAMCs"] = lDisabledAMCs;
    aJsonOutput["disabledDTMs"] = lDisabledDTMs;
    aJsonOutput["maskedLinks"] = lMaskedLinks;
    aJsonOutput["maskedAMCPorts"] = lMaskedAMCPorts;
    aJsonOutput["maskedInputs"] = lMaskedInputs;
    aJsonOutput["otherMaskedObjects"] = lMaskedObjects;
    aJsonOutput["otherDisabledObjects"] = lDisabledObjects;

    return;
  }

} // end ms swatchframework

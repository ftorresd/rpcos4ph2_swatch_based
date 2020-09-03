/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   MaskingConfigurator.cpp
 * Author: ale
 *
 * Created on February 15, 2016, 9:02 AM
 */

#include "swatch/system/MaskingConfigurator.hpp"



#include <deque>                        // for deque, operator==
#include <ostream>                      // for operator<<, basic_ostream, etc
#include <utility>                      // for pair

// boost headers
#include "boost/foreach.hpp"

// log4cplus headers
#include <log4cplus/loggingmacros.h>

// SWATCH Headers
#include "swatch/action/ActionableObject.hpp"
#include "swatch/action/GateKeeper.hpp"
#include "swatch/action/MaskableObject.hpp"
#include "swatch/action/MonitoringSetting.hpp"
#include "swatch/core/Object.hpp"
#include "swatch/dtm/DaqTTCManager.hpp"
#include "swatch/dtm/AMCPort.hpp"
#include "swatch/dtm/AMCPortCollection.hpp"
#include "swatch/system/Crate.hpp"
#include "swatch/system/Link.hpp"
#include "swatch/system/System.hpp"
#include "swatch/system/SystemStub.hpp"
#include "swatch/processor/Processor.hpp"
#include "swatch/processor/Port.hpp"


namespace swatch {
namespace system {

MaskingConfigurator::MaskingConfigurator( swatch::system::System& aSystem ) :
  mSystem(aSystem)
{
}


const std::vector<std::string>& MaskingConfigurator::getAutoDisabledDTMs() const
{
  return mAutoDisabledDtms;
}


const std::vector<std::string>& MaskingConfigurator::getAutoDisabledAMCs() const
{
  return mAutoDisabledAMCs;
}


const std::vector<std::string>& MaskingConfigurator::getAutoDisabledAMCPorts() const
{
  return mAutoDisabledAMCPorts;
}


const std::vector<std::string>& MaskingConfigurator::getAutoMaskedLinks() const
{
  return mAutoMaskedLinks;
}


const std::vector<std::string>& MaskingConfigurator::getAutoMaskedInputs() const
{
  return mAutoMaskedInputs;
}


void MaskingConfigurator::clearLists()
{
  mAutoDisabledDtms.clear();
  mAutoDisabledAMCs.clear();
  mAutoDisabledAMCPorts.clear();
  mAutoMaskedLinks.clear();
  mAutoMaskedInputs.clear();
}

//---
void MaskingConfigurator::disableCrates(const FEDEnableMap_t& aFEDEnableMap)
{


  mAutoDisabledDtms.clear();
  mAutoDisabledAMCs.clear();

  // First pass, disable AMC13s and connected AMCs
  BOOST_FOREACH(const swatch::system::System::CratesMap_t::value_type& c, mSystem.getCrates()) {
    swatch::system::Crate* lCrate = c.second;

    swatch::dtm::DaqTTCManager* lDtm = lCrate->amc13();
    if (!lDtm) continue;

    // Disable amc13 is FEDEnableMap says so
    FEDEnableMap_t::const_iterator it;
    if ((it = aFEDEnableMap.find(lDtm->getFedId())) != aFEDEnableMap.end() and it->second == 0) {
      LOG4CPLUS_INFO(mSystem.getLogger(), "Masking AMC13 " << lDtm->getId() << ", FED " << lDtm->getFedId() << ", value " << it->second);
      lDtm->disable();
      // Track AMC13s masked by FEDMap
      mAutoDisabledDtms.push_back(lDtm->getPath());
    }

    std::vector<uint32_t> slots = lCrate->getPopulatedSlots();
    // Disable
    if (!lDtm->getStatus().isEnabled()) {
      LOG4CPLUS_INFO(mSystem.getLogger(), "AMC13 " << lDtm->getId() << " is disabled. Disabling all processors in the same crate " << lCrate->getId());

      BOOST_FOREACH(uint32_t s, slots) {
        if ( s == lDtm->getSlot() ) continue;
        lCrate->amc(s)->disable();

        // Track masked amcs
        mAutoDisabledAMCs.push_back(lCrate->amc(s)->getPath());
      }
    }
  }
}


//---
void MaskingConfigurator::maskAMCPorts()
{

  mAutoDisabledAMCPorts.clear();

  // Second pass, mask AMC13 ports
  BOOST_FOREACH(const swatch::system::System::CratesMap_t::value_type& c, mSystem.getCrates()) {
    swatch::system::Crate* lCrate = c.second;
    swatch::dtm::DaqTTCManager* lDtm = lCrate->amc13();

    if (!lDtm) continue;

    BOOST_FOREACH(swatch::dtm::AMCPort* port, lDtm->getAMCPorts().getPorts()) {

      swatch::processor::Processor* lAmc = lCrate->amc(port->getSlot());

      // Mask port if slot is not populated or amc is disabled
      if ( !lAmc or !lAmc->getStatus().isEnabled()) {
        port->setMasked();
        //TODO: Dynamic monitoring settings?
        port->setMonitoringStatus(swatch::core::monitoring::kDisabled);

        // Keep track of masked ports
        mAutoDisabledAMCPorts.push_back(port->getPath());
      }
    }

  }
}


void MaskingConfigurator::maskInternalLinks()
{
  mAutoMaskedLinks.clear();

  const std::deque<swatch::system::Link*>& lLinks = mSystem.getLinks();
  if ( !lLinks.empty() ) {

    BOOST_FOREACH( swatch::system::Link* lLink, mSystem.getLinks() ) {
      if ( lLink->getSrcProcessor()->getStatus().isEnabled() ) continue;

      lLink->getDstPort()->setMasked();
      //TODO: Dynamic monitoring settings?
      lLink->getDstPort()->setMonitoringStatus(swatch::core::monitoring::kDisabled);

      // Keep track of masked links
      mAutoMaskedLinks.push_back(lLink->getPath());
    }
  }

}


void MaskingConfigurator::maskExternalInputs(const FEDEnableMap_t& aFEDEnableMap)
{

  mAutoMaskedInputs.clear();

  // Final step, FEDEnableMap-based masking
  FEDEnableMap_t::const_iterator itFED;
  BOOST_FOREACH( const SystemStub::FEDInputPortsMap_t::value_type& lPair, mSystem.getStub().connectedFEDs) {
    // Fed is there and is enabled
    if ( (itFED = aFEDEnableMap.find(lPair.first)) != aFEDEnableMap.end() and itFED->second != 0) continue;

    BOOST_FOREACH( const std::string& path, lPair.second) {
      swatch::action::MaskableObject* lMaskable = mSystem.getObjPtr<swatch::action::MaskableObject>(path);

      // I should really throw here
      if ( !lMaskable ) continue;

      lMaskable->setMasked();

      //TODO: Dynamic monitoring settings?
      lMaskable->setMonitoringStatus(swatch::core::monitoring::kDisabled);

      // Keep track of the masked input ports
      mAutoMaskedInputs.push_back(lMaskable->getPath());
    }
  }
}


//---
void MaskingConfigurator::applyDynamicMasks( const FEDEnableMap_t& aFEDEnableMap  )
{

  // First pass, mask AMC13 ports
  maskAMCPorts();

  // Second step, mask input ports linked to disabled processors
  maskInternalLinks();

  // Final step, FEDEnableMap-based masking
  maskExternalInputs(aFEDEnableMap);

}


//---
void MaskingConfigurator::clear()
{
  clearLists();

  BOOST_FOREACH(const auto& p, mSystem.getActionableChildren()) {
    // Enable
    p.second->enable();


    for ( core::Object::iterator itObj = mSystem.begin(); itObj != mSystem.end(); ++itObj) {
      action::MaskableObject* lMaskable = dynamic_cast<swatch::action::MaskableObject*> (&(*itObj));

      if ( !lMaskable ) continue;

      lMaskable->setMasked(false);

    }
    // Unmask everything
    // p.second->clearMasks();
  }
}


//---
void MaskingConfigurator::applyStatic(const swatch::action::GateKeeper& aGateKeeper)
{
  clearLists();

  // Apply static masks
  BOOST_FOREACH(const auto& p, mSystem.getActionableChildren()) {

    swatch::action::ActionableObject& lAct = *p.second;

    // Disable
    ( aGateKeeper.isEnabled(lAct.getId()) ? lAct.enable() : lAct.disable());

    lAct.resetAndApplyMasks(aGateKeeper);
  }
}


} // namespace system
} //namespace swatch

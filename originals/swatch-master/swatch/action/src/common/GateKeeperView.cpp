/*
 * File:   GateKeeperView.cpp
 * Author: Luke Kreczko
 * Date: February 2016
 */


#include <stddef.h>                     // for size_t
#include <string>                       // for string
#include <vector>                       // for vector

// boost headers
#include "boost/foreach.hpp"
#include "boost/smart_ptr/shared_ptr.hpp"  // for shared_ptr
#include "boost/unordered/unordered_map.hpp"  // for unordered_map, etc
#include "boost/unordered/unordered_set.hpp"  // for unordered_set, etc

// SWATCH headers
#include "swatch/action/GateKeeperView.hpp"


namespace swatch {
namespace action {

GateKeeperView::GateKeeperView(const GateKeeper& aGateKeeper) :
  mParameters(), mMonsettings(), mMasks(), mDisabledFlags()
{
  extractDisabled(aGateKeeper);
  extractMasks(aGateKeeper);
  extractMonitoringSettings(aGateKeeper);
  extractParameters(aGateKeeper);
}

GateKeeperView::GateKeeperView(const GateKeeper& aGateKeeper, const std::vector<std::string>& aContextsToLookIn) :
  mParameters(), mMonsettings(), mMasks(), mDisabledFlags()
{
  extractDisabled(aGateKeeper);
  extractMasks(aGateKeeper, aContextsToLookIn);
  extractMonitoringSettings(aGateKeeper, aContextsToLookIn);
  extractParameters(aGateKeeper, aContextsToLookIn);
}

void GateKeeperView::extractDisabled(const GateKeeper& aGateKeeper)
{
  mDisabledFlags.clear();
  mDisabledFlags.reserve(aGateKeeper.mDisabledObjs.size());
//  BOOST_FOREACH(const GateKeeper::DisabledSet_t lIt, aGateKeeper.mDisabledObjs){
  for (GateKeeper::DisabledSet_t::const_iterator lIt = aGateKeeper.mDisabledObjs.cbegin();
       lIt != aGateKeeper.mDisabledObjs.cend(); ++lIt) {
    DisabledFlagView_t lView(*lIt, *lIt);
    mDisabledFlags.insert(lView);
  }
}

void GateKeeperView::extractMasks(const GateKeeper& aGateKeeper, const std::vector<std::string>& aContextsToLookIn)
{
  mMasks.clear();
  std::vector<std::string> lContextsToLookIn(aContextsToLookIn);
  if (aContextsToLookIn.empty()) {
    lContextsToLookIn = extractMapKeys(aGateKeeper.mMasks);
  }

  BOOST_FOREACH(const std::string& lContext, lContextsToLookIn) {
    GateKeeper::MasksContext_t lMasks(aGateKeeper.mMasks.at(lContext));
    for (GateKeeper::Masks_t::const_iterator lMask = lMasks->cbegin(); lMask != lMasks->cend(); ++lMask) {
      MaskView_t lView(*lMask, *lMask, lContext);
      mMasks.insert(lView);
    }
  }
}

void GateKeeperView::extractMonitoringSettings(const GateKeeper& aGateKeeper,
    const std::vector<std::string>& aContextsToLookIn)
{
  mMonsettings.clear();
  std::vector<std::string> lContextsToLookIn(aContextsToLookIn);
  if (aContextsToLookIn.empty()) {
    lContextsToLookIn = extractMapKeys(aGateKeeper.mSettings);
  }

  BOOST_FOREACH(const std::string& lContext, lContextsToLookIn) {
    GateKeeper::SettingsContext_t lSettings(aGateKeeper.mSettings.at(lContext));
    for (GateKeeper::MonitoringSettings_t::const_iterator lSetting = lSettings->cbegin(); lSetting != lSettings->cend();
         ++lSetting) {
      MonitoringView_t lView(lSetting->second, lSetting->first, lContext);
      mMonsettings.insert(lView);
    }
  }
}

void GateKeeperView::extractParameters(const GateKeeper& aGateKeeper,
                                       const std::vector<std::string>& aContextsToLookIn)
{
  mParameters.clear();
  std::vector<std::string> lContextsToLookIn(aContextsToLookIn);
  if (aContextsToLookIn.empty()) {
    lContextsToLookIn = extractMapKeys(aGateKeeper.mParameters);
  }

  BOOST_FOREACH(const std::string& lContext, lContextsToLookIn) {
    GateKeeper::ParametersContext_t lParameters(aGateKeeper.mParameters.at(lContext));
    for (GateKeeper::Parameters_t::const_iterator lParameter = lParameters->cbegin(); lParameter != lParameters->cend();
         ++lParameter) {
      ParameterView_t lView(lParameter->second, lParameter->first, lContext);
      mParameters.insert(lView);
    }
  }
}

GateKeeperView::~GateKeeperView()
{
}

GateKeeperView::Parameters_t::const_iterator GateKeeperView::parametersBegin() const
{
  return mParameters.cbegin();
}
GateKeeperView::Parameters_t::const_iterator GateKeeperView::parametersEnd() const
{
  return mParameters.cend();
}
const size_t GateKeeperView::parametersSize() const
{
  return mParameters.size();
}

GateKeeperView::MonitoringSettings_t::const_iterator GateKeeperView::monitoringSettingsBegin() const
{
  return mMonsettings.cbegin();
}
GateKeeperView::MonitoringSettings_t::const_iterator GateKeeperView::monitoringSettingsEnd() const
{
  return mMonsettings.cend();
}
const size_t GateKeeperView::monitoringSettingsSize() const
{
  return mMonsettings.size();
}

GateKeeperView::Masks_t::const_iterator GateKeeperView::masksBegin() const
{
  return mMasks.cbegin();
}
GateKeeperView::Masks_t::const_iterator GateKeeperView::masksEnd() const
{
  return mMasks.cend();
}
const size_t GateKeeperView::masksSize() const
{
  return mMasks.size();
}

GateKeeperView::DisabledFlags_t::const_iterator GateKeeperView::disabledFlagsBegin() const
{
  return mDisabledFlags.cbegin();
}
GateKeeperView::DisabledFlags_t::const_iterator GateKeeperView::disabledFlagsEnd() const
{
  return mDisabledFlags.cend();
}

const size_t GateKeeperView::disabledFlagsSize() const
{
  return mDisabledFlags.size();
}
}
}

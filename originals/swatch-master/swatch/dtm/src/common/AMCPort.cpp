/*
 * File:   AMCLink.cpp
 * Author: ale
 *
 * Created on October 5, 2015, 5:00 PM
 */

#include "swatch/dtm/AMCPort.hpp"


#include "swatch/core/utilities.hpp"


namespace swatch {
namespace dtm {

AMCPort::AMCPort(uint32_t aSlot) :
  swatch::action::MaskableObject(swatch::core::strPrintf("amc%02d",aSlot)),
  mSlotId(aSlot)
{
}

AMCPort::~AMCPort()
{
}

uint32_t AMCPort::getSlot() const
{
  return mSlotId;
}


} // namespace dtm
} // namespace swatch

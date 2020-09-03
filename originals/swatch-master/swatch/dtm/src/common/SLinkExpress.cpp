/*
 * File:   SLink.cpp
 * Author: ale
 *
 * Created on October 1, 2015, 3:13 PM
 */


#include "swatch/dtm/SLinkExpress.hpp"

#include "swatch/core/MetricConditions.hpp"
#include "swatch/core/utilities.hpp"

namespace swatch {
namespace dtm {

// --------------------------------------------------------
SLinkExpress::SLinkExpress( uint32_t aSfpId ) :
  swatch::core::MonitorableObject(swatch::core::strPrintf("slinkXp%d",aSfpId)),
  mSfpID(aSfpId)
{
}


// --------------------------------------------------------
SLinkExpress::~SLinkExpress()
{
}


} // namespace dtm
} // namespace swatch
/*
 * File:   SLinkExpress.hpp
 * Author: ale
 *
 * Created on October 1, 2015, 3:12 PM
 */

#ifndef __SWATCH_DTM_SLINKEXPRESS_HPP__
#define __SWATCH_DTM_SLINKEXPRESS_HPP__

#include "swatch/core/MonitorableObject.hpp"

namespace swatch {
namespace dtm {

class SLinkExpress : public core::MonitorableObject {
public:
  SLinkExpress( uint32_t aSfpID );
  virtual ~SLinkExpress();

protected:

  const uint32_t mSfpID;
};

} // namespace dtm
} // namespace swatch

#endif  /* __SWATCH_DTM_SLINK_HPP__ */


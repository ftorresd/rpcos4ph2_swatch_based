/*
 * File:   TTCInterface.hpp
 * Author: ale
 *
 * Created on September 21, 2015, 12:20 PM
 */

#ifndef __SWATCH_DTM_TTCINTERFACE_HPP__
#define __SWATCH_DTM_TTCINTERFACE_HPP__

#include "swatch/core/MonitorableObject.hpp"

namespace swatch {
namespace dtm {

class TTCInterface : public core::MonitorableObject {
protected:
  TTCInterface();
public:
  virtual ~TTCInterface( );

};


} // namespace amc13
} // namespace swatch



#endif  /* __SWATCH_DTM_TTCINTERFACE_HPP___ */


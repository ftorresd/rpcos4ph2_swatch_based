/*
 * File:   EVBInterface.hpp
 * Author: ale
 *
 * Created on September 21, 2015, 12:21 PM
 */

#ifndef __SWATCH_DTM_EVBINTERFACE_HPP__
#define __SWATCH_DTM_EVBINTERFACE_HPP__


#include "swatch/core/MonitorableObject.hpp"


namespace swatch {
namespace dtm {

class EVBInterface : public core::MonitorableObject {
protected:
  EVBInterface();
public:
  virtual ~EVBInterface();
private:

};

} // namespace dtm
} // namespace swatch


#endif  /* __SWATCH_DTM_EVBINTERFACE_HPP__ */


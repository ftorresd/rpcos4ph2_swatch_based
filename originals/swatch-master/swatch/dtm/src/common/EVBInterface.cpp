/*
 * File:   DAQInterface.cpp
 * Author: ale
 *
 * Created on September 21, 2015, 12:21 PM
 */

#include "swatch/dtm/EVBInterface.hpp"

namespace swatch {
namespace dtm {


EVBInterface::EVBInterface() :
  core::MonitorableObject( "evb" )
{
}

EVBInterface::~EVBInterface()
{
}


} // namespace dtm
} // namespace swatch



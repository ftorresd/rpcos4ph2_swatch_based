/*
 * File:   AMCLinkCollection.cpp
 * Author: ale
 *
 * Created on October 5, 2015, 4:57 PM
 */

#include "swatch/dtm/AMCPortCollection.hpp"


#include "swatch/dtm/AMCPort.hpp"


namespace swatch {
namespace dtm {


AMCPortCollection::AMCPortCollection() :
  core::MonitorableObject("amcports")
{
}


AMCPortCollection::~AMCPortCollection()
{
}


size_t AMCPortCollection::getNumPorts() const
{
  return mPorts.size();
}


const std::deque<const AMCPort*>& AMCPortCollection::getPorts() const
{
  return mConstPorts;
}


const std::deque<AMCPort*>& AMCPortCollection::getPorts()
{
  return mPorts;
}


void AMCPortCollection::addPort(AMCPort* aAMCPort)
{
  this->addMonitorable(aAMCPort);
  mConstPorts.push_back(aAMCPort);
  mPorts.push_back(aAMCPort);
}


} // namespace dtm
} // namespace swatch

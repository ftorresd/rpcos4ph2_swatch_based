/*
 * File:   Link.cpp
 * Author: ale
 */

#include "swatch/system/Link.hpp"


#include "swatch/processor/Port.hpp"


namespace swatch {
namespace system {

Link::Link(const std::string& aId, const std::string& aAlias, processor::Processor* aSrcProcessor, processor::OutputPort* aSrcPort, processor::Processor* aDstProcessor, processor::InputPort* aDstPort) :
  ObjectView( aId, aAlias ),
  mSrcProcessor(aSrcProcessor),
  mSrcPort(aSrcPort),
  mDstProcessor(aDstProcessor),
  mDstPort(aDstPort)
{
  this->addObj(mSrcPort, "src");
  this->addObj(mDstPort, "dst");
}

Link::~Link()
{
}


const processor::Processor* Link::getSrcProcessor() const
{
  return mSrcProcessor;
}

processor::Processor* Link::getSrcProcessor()
{
  return mSrcProcessor;
}


const processor::OutputPort* Link::getSrcPort() const
{
  return mSrcPort;
}

processor::OutputPort* Link::getSrcPort()
{
  return mSrcPort;
}

const processor::Processor* Link::getDstProcessor() const
{
  return mDstProcessor;
}

processor::Processor* Link::getDstProcessor()
{
  return mDstProcessor;
}

const processor::InputPort* Link::getDstPort() const
{
  return mDstPort;
}

processor::InputPort* Link::getDstPort()
{
  return mDstPort;
}



} // namespace core
} // namespace swatch

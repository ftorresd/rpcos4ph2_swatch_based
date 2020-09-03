
#include "swatch/processor/PortCollection.hpp"


// Standard headers
#include <stdexcept>                    // for runtime_error

// SWATCH headers
#include "swatch/processor/Port.hpp"


namespace swatch {
namespace processor {


InputPortCollection::InputPortCollection() :
  core::MonitorableObject( "inputPorts" )
{
}


InputPortCollection::~InputPortCollection()
{
}


size_t InputPortCollection::getNumPorts() const
{
  return mPorts.size();
}


const std::deque<const InputPort*>& InputPortCollection::getPorts() const
{
  return mConstPorts;
}


const std::deque<InputPort*>& InputPortCollection::getPorts()
{
  return mPorts;
}


const InputPort& InputPortCollection::getPort( const std::string& aId ) const
{
  if (const InputPort* in = getObjPtr<InputPort>( aId ))
    return *in;
  else
    XCEPT_RAISE(core::RuntimeError,"PortCollection \"" + this->getPath() + "\" does not contain any input port of ID \"" + aId + "\"");
}


InputPort& InputPortCollection::getPort( const std::string& aId )
{
  if (InputPort* in = getObjPtr<InputPort>( aId ))
    return *in;
  else
    XCEPT_RAISE(core::RuntimeError,"PortCollection \"" + this->getPath() + "\" does not contain any input port of ID \"" + aId + "\"");
}


void InputPortCollection::addPort(InputPort* aInput)
{
  addMonitorable(aInput);
  mConstPorts.push_back(aInput);
  mPorts.push_back(aInput);
}




OutputPortCollection::OutputPortCollection() :
  core::MonitorableObject( "outputPorts" )
{
}


OutputPortCollection::~OutputPortCollection()
{
}


size_t OutputPortCollection::getNumPorts() const
{
  return mPorts.size();
}


const std::deque<const OutputPort*>& OutputPortCollection::getPorts() const
{
  return mConstPorts;
}


const std::deque<OutputPort*>& OutputPortCollection::getPorts()
{
  return mPorts;
}


const OutputPort& OutputPortCollection::getPort( const std::string& aId ) const
{
  if (const OutputPort* in = getObjPtr<OutputPort>( aId ))
    return *in;
  else
    XCEPT_RAISE(core::RuntimeError,"PortCollection \"" + this->getPath() + "\" does not contain any output port of ID \"" + aId + "\"");
}


OutputPort& OutputPortCollection::getPort( const std::string& aId )
{
  if (OutputPort* in = getObjPtr<OutputPort>( aId ))
    return *in;
  else
    XCEPT_RAISE(core::RuntimeError,"PortCollection \"" + this->getPath() + "\" does not contain any output port of ID \"" + aId + "\"");
}


void OutputPortCollection::addPort(OutputPort* aOutput)
{
  addMonitorable(aOutput);
  mConstPorts.push_back(aOutput);
  mPorts.push_back(aOutput);
}


} // end ns processor
} // end ns swatch

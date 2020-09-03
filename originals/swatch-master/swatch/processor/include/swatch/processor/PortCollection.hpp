/**
 * @file    PortCollection.hpp
 * @author  Alessandro Thea
 * @date    20.08.2014
 */

#ifndef __SWATCH_PROCESSOR_PORTCOLLECTION_HPP__
#define __SWATCH_PROCESSOR_PORTCOLLECTION_HPP__


// Standard headers
#include <stddef.h>                     // for size_t
#include <deque>                        // for deque, etc
#include <string>                       // for string

// SWATCH headers
#include "swatch/core/MonitorableObject.hpp"


namespace swatch {
namespace processor {

class InputPort;
class OutputPort;


//! Class that aggregates the input ports of a processor
class InputPortCollection : public core::MonitorableObject {
public:
  InputPortCollection();

  virtual ~InputPortCollection();

  //! Returns number of input channels
  size_t getNumPorts() const;

  typedef std::deque<const InputPort*>::const_iterator const_iterator;
  typedef std::deque<InputPort*>::const_iterator iterator;

  const std::deque<const InputPort*>& getPorts() const;

  const std::deque<InputPort*>& getPorts();

  const InputPort& getPort( const std::string& aId ) const;

  InputPort& getPort( const std::string& aId );

  void addPort( InputPort* aInput );

protected:
  void retrieveMetricValues() {}

private:
  std::deque<const InputPort*> mConstPorts;
  std::deque<InputPort*> mPorts;
};


//! Class that aggregates the output ports of a processor
class OutputPortCollection : public core::MonitorableObject {
public:
  OutputPortCollection();

  virtual ~OutputPortCollection();

  //! Returns number of input channels
  size_t getNumPorts() const;

  typedef std::deque<const OutputPort*>::const_iterator const_iterator;
  typedef std::deque<OutputPort*>::const_iterator iterator;

  const std::deque<const OutputPort*>& getPorts() const;

  const std::deque<OutputPort*>& getPorts();

  const OutputPort& getPort( const std::string& aId ) const;

  OutputPort& getPort( const std::string& aId );

  void addPort( OutputPort* aOutput );

protected:
  void retrieveMetricValues() {}

private:
  std::deque<const OutputPort*> mConstPorts;
  std::deque<OutputPort*> mPorts;
};


}
}

#endif  /* __SWATCH_PROCESSOR_PORTCOLLECTION_HPP__ */


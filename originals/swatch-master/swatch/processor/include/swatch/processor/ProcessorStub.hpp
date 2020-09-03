/**
 * @file    ProcessorStub.hpp
 * @author  Alessandro Thea
 * @date    09/11/14
 */

#ifndef __SWATCH_PROCESSOR_PROCESSORSTUB_HPP__
#define __SWATCH_PROCESSOR_PROCESSORSTUB_HPP__


// C++ headers
#include <iosfwd>
#include <stdint.h>                     // for uint32_t
#include <string>                       // for string
#include <vector>

// SWATCH headers
#include "swatch/core/AbstractStub.hpp"


namespace swatch {
namespace processor {

//! Structure to hold data to build a Processor port.
class ProcessorPortStub : public swatch::core::AbstractStub {
public:

  ProcessorPortStub(const std::string& aId);

  ProcessorPortStub(const std::string& aId, uint32_t aNumber);

  virtual ~ProcessorPortStub();

  //! Port number
  uint32_t number;
};


bool operator==(const ProcessorPortStub& aStub1, const ProcessorPortStub& aStub2);


//! Structure that holds the data required to construct a processor
class ProcessorStub : public swatch::core::AbstractStub {
public:
  typedef std::vector<ProcessorPortStub> ProcessorPorts_t;

  ProcessorStub(const std::string& aId);

  virtual ~ProcessorStub();

  //! Class to create the Processor object
  std::string creator;

  //! Type of the Processor
  std::string hwtype;

  //! Role of the processor
  std::string role;

  //! Uri to access the hardware resource
  std::string uri;

  //! Address table
  std::string addressTable;

  //! Id of the uTCA crate where the Processor is installed
  std::string crate;

  //! Slot where the board is installed
  uint32_t slot;

  //! List of receiver port bags
  ProcessorPorts_t rxPorts;

  //! List of transmitter port bags
  ProcessorPorts_t txPorts;
};


bool operator==(const ProcessorStub& aStub1, const ProcessorStub& aStub2);

std::ostream& operator<<(std::ostream& aStream, const swatch::processor::ProcessorStub& aStub);

typedef std::vector<ProcessorStub> ProcessorStubs_t;


} // namespace processor
} // namespace swatch


#endif /* __SWATCH_PROCESSOR_PROCESSORSTUB_HPP__ */

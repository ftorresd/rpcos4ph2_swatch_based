/**
 * @file    SystemStub.hpp
 * @author  Alessandro Thea
 * @date    11/11/14
 */

#ifndef __SWATCH_SYSTEM_SYSTEMSTUB_HPP__
#define __SWATCH_SYSTEM_SYSTEMSTUB_HPP__


// C++ headers
#include <iosfwd>
#include <map>
#include <stdint.h>                     // for uint32_t
#include <string>                       // for string
#include <vector>                       // for vector

// SWATCH headers
#include "swatch/core/AbstractStub.hpp"
#include "swatch/processor/ProcessorStub.hpp"
#include "swatch/system/LinkStub.hpp"
#include "swatch/system/CrateStub.hpp"
#include "swatch/dtm/DaqTTCStub.hpp"


namespace swatch {
namespace system {


//! Structure that holds the data required to build a system
class SystemStub : public swatch::core::AbstractStub {
public:

  typedef std::map<uint32_t, std::vector<std::string> > FEDInputPortsMap_t;
  typedef std::vector<std::string> ExcludedBoards_t;
  typedef std::vector<swatch::system::CrateStub> Crates_t;
  typedef swatch::processor::ProcessorStubs_t Processors_t;
  typedef std::vector<swatch::dtm::DaqTTCStub> DaqTTCs_t;
  typedef std::vector<swatch::system::LinkStub> Links_t;

  SystemStub(const std::string& aId);

  SystemStub(const std::string& aId, const std::string& aCreator);

  virtual ~SystemStub();

  //! Name of system class
  std::string creator;

  ExcludedBoards_t excludedBoards;

  Crates_t crates;

  Processors_t processors;

  DaqTTCs_t daqttcs;

  Links_t links;

  FEDInputPortsMap_t connectedFEDs;
};


bool operator==(const SystemStub& aStub1, const SystemStub& aStub2);

std::ostream& operator<<(std::ostream& aStream, const swatch::system::SystemStub& aStub);

} // namespace system
} // namespace swatch

#endif /* __SWATCH_SYSTEM_SYSTEMSTUB_HPP__ */


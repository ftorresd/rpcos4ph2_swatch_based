/**
 * @file    DAQTTCStub.hpp
 * @author  Alessandro Thea
 * @date    11/11/14
 */

#ifndef __SWATCH_DTM_DAQTTCSTUB_HPP__
#define __SWATCH_DTM_DAQTTCSTUB_HPP__


// C++ headers
#include <iosfwd>
#include <stdint.h>
#include <string>                       // for string

// SWATCH headers
#include "swatch/core/AbstractStub.hpp"


namespace swatch {
namespace dtm {


//! Structure that holds the data required to build a DaqTTCManager
class DaqTTCStub : public swatch::core::AbstractStub {
public:

  DaqTTCStub(const std::string& aId);

  virtual ~DaqTTCStub();

  //! Class to create the DaqTTC object
  std::string creator;

  //! Role of the DaqTTC object
  std::string role;

  //! Uri to access the hardware resource
  std::string uriT1;

  //! Address table
  std::string addressTableT1;

  //! Uri to access the hardware resource
  std::string uriT2;

  //! Address table
  std::string addressTableT2;

  //! Id of the uTCA crate where the DaqTTC is installed
  std::string crate;

  //! Slot where the board is installed
  uint32_t slot;

  //! ID for this FED
  uint16_t fedId;
};


bool operator==(const DaqTTCStub& aStub1, const DaqTTCStub& aStub2);

std::ostream& operator<<(std::ostream& aStream, const swatch::dtm::DaqTTCStub& aStub);


} // namespace dtm
} // namespace swatch


#endif /* __SWATCH_DTM_DAQTTCSTUB_HPP__ */

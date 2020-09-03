/**
 * @file    LinkStub.hpp
 * @author  Tom Williams
 * @date    June 2015
 */

#ifndef __SWATCH_SYSTEM_LINKSTUB_HPP__
#define __SWATCH_SYSTEM_LINKSTUB_HPP__


// C++ headers
#include <iosfwd>
#include <string>                       // for string

// SWATCH headers
#include "swatch/core/AbstractStub.hpp"


namespace swatch {
namespace system {


//! Structure that holds data required to build a link
class LinkStub : public swatch::core::AbstractStub {
public:

  LinkStub(const std::string& aId) :
    AbstractStub(aId) { }

  virtual ~LinkStub() { }

  //! Name of the source processor
  std::string srcProcessor;

  //! Name of the source port
  std::string srcPort;

  //! Name of the destination port
  std::string dstProcessor;

  //! Name of the destination port
  std::string dstPort;
};


bool operator==(const LinkStub& aStub1, const LinkStub& aStub2);

std::ostream& operator<<(std::ostream& aStream, const swatch::system::LinkStub& aStub);


} // namespace processor
} // namespace swatch


#endif /* __SWATCH_SYSTEM_LINKSTUB_HPP__ */

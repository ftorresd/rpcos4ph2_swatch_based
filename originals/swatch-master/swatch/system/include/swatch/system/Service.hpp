/*
 * File:   Service.hpp
 * Author: ale
 * Date:   July 2014
 */

#ifndef __SWATCH_SYSTEM_SERVICE_HPP__
#define __SWATCH_SYSTEM_SERVICE_HPP__


#include "swatch/core/Object.hpp"


namespace swatch {

namespace core {
class AbstractStub;
}


namespace system {


class Service : public core::Object {
public:
  Service( const swatch::core::AbstractStub& aStub );
  virtual ~Service();
};


}
}

#endif  /* __SWATCH_SYSTEM_SERVICE_HPP__ */


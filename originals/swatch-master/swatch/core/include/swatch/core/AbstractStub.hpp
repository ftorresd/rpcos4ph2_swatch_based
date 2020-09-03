/**
 * @file    AbstractStub.hpp
 * @author  Alessandro Thea
 * @date    July 2015
 */

#ifndef __SWATCH_CORE_ABSTRACTSTUB_HPP__
#define __SWATCH_CORE_ABSTRACTSTUB_HPP__


// C++ Headers
#include <string>


namespace swatch {
namespace core {

class AbstractStub {
public:
  explicit AbstractStub( const std::string& aId ) : id(aId), loggerName("swatch") {}
  virtual ~AbstractStub() {}

  std::string id;

  std::string alias;

  //! Hierarchical name of the logger (e.g. systemA.processorB)
  std::string loggerName;
};

}
}

#endif  /* __SWATCH_CORE_ABSTRACTSTUB_HPP__ */


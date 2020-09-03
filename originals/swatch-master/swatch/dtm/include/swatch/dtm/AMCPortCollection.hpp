/*
 * File:   AMCLinkCollection.hpp
 * Author: ale
 *
 * Created on October 5, 2015, 4:57 PM
 */

#ifndef __SWATCH_DTM_AMCPORTCOLLECTION_HPP__
#define __SWATCH_DTM_AMCPORTCOLLECTION_HPP__


// Standard headers
#include <stddef.h>                     // for size_t
#include <deque>                        // for deque, etc

// SWATCH headers
#include "swatch/core/MonitorableObject.hpp"


namespace swatch {
namespace dtm {

class AMCPort;

class AMCPortCollection : public core::MonitorableObject {
public:
  AMCPortCollection();

  virtual ~AMCPortCollection();

  //! Returns number of ports
  size_t getNumPorts() const;

  typedef std::deque<const AMCPort*>::const_iterator const_iterator;
  typedef std::deque<AMCPort*>::const_iterator iterator;

  const std::deque<const AMCPort*>& getPorts() const;

  const std::deque<AMCPort*>& getPorts();

  void addPort( AMCPort* aAMCPort );

protected:
  void retrieveMetricValues() {};

private:
  std::deque<const AMCPort*> mConstPorts;
  std::deque<AMCPort*> mPorts;
};


} // namespace dtm
} // namespace swatch

#endif  /* __SWATCH_DTM_AMCPORTCOLLECTION_HPP__ */


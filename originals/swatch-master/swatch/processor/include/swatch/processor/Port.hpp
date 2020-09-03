/*
 * File:   Port.hpp
 * Author: ale
 */

#ifndef __SWATCH_PROCESSOR_PORT_HPP__
#define __SWATCH_PROCESSOR_PORT_HPP__


// Standard headers
#include <stdint.h>                     // for uint32_t
#include <string>                       // for string
#include <vector>                       // for vector

// SWATCH headers
#include "swatch/action/MaskableObject.hpp"
#include "swatch/core/MonitorableObject.hpp"


namespace swatch {

namespace core {
template <typename DataType> class SimpleMetric;
}

namespace processor {

//! Base class defining the interface of an input optical port
class InputPort : public action::MaskableObject {
protected:
  explicit InputPort( const std::string& aId, const std::string& aAlias = "");

public:
  virtual ~InputPort() {}

  static const std::string kMetricIdIsLocked;
  static const std::string kMetricIdIsAligned;
  static const std::string kMetricIdCRCErrors;

  static const std::vector<std::string> kDefaultMetrics;

protected:
  core::SimpleMetric<bool>& mMetricIsLocked;
  core::SimpleMetric<bool>& mMetricIsAligned;
  core::SimpleMetric<uint32_t>& mMetricCRCErrors;
};


//! Base class defining the interface of an output optical port
class OutputPort : public core::MonitorableObject {
protected:
  explicit OutputPort( const std::string& aId, const std::string& aAlias = "");

public:
  virtual ~OutputPort() {}

  static const std::string kMetricIdIsOperating;

  static const std::vector<std::string> kDefaultMetrics;

protected:
  core::SimpleMetric<bool>& mMetricIsOperating;
};

}
}

#endif  /* __SWATCH_PROCESSOR_PORT_HPP__ */


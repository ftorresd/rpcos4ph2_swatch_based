/**
 * @file    AlgoInterface.cpp
 * @author  Alessandro Thea
 */

#ifndef __SWATCH_PROCESSOR_ALGOINTERFACE_HPP__
#define __SWATCH_PROCESSOR_ALGOINTERFACE_HPP__


// SWATCH headers
#include "swatch/core/MonitorableObject.hpp"


namespace swatch {
namespace processor {

/**
 * @class  AlgoInterface
 * @author Alessandro Thea
 * @brief  Abstract class defining the algorithm component interface
 */
class AlgoInterface: public core::MonitorableObject {
public:

  AlgoInterface() : core::MonitorableObject( "algo" ) {}

  virtual ~AlgoInterface() {}

//    virtual void reset() = 0;
};

} // namespace processor
} // namespace swatch

#endif /* SWATCH_PROCESSOR_ALGOINTERFACE_HPP */

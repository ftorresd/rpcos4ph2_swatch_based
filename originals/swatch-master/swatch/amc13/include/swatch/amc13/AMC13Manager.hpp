/**
 * @file    AMC13Manager.hpp
 * @author  Alessandro Thea
 * @brief   Brief description
 * @date    07/11/14
 */

#ifndef __SWATCH_AMC13_AMC13MANAGER_HPP__
#define __SWATCH_AMC13_AMC13MANAGER_HPP__


#include <stdint.h>                     // for uint32_t

// Swatch Headers
#include "swatch/dtm/DaqTTCManager.hpp"


// Forward declaration
namespace amc13 {
class AMC13;
}


namespace swatch {

namespace core {
class AbstractStub;
template <typename DataType> class SimpleMetric;
}


namespace amc13 {

class AMC13Manager : public swatch::dtm::DaqTTCManager {
public:
  AMC13Manager(const swatch::core::AbstractStub& aStub);
  ~AMC13Manager();

  ::amc13::AMC13& driver()
  {
    return *mDriver;
  }

protected:

  virtual void retrieveMetricValues();


private:
  ::amc13::AMC13* mDriver;

  swatch::core::SimpleMetric<uint32_t>& mFwVersionT1;
  swatch::core::SimpleMetric<uint32_t>& mFwVersionT2;
};


} // namespace amc13
} // namespace swatch

#endif /* SWATCH_AMC13_AMC13MANAGER_HPP */

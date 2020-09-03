
#ifndef __SWATCH_DTM_TEST_DUMMYAMC13MANAGER_HPP__
#define __SWATCH_DTM_TEST_DUMMYAMC13MANAGER_HPP__


#include "swatch/dtm/DaqTTCManager.hpp"


namespace swatch {
namespace dtm {
namespace test {


class DummyAMC13Manager : public dtm::DaqTTCManager {
public:
  DummyAMC13Manager( const swatch::core::AbstractStub& aStub );

  virtual ~DummyAMC13Manager();

private:
  virtual void retrieveMetricValues();
};


} // namespace test
} // namespace dtm
} // namespace swatch


#endif  /* __SWATCH_DTM_TEST_DUMMYAMC13MANAGER_HPP__ */


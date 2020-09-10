
#ifndef _RPCOS4PH2_DUMMY_DUMMYAMC13MANAGER_HPP__
#define _RPCOS4PH2_DUMMY_DUMMYAMC13MANAGER_HPP__


// boost headers
#include "boost/smart_ptr/scoped_ptr.hpp"

// SWATCH headers
#include "swatch/core/AbstractStub.hpp"
#include "swatch/dtm/DaqTTCManager.hpp"


namespace rpcos4ph2 {
namespace dummy {


class DummyAMC13Driver;

class DummyAMC13Manager : public swatch::dtm::DaqTTCManager {
public:
  DummyAMC13Manager( const swatch::core::AbstractStub& aStub );

  virtual ~DummyAMC13Manager();

  DummyAMC13Driver& getDriver()
  {
    return *mDriver;
  }

private:
  virtual void retrieveMetricValues();

  boost::scoped_ptr<DummyAMC13Driver> mDriver;
};


} // namespace dummy
} // namespace rpcos4ph2


#endif  /* _RPCOS4PH2_DUMMY_DUMMYAMC13MANAGER_HPP__ */


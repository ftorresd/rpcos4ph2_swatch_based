
#include "swatch/dtm/test/DummyAMC13Manager.hpp"


#include "swatch/core/Factory.hpp"



SWATCH_REGISTER_CLASS(swatch::dtm::test::DummyAMC13Manager)


namespace swatch {
namespace dtm {
namespace test {


DummyAMC13Manager::DummyAMC13Manager( const swatch::core::AbstractStub& aStub ) :
  dtm::DaqTTCManager(aStub)
{
  if ((getStub().uriT1 == "throw") != (getStub().uriT2 == "throw"))
    XCEPT_RAISE(core::RuntimeError,"DummyAMC13Manager CTOR instructed to throw");
}


DummyAMC13Manager::~DummyAMC13Manager()
{
}


void DummyAMC13Manager::retrieveMetricValues()
{
}


} // namespace test
} // namespace system
} // namespace swatch


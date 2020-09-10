
#ifndef _RPCOS4PH2_DUMMY_DUMMYREADOUT_HPP__
#define _RPCOS4PH2_DUMMY_DUMMYREADOUT_HPP__


#include "swatch/processor/ReadoutInterface.hpp"


namespace rpcos4ph2 {
namespace dummy {

class DummyProcDriver;

/**
 * @class DummyReadoutInterface
 * @brief Dummy readout interface implementation
 */
class DummyReadoutInterface : public swatch::processor::ReadoutInterface {
public:
  DummyReadoutInterface(DummyProcDriver& aDriver);

  virtual ~DummyReadoutInterface();

  virtual void retrieveMetricValues();

private:
  DummyProcDriver& mDriver;
};

} // namespace dummy
} // namespace rpcos4ph2

#endif /* SWATCH_DUMMY_DUMMYREADOUT_HPP */

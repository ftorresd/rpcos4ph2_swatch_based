
#ifndef __SWATCH_DUMMY_DUMMYREADOUT_HPP__
#define __SWATCH_DUMMY_DUMMYREADOUT_HPP__


#include "swatch/processor/ReadoutInterface.hpp"


namespace swatch {
namespace dummy {

class DummyProcDriver;

/**
 * @class DummyReadoutInterface
 * @brief Dummy readout interface implementation
 */
class DummyReadoutInterface : public processor::ReadoutInterface {
public:
  DummyReadoutInterface(DummyProcDriver& aDriver);

  virtual ~DummyReadoutInterface();

  virtual void retrieveMetricValues();

private:
  DummyProcDriver& mDriver;
};

} // namespace dummy
} // namespace swatch

#endif /* SWATCH_DUMMY_DUMMYREADOUT_HPP */

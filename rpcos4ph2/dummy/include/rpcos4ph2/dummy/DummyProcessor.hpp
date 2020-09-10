
#ifndef _RPCOS4PH2_DUMMY_DUMMYPROCESSOR_HPP__
#define _RPCOS4PH2_DUMMY_DUMMYPROCESSOR_HPP__


// boost headers
#include "boost/scoped_ptr.hpp"

// SWATCH headers
#include "swatch/processor/Processor.hpp"


namespace rpcos4ph2 {
namespace dummy {

class DummyProcDriver;


bool filterOutMaskedPorts(const swatch::core::MonitorableObject& aObj);

const uint32_t* sumUpCRCErrors(const std::vector<swatch::core::MetricSnapshot>& aSnapshots);


class DummyProcessor : public swatch::processor::Processor {
public:
  DummyProcessor( const swatch::core::AbstractStub& aStub );
  virtual ~DummyProcessor();

  virtual std::string firmwareInfo() const;

  DummyProcDriver& getDriver()
  {
    return *mDriver;
  }

protected:
  virtual void retrieveMetricValues();

private:
  boost::scoped_ptr<DummyProcDriver> mDriver;
};


} // namespace dummy
} // namespace rpcos4ph2

#endif  /* SWATCH_DUMMY_DUMMYPROCESSOR_HPP */


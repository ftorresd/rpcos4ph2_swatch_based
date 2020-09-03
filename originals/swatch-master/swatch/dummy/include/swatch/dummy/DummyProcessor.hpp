
#ifndef __SWATCH_DUMMY_DUMMYPROCESSOR_HPP__
#define __SWATCH_DUMMY_DUMMYPROCESSOR_HPP__


// boost headers
#include "boost/scoped_ptr.hpp"

// SWATCH headers
#include "swatch/processor/Processor.hpp"


namespace swatch {
namespace dummy {

class DummyProcDriver;


bool filterOutMaskedPorts(const core::MonitorableObject& aObj);

const uint32_t* sumUpCRCErrors(const std::vector<core::MetricSnapshot>& aSnapshots);


class DummyProcessor : public processor::Processor {
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
} // namespace swatch

#endif  /* SWATCH_DUMMY_DUMMYPROCESSOR_HPP */


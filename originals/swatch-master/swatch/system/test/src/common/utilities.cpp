
#include "swatch/system/test/utilities.hpp"


#include <iomanip>
#include <sstream>

#include "swatch/processor/ProcessorStub.hpp"


namespace swatch {
namespace system {
namespace test {

void addRxTxPortStubs(swatch::processor::ProcessorStub& aProcStub)
{
  for (size_t lChan=0; lChan < 4; ++lChan) {
    std::ostringstream lSS;
    lSS << "rx" << std::setw(2) << std::setfill('0') << lChan;
    swatch::processor::ProcessorPortStub lPortStub( lSS.str() );
    lPortStub.number = lChan;
    aProcStub.rxPorts.push_back(lPortStub);
  }

  for (size_t lChan=0; lChan < 2; ++lChan) {
    std::ostringstream lSS;
    lSS << "tx" << std::setw(2) << std::setfill('0') << lChan;
//        xdata::Bag<swatch::processor::ProcessorPortStub> portBag;
    swatch::processor::ProcessorPortStub lPortStub( lSS.str() );
    lPortStub.number = lChan;
    aProcStub.txPorts.push_back(lPortStub);
  }
}

}
}
}

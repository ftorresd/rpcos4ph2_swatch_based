
#ifndef _RPCOS4PH2_DUMMY_DUMMYAMC13INTERFACES_HPP__
#define _RPCOS4PH2_DUMMY_DUMMYAMC13INTERFACES_HPP__

#include "swatch/dtm/AMCPort.hpp"
#include "swatch/dtm/EVBInterface.hpp"
#include "swatch/dtm/SLinkExpress.hpp"
#include "swatch/dtm/TTCInterface.hpp"

namespace rpcos4ph2
{
    namespace dummy
    {

        class DummyAMC13Driver;

        class AMC13BackplaneDaqPort : public swatch::dtm::AMCPort
        {
        public:
            AMC13BackplaneDaqPort(uint32_t aSlot, DummyAMC13Driver &aDriver);
            ~AMC13BackplaneDaqPort();

        private:
            void retrieveMetricValues();

            DummyAMC13Driver &mDriver;
            swatch::core::SimpleMetric<bool> &mOOS;
            swatch::core::SimpleMetric<bool> &mTTSWarning;
            swatch::core::SimpleMetric<uint64_t> &mAMCEventCount;
        };

        class AMC13EventBuilder : public swatch::dtm::EVBInterface
        {
        public:
            AMC13EventBuilder(DummyAMC13Driver &aDriver);
            ~AMC13EventBuilder();

        private:
            void retrieveMetricValues();

            DummyAMC13Driver &mDriver;
            swatch::core::SimpleMetric<bool> &mOOS;
            swatch::core::SimpleMetric<bool> &mTTSWarning;
            swatch::core::SimpleMetric<uint64_t> &mL1ACount;
        };

        class AMC13SLinkExpress : public swatch::dtm::SLinkExpress
        {
        public:
            AMC13SLinkExpress(uint32_t aSfpID, DummyAMC13Driver &aDriver);
            ~AMC13SLinkExpress();

        private:
            void retrieveMetricValues();

            DummyAMC13Driver &mDriver;
            swatch::core::SimpleMetric<bool> &mCoreInitialised;
            swatch::core::SimpleMetric<bool> &mBackPressure;
            swatch::core::SimpleMetric<uint32_t> &mWordsSent;
            swatch::core::SimpleMetric<uint32_t> &mPacketsSent;
        };

        class AMC13TTC : public swatch::dtm::TTCInterface
        {
        public:
            AMC13TTC(DummyAMC13Driver &aDriver);
            ~AMC13TTC();

        private:
            void retrieveMetricValues();

            DummyAMC13Driver &mDriver;
            swatch::core::SimpleMetric<double> &mClockFreq;
            swatch::core::SimpleMetric<uint32_t> &mBC0Counter;
            swatch::core::SimpleMetric<uint32_t> &mErrCountBC0;
            swatch::core::SimpleMetric<uint32_t> &mErrCountSingleBit;
            swatch::core::SimpleMetric<uint32_t> &mErrCountDoubleBit;
            swatch::core::SimpleMetric<bool> &mWarningSign;
        };

    } // namespace dummy
} // namespace rpcos4ph2

#endif /* _RPCOS4PH2_DUMMY_DUMMYAMC13MANAGER_HPP__ */

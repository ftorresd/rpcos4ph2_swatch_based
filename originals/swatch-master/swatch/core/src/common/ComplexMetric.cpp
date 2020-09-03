
#include "swatch/core/ComplexMetric.hpp"


// log4cplus headers
#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"


namespace swatch {
namespace core {


template<typename DataType>
log4cplus::Logger ComplexMetric<DataType>::sLogger = log4cplus::Logger::getInstance("swatch.core.ComplexMetric");


template<typename DataType>
ComplexMetric<DataType>::~ComplexMetric()
{
}


template<typename DataType>
void ComplexMetric<DataType>::recalculateValue()
{
  {
    boost::lock_guard<boost::mutex> lLock(this->mMutex);

    try {
      if (mCalculateFunc) {
        std::vector<MetricSnapshot> lMetricSnapshots;
        lMetricSnapshots.reserve(mMetrics.size());
        for (auto lIt = mMetrics.begin(); lIt != mMetrics.end(); lIt++) {
          if ( mFilterFunc.empty() || mFilterFunc(*lIt->first) )
            lMetricSnapshots.push_back(lIt->second->getSnapshot());
        }

        this->mValue.reset(mCalculateFunc(lMetricSnapshots));
      }
      else {
        std::vector<MonitorableObjectSnapshot> lMonObjSnapshots;
        lMonObjSnapshots.reserve(mMetrics.size());
        for (auto lIt = mMetrics.begin(); lIt != mMetrics.end(); lIt++) {
          if ( mFilterFunc.empty() || mFilterFunc(*lIt->first) )
            lMonObjSnapshots.push_back(lIt->first->getStatus());
        }

        this->mValue.reset(mCalculateFunc2(lMonObjSnapshots));
      }
    }
    catch (const std::exception& aExc) {
      this->mValue.reset( (const DataType*) NULL);
      LOG4CPLUS_WARN(sLogger, "Caught exception when re-calculating value of metric '" << this->getPath()
                     << "'. Type '" << swatch::core::demangleName(typeid(aExc).name()) << "', message: " << aExc.what());
    }
    this->mUpdateTime = TimePoint::now();
  } // N.B. mutex must be unlocked before dependent metrics re-calculated - since they will call getSnapshot on this metric

  for (auto lIt=this->getDependantMetrics().begin(); lIt != this->getDependantMetrics().end(); lIt++)
    lIt->second();
}


template class ComplexMetric<bool>;
template class ComplexMetric<uint16_t>;
template class ComplexMetric<uint32_t>;
template class ComplexMetric<uint64_t>;
template class ComplexMetric<int>;
template class ComplexMetric<float>;
template class ComplexMetric<double>;
template class ComplexMetric<std::string>;
template class ComplexMetric<tts::State>;


}
}

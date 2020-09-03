
#include "swatch/core/Metric.hpp"


#include <ostream>                      // for ostringstream, etc


namespace swatch {
namespace core {


template<typename DataType>
Metric<DataType>::Metric(const std::string& aId, const std::string& aAlias) :
  AbstractMetric(aId, aAlias),
  mValue((const DataType*) NULL),
  mUpdateTime(),
  mErrorCondition(),
  mWarnCondition(),
  mMonitoringStatus(monitoring::kEnabled)
{
}


template<typename DataType>
Metric<DataType>::Metric(const std::string& aId, const std::string& aAlias,
                         MetricCondition<DataType>* aErrorCondition,
                         MetricCondition<DataType>* aWarnCondition) try :
  AbstractMetric(aId, aAlias),
  mValue((const DataType*) NULL),
  mUpdateTime(),
  mErrorCondition(aErrorCondition),
  mWarnCondition(aWarnCondition),
  mMonitoringStatus(monitoring::kEnabled)
{
}
catch (...)
{
  delete aErrorCondition;
  delete aWarnCondition;
  // (re)throw; is implicit in CTOR try-catch block
}


template<typename DataType>
Metric<DataType>::~Metric()
{
}


template<typename DataType>
MetricSnapshot Metric<DataType>::getSnapshot() const
{
  boost::lock_guard<boost::mutex> lLock(mMutex);

  swatch::core::StatusFlag lFlag = getStatus(lLock).first;

  if (mValue != NULL)
    return MetricSnapshot(getPath(), getAlias(), lFlag, *mValue, mUpdateTime, mErrorCondition, mWarnCondition, mMonitoringStatus);
  else
    return MetricSnapshot(getPath(), getAlias(), lFlag, MetricSnapshot::TypeConverter<DataType>(), mUpdateTime, mErrorCondition, mWarnCondition, mMonitoringStatus);
}


template<typename DataType>
std::pair<StatusFlag, monitoring::Status> Metric<DataType>::getStatus() const
{
  boost::lock_guard<boost::mutex> lLock(mMutex);
  return getStatus(lLock);
}


template<typename DataType>
std::pair<StatusFlag, monitoring::Status> Metric<DataType>::getStatus(const boost::lock_guard<boost::mutex>& aLock) const
{
  swatch::core::StatusFlag lFlag = kUnknown;

  if (this->mValue != NULL) {
    if ((mErrorCondition == NULL) && (mWarnCondition == NULL))
      lFlag = kNoLimit;
    else if ( mErrorCondition && (*mErrorCondition)(*mValue))
      lFlag = kError;
    else if ( mWarnCondition && (*mWarnCondition)(*mValue))
      lFlag = kWarning;
    else
      lFlag = kGood;
  }

  if (mMonitoringStatus == monitoring::kDisabled)
    lFlag = kNoLimit; //disabled metrics always return kNoLimit

  return std::make_pair(lFlag, mMonitoringStatus);
}


template<typename DataType>
SteadyTimePoint_t Metric<DataType>::getUpdateTime() const
{
  boost::lock_guard<boost::mutex> lLock(mMutex);
  return mUpdateTime.steady;
}


template<typename DataType>
void Metric<DataType>::setValueUnknown()
{
  boost::lock_guard<boost::mutex> lLock(mMutex);
  mUpdateTime = TimePoint::now();
  mValue.reset((const DataType*) NULL);
  for (auto lIt=this->getDependantMetrics().begin(); lIt != this->getDependantMetrics().end(); lIt++)
    lIt->second();
}


template<typename DataType>
void Metric<DataType>::setMonitoringStatus(monitoring::Status aMonStatus)
{
  boost::lock_guard<boost::mutex> lLock(mMutex);
  mMonitoringStatus = aMonStatus;
}


template class Metric<bool>;
template class Metric<uint16_t>;
template class Metric<uint32_t>;
template class Metric<uint64_t>;
template class Metric<int>;
template class Metric<float>;
template class Metric<double>;
template class Metric<std::string>;
template class Metric<tts::State>;




template<typename DataType>
MetricCondition<DataType>::MetricCondition()
{
}


template<typename DataType>
MetricCondition<DataType>::~MetricCondition()
{
}


template class MetricCondition<bool>;
template class MetricCondition<uint16_t>;
template class MetricCondition<uint32_t>;
template class MetricCondition<uint64_t>;
template class MetricCondition<int>;
template class MetricCondition<float>;
template class MetricCondition<double>;
template class MetricCondition<std::string>;
template class MetricCondition<tts::State>;


}
}

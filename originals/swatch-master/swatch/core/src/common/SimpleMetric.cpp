
#include "swatch/core/SimpleMetric.hpp"


#include <ostream>                      // for ostringstream, etc


namespace swatch {
namespace core {


template<typename DataType>
std::string convertMetricDataToString(DataType aData)
{
  return boost::lexical_cast<std::string>(aData);
}

template<>
std::string convertMetricDataToString<bool>(bool aData)
{
  std::ostringstream lOSS;
  lOSS << std::boolalpha << aData;
  return lOSS.str();
}





template<typename DataType>
SimpleMetric<DataType>::SimpleMetric(const std::string& aId, const std::string& aAlias) :
  Metric<DataType>(aId, aAlias)
{
}


template<typename DataType>
SimpleMetric<DataType>::SimpleMetric(const std::string& aId, const std::string& aAlias, MetricCondition<DataType>* aErrorCondition, MetricCondition<DataType>* aWarnCondition) :
  Metric<DataType>(aId, aAlias, aErrorCondition, aWarnCondition)
{
}


template<typename DataType>
SimpleMetric<DataType>::~SimpleMetric()
{
}


template<typename DataType>
void SimpleMetric<DataType>::setValue(const DataType& aValue)
{
  {
    boost::lock_guard<boost::mutex> lLock(this->mMutex);
    this->mUpdateTime = TimePoint::now();
    this->mValue.reset(new DataType(aValue));
  } // N.B. mutex must be unlocked before dependent metrics re-calculated - since they will call getSnapshot on this metric

  for (auto lIt=this->getDependantMetrics().begin(); lIt != this->getDependantMetrics().end(); lIt++)
    lIt->second();
}


template class SimpleMetric<bool>;
template class SimpleMetric<uint16_t>;
template class SimpleMetric<uint32_t>;
template class SimpleMetric<uint64_t>;
template class SimpleMetric<int>;
template class SimpleMetric<float>;
template class SimpleMetric<double>;
template class SimpleMetric<std::string>;
template class SimpleMetric<tts::State>;


}
}

/**
* @file    DummyMetric.hpp
* @author  Luke Kreczko
* @brief   Dummy class for Metrics
* @date    24.09.2015
*/

#ifndef __SWATCH_CORE_TEST_DUMMYMETRIC_HPP__
#define __SWATCH_CORE_TEST_DUMMYMETRIC_HPP__


#include "swatch/core/SimpleMetric.hpp"


namespace swatch {
namespace core {
namespace test {

template<typename DataType>
class DummyMetric: public swatch::core::SimpleMetric<DataType> {

public:
  DummyMetric(const std::string& aId, const std::string& aAlias, const DataType aValue) :
    swatch::core::SimpleMetric<DataType>(aId, aAlias)
  {
    setValue(aValue);
  }

  DummyMetric(const std::string& aId, const std::string& aAlias) :
    swatch::core::SimpleMetric<DataType>(aId, aAlias)
  {
  }

  DummyMetric(const std::string& aId, const std::string& aAlias, const DataType aValue,
              MetricCondition<DataType>* aErrorCondition,
              MetricCondition<DataType>* aWarnCondition = NULL) :
    swatch::core::SimpleMetric<DataType>(aId, aAlias, aErrorCondition, aWarnCondition)
  {
    setValue(aValue);
  }

  DummyMetric(const std::string& aId, const std::string& aAlias,
              MetricCondition<DataType>* aErrorCondition,
              MetricCondition<DataType>* aWarnCondition = NULL) :
    swatch::core::SimpleMetric<DataType>(aId, aAlias, aErrorCondition, aWarnCondition)
  {
  }

  void setValue(const DataType& aValue)
  {
    SimpleMetric<DataType>::setValue(aValue);
  }

};

} // namespace swatch
} // namespace core
} // namespace test

#endif  /* SWATCH_CORE_TEST_DUMMYMETRIC_HPP */


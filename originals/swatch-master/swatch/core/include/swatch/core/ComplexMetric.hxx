
#ifndef __SWATCH_CORE_COMPLEXMETRIC_HXX__
#define __SWATCH_CORE_COMPLEXMETRIC_HXX__

#include "SimpleMetric.hpp"
#include "ComplexMetric.hpp"
#include "Object.hpp"
#include "MonitorableObject.hpp"



// IWYU pragma: private, include "swatch/core/ComplexMetric.hpp"


namespace swatch {
namespace core {


template<typename DataType>
template<typename MetricIt>
ComplexMetric<DataType>::ComplexMetric(const std::string& aId, const std::string& aAlias,
                                       const MetricIt& aMetricBegin, const MetricIt& aMetricEnd,
                                       const CalculateFunction_t& aCalcFunction, const FilterFunction_t& aFilterFunction) :
  Metric<DataType>(aId, aAlias),
  mCalculateFunc(aCalcFunction),
  mFilterFunc(aFilterFunction)
{
  typedef typename std::iterator_traits< MetricIt >::value_type MetricItVal_t;
  BOOST_STATIC_ASSERT_MSG( (boost::is_convertible<MetricItVal_t, std::pair<const MonitorableObject*, const AbstractMetric*> >::value) , "Dereferencing type MetricIt must result in a std::pair<const MonitorableObject*, const AbstractMetric*>");

  std::copy(aMetricBegin, aMetricEnd, std::back_inserter(mMetrics));

  // TODO: Check that list of metrics is not empty (i.e. aMetricBegin != aMetricEnd)

  //TODO: Check that none of the received pointers are NULL
  //TODO: Check that function pointer isn't NULL
}


template<typename DataType>
template<typename MonObjIt>
ComplexMetric<DataType>::ComplexMetric(const std::string& aId, const std::string& aAlias,
                                       const MonObjIt& aMonObjBegin, const MonObjIt& aMonObjEnd, 
                                       const CalculateFunction2_t& aCalcFunction, 
                                       const FilterFunction_t& aFilterFunction) :
  Metric<DataType>(aId, aAlias),
  mCalculateFunc2(aCalcFunction),
  mFilterFunc(aFilterFunction)
{
  typedef typename std::iterator_traits< MonObjIt >::value_type MonObjItVal_t;
  BOOST_STATIC_ASSERT_MSG( (boost::is_convertible<MonObjItVal_t, const MonitorableObject* >::value) , "Dereferencing type MonObjIt must result in a 'const MonitorableObject*'");

  for(MonObjIt lIt=aMonObjBegin; lIt != aMonObjEnd; lIt++) {
    const MonitorableObject& lMonObj = **lIt;

    mMetrics.push_back( std::make_pair(&lMonObj, (const AbstractMetric*)NULL) );
  }
}

} // end ns core
} // end ns swatch


#endif /* __SWATCH_CORE_COMPLEXMETRIC_HXX__ */


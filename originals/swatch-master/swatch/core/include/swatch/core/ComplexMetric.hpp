
#ifndef __SWATCH_CORE_COMPLEXMETRIC_HPP__
#define __SWATCH_CORE_COMPLEXMETRIC_HPP__


#include "boost/function/function_fwd.hpp"

#include "swatch/core/SimpleMetric.hpp"


// forward declarations
namespace log4cplus {
class Logger;
}

namespace swatch {
namespace core {

class MonitorableObject;
class MonitorableObjectSnapshot;


template <typename DataType>
class ComplexMetric : public Metric<DataType> {
public:
  typedef boost::function<const DataType* (const std::vector<MetricSnapshot>&)> CalculateFunction_t ;
  typedef boost::function<const DataType* (const std::vector<MonitorableObjectSnapshot>&)> CalculateFunction2_t ;
  typedef boost::function<bool (const MonitorableObject&)> FilterFunction_t ;

  template<typename MetricIt>
  ComplexMetric(const std::string& aId, const std::string& aAlias, const MetricIt& aMetricBegin, const MetricIt& aMetricEnd, const CalculateFunction_t& aCalcFunction, const FilterFunction_t& aFilterFunc);

  template<typename MonObjIt>
  ComplexMetric(const std::string& aId, const std::string& aAlias, const MonObjIt& aMonObjBegin, const MonObjIt& aMonObjEnd, const CalculateFunction2_t& aCalcFunction, const FilterFunction_t& aFilterFunction);

  ~ComplexMetric();

private:
  //! Re-calculate value of this metric
  void recalculateValue();

  const CalculateFunction_t mCalculateFunc;
  const CalculateFunction2_t mCalculateFunc2;
  const FilterFunction_t mFilterFunc;

  std::vector<std::pair<const MonitorableObject*, const AbstractMetric*> > mMetrics;

  static log4cplus::Logger sLogger;

  friend class MonitorableObject;
};


} // end ns core
} // end ns swatch


#include "swatch/core/ComplexMetric.hxx"

#endif /* __SWATCH_CORE_COMPLEXMETRIC_HPP__ */


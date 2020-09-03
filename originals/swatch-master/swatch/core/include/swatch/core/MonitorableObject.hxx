
#ifndef __SWATCH_CORE_MONITORABLEOBJECT_HXX__
#define __SWATCH_CORE_MONITORABLEOBJECT_HXX__


// IWYU pragma: private, include "swatch/core/MonitorableObject.hpp"

#include "swatch/core/ComplexMetric.hpp"
#include "swatch/core/SimpleMetric.hpp"
#include "swatch/core/MetricView.hpp"
#include "swatch/core/MonitorableObject.hpp"


namespace swatch {
namespace core {


template <typename DataType>
const SimpleMetric<DataType>& MonitorableObject::getMetric(const std::string& aId) const
{
  if (const SimpleMetric<DataType>* lMetric = dynamic_cast<const SimpleMetric<DataType>*>(&getMetric(aId)))
    return *lMetric;
  else
    XCEPT_RAISE(ObjectFailedCast,"Could not cast metric '" + aId + "' of object '" + getPath() + "' to type " + demangleName(typeid(DataType).name()));
}


template <typename DataType>
SimpleMetric<DataType>& MonitorableObject::getMetric(const std::string& aId)
{
  if (SimpleMetric<DataType>* lMetric = dynamic_cast<SimpleMetric<DataType>*>(&getMetric(aId)))
    return *lMetric;
  else
    XCEPT_RAISE(ObjectFailedCast,"Could not cast metric '" + aId + "' of object '" + getPath() + "' to type " + demangleName(typeid(DataType).name()));
}


template <typename DataType>
const ComplexMetric<DataType>& MonitorableObject::getComplexMetric(const std::string& aId) const
{
  if (const ComplexMetric<DataType>* lMetric = dynamic_cast<const ComplexMetric<DataType>*>(&getMetric(aId)))
    return *lMetric;
  else
    XCEPT_RAISE(ObjectFailedCast,"Could not cast metric '" + aId + "' of object '" + getPath() + "' to type " + demangleName(typeid(DataType).name()));
}


template <typename DataType>
ComplexMetric<DataType>& MonitorableObject::getComplexMetric(const std::string& aId)
{
  if (ComplexMetric<DataType>* lMetric = dynamic_cast<ComplexMetric<DataType>*>(&getMetric(aId)))
    return *lMetric;
  else
    XCEPT_RAISE(ObjectFailedCast,"Could not cast metric '" + aId + "' of object '" + getPath() + "' to type " + demangleName(typeid(DataType).name()));
}


template <typename DataType>
SimpleMetric<DataType>& MonitorableObject::registerMetric(const std::string& aId)
{
  return this->registerMetric<DataType>(aId, "");
}


template <typename DataType>
SimpleMetric<DataType>& MonitorableObject::registerMetric(const std::string& aId, const char* aAlias)
{
  return this->registerMetric<DataType>(aId, std::string(aAlias));
}

template <typename DataType>
SimpleMetric<DataType>& MonitorableObject::registerMetric(const std::string& aId, const std::string& aAlias)
{
  SimpleMetric<DataType>* lMetric = new SimpleMetric<DataType>(aId, aAlias);
  addObj(lMetric);
  mSimpleMetrics.insert( MetricMap_t::value_type(aId, lMetric) );
  mMetrics.insert( MetricMap_t::value_type(aId, lMetric) );
  return *lMetric;
}


template <typename DataType, class ErrorCondition>
SimpleMetric<DataType>& MonitorableObject::registerMetric(const std::string& aId, const ErrorCondition& aErrorCondition)
{
  return this->registerMetric<DataType>(aId, "", aErrorCondition);
}


template <typename DataType, class ErrorCondition>
SimpleMetric<DataType>& MonitorableObject::registerMetric(const std::string& aId, const char* aAlias, const ErrorCondition& aErrorCondition)
{
  return this->registerMetric<DataType>(aId, std::string(aAlias), aErrorCondition);
}


template <typename DataType, class ErrorCondition>
SimpleMetric<DataType>& MonitorableObject::registerMetric(const std::string& aId, const std::string& aAlias, const ErrorCondition& aErrorCondition)
{
  BOOST_STATIC_ASSERT_MSG( (boost::is_base_of<MetricCondition<DataType>, ErrorCondition >::value) , "class ErrorCondition must be a descendant of MetricCondtion<DataType>" );

  SimpleMetric<DataType>* lMetric = new SimpleMetric<DataType>(aId, aAlias, new ErrorCondition(aErrorCondition) );
  addObj(lMetric);
  mSimpleMetrics.insert( MetricMap_t::value_type(aId, lMetric) );
  mMetrics.insert( MetricMap_t::value_type(aId, lMetric) );
  return *lMetric;
}


template <typename DataType, class ErrorCondition, class WarnCondition>
SimpleMetric<DataType>& MonitorableObject::registerMetric(const std::string& aId, const ErrorCondition& aErrorCondition, const WarnCondition& aWarnCondition)
{
  return this->registerMetric<DataType>(aId, "", aErrorCondition, aWarnCondition);
}


template <typename DataType, class ErrorCondition, class WarnCondition>
SimpleMetric<DataType>& MonitorableObject::registerMetric(const std::string& aId, const std::string& aAlias, const ErrorCondition& aErrorCondition, const WarnCondition& aWarnCondition)
{
  BOOST_STATIC_ASSERT_MSG( (boost::is_base_of<MetricCondition<DataType>, ErrorCondition >::value) , "class ErrorCondition must be a descendant of MetricCondtion<DataType>" );
  BOOST_STATIC_ASSERT_MSG( (boost::is_base_of<MetricCondition<DataType>, WarnCondition >::value) , "class WarnCondition must be a descendant of MetricCondtion<DataType>" );

  SimpleMetric<DataType>* lMetric = new SimpleMetric<DataType>(aId, aAlias, new ErrorCondition(aErrorCondition), new WarnCondition(aWarnCondition) );
  addObj(lMetric);
  mSimpleMetrics.insert( MetricMap_t::value_type(aId, lMetric) );
  mMetrics.insert( MetricMap_t::value_type(aId, lMetric) );
  return *lMetric;
}


template <typename DataType, class MetricIt>
ComplexMetric<DataType>& MonitorableObject::registerComplexMetric(const std::string& aId, const MetricIt& aMetricBegin, const MetricIt& aMetricEnd,
    const typename ComplexMetric<DataType>::CalculateFunction_t& aCalculateFunction,
    const typename ComplexMetric<DataType>::FilterFunction_t& aFilterFunction)
{
  return this->registerComplexMetric<DataType>(aId, "", aMetricBegin, aMetricEnd, aCalculateFunction, aFilterFunction);
}


template <typename DataType, class MetricIt>
ComplexMetric<DataType>& MonitorableObject::registerComplexMetric(const std::string& aId, const std::string& aAlias,
    const MetricIt& aMetricBegin, const MetricIt& aMetricEnd,
    const typename ComplexMetric<DataType>::CalculateFunction_t& aCalculateFunction,
    const typename ComplexMetric<DataType>::FilterFunction_t& aFilterFunction)
{
  typedef typename std::iterator_traits< MetricIt >::value_type MetricItVal_t;
  BOOST_STATIC_ASSERT_MSG( (boost::is_convertible<MetricItVal_t, AbstractMetric*>::value) , "Dereferencing type MetricIt must result in a type that is convertible to 'const AbstractMetric*'");

  // Check that the function pointer isn't null
  if (aCalculateFunction.empty())
    XCEPT_RAISE(MetricRegistrationError,"Null calculate function pointer given for complex metric '" + aId + "' in object '" + this->getPath() + "'");
  // Check that more than 0 source metrics have been specified
  if (aMetricBegin == aMetricEnd)
    XCEPT_RAISE(MetricRegistrationError,"No source metrics specified for complex metric '" + aId + "' in object '" + this->getPath() + "'");


  std::vector<std::pair<const MonitorableObject*, const AbstractMetric*> > lMonObjMetricPairs;
  for (auto lIt = aMetricBegin; lIt != aMetricEnd; lIt++) {
    // Check that none of the received pointers are null
    if (*lIt == NULL)
      XCEPT_RAISE(MetricRegistrationError,"Null metric pointer given for complex metric '" + aId + "' in object '" + this->getPath() + "'");

    // Check that source metric is descendant of this monitorable object
    const AbstractMetric& lMetric = **lIt;
    if ( !this->isAncestorOf(lMetric) )
      XCEPT_RAISE(MetricRegistrationError,"Source metric '" + lMetric.getPath() + "' specified for complex metric '" + aId + "' is not a descendant of monitorable object '" + this->getPath() + "'");

    // Check that none two entries in the metric pointer collection are the same
    for (auto lIt2 = lIt+1; lIt2 != aMetricEnd; lIt2++) {
      if (*lIt == *lIt2)
        XCEPT_RAISE(MetricRegistrationError,"Metric '" + lMetric.getPath() + "' specified twice in registration of complex metric '" + aId + "' in object '" + this->getPath() + "'");
    }

    // Form relative path of monitorable object by first removing this object's path, then removing metric's ID from tail
    const std::string lMetricRelPath = lMetric.getPath().substr(getPath().size() + 1);
    if (lMetricRelPath == lMetric.getId())
      lMonObjMetricPairs.push_back( std::make_pair(this, &lMetric));
    else {
      std::string lMonObjRelPath = lMetricRelPath.substr(0, lMetricRelPath.size() - lMetric.getId().size() - 1);

      const MonitorableObject& lMonObj = getObj<MonitorableObject>(lMonObjRelPath);
      lMonObjMetricPairs.push_back( std::make_pair(&lMonObj, &lMetric) );
    }
  }

  ComplexMetric<DataType>* lMetric = new ComplexMetric<DataType>(aId, aAlias, lMonObjMetricPairs.begin(), lMonObjMetricPairs.end(), aCalculateFunction, aFilterFunction);
  addObj(lMetric);
  mMetrics.insert( MetricMap_t::value_type(aId, lMetric) );

  for (MetricIt lIt=aMetricBegin; lIt != aMetricEnd; lIt++) {
    AbstractMetric& lSourceMetric = **lIt;
    const boost::function<void ()> lBoundFunc = boost::bind(&ComplexMetric<DataType>::recalculateValue, lMetric);
    lSourceMetric.addDependantMetric(*lMetric, lBoundFunc);
  }

  return *lMetric;
}


template <typename DataType>
ComplexMetric<DataType>& MonitorableObject::registerComplexMetric(const std::string& aId, const MetricView& aMetricView,
    const typename ComplexMetric<DataType>::CalculateFunction_t& aCalculateFunction,
    const typename ComplexMetric<DataType>::FilterFunction_t& aFilterFunction)
{
  return this->registerComplexMetric<DataType>(aId, "", aMetricView, aCalculateFunction, aFilterFunction);
}


template <typename DataType>
ComplexMetric<DataType>& MonitorableObject::registerComplexMetric(const std::string& aId, const std::string& aAlias,
    const MetricView& aMetricView,
    const typename ComplexMetric<DataType>::CalculateFunction_t& aCalculateFunction,
    const typename ComplexMetric<DataType>::FilterFunction_t& aFilterFunction)
{
  // 1) Extract metric pointers from metric view into a std::vector
  std::vector<swatch::core::AbstractMetric*> lSourceMetrics;
  lSourceMetrics.reserve(aMetricView.size());
  for (MetricView::const_iterator lIt = aMetricView.begin(); lIt != aMetricView.end(); lIt++)
    lSourceMetrics.push_back(lIt->second);

  // 2) Call the other registration method
  return this->registerComplexMetric<DataType>(aId, aAlias, lSourceMetrics.begin(), lSourceMetrics.end(), aCalculateFunction, aFilterFunction);
}


template <typename DataType, class MonObjIt>
ComplexMetric<DataType>& MonitorableObject::registerComplexMetric(const std::string& aId, const MonObjIt& aMonObjBegin, const MonObjIt& aMonObjEnd,
    const typename ComplexMetric<DataType>::CalculateFunction2_t& aCalculateFunction,
    const typename ComplexMetric<DataType>::FilterFunction_t& aFilterFunction)
{
  return this->registerComplexMetric<DataType>(aId, "", aMonObjBegin, aMonObjEnd, aCalculateFunction, aFilterFunction);
}


template <typename DataType, class MonObjIt>
ComplexMetric<DataType>& MonitorableObject::registerComplexMetric(const std::string& aId, const std::string& aAlias,
    const MonObjIt& aMonObjBegin, const MonObjIt& aMonObjEnd,
    const typename ComplexMetric<DataType>::CalculateFunction2_t& aCalculateFunction,
    const typename ComplexMetric<DataType>::FilterFunction_t& aFilterFunction)
{
  typedef typename std::iterator_traits< MonObjIt >::value_type MonObjItVal_t;
  BOOST_STATIC_ASSERT_MSG( (boost::is_convertible<MonObjItVal_t, MonitorableObject*>::value) , "Dereferencing type MonObjIt must result in a type that is convertible to 'MonitorableObject*'");

  // Check that the function pointer isn't null
  if (aCalculateFunction.empty())
    XCEPT_RAISE(MetricRegistrationError,"Null calculate function pointer given for complex metric '" + aId + "' in object '" + this->getPath() + "'");
  // Check that more than 0 source metrics have been specified
  if (aMonObjBegin == aMonObjEnd)
    XCEPT_RAISE(MetricRegistrationError,"No source monitorable objects specified for complex metric '" + aId + "' in object '" + this->getPath() + "'");


  std::vector<const MonitorableObject*> lMonObjs;
  for (auto lIt = aMonObjBegin; lIt != aMonObjEnd; lIt++) {
    // Check that none of the received pointers are null
    if (*lIt == NULL)
      XCEPT_RAISE(MetricRegistrationError,"Null monitorable object pointer given for complex metric '" + aId + "' in object '" + this->getPath() + "'");

    // Check that source metric is descendant of this monitorable object
    const MonitorableObject& lSourceMonObj = **lIt;
    if ( !this->isAncestorOf(lSourceMonObj) )
      XCEPT_RAISE(MetricRegistrationError,"Source monitorable object '" + lSourceMonObj.getPath() + "' specified for complex metric '" + aId + "' is not a descendant of monitorable object '" + this->getPath() + "'");

    // Check that none two entries in the metric pointer collection are the same
    for (auto lIt2 = lIt+1; lIt2 != aMonObjEnd; lIt2++) {
      if (*lIt == *lIt2)
        XCEPT_RAISE(MetricRegistrationError,"Monitorable object '" + lSourceMonObj.getPath() + "' specified twice in registration of complex metric '" + aId + "' in object '" + this->getPath() + "'");
    }

    lMonObjs.push_back( &lSourceMonObj );
  }

  ComplexMetric<DataType>* lMetric = new ComplexMetric<DataType>(aId, aAlias, lMonObjs.begin(), lMonObjs.end(), aCalculateFunction, aFilterFunction);
  addObj(lMetric);
  mMetrics.insert( MetricMap_t::value_type(aId, lMetric) );

  
  std::vector<MonitorableObject*> lSourceMonObjs;
  for (MonObjIt lIt = aMonObjBegin; lIt != aMonObjEnd; lIt++) {
    for (auto lIt2 = (*lIt)->begin(); lIt2 != (*lIt)->end(); lIt2++) {
      if (MonitorableObject* lSourceMonObj = dynamic_cast<MonitorableObject*>(&*lIt2))
        lSourceMonObjs.push_back(lSourceMonObj);
    }
  }
  std::sort(lSourceMonObjs.begin(), lSourceMonObjs.end());
  auto lLastSourceMonObj = std::unique(lSourceMonObjs.begin(), lSourceMonObjs.end());
  const boost::function<void ()> lBoundFunc = boost::bind(&ComplexMetric<DataType>::recalculateValue, lMetric);
  for (auto lIt = lSourceMonObjs.begin(); lIt != lLastSourceMonObj; lIt++) {
    (*lIt)->mDependantMetrics.push_back( std::make_pair(lMetric, lBoundFunc) );
  }

  return *lMetric;
}


template <typename DataType>
void MonitorableObject::setMetricValue(SimpleMetric<DataType>& aMetric, const DataType& aValue)
{
  aMetric.setValue(aValue);
}


template <typename DataType, class ConditionType>
void MonitorableObject::setErrorCondition(Metric<DataType>& aMetric, const ConditionType& aErrorCondition)
{
  aMetric.setErrorCondition(aErrorCondition);
}


template <typename DataType, class ConditionType>
void MonitorableObject::setWarningCondition(Metric<DataType>& aMetric, const ConditionType& aWarningCondition)
{
  aMetric.setWarningCondition(aWarningCondition);
}


template <typename DataType, class ErrorConditionType, class WarnConditionType>
void MonitorableObject::setConditions(Metric<DataType>& aMetric, const ErrorConditionType& aErrorCondition, const WarnConditionType& aWarnCondition)
{
  aMetric.setErrorCondition(aErrorCondition);
  aMetric.setWarningCondition(aWarnCondition);
}


template <class DeleterType>
void MonitorableObject::addMonitorable(MonitorableObject* aMonObj, DeleterType aDeleter)
{
  addObj(aMonObj, aDeleter);
  finishAddingMonitorable(aMonObj);
}


}
}

#endif	/* __SWATCH_CORE_MONITORABLEOBJECT_HXX__ */

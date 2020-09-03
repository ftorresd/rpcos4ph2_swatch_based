/**
 * @file    BaseMetric.hxx
 * @author  Tom Williams
 * @date    July 2015
 */

#ifndef __SWATCH_CORE_BASEMETRIC_HXX__
#define __SWATCH_CORE_BASEMETRIC_HXX__


// IWYU pragma: private, include "swatch/core/BaseMetric.hpp"

#include <exception>

#include "boost/lexical_cast.hpp"
#include "boost/thread/lock_guard.hpp"

#include "swatch/core/MetricSnapshot.hpp"


namespace swatch {
namespace core {


template<typename DataType>
template<class ConditionType>
void Metric<DataType>::setErrorCondition(const ConditionType& aErrorCondition)
{
  BOOST_STATIC_ASSERT_MSG( (boost::is_base_of<MetricCondition<DataType>, ConditionType >::value) , "class ConditionType must be a descendant of MetricCondtion<DataType>" );

  boost::lock_guard<boost::mutex> lLock(mMutex);
  mErrorCondition.reset(new ConditionType(aErrorCondition));
}


template<typename DataType>
template<class ConditionType>
void Metric<DataType>::setWarningCondition(const ConditionType& aWarningCondition)
{
  BOOST_STATIC_ASSERT_MSG( (boost::is_base_of<MetricCondition<DataType>, ConditionType >::value) , "class ConditionType must be a descendant of MetricCondtion<DataType>" );

  boost::lock_guard<boost::mutex> lLock(mMutex);
  mWarnCondition.reset(new ConditionType(aWarningCondition));
}


} // namespace core
} // namespace swatch



#endif	/* __SWATCH_CORE_BASEMETRIC_HXX__ */


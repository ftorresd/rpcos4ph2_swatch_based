/**
 * @file    MetricConditions.hxx
 * @author  Tom Williams
 * @date    June 2015
 */

#ifndef __SWATCH_CORE_METRICCONDITIONS_HXX__
#define __SWATCH_CORE_METRICCONDITIONS_HXX__


// IWYU pragma: private, include "swatch/core/GateKeeperView.hpp"

#include <iostream>

#include "MetricConditions.hpp"


namespace swatch {
namespace core {

/* ---  EqualCondition  --- */

template<typename DataType>
EqualCondition<DataType>::EqualCondition(const DataType& aValue) :
  mValue(aValue)
{
}

template<typename DataType>
EqualCondition<DataType>::~EqualCondition() {}

template<typename DataType>
bool EqualCondition<DataType>::operator()(const DataType& aValue) const
{
  return (mValue == aValue);
}

template<typename DataType>
void EqualCondition<DataType>::print(std::ostream& aStream) const
{
  aStream << " == " << mValue;
}


/* ---  NotEqualCondition  --- */

template<typename DataType>
NotEqualCondition<DataType>::NotEqualCondition(const DataType& aValue) :
  mValue(aValue)
{
}

template<typename DataType>
NotEqualCondition<DataType>::~NotEqualCondition() {}

template<typename DataType>
bool NotEqualCondition<DataType>::operator()(const DataType& aValue) const
{
  return (mValue != aValue);
}

template<typename DataType>
void NotEqualCondition<DataType>::print(std::ostream& aStream) const
{
  aStream << " != " << mValue;
}


/* ---  RangeCondition  --- */


template<typename DataType>
RangeCondition<DataType>::RangeCondition(const DataType& aLower, const DataType& aUpper) :
  mLower(aLower),
  mUpper(aUpper)
{
}

template<typename DataType>
RangeCondition<DataType>::~RangeCondition() {}

template<typename DataType>
bool RangeCondition<DataType>::operator()(const DataType& aValue) const
{
  return ( (aValue >= mLower) && (aValue <= mUpper) );
}

template<typename DataType>
void RangeCondition<DataType>::print(std::ostream& aStream) const
{
  aStream << "[" << mLower << ", " << mUpper << "]";
}


/* ---  InvRangeCondition  --- */


template<typename DataType>
InvRangeCondition<DataType>::InvRangeCondition(const DataType& aLower, const DataType& aUpper) :
  mLower(aLower),
  mUpper(aUpper)
{
}

template<typename DataType>
InvRangeCondition<DataType>::~InvRangeCondition() {}

template<typename DataType>
bool InvRangeCondition<DataType>::operator()(const DataType& aValue) const
{
  return ( (aValue < mLower) || (aValue > mUpper) );
}

template<typename DataType>
void InvRangeCondition<DataType>::print(std::ostream& aStream) const
{
  aStream << " < " << mLower << " or > " << mUpper;
}


/* ---  LessThanCondition  --- */


template<typename DataType>
LessThanCondition<DataType>::LessThanCondition(const DataType& aLimit) :
  mLimit(aLimit)
{
}

template<typename DataType>
LessThanCondition<DataType>::~LessThanCondition() {}

template<typename DataType>
bool LessThanCondition<DataType>::operator()(const DataType& aValue) const
{
  return (aValue < mLimit);
}

template<typename DataType>
void LessThanCondition<DataType>::print(std::ostream& aStream) const
{
  aStream << "< " << mLimit ;
}


/* ---  GreaterThanCondition  --- */


template<typename DataType>
GreaterThanCondition<DataType>::GreaterThanCondition(const DataType& aLimit) :
  mLimit(aLimit)
{
}

template<typename DataType>
GreaterThanCondition<DataType>::~GreaterThanCondition() {}

template<typename DataType>
bool GreaterThanCondition<DataType>::operator()(const DataType& aValue) const
{
  return (aValue > mLimit);
}

template<typename DataType>
void GreaterThanCondition<DataType>::print(std::ostream& aStream) const
{
  aStream << "> " << mLimit;
}


} // namespace core
} // namespace swatch



#endif	/* __SWATCH_CORE_METRICCONDITIONS_HPP__ */


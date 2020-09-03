/**
 * @file    MetricConditions.hpp
 * @author  Tom Williams
 * @date    June 2015
 */

#ifndef __SWATCH_CORE_METRICCONDITIONS_HPP__
#define __SWATCH_CORE_METRICCONDITIONS_HPP__


#include "swatch/core/SimpleMetric.hpp"


namespace swatch {
namespace core {

template<typename DataType>
class EqualCondition : public MetricCondition<DataType> {
public:
  EqualCondition(const DataType& aValue);
  ~EqualCondition();

  void print(std::ostream& aStream) const;

  bool operator()(const DataType& aValue) const;

private:
  const DataType mValue;
};


template<typename DataType>
class NotEqualCondition : public MetricCondition<DataType> {
public:
  NotEqualCondition(const DataType& aValue);
  ~NotEqualCondition();

  void print(std::ostream& aStream) const;

  bool operator()(const DataType& aValue) const;

private:
  const DataType mValue;
};



template<typename DataType>
class RangeCondition : public MetricCondition<DataType> {
public:
  RangeCondition(const DataType& aLower, const DataType& aUpper);
  ~RangeCondition();

  bool operator()(const DataType& aValue) const;

private:
  void print(std::ostream& aStream) const;

  const DataType mLower;
  const DataType mUpper;
};


template<typename DataType>
class InvRangeCondition : public MetricCondition<DataType> {
public:
  InvRangeCondition(const DataType& aLower, const DataType& aUpper);
  ~InvRangeCondition();

  bool operator()(const DataType& aValue) const;

private:
  void print(std::ostream& aStream) const;

  const DataType mLower;
  const DataType mUpper;
};


template<typename DataType>
class LessThanCondition : public MetricCondition<DataType> {
public:
  LessThanCondition(const DataType& aLimit);
  ~LessThanCondition();


  bool operator()(const DataType& aValue) const;

private:
  void print(std::ostream& aStream) const;

  const DataType mLimit;
};


template<typename DataType>
class GreaterThanCondition : public MetricCondition<DataType> {
public:
  GreaterThanCondition(const DataType& aLimit);
  ~GreaterThanCondition();

  bool operator()(const DataType& aValue) const;

private:
  void print(std::ostream& aStream) const;

  const DataType mLimit;
};


} // namespace core
} // namespace swatch



#include "MetricConditions.hxx"

#endif  /* __SWATCH_CORE_METRICCONDITIONS_HPP__ */


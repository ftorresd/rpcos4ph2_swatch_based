/**
 * @file    MetricSnapshot.hpp
 * @author  Tom Williams
 * @date    May 2016
 */

#ifndef __SWATCH_CORE_METRICSNAPSHOT_HPP__
#define __SWATCH_CORE_METRICSNAPSHOT_HPP__


#include <string>
#include <sys/time.h>

#include "boost/any.hpp"
#include "boost/function.hpp"
#include "boost/smart_ptr/shared_ptr.hpp"

#include "xdata/Boolean.h"
#include "xdata/Double.h"
#include "xdata/Integer.h"
#include "xdata/Float.h"
#include "xdata/String.h"
#include "xdata/UnsignedInteger.h"
#include "xdata/UnsignedInteger32.h"
#include "xdata/UnsignedLong.h"
#include "xdata/UnsignedShort.h"
#include "xdata/SimpleType.h"

#include "swatch/core/exception.hpp"
#include "swatch/core/LeafObject.hpp"
#include "swatch/core/StatusFlag.hpp"
#include "swatch/core/monitoring/Status.hpp"
#include "swatch/core/TimePoint.hpp"
#include "swatch/core/TTSUtils.hpp"


namespace swatch {
namespace core {

class AbstractMetricCondition;


class MetricSnapshot {
public:
  template <typename T> struct TypeConverter {};

  template <typename T>
  MetricSnapshot(const std::string& aIdPath, const std::string& aAlias,
                 swatch::core::StatusFlag , const T& aValue,
                 const TimePoint&,
                 boost::shared_ptr<AbstractMetricCondition> aErrCond,
                 boost::shared_ptr<AbstractMetricCondition> aWarnCond,
                 swatch::core::monitoring::Status aMonStatus = monitoring::kEnabled);

  template <typename T>
  MetricSnapshot(const std::string& aIdPath, const std::string& aAlias,
                 swatch::core::StatusFlag, TypeConverter<T> aType,
                 const TimePoint&,
                 boost::shared_ptr<AbstractMetricCondition> aErrCond,
                 boost::shared_ptr<AbstractMetricCondition> aWarnCond,
                 swatch::core::monitoring::Status aMonStatus = monitoring::kEnabled);

  //! Returns metric's ID path
  const std::string& getPath() const;

  //! Returns metric's ID
  std::string getMetricId() const;

  //! Returns alias to metric
  const std::string& getAlias() const;

  //! Returns ID path of monitorable object (the metric's parent)
  std::string getMonitorableObjectPath() const;

  //! Returns ID string of monitorable object (the metric's parent)
  std::string getMonitorableObjectId() const;

  //! Returns status flag deduced from comparing the stored value with limits
  swatch::core::StatusFlag getStatusFlag() const;

  //! Returns whether or not value is known
  bool isValueKnown() const;

  /*!
   * @brief Returns the metric's value; throws if the value is not known, or if the incorrect type is given
   *
   * @throw MetricValueNotKnown If value is not known
   * @throw MetricValueFailedCast If value is known, but incorrect type given
   */
  template <typename DataType>
  DataType getValue() const;

  const xdata::Serializable& getAsSerializable() const;

  std::string getValueAsString() const;

  //! Returns time at which metric's value was last updated
  const TimePoint& getUpdateTimestamp() const;

  //! Returns metric's warning condition; NULL returned if metric doesn't have any warning condition
  const AbstractMetricCondition* getWarningCondition() const;

  //! Returns metric's error condition; NULL returned if metric doesn't have any error condition
  const AbstractMetricCondition* getErrorCondition() const;

  swatch::core::monitoring::Status getMonitoringStatus() const;

private:
  std::string mIdPath;
  std::string mAlias;
  swatch::core::StatusFlag mFlag;
  boost::any mValue; //TODO ??? Update to boost shared_ptr to actual data value (without templating class, just templating CTOR) ???
  boost::shared_ptr<xdata::Serializable> mSerializable;
  TimePoint mUpdateTime;
  boost::shared_ptr<AbstractMetricCondition> mErrorCondition;
  boost::shared_ptr<AbstractMetricCondition> mWarnCondition;
  swatch::core::monitoring::Status mMonitoringStatus;
  boost::function<std::string (const boost::any&)> mValueStringConverter;

  template<typename DataType>
  static xdata::Serializable* convertValueToSerializable(const DataType* aValue);

  template<typename DataType>
  static std::string convertValueToString(const boost::any& aValue);
};


template <> struct MetricSnapshot::TypeConverter<bool> {
  typedef xdata::Boolean Serializable_t;
};
template <> struct MetricSnapshot::TypeConverter<uint64_t> {
  typedef xdata::UnsignedLong Serializable_t;
};
template <> struct MetricSnapshot::TypeConverter<uint32_t> {
  typedef xdata::UnsignedInteger32 Serializable_t;
};
template <> struct MetricSnapshot::TypeConverter<uint16_t> {
  typedef xdata::UnsignedShort Serializable_t;
};
template <> struct MetricSnapshot::TypeConverter<int> {
  typedef xdata::Integer Serializable_t;
};
//template <> struct MetricSnapshot::TypeConverter<unsigned int> {typedef xdata::UnsignedInteger Serializable_t;};
//template <> struct MetricSnapshot::TypeConverter<unsigned short> {typedef xdata::UnsignedShort Serializable_t;};
template <> struct MetricSnapshot::TypeConverter<float> {
  typedef xdata::Float Serializable_t;
};
template <> struct MetricSnapshot::TypeConverter<double> {
  typedef xdata::Double Serializable_t;
};
template <> struct MetricSnapshot::TypeConverter<std::string> {
  typedef xdata::String Serializable_t;
};


template <> struct MetricSnapshot::TypeConverter<swatch::core::tts::State> {
  typedef xdata::UnsignedShort Serializable_t;
};

template<typename DataType>
xdata::Serializable* MetricSnapshot::convertValueToSerializable(const DataType* aValue)
{
  if (aValue == NULL)
    return new typename TypeConverter<DataType>::Serializable_t();
  else
    return new typename TypeConverter<DataType>::Serializable_t(*aValue);
}

template<>
std::string MetricSnapshot::convertValueToString<bool>(const boost::any& aValue);


SWATCH_DEFINE_EXCEPTION(MetricValueNotKnown)
SWATCH_DEFINE_EXCEPTION(MetricValueFailedCast)


} // namespace core
} // namespace swatch


#include "swatch/core/MetricSnapshot.hxx"


#endif  /* __SWATCH_CORE_METRICSNAPSHOT_HPP__ */

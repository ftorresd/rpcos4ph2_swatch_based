/**
 * @file    MetricSnapshot.hxx
 * @author  Tom Williams
 * @date    May 2016
 */

#ifndef __SWATCH_CORE_METRICSNAPSHOT_HXX__
#define __SWATCH_CORE_METRICSNAPSHOT_HXX__


// IWYU pragma: private, include "swatch/core/MetricSnapshot.hpp"


#include <typeinfo>

#include "swatch/core/utilities.hpp"


namespace swatch {
namespace core {


template <typename DataType>
MetricSnapshot::MetricSnapshot(const std::string& aIdPath,
                               const std::string& aAlias,
                               swatch::core::StatusFlag flag,
                               const DataType& aValue,
                               const TimePoint& updateTime,
                               boost::shared_ptr<AbstractMetricCondition> errCond,
                               boost::shared_ptr<AbstractMetricCondition> warnCond,
                               swatch::core::monitoring::Status m_status) :
  mIdPath(aIdPath),
  mAlias(aAlias),
  mFlag(flag),
  mValue(aValue),
  mSerializable(convertValueToSerializable<DataType>(&aValue)),
  mUpdateTime(updateTime),
  mErrorCondition(errCond),
  mWarnCondition(warnCond),
  mMonitoringStatus(m_status),
  mValueStringConverter(&convertValueToString<DataType>)
{
}


template <typename DataType>
MetricSnapshot::MetricSnapshot(const std::string& aIdPath,
                               const std::string& aAlias,
                               swatch::core::StatusFlag aFlag,
                               TypeConverter<DataType>,
                               const TimePoint& aUpdateTime,
                               boost::shared_ptr<AbstractMetricCondition> aErrCond,
                               boost::shared_ptr<AbstractMetricCondition> aWarnCond,
                               swatch::core::monitoring::Status aMonStatus) :
  mIdPath(aIdPath),
  mAlias(aAlias),
  mFlag(aFlag),
  mValue(),
  mSerializable(convertValueToSerializable<DataType>(NULL)),
  mUpdateTime(aUpdateTime),
  mErrorCondition(aErrCond),
  mWarnCondition(aWarnCond),
  mMonitoringStatus(aMonStatus)
{
}


template <typename DataType>
DataType MetricSnapshot::getValue() const
{
  if (mValue.empty())
    XCEPT_RAISE(MetricValueNotKnown,"Value of metric '" + getPath() + "' is not known");
  else {
    try {
      return boost::any_cast<DataType>(mValue);
    }
    catch (const boost::bad_any_cast& lExc) {
      XCEPT_RAISE(MetricValueFailedCast,"Cannot cast value of metric '" + getPath() +
                                  "' from type '" + swatch::core::demangleName(mValue.type().name()) +
                                  "' to type '" + swatch::core::demangleName(typeid(DataType).name()) + "'");
    }
  }
}


template<typename DataType>
std::string MetricSnapshot::convertValueToString(const boost::any& aValue)
{
  std::ostringstream lOSStream;
  lOSStream << boost::any_cast<DataType>(aValue);
  return lOSStream.str();
}

} // namespace core
} // namespace swatch



#endif	/* __SWATCH_CORE_METRICSNAPSHOT_HXX__ */

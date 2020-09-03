/*
 * File:   MetricSnapshot.cpp
 * Author: Tom Williams
 * Date:   May 2016
 */

#include "swatch/core/MetricSnapshot.hpp"



namespace swatch {
namespace core {


const std::string& MetricSnapshot::getPath() const
{
  return mIdPath;
}



std::string MetricSnapshot::getMetricId() const
{
  std::size_t lIdxLastDot = mIdPath.rfind('.');
  if (lIdxLastDot == std::string::npos)
    return mIdPath;
  else
    return mIdPath.substr(lIdxLastDot+1);
}


const std::string& MetricSnapshot::getAlias() const
{
  return mAlias;
}


std::string MetricSnapshot::getMonitorableObjectPath() const
{
  std::size_t lIdxLastDot = mIdPath.rfind('.');
  if (lIdxLastDot == std::string::npos)
    return "";
  else
    return mIdPath.substr(0, lIdxLastDot);
}


std::string MetricSnapshot::getMonitorableObjectId() const
{
  std::size_t lIdxLastDot = mIdPath.rfind('.');
  if (lIdxLastDot == std::string::npos)
    return "";
  else {
    std::size_t lIdxPenultimateDot = mIdPath.rfind('.', lIdxLastDot-1);
    if (lIdxPenultimateDot == std::string::npos)
      return mIdPath.substr(0, lIdxLastDot);
    else
      return mIdPath.substr(lIdxPenultimateDot+1, lIdxLastDot-lIdxPenultimateDot-1);
  }
}


swatch::core::StatusFlag MetricSnapshot::getStatusFlag() const
{
  return mFlag;
}


bool MetricSnapshot::isValueKnown() const
{
  return !mValue.empty();
}


const xdata::Serializable& MetricSnapshot::getAsSerializable() const
{
  return *mSerializable;
}


std::string MetricSnapshot::getValueAsString() const
{
  if (mValue.empty())
    XCEPT_RAISE(MetricValueNotKnown,"Value of metric '" + getPath() + "' is not known");
  else
    return mValueStringConverter(mValue);
}


const TimePoint& MetricSnapshot::getUpdateTimestamp() const
{
  return mUpdateTime;
}


const AbstractMetricCondition* MetricSnapshot::getWarningCondition() const
{
  return mWarnCondition.get();
}


const AbstractMetricCondition* MetricSnapshot::getErrorCondition() const
{
  return mErrorCondition.get();
}

swatch::core::monitoring::Status MetricSnapshot::getMonitoringStatus() const
{
  return mMonitoringStatus;
}


template<>
std::string MetricSnapshot::convertValueToString<bool>(const boost::any& aData)
{
  std::ostringstream lOSS;
  lOSS << std::boolalpha << boost::any_cast<bool>(aData);
  return lOSS.str();
}


}
}

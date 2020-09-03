/*
 * File:   AbstractMetric.cpp
 * Author: Tom Williams
 * Date:   June 2015
 */

#include "swatch/core/AbstractMetric.hpp"


#include <iostream>


namespace swatch {
namespace core {


AbstractMetric::AbstractMetric(const std::string& aId, const std::string& aAlias) :
  LeafObject(aId, aAlias)
{
}

AbstractMetric::~AbstractMetric()
{
}


AbstractMetricCondition::AbstractMetricCondition()
{
}


AbstractMetricCondition::~AbstractMetricCondition()
{
}



std::ostream& operator<<(std::ostream& aOut, const AbstractMetricCondition& aCondition)
{
  aCondition.print(aOut);
  return aOut;
}


}
}

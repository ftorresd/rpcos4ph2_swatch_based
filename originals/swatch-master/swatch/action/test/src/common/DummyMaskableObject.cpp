/**
 * @file   DummyMaskableObject.cpp
 * @author Tom Williams
 * @date   December 2015
 */


#include "swatch/action/test/DummyMaskableObject.hpp"



namespace swatch {
namespace action {
namespace test {


DummyMaskableObject::DummyMaskableObject(const std::string& aId) :
  MaskableObject(aId)
{
}

DummyMaskableObject::DummyMaskableObject(const std::string& aId, const std::string& aAlias) :
  MaskableObject(aId, aAlias)
{
}

void DummyMaskableObject::retrieveMetricValues() {}


} // namespace test
} // namespace action
} // namespace swatch


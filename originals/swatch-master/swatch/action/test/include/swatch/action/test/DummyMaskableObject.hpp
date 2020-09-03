/**
 * @file   DummyMaskableObject.hpp
 * @author Tom Williams
 * @date   December 2015
 */

#ifndef __SWATCH_ACTION_TEST_DUMMYMASKABLEOBJECT_HPP__
#define __SWATCH_ACTION_TEST_DUMMYMASKABLEOBJECT_HPP__


#include "swatch/action/MaskableObject.hpp"


namespace swatch {
namespace action {
namespace test {


// Can't use MaskableObject instances in tests below since MonitorableObject::retrieveMetricValues is still pure virtual
class DummyMaskableObject : public MaskableObject {
public:
  explicit DummyMaskableObject(const std::string& aId);

  explicit DummyMaskableObject(const std::string& aId, const std::string& aAlias);

  void retrieveMetricValues();
};


} // namespace test
} // namespace action
} // namespace swatch


#endif  /* SWATCH_ACTION_TEST_DUMMYMASKABLEOBJECT_HPP */


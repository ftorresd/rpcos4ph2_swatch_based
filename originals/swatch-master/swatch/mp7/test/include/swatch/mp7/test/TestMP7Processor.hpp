/*
 * File:   TestMP7Processor.hpp
 * Author: ale
 *
 * Created on December 8, 2015, 2:19 PM
 */

#ifndef __SWATCH_MP7_TEST_TESTMP7PROCESSOR_HPP__
#define __SWATCH_MP7_TEST_TESTMP7PROCESSOR_HPP__


#include "swatch/mp7/MP7NullAlgoProcessor.hpp"
#include "swatch/action/Command.hpp"
#include "swatch/mp7/cmds/AlignRxsTo.hpp"
#include "swatch/mp7/IOChannelSelector.hpp"
#include "swatch/core/ReadOnlyXParameterSet.hpp"

namespace swatch {
namespace mp7 {
namespace test {

class TestMP7Processor : public MP7NullAlgoProcessor {
public:
  TestMP7Processor( const swatch::core::AbstractStub& aStub );
  virtual ~TestMP7Processor();
private:

};

class PrintRxDescriptors : public swatch::action::Command {
public:

  PrintRxDescriptors(const std::string& aId, action::ActionableObject& aResource);

  virtual State code(const core::XParameterSet& aParams);

private:

  RxBufferSelector mCore;
};

struct TmtParameterMapper {

  static void replaceParameters( swatch::action::Command& aCommand );
  static core::ReadOnlyXParameterSet rebuild( const core::ReadOnlyXParameterSet& aParams, const ::mp7::orbit::Metric& aMetric );

  static const std::string kBxParam;
  static const std::string kCycleParam;
  static const std::string kTmtBxParam;
  static const std::string kTmtCycleParam;
  static const std::string kTmtIdParam;
  static const std::string kOrbitConstr;
};

class TMTAlign : public cmds::AlignRxsTo {
public:
  TMTAlign(const std::string& aId, swatch::action::ActionableObject& aActionable);
  virtual State code(const core::XParameterSet& aParams);

};
} // namespace test
} // namespace mp7
} // namespace swatch

#endif  /* __SWATCH_MP7_TESTMP7PROCESSOR_HPP__ */


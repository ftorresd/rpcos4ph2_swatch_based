#ifndef __SWATCH_MP7_CMDS_ORBITCONSTRAINT_HPP__
#define __SWATCH_MP7_CMDS_ORBITCONSTRAINT_HPP__

#include "swatch/core/XPSetConstraint.hpp"

#include "mp7/Orbit.hpp"

#include "xdata/UnsignedInteger.h"


namespace swatch {
namespace mp7 {
namespace cmds {

// ----------------------------------------------------------------------------
class OrbitConstraint : public core::XPSetConstraint
{
public:
  OrbitConstraint(const std::string& aBxName, const std::string& aCycleName, ::mp7::orbit::Metric aMetric, bool aAllowNull=false);

  /**
   * @brief      Destroys the object.
   */
  virtual ~OrbitConstraint() {};

  /**
   * @brief      Print the description of this contraint to a output stream.
   *
   * @param      aStream  Target output stream.
   */
  virtual void describe(std::ostream& aStream) const;

private:

  /**
   * @brief      Apply this rule to a parameter set.
   *
   * @param[in]  aParams  Input parameters.
   *
   * @return     True if the input parameters match this rule.
   */
  virtual core::XMatch verify(const core::XParameterSet& aParams) const;

  //!
  const std::string mBxName;
  
  //!
  const std::string mCycleName;
  
  //!
  ::mp7::orbit::Metric mMetric;
  
  bool mAllowNull;
};
// ----------------------------------------------------------------------------

} // namespace cmds
} // namespace mp7
} // namespace swatch

#endif /* __SWATCH_MP7_CMDS_ORBITCONSTRAINT_HPP__ */
#ifndef __SWATCH_MP7_CMDS_SETUPREADOUT_HPP__
#define __SWATCH_MP7_CMDS_SETUPREADOUT_HPP__

#include <string>                       // for string

#include "swatch/action/Command.hpp"


namespace swatch {
namespace mp7 {
namespace cmds {

class SetupReadout : public action::Command {
public:
  /**
   * @brief      Constructor.
   *
   * @details    Defines input parameters
   *             - internal (uint):
   *             - bxOffset (uint):
   *             - drain (uint):
   *             - drain (uint):
   *             - bufferHWM (uint):
   *             - bufferLWM (uint):
   *             - fake (uint):
   *
   * @param[in]  aId          Command identifier
   * @param      aActionable  Target actionable object
   */
  SetupReadout( const std::string& aId, swatch::action::ActionableObject& aActionable );
  
  /**
   * @brief      Destroys the object.
   */
  virtual ~SetupReadout() {}
  
  /**
   * @brief      Configure the basic MP7 readout block functions
   *
   * @param[in]  aParams  Input parameters
   *
   * @return     Command execution status
   */
  virtual action::Command::State code(const ::swatch::core::XParameterSet& aParams);
};

// ----------------------------------------------------------------------------
class WatermarkConstraint : public core::XPSetConstraint
{
public:
  WatermarkConstraint(const std::string& aLowName, const std::string& aHighName);

  /**
   * @brief      Destroys the object.
   */
  virtual ~WatermarkConstraint() {};

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
  const std::string mLowName;
  
  //!
  const std::string mHighName;
  
};
// ----------------------------------------------------------------------------

} // namespace cmds
} // namespace mp7
} // namespace swatch



#endif /* __SWATCH_MP7_CMDS_SETUPREADOUT_HPP__ */
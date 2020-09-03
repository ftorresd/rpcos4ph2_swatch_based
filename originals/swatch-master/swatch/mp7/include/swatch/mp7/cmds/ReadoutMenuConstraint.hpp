#ifndef __SWATCH_MP7_CMDS_READOUTMENUCONSTRAINT_HPP__
#define __SWATCH_MP7_CMDS_READOUTMENUCONSTRAINT_HPP__

#include "swatch/core/XPSetConstraint.hpp"

namespace swatch {
namespace mp7 {
namespace cmds {

// ----------------------------------------------------------------------------
class ReadoutMenuConstraint : public core::XPSetConstraint
{
public:
  ReadoutMenuConstraint(uint32_t aBanks, uint32_t aModes, uint32_t aCaptures);

  /**
   * @brief      Destroys the object.
   */
  virtual ~ReadoutMenuConstraint() {};

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

  uint32_t mBanks;
  uint32_t mModes;
  uint32_t mCaptures;

  friend class ReadoutMenuHelper;
};
// ----------------------------------------------------------------------------

} // namespace cmds
} // namespace mp7
} // namespace swatch

#endif /* __SWATCH_MP7_CMDS_READOUTMENUCONSTRAINT_HPP__ */
#ifndef __SWATCH_CORE_XPARAMETERSETCONSTRAINT_HPP__
#define __SWATCH_CORE_XPARAMETERSETCONSTRAINT_HPP__


// Standard C++ headers
#include <typeinfo>

// boost headers
#include "boost/unordered_map.hpp"

// SWATCH headers
#include "swatch/core/exception.hpp"
#include "swatch/core/XMatch.hpp"


namespace swatch {
namespace core {

class XParameterSet;
class ConstraintParameterAlreadyExists;

//! Abstract base class for rules on sets of parameters
class XPSetConstraint {
public:

  virtual ~XPSetConstraint() = default;

  /**
   * @brief      Lists the parameters the constraints depends on.
   *
   * @return     Vector of parameter names.
   */
  std::set<std::string> getParameterNames() const;

  /**
   * @brief      Applies this constraint to the parameters
   * 
   * @throw      
   * @param      Set of parameters that the constraint is applied to
   * @return     true if the supplied XParameterSet passes this constraint; false otherwise.
   */
  XMatch operator()(const XParameterSet& aParams) const;


protected:

  /**
   * @brief      Registers a new parameter with of type T with this
   *             parameterset rule.
   *
   * @param[in]  aName  Name of the parameter
   *
   * @tparam     T      Type of the new parameter
   */
  template <typename T>
  void require(const std::string& aName);

  /**
   * @brief      Apply this rule to a parameter set.
   *
   * @param[in]  aParams  Input parameters.
   * @return     True if the input parameters match this rule.
   */
  virtual XMatch verify(const XParameterSet& aParams) const = 0;

  /**
   * @brief      Print the description of this contraint to a output stream.
   *
   * @param      aStream  Target output stream.
   */
  virtual void describe(std::ostream& aStream) const = 0;

  XPSetConstraint() = default;

private:
  /**
   * @brief      Check compatibility between the parameters and the rules.
   *
   * @param[in]  aParams  Input parameter set.
   */
  void throwIfNotApplicable(const XParameterSet& aParams) const;

  typedef boost::unordered_map<std::string, const std::type_info*> ParameterTypeMap_t;

  //! Map of parameter names and associated types.
  ParameterTypeMap_t mParameterTypeMap;

  friend std::ostream& operator<<(std::ostream& aOut, const XPSetConstraint& aConstraint);

};
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& aOut, const XPSetConstraint& aConstraint);
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
template <typename T>
void XPSetConstraint::require(const std::string& aName)
{
  if (!mParameterTypeMap.insert({aName, &typeid (T)}).second) {
    XCEPT_RAISE(ConstraintParameterAlreadyExists,"Parameter '" + aName + "' is already defined.");
  }
}
// ----------------------------------------------------------------------------


SWATCH_DEFINE_EXCEPTION(ConstraintIncompatibleParameter);
SWATCH_DEFINE_EXCEPTION(ConstraintParameterAlreadyExists);
SWATCH_DEFINE_EXCEPTION(ConstraintError);

} // namespace core
} // namespace swatch



#endif /* __SWATCH_CORE_XPARAMETERSETCONSTRAINT_HPP__ */
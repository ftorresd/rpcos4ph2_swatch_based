#ifndef __SWATCH_CORE_RULES_NOT_HPP__
#define __SWATCH_CORE_RULES_NOT_HPP__


#include "swatch/core/XRule.hpp"
#include "swatch/core/utilities.hpp"

#include <memory>

namespace swatch {
namespace core {
namespace rules {

// ----------------------------------------------------------------------------


//! Class representing no rule - i.e. all xdata::Serializable-derived objects pass this rule.
template <typename T>
class Not: public XRule<T> {
public:

	/**
	 * @brief      Not rule constructor
	 *
	 * @param[in]  aRule  A rule
	 *
	 * @tparam     Rule   { description }
	 */
	template<typename SubRule>
	Not( const SubRule& aSubRule ); 

	/**
	 * @brief      Copy constructor
	 *
	 * @param[in]  aOther  A copy of me
	 */
	Not( const Not& aOther );

  virtual XMatch verify( const T& aValue ) const final;

private:

  virtual void describe(std::ostream& aStream) const final;

  // Left rule
	std::unique_ptr<XRule<T>> mRule;

	//! Left rule cloner function to be used in the Copy Constructor
	XRuleCloner_t mRuleCloner;

};
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template<typename T>
template<typename SubRule>
Not<T>::Not( const SubRule& aSubRule ) {
	BOOST_STATIC_ASSERT_MSG( (std::is_base_of<XRule<T>, SubRule>::value), "class SubRule in Not( const SubRule& aSubRule ) must be a descendant of XRule<T>" );

	mRule = std::unique_ptr<XRule<T>>(new SubRule(aSubRule));

	mRuleCloner = XRuleCloner<SubRule>;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template<typename T>
Not<T>::Not( const Not& aOther ) :
	mRule(dynamic_cast<XRule<T>*>((*aOther.mRuleCloner)(*aOther.mRule))),
	mRuleCloner(aOther.mRuleCloner) {
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template<typename T>
XMatch Not<T>::verify( const T& aValue ) const 
{

	XMatch lResult = (*mRule)(aValue);

	// Flip the retirn status
	lResult.ok = !lResult.ok;

	// Return the merger
  return lResult;
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
template<typename T>
void Not<T>::describe(std::ostream& aStream) const 
{
  aStream << "!(" << *mRule << ")"; 
}
// ----------------------------------------------------------------------------

} // namespace rules
} // namespace core
} // namespace swatch

#endif /* __SWATCH_CORE_RULES_NOT_HPP__ */
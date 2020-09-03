#ifndef __SWATCH_CORE_RULES_OR_HPP__
#define __SWATCH_CORE_RULES_OR_HPP__


#include "swatch/core/XRule.hpp"
#include "swatch/core/utilities.hpp"

#include <memory>

namespace swatch {
namespace core {
namespace rules {

// ----------------------------------------------------------------------------


//! Class representing no rule - i.e. all xdata::Serializable-derived objects pass this rule.
template <typename T>
class Or: public XRule<T> {
public:

	/**
	 * @brief      Or rule constructor
	 *
	 * @param[in]  aLeft   Left rule
	 * @param[in]  aRight  Right rule
	 *
	 * @tparam     Left    Left rule class
	 * @tparam     Right   Right rule class
	 */
	template<typename Left, typename Right>
	Or( const Left& aLeft, const Right& aRight ); 

	/**
	 * @brief      Copy constructor
	 *
	 * @param[in]  aOther  A copy of me
	 */
	Or( const Or& aOther );

  virtual XMatch verify( const T& aValue ) const final;

private:

  virtual void describe(std::ostream& aStream) const final;

  // Left rule
	std::unique_ptr<XRule<T>> mLeft;

  // Right rule
	std::unique_ptr<XRule<T>> mRight;

	//! Left rule cloner function to be used in the Copy Constructor
	XRuleCloner_t mLeftCloner;

	//! Right rule cloner function to be used in the Copy Constructor
	XRuleCloner_t mRightCloner;

};
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template<typename T>
template<typename Left, typename Right>
Or<T>::Or( const Left& aLeft, const Right& aRight ) {
	BOOST_STATIC_ASSERT_MSG( (std::is_base_of<XRule<T>, Left>::value) , "class Left in Or( const Left& aLeft, const Right& aRight ) must be a descendant of XRule<T>" );
	BOOST_STATIC_ASSERT_MSG( (std::is_base_of<XRule<T>, Right>::value) , "class Left in Or( const Left& aLeft, const Right& aRight ) must be a descendant of XRule<T>" );

	mLeft = std::unique_ptr<XRule<T>>(new Left(aLeft));
	mRight = std::unique_ptr<XRule<T>>(new Right(aRight));

	mLeftCloner = XRuleCloner<Left>;
	mRightCloner = XRuleCloner<Right>;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template<typename T>
Or<T>::Or( const Or& aOther ) :
	mLeft(dynamic_cast<XRule<T>*>((*aOther.mLeftCloner)(*aOther.mLeft))),
	mRight(dynamic_cast<XRule<T>*>((*aOther.mRightCloner)(*aOther.mRight))),
	mLeftCloner(aOther.mLeftCloner),
	mRightCloner(aOther.mRightCloner) {
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template<typename T>
XMatch Or<T>::verify( const T& aValue ) const 
{

	// Apply the sub rules
	XMatch lLeft = (*mLeft)(aValue);
	XMatch lRight = (*mRight)(aValue);

	// Merge details
	std::vector<std::string> lDetails;
	if (!lLeft.details.empty()) lDetails.push_back(lLeft.details);
	if (!lRight.details.empty()) lDetails.push_back(lRight.details);

	// Return the merger
  return XMatch(lLeft.ok || lRight.ok, core::join(lDetails, "; "));
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
template<typename T>
void Or<T>::describe(std::ostream& aStream) const 
{
  aStream << "(" << *mLeft << " || " << *mRight << ")"; 
}
// ----------------------------------------------------------------------------

} // namespace rules
} // namespace core
} // namespace swatch

#endif /* __SWATCH_CORE_RULES_OR_HPP__ */
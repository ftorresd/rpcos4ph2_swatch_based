#ifndef __SWATCH_CORE_RULES_OUTOFRANGE_HPP__
#define __SWATCH_CORE_RULES_OUTOFRANGE_HPP__


#include "swatch/core/XRule.hpp"


namespace swatch {
namespace core {
namespace rules {

// ----------------------------------------------------------------------------
template<typename T>
class OutOfRange : public XRule<T> {
    
  BOOST_STATIC_ASSERT_MSG( (std::is_base_of<xdata::Serializable, T>::value) , "class T must be a descendant of xdata::Serializable" );

public:
  
    /**
   * @brief      Constructor
   *
   * @param[in]  aLowerBound  The range lower bound
   * @param[in]  aUpperBound  The range upper bound
   */
  OutOfRange( const T& aLowerBound, const T& aUpperBound );
  virtual ~OutOfRange() {};

  /**
   * @brief      Checks if aValue is greater than .
   *
   * @param[in]  aValue  Input value
   *
   * @return     True if aValue is finite
   */
  virtual XMatch verify( const T& aValue ) const;

private:

  virtual void describe(std::ostream& aStream) const;

  const T mLowerBound;
  const T mUpperBound;
};
// ----------------------------------------------------------------------------


} // namespace rules
} // namespace core
} // namespace swatch

#endif /* __SWATCH_CORE_RULES_OUTOFRANGE_HPP__ */
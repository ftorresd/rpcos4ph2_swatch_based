#ifndef __SWATCH_CORE_RULES_FINITENUMBER_HPP__
#define __SWATCH_CORE_RULES_FINITENUMBER_HPP__


#include "swatch/core/XRule.hpp"


namespace swatch {
namespace core {
namespace rules {

// ----------------------------------------------------------------------------
template<typename T>
class FiniteNumber : public XRule<T> {
    
  BOOST_STATIC_ASSERT_MSG( (std::is_base_of<xdata::Serializable, T>::value) , "class T must be a descendant of xdata::Serializable" );

public:
  FiniteNumber() {};
  virtual ~FiniteNumber() {};

  /**
   * @brief      Checks if aValue is finite.
   *
   * @param[in]  aValue  Input value
   *
   * @return     True if aValue is finite
   */
  virtual XMatch verify( const T& aValue ) const;

private:

  virtual void describe(std::ostream& aStream) const;

};
// ----------------------------------------------------------------------------


} // namespace rules
} // namespace core
} // namespace swatch

#endif /* __SWATCH_CORE_RULES_FINITENUMBER_HPP__ */
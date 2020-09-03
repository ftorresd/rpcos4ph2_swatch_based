#ifndef __SWATCH_CORE_RULES_ISAMONG_HPP__
#define __SWATCH_CORE_RULES_ISAMONG_HPP__


// XDAQ headers
#include "xdata/String.h"

// SWATCH headers
#include "swatch/core/XRule.hpp"


namespace swatch {
namespace core {
namespace rules {

class IsAmong : public XRule<xdata::String> {
  // BOOST_STATIC_ASSERT_MSG( (std::is_base_of<xdata::Serializable, T>::value) , "class T must be a descendant of xdata::Serializable" );

public:
  IsAmong( const std::vector<std::string>& aChoices ) :
    mChoices(aChoices)
  {};
  ~IsAmong() {};

  virtual XMatch verify( const xdata::String& aValue ) const;

private:

  virtual void describe(std::ostream& aStream) const;
  
  std::vector<std::string> mChoices;
};

} // namespace rules
} // namespace core
} // namespace swatch


#endif /* __SWATCH_CORE_RULES_ISAMONG_HPP__ */
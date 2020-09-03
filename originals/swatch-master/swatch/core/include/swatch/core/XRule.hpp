/**
 * @file    XRule.hpp
 * @author  Alessandro Thea
 * @brief   
 * @date    April 2017
 *
 */

#ifndef __SWATCH_CORE_XRULE_HPP__
#define __SWATCH_CORE_XRULE_HPP__

#include <iosfwd>
#include <typeinfo>

#include <boost/static_assert.hpp>

#include "swatch/core/exception.hpp"
#include "swatch/core/XMatch.hpp"

namespace xdata {
class Serializable;
}


namespace swatch {
namespace core {

// ----------------------------------------------------------------------------

/**
 * @brief      Base class for Rule Validator objects
 * @details    Defined the abstract interface for rule objects
 */
class AbstractXRule {
public:
  virtual ~AbstractXRule() {}

  /**
   * @brief      Applies this rule against a xdata::Serializable-derived object
   *
   * @param      The                object instance that the rule is applied to
   * @throws     XRuleTypeMismatch  if the supplied object instance is not of
   *                                the expected type
   * @return     true if the supplied object instance passes this rule; false
   *             otherwise.
   */
  virtual XMatch operator()(const xdata::Serializable& aObject) const = 0;

  //! Returns type of xdata::Serializable-derived object that this rule can be applied to
  virtual const std::type_info& type() const = 0;

protected:

  virtual void describe(std::ostream& aStream) const = 0;

private:
  AbstractXRule() {}

  //  AbstractXRule can only extended by XRule template 
  template<typename T>
  friend class XRule; // every XRule<T> is a friend of AbstractXRule

  friend std::ostream& operator<<(std::ostream& aOut, const AbstractXRule& aRule);
};
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& aOut, const AbstractXRule& aRule);
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
template<typename T>
class XRule : public AbstractXRule {

  // This template argument must be a descendent of xdata::Serializable
  BOOST_STATIC_ASSERT_MSG( (std::is_base_of<xdata::Serializable, T>::value) , "class T in XRule<T> must be a descendant of xdata::Serializable" );

public:

  virtual ~XRule() {};

  virtual const std::type_info& type() const final;

  virtual XMatch operator()(const xdata::Serializable& aSerializable) const final;

  virtual XMatch verify(const T&) const = 0;

protected:

  XRule() {};
};
// ----------------------------------------------------------------------------


typedef AbstractXRule* (*XRuleCloner_t)(const AbstractXRule&);

// ----------------------------------------------------------------------------
//
// @param[in]  aRule  Rule to Clone
//
// @tparam     R      Target Rule class
//
// @return     Clone of aRule
//
template<typename R>
AbstractXRule* XRuleCloner( const AbstractXRule& aRule ) {
  return new R(dynamic_cast<const R&>(aRule));
}
// ----------------------------------------------------------------------------

} // namespace core
} // namespace swatch


SWATCH_DEFINE_EXCEPTION(XRuleTypeMismatch);
SWATCH_DEFINE_EXCEPTION(XRuleArgumentError);


#include "swatch/core/XRule.hxx"

#endif /* __SWATCH_CORE_XRULE_HPP__ */
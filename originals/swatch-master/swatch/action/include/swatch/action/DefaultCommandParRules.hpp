#ifndef __SWATCH_ACTION_COMMANDPARVALIDATORDEFAULTS_HPP__
#define __SWATCH_ACTION_COMMANDPARVALIDATORDEFAULTS_HPP__

#include "swatch/core/XRule.hpp"
#include "swatch/core/rules/None.hpp"
#include "swatch/core/rules/FiniteNumber.hpp"
#include "swatch/core/rules/FiniteVector.hpp"
#include "swatch/core/rules/NoEmptyCells.hpp"

// XDAQ Headers
#include "xdata/Boolean.h"
#include "xdata/Integer.h"
#include "xdata/Integer32.h"
#include "xdata/Integer64.h"
#include "xdata/UnsignedInteger.h"
#include "xdata/UnsignedInteger32.h"
#include "xdata/UnsignedInteger64.h"
#include "xdata/UnsignedShort.h"
#include "xdata/UnsignedLong.h"
#include "xdata/Float.h"
#include "xdata/Double.h"
#include "xdata/Vector.h"
#include "xdata/String.h"
#include "xdata/Table.h"

namespace swatch {
namespace action {

/**
 * @brief      Class for undefined default validator of type T.
 *
 * @tparam     T     Type of the parameter class.
 */
template <typename T>
class UndefinedDefaultRule: public core::XRule<T> {
    // Triggered the following assert only if the class is instantiated
    BOOST_STATIC_ASSERT_MSG( sizeof(T) == 0 , "No default validator defined for class T" );
public:
    
    virtual bool apply( const T& aValue ) const final { return false; }

private:

    virtual void describe(std::ostream& aStream) const final {}

};
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
template<typename T, class Enable = void>
struct DefaultCmdParRule {
  typedef UndefinedDefaultRule<T> type;
};
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
template<typename T>
struct DefaultCmdParRule< T, typename std::enable_if<
    std::is_same<xdata::Boolean, T>{} || 
    std::is_same<xdata::Integer, T>{} || 
    std::is_same<xdata::Integer32, T>{} || 
    std::is_same<xdata::Integer64, T>{} || 
    std::is_same<xdata::UnsignedInteger, T>{} || 
    std::is_same<xdata::UnsignedInteger32, T>{} || 
    std::is_same<xdata::UnsignedInteger64, T>{} || 
    std::is_same<xdata::UnsignedShort, T>{} || 
    std::is_same<xdata::UnsignedLong, T>{} || 
    std::is_same<xdata::Float, T>{} ||
    std::is_same<xdata::Double, T>{}
  >::type > {
  
  typedef core::rules::FiniteNumber<T> type;
};
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
template<typename T>
struct DefaultCmdParRule< T, typename std::enable_if<
    std::is_same<xdata::Vector<xdata::Boolean>, T>{} || 
    std::is_same<xdata::Vector<xdata::Integer>, T>{} || 
    std::is_same<xdata::Vector<xdata::Integer32>, T>{} || 
    std::is_same<xdata::Vector<xdata::Integer64>, T>{} || 
    std::is_same<xdata::Vector<xdata::UnsignedInteger>, T>{} || 
    std::is_same<xdata::Vector<xdata::UnsignedInteger32>, T>{} || 
    std::is_same<xdata::Vector<xdata::UnsignedInteger64>, T>{} || 
    std::is_same<xdata::Vector<xdata::UnsignedShort>, T>{} || 
    std::is_same<xdata::Vector<xdata::UnsignedLong>, T>{} || 
    std::is_same<xdata::Vector<xdata::Float>, T>{} ||
    std::is_same<xdata::Vector<xdata::Double>, T>{}
  >::type > {

  typedef core::rules::FiniteVector<T> type;
};
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
template<typename T>
struct DefaultCmdParRule< T, typename std::enable_if<
    std::is_same<xdata::String, T>{}
  >::type > {
  
  typedef core::rules::None<T> type;
};
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
template<typename T>
struct DefaultCmdParRule< T, typename std::enable_if<
    std::is_same<xdata::Table, T>{}
  >::type > {
  
  typedef core::rules::NoEmptyCells type;
};
// ----------------------------------------------------------------------------


} // namespace action
} // namespace swatch


#endif

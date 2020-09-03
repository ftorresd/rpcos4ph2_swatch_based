/**
 * @file    XParameterSet.hpp
 * @author  Alessandro Thea
 * @date    February 2015
 */

#ifndef __SWATCH_CORE_XPARAMETERSET_HPP__
#define __SWATCH_CORE_XPARAMETERSET_HPP__


// Standard headers
#include <stddef.h>                     // for size_t
#include <iosfwd>                       // for ostream
#include <set>                          // for set
#include <string>                       // for string

// SWATCH headers
#include "swatch/core/exception.hpp"


// Forward declarations
namespace xdata {
class Serializable;
}

namespace swatch {
namespace core {


class XParameterSet;

std::ostream& operator<< ( std::ostream& aStr , const swatch::core::XParameterSet& aXParameterSet );


/**
 * @brief      Generic container (read-only) interface for containers of
 *             xdata::Serializable objects.
 */
class XParameterSet {
public:

  friend std::ostream& (operator<<) ( std::ostream& aStr , const swatch::core::XParameterSet& aXParameterSet );

  XParameterSet();

  XParameterSet(const XParameterSet& aOther);

  virtual ~XParameterSet();

  /**
   * @brief      Size of the collection
   *
   * @return     Number of stored parameters
   */
  virtual size_t size() const = 0;

  /**
   * @brief      List of names of stored parameters.
   *
   * @return     names of stored parameters
   */
  virtual std::set<std::string> keys() const = 0;

  /**
   * @brief      Returns whether parameter with given name exists
   *
   * @param      aName  Name of the entry to test
   *
   * @return     True if parameter exists
   */
  virtual bool has( const std::string& aName ) const = 0;

  /**
   * @brief      Retrieve reference to parameter with specified name; throws
   *             XParameterNotFound if doesn't contain a parameter with
   *             specified name
   *
   * @param      aName  Name of the entry to retrieve
   *
   * @return     The requested entry
   */
  virtual const xdata::Serializable& get( const std::string& aName ) const = 0;

  /**
   * @brief      Retrieve reference to parameter with specified name; throws
   *             XParameterNotFound if doesn't contain a parameter with
   *             specified name
   *
   * @param      aName  Name of the entry to retrieve
   *
   * @return     The requested entry
   */
  virtual const xdata::Serializable& operator[]( const std::string& aName ) const = 0;

  /**
   * @brief      Retrieve reference to parameter with specified name; throws
   *             XParameterFailedCast if dynamic cast fails; throws
   *             XParameterNotFound if doesn't contain a parameter with
   *             specified name
   *
   * @param[in]  aName  Name of the parameter.
   *
   * @tparam     T      Type of the parameter.
   *
   * @return     The requested parameter
   */
  template<typename T>
  const T& get( const std::string& aName ) const;

  /**
   * @brief      Removes parameter of specified name from the list of variables.
   *
   * @param      aName  ParameterSet entry to delete
   */
  virtual void erase( const std::string& aName ) = 0;

};

// Exceptions
SWATCH_DEFINE_EXCEPTION(XParameterNotFound)
SWATCH_DEFINE_EXCEPTION(XParameterExists)
SWATCH_DEFINE_EXCEPTION(XParameterFailedCast)
SWATCH_DEFINE_EXCEPTION(XParameterUnknownType)

} // namespace core
} // namespace swatch

#include "swatch/core/XParameterSet.hxx"

#endif /* __SWATCH_CORE_XPARAMETERSET_HPP__ */

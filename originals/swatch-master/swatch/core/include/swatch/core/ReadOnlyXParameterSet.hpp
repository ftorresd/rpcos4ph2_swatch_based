/**
 * @file    ReadOnlyXParameterSet.hpp
 * @author  Tom Williams, Alessandro Thea
 * @date    June 2015
 *
 */

#ifndef __SWATCH_CORE_READONLYXPARAMETERSET_HPP__
#define __SWATCH_CORE_READONLYXPARAMETERSET_HPP__


// Standard headers
#include <stddef.h>                     // for size_t
#include <set>                          // for set
#include <string>                       // for string, operator==
#include <utility>                      // for operator!=

// boost headers
#include "boost/smart_ptr/shared_ptr.hpp"  // for shared_ptr, operator==
#include "boost/unordered/unordered_map.hpp"  // for unordered_map

// SWATCH headers
#include "swatch/core/XParameterSet.hpp"


// forward declarations
namespace xdata {
class Serializable;
}


namespace swatch {
namespace core {

// Forward declaration
class ReadWriteXParameterSet;

//! Generic container for xdata::Serializable objects, with shared ownership via boost::shared_ptr<xdata::Serializable>, and read-only access to the underlying data
class ReadOnlyXParameterSet : public XParameterSet {
public:

  ReadOnlyXParameterSet();

  //! Copy constructor; performs SHALLOW COPY
  ReadOnlyXParameterSet(const ReadOnlyXParameterSet& aOrig);

  //! Constructor from any XParameterSet; performs SHALLOW COPY
  ReadOnlyXParameterSet(const XParameterSet& aOrig);

  virtual ~ReadOnlyXParameterSet();


  bool operator==(const ReadOnlyXParameterSet& aOther) const;


  /**
   * Size of the collection
   * @return Number of stored parameters
   */
  virtual size_t size() const;

  /**
   * List of names of stored parameters.
   * @return names of stored parameters
   */
  virtual std::set<std::string> keys() const;

  //! Returns whether parameter with given name exists
  virtual bool has( const std::string& aName ) const;

  virtual const xdata::Serializable& get( const std::string& aName ) const;

  virtual const xdata::Serializable& operator[]( const std::string& aName ) const;

  template<typename T>
  const T& get( const std::string& aName ) const
  {
    return this->XParameterSet::get<T>(aName);
  }

  /**
   * Adopt a parameter in the set; the data that is pointed to is not copied, and stored internally via a shared_ptr. The data type must be a derived from xdata::Serializable
   *
   * @param aName Parameter name
   * @param aData Pointer to import in the set
   */
  void adopt( const std::string& aName , const boost::shared_ptr<const xdata::Serializable>& aData );

  /**
   * Adopt a parameter that's already stored in another the set; the data that is pointed to is not copied - instead its shared between the parameter sets.
   *
   * @param aName Parameter name
   */
  void adopt( const std::string& aName , const ReadOnlyXParameterSet& aOtherSet);

  /**
   * Adopt a parameter that's already stored in another the set; the data that is pointed to is not copied - instead its shared between the parameter sets.
   *
   * @param aName Parameter name
   */
  void adopt( const std::string& aName , const ReadWriteXParameterSet& aOtherSet);


  virtual void erase( const std::string& aName );


private:
  typedef boost::unordered_map<std::string, boost::shared_ptr<const xdata::Serializable> > EntryMap_t;

  EntryMap_t mEntries;
};


} // namespace core
} // namespace swatch

#endif /* __SWATCH_CORE_READONLYXPARAMETERSET_HPP__ */

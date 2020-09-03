/**
 * @file    ReadWriteXParameterSet.hpp
 * @author  Tom Williams, Alessandro Thea
 * @date    June 2015
 *
 */

#ifndef __SWATCH_CORE_READWRITEXPARAMETERSET_HPP__
#define __SWATCH_CORE_READWRITEXPARAMETERSET_HPP__


// Standard headers
#include <set>                          // for set
#include <stddef.h>                     // for size_t
#include <string>                       // for string, operator==
#include <typeinfo>
#include <utility>                      // for pair, operator!=

// boost headers
#include "boost/smart_ptr/shared_ptr.hpp"  // for shared_ptr
#include "boost/unordered/unordered_map.hpp"  // for unordered_map

// SWATCH headers
#include "swatch/core/XParameterSet.hpp"


// forward declarations
namespace xdata {
class Serializable;
}

namespace swatch {
namespace core {

class ReadOnlyXParameterSet;


/**
 * @brief      Generic container for xdata::Serializable objects, with shared
 *             ownership via boost::shared_ptr<xdata::Serializable>, and
 *             read-write access to the underlying data
 */
class ReadWriteXParameterSet : public XParameterSet {
public:

  ReadWriteXParameterSet();

  virtual ~ReadWriteXParameterSet();


  /* --- DATA ACCESS --- */

  bool operator==( const ReadWriteXParameterSet& ) const;

  /**
   * @brief      Size of the collection
   *
   * @return     Number of stored parameters
   */
  size_t size() const;

  /**
   *  @brief      List of names of stored parameters.
   *
   * @return     names of stored parameters
   */
  std::set<std::string> keys() const;

  /**
   * @brief      Returns whether parameter with given name exists.
   *
   * @param[in]  aName  Name of the parameter.
   *
   * @return     True if the parameter is in this parameter set.
   */
  bool has( const std::string& aName ) const;


  /**
   * @brief      Returns a reference to the specified parameter.
   *
   * @param[in]  aName  Name of the parameter.
   *
   * @return     Referece to the parameter.
   */
  xdata::Serializable& get( const std::string& aName );

  /**
   * @brief      Returns a const reference to the specified parameter.
   *
   * @param[in]  aName  Name of the parameter.
   *
   * @return     Const referece to the parameter.
   */
  const xdata::Serializable& get( const std::string& aName ) const;

  xdata::Serializable& operator[]( const std::string& aName );

  const xdata::Serializable& operator[]( const std::string& aName ) const;

  template<typename T>
  const T& get( const std::string& aName ) const;

  template<typename T>
  T& get( const std::string& aName );


  /* --- ADDING ENTRIES TO SET --- */

  /**
   * @brief      Adopt a parameter in the set; the data that is pointed to is
   *             not copied, and stored internally via a shared_ptr. The data
   *             type must be a derived from xdata::Serializable
   *
   * @param      aName  Parameter name.
   * @param      aData  Pointer to import in the set.
   *
   * @tparam     T      Parameter type.
   */
  template<typename T>
  void adopt( const std::string& aName , const boost::shared_ptr<T>& aData );

  /**
   * @brief      Add a parameter to the set, by copying data into the set
   *
   * @param      aName  Parameter name
   * @param      aData  Value to copy in the set.
   *
   * @tparam     T      Parameter type.
   */
  template<typename T>
  void add( const std::string& aName , const T& aData );

  /**
   * @brief      Deep copy the content of another set in this one.
   *
   * @param[in]  aOtherSet  The set to copy the content from.
   */
  void deepCopyFrom(const ReadWriteXParameterSet& aOtherSet);


  /**
   * @brief      Removes a parameter from this set.
   *
   * @param[in]  aName  Name of the parameter to remove.
   */
  void erase( const std::string& aName );


private:

  template<typename T>
  static xdata::Serializable* clone( const xdata::Serializable* aOther );

  typedef xdata::Serializable* (*XCloner_t)( const xdata::Serializable* );

  struct XEntry {

    /**
     * Standard constructor
     *
     * @param aType Type info pointer
     * @param aCloner Cloner function pointer
     * @param aData Data to store in the entry
     */
    XEntry( const std::type_info* aType, XCloner_t aCloner, const boost::shared_ptr<xdata::Serializable>& aData );

    /**
     * Copy constructor
     * The object pointer is not copied but a new instance is created instead.
     *
     * @param aOrig Entry to copy from
     */
    XEntry( const XEntry& aOrig );

    bool operator==(const XEntry& aOther) const;

    const std::type_info* typeinfo;

    XCloner_t cloner;
    // xdata::Serializable* (*cloner)( const xdata::Serializable* );

    boost::shared_ptr<xdata::Serializable> object;
  };

  typedef boost::unordered_map<std::string, XEntry> EntryMap_t;

  std::pair<EntryMap_t::iterator,bool> emplace( const std::string& aName, const std::type_info* aType, XCloner_t aCloner, const boost::shared_ptr<xdata::Serializable>& aData );
  std::pair<EntryMap_t::iterator,bool> emplace( const std::string& aName, const XEntry& );

  //! Map storing the values
  EntryMap_t mEntries;

  friend class ReadOnlyXParameterSet;

  ReadWriteXParameterSet(const ReadWriteXParameterSet& ); // non scopyable
  ReadWriteXParameterSet& operator=( const ReadWriteXParameterSet& ); // non copyable
};


} // namespace core
} // namespace swatch

#include "swatch/core/ReadWriteXParameterSet.hxx"

#endif /* __SWATCH_CORE_READWRITEXPARAMETERSET_HPP__ */

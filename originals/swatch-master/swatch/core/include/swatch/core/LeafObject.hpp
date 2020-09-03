
#ifndef __SWATCH_CORE_LEAFOBJECT_HPP__
#define __SWATCH_CORE_LEAFOBJECT_HPP__

/**
 * @file    LeafObject.hpp
 * @author  Tom
 * @brief
 * @date    May 2016
 */


#include <deque>
#include <string>

#include "boost/noncopyable.hpp"

#include "swatch/core/exception.hpp"


namespace swatch {
namespace core {

class Object;


class LeafObject : public boost::noncopyable {
public:
  explicit LeafObject(const std::string& aId);

  LeafObject(const std::string& aId, const std::string& aAlias);

  virtual ~LeafObject();

  /**
    Return this object's ID
    @return this object's ID string
  */
  const std::string& getId() const;

  /**
    Return the dot-delimited path through the tree to this object
    @return the dot-delimited path through the tree to this object
  */
  std::string getPath() const;

  //! Returns the alias that was set during construction
  const std::string& getAlias() const;

private:
  /**
    Set the owner of the current Object
    @param aParent an Object to be made the parent of the current Object
  */
  void setParent(const LeafObject* aParent );

  /**
    Fill a deque with the ancestors (parent, grandparent, ...) of the current object
    @param aGenealogy a deque to be filled with the ancestors (parent, grandparent, ...) of the current object
  */
  void getAncestors ( std::deque< const LeafObject* >& aGenealogy ) const;

  const std::string mId;

  const std::string mAlias;

  //! Pointer to this object's parent
  const LeafObject* mParent;

  friend class Object;
};


SWATCH_DEFINE_EXCEPTION(InvalidObjectId)
SWATCH_DEFINE_EXCEPTION(FailedToSetParent)


} // end ns: core
} // end ns: swatch

#endif /* __SWATCH_CORE_LEAFOBJECT_HPP__ */


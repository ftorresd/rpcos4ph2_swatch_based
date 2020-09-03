/**
 * @file    Object.hpp
 * @author  Alessandro Thea
 * @brief   Base class to exercise the Object interface
 * @date    August 2014
 *
 */

#ifndef __SWATCH_CORE_OBJECT_HPP__
#define __SWATCH_CORE_OBJECT_HPP__


// Standard headers
#include <deque>                        // for deque<>::const_iterator, etc
#include <iterator>                     // for forward_iterator_tag, etc
#include <iosfwd>
#include <stddef.h>                     // for NULL
#include <stdint.h>                     // for uint32_t
#include <string>                       // for allocator, string, etc
#include <typeinfo>
#include <utility>                      // for pair, make_pair
#include <vector>                       // for vector

// boost headers
#include "boost/static_assert.hpp"      // for BOOST_STATIC_ASSERT_MSG
#include "boost/type_traits/is_base_of.hpp"
#include "boost/unordered/unordered_map.hpp"
#include "boost/unordered/unordered_set.hpp"

// SWATCH headers
#include "swatch/core/exception.hpp"
#include "swatch/core/LeafObject.hpp"
#include "swatch/core/utilities.hpp"


namespace swatch {
namespace core {

// Forward declarations
class Object;
class ObjectView;

// Forward declaration of the object-streaming operator
std::ostream& operator<< ( std::ostream& aStr , const swatch::core::Object& aObject );

/**
  Hierarchical base class for swatch classes. Inspired by uhal::Node
*/
//! Base class allowing for registration of children using string ID; child objects must be heap-allocated are owned by their parent
class Object : public LeafObject {
public:
  //! Functor interface used for destruction of objects in object tree (allows deletion to be delayed until all threads are done with the object)
  class Deleter {
  public:
    Deleter() {}
    virtual ~Deleter() {}

    virtual void operator()(LeafObject* aObject) = 0;
  };


  //! Class to iterate over the children of an object
  class iterator : public std::iterator< std::forward_iterator_tag , Object > {
    typedef std::deque< std::deque< std::pair< LeafObject*, Deleter* > >::const_iterator > Stack_t;
  public:
    /**
      Default constructor
    */
    iterator();

    /**
      Constructor from pointer to Object
      @param aBegin Top-level Object over which to iterate
    */
    iterator ( Object& aBegin );

    /**
      Copy constructor
      @param aOrig An Object-iterator to copy
    */
    iterator ( const iterator& aOrig );

    /**
      Destructor
    */
    virtual ~iterator();

    /**
      Dereferencing function
      @return the underlying object
    */
    LeafObject& value() const;

    /**
      Dereferencing function
      @return the underlying object
    */
    LeafObject& operator*() const;

    /**
      Dereferencing function
      @return pointer to the underlying object
    */
    LeafObject* operator->() const;

    /**
      Move the iterator to point to the next object
      @return whether the new iterator is valid
    */
    bool next();

    /**
      Prefix operator - increment the iterator
      @return new value of the iterator
    */
    iterator& operator++();

    /**
      Postfix operator - increment the iterator
      @param aDummy a dummy arg to distinguish between pre- and postfix operators
      @return a copy of the iterator before the increment
    */
    iterator operator++ ( int aDummy );

    /**
      Equality operator
      @param aIt an operator to compare whether two iterators are equal
      @return whether the two iterators are equal
    */
    bool operator== ( const iterator& aIt ) const;

    /**
      Inequality operator
      @param aIt an operator to compare whether two iterators are different
      @return whether the two iterators are different
    */
    bool operator!= ( const iterator& aIt ) const;

  private:
    /// Pointer to the current object or NULL if there are no more objects available
    Object* mBegin;
    /// A stack to store the vertical position in the hierarchy
    Stack_t mItStack;
  };


  friend std::ostream& ( operator<< ) ( std::ostream& aStr , const swatch::core::Object& aObject );

  /**
    Constructor
    @param aId The ID string of the object
  */
  explicit Object ( const std::string& aId );

  Object ( const std::string& aId, const std::string& aAlias );

  //! Destructor
  virtual ~Object();

  /**
    Construct an iterator starting at the current Object
  */
  iterator begin();

  /**
    Return a null iterator which will return true if we are comparing against the entry beyond the last valid object
  */
  iterator end();

  /**
    Return the type name associated with the stored entry
    @return the type name associated with the stored entry
  */
  std::string getTypeName() const;

  /**
    Return the full path to each of the children of the current object
    @return the full path to each of the children of the current object
  */
  std::vector<std::string> getPaths() const;

  /**
    Return ID strings of all children of this object
    @return ID strings of all children of this object
  */
  std::vector<std::string> getChildren() const;

  /**
    Return relative ID path of all this descendents (children, grandchildren, etc) of this object
    @return relative ID path of all this descendents (children, grandchildren, etc) of this object
  */
  std::vector<std::string> getDescendants() const;

  /**
    Navigate down the dot-delimited path from the current object and return the target
    @param aId a dot-delimited path from the current object to the target
    @throw ObjectDoesNotExist if object of specified ID path doesn't exist
    @return the object indicated by the dot-delimited path, or throw if the target doesn't exist
  */
  LeafObject& getObj ( const std::string& aId );

  /**
    Navigate down the dot-delimited path from the current object and return the target
    @param aId a dot-delimited path from the current object to the target
    @throw ObjectDoesNotExist if object of specified ID path doesn't exist
    @return the object indicated by the dot-delimited path, or throw if the target doesn't exist
  */
  const LeafObject& getObj ( const std::string& aId ) const;

  /**
    Navigate down the dot-delimited path from the current object and return the target, dynamic cast to type T
    @param aId a dot-delimited path from the current object to the target
    @throw ObjectDoesNotExist if object of specified ID path doesn't exist
    @throw ObjectFailedCast if object of specified ID path exists, but dynamic cast fails
    @return the object indicated by the dot-delimited path, dynamic casted to type T
  */
  template<typename T>
  T& getObj(const std::string& aId);

  /**
    Navigate down the dot-delimited path from the current object and return the target, dynamic cast to type T
    @param aId a dot-delimited path from the current object to the target
    @throw ObjectDoesNotExist if object of specified ID path doesn't exist
    @throw ObjectFailedCast if object of specified ID path exists, but dynamic cast fails
    @return the object indicated by the dot-delimited path, dynamic casted to type T
  */
  template<typename T>
  const T& getObj(const std::string& aId) const;

  /**
    Navigate down the dot-delimited path from the current object and return the target, dynamic cast to type T
    @param aId a dot-delimited full ID path of the target
    @throw ObjectDoesNotExist if object of specified ID path doesn't exist
    @throw ObjectFailedCast if object of specified ID path exists, but dynamic cast fails
    @return the object indicated by the dot-delimited path, dynamic casted to type T
  */
  template<typename T>
  T& getObjByFullPath(const std::string& aIdPath);

  /**
    Navigate down the dot-delimited path from the current object and return the target, dynamic cast to type T
    @param aId a dot-delimited full ID path of the target
    @throw ObjectDoesNotExist if object of specified ID path doesn't exist
    @throw ObjectFailedCast if object of specified ID path exists, but dynamic cast fails
    @return the object indicated by the dot-delimited path, dynamic casted to type T
  */
  template<typename T>
  const T& getObjByFullPath(const std::string& aIdPath) const;

  /**
    Navigate down the dot-delimited path from the current object and return the target, dynamic cast to type T
    @param aId a dot-delimited path from the current object to the target
    @throw ObjectDoesNotExist if object of specified ID path doesn't exist
    @return the object indicated by the dot-delimited path, dynamic casted to type T; NULL pointer if dynamic cast fails; or throw if the target doesn't exist
  */
  template<typename T>
  T* getObjPtr ( const std::string& aId );

  /**
    Navigate down the dot-delimited path from the current object and return the target, dynamic cast to type T
    @param aId a dot-delimited path from the current object to the target
    @throw ObjectDoesNotExist if object of specified ID path doesn't exist
    @return the object indicated by the dot-delimited path, dynamic casted to type T; NULL pointer if dynamic cast fails; or throw if the target doesn't exist
  */
  template<typename T>
  const T* getObjPtr ( const std::string& aId ) const;

//         template<typename T>
//         std::deque<T*> getChildrenOfType();

  bool isAncestorOf(const LeafObject& aPotentialDescendant) const;

  size_t getNumberOfGenerationsTo(const LeafObject& aDescendant) const;

protected:

  /**
    Add a new Object as a child of the current Object; throws if this object already has another child object with the same ID
    @warning the parent takes ownership of the child and takes responsibility for deleting it
    @param aChild a pointer to a new Object
  */
  void addObj ( LeafObject* aChild );

  /**
    Add a new object as a child of the current Object
    @warning the parent takes ownership of the child, and takes responsibility for deleting it using the supplied functor
    @param aChild a pointer to a new Object
    @param aDeleter a functor that inherits from Object::Deleter. Must be copy-constructible. The functor's void operator()(Object*) method will be called later in order to delete this object
  */
  template <class T>
  void addObj ( LeafObject* aChild, T aDeleter);

private:
//  /**
//    Return the ancestor (parent, grandparent, ...) of the object at specified depth
//    @param aDepth the depth of the target to retrieve (0=current, 1=parent, 2=grandparent, ...)
//    @return the target object or NULL if no such target exists
//  */
//  const Object* getAncestor ( const uint32_t& aDepth = 1 ) const;
//
//  /**
//    Return the ancestor (parent, grandparent, ...) of the object at specified depth
//    @param aDepth the depth of the target to retrieve (0=current, 1=parent, 2=grandparent, ...)
//    @return the target object or NULL if no such target exists
//  */
//  Object* getAncestor ( const uint32_t& aDepth = 1 );

  /**
    Recursive stream-formatting function
    @param aStr a stream to append the formatted output to
    @param aIndent the current layer of recursion = the amount of indentation
  */
  void print ( std::ostream& aStr , const uint32_t& aIndent = 0 ) const;

  /**
    Fills up a map with all descendants of this object; the path of each object in the map is prepended by the string specified by the caller
    @param aBasePath String that is prepended to object paths
    @param aChart Map that will be filled with all descendants
  */
  void getCharts ( const std::string& aBasePath, boost::unordered_map<std::string, LeafObject*>& aChart ) const;

  //! Container for child objects, and their deleter functors. Children in this list are deleted by the destructor
  std::deque< std::pair< LeafObject*, Deleter* > > mChildren;

  //! Map of children
  boost::unordered_map< std::string, LeafObject* > mObjectsChart;

  boost::unordered_set<std::string> mChildAliases;

  friend class ObjectView;
};


class ObjectView : public Object {
public:

  ObjectView ( const std::string& aId, const std::string& aAlias );

  virtual ~ObjectView();

protected:

  void addObj ( LeafObject* aChild, const std::string& aAlias );

  void addObj ( LeafObject* aChild );
};


SWATCH_DEFINE_EXCEPTION(ObjectOfSameIdAlreadyExists)
SWATCH_DEFINE_EXCEPTION(ObjectOfSameAliasAlreadyExists)
SWATCH_DEFINE_EXCEPTION(ObjectDoesNotExist)
SWATCH_DEFINE_EXCEPTION(ObjectFailedCast)
SWATCH_DEFINE_EXCEPTION(ObjectIsNotDescendant)


template<typename T>
T& Object::getObj ( const std::string& aId )
{
  T* lObj = dynamic_cast<T*> ( & this->getObj(aId) );
  if (lObj == NULL)
    XCEPT_RAISE(ObjectFailedCast,"Could not cast descendent '" + aId + "' of object '" + getPath() + "' to type " + demangleName(typeid(T).name()));
  else
    return *lObj;
}


template<typename T>
const T& Object::getObj ( const std::string& aId ) const
{
  const T* lObj = dynamic_cast<const T*> ( &getObj(aId) );
  if (lObj == NULL)
    XCEPT_RAISE(ObjectFailedCast,"Could not cast descendent '" + aId + "' of object '" + getPath() + "' to type " + demangleName(typeid(T).name()));
  else
    return *lObj;
}


template<typename T>
T& Object::getObjByFullPath(const std::string& aFullIdPath)
{
  if (aFullIdPath.find(getPath()) != 0)
    XCEPT_RAISE(ObjectDoesNotExist, "Beginning of full object ID path '" + aFullIdPath + "' does not match path of object '" + getPath() + "'");
  else
    return getObj<T>(aFullIdPath.substr(getPath().size()+1));
}


template<typename T>
const T& Object::getObjByFullPath(const std::string& aFullIdPath) const
{
  if (aFullIdPath.find(getPath()) != 0)
    XCEPT_RAISE(ObjectDoesNotExist, "Beginning of full object ID path '" + aFullIdPath + "' does not match path of object '" + getPath() + "'");
  else
    return getObj<T>(aFullIdPath.substr(getPath().size()+1));
}


template<typename T>
T* Object::getObjPtr ( const std::string& aId )
{
  return dynamic_cast<T*> ( & this->getObj ( aId ) );
}


template<typename T>
const T* Object::getObjPtr ( const std::string& aId ) const
{
  return dynamic_cast<const T*> ( & this->getObj ( aId ) );
}


template<class T>
void Object::addObj (LeafObject* aChild, T aDeleter)
{
  BOOST_STATIC_ASSERT_MSG( (boost::is_base_of<Object::Deleter, T >::value) , "class T must be a descendant of swatch::core::Object::Deleter" );

  aChild->setParent(this);

  // Ensure the child does not have a twin
  if (mObjectsChart.find(aChild->getId()) != mObjectsChart.end()) {
    const std::string lErrorMsg("Object with ID '" + aChild->getId() + "' already exists as child of '" + getPath() + "'");
    delete aChild;
    XCEPT_RAISE(ObjectOfSameIdAlreadyExists, lErrorMsg);
  }

  if ( (not aChild->getAlias().empty()) and (mChildAliases.find(aChild->getAlias()) != mChildAliases.end()) ) {
    const std::string lErrorMsg("Object with alias '" + aChild->getAlias() + "' already exists as child of '" + getPath() + "'");
    delete aChild;
    XCEPT_RAISE(ObjectOfSameAliasAlreadyExists, lErrorMsg);
  }

  mChildren.push_back(std::make_pair(aChild, new T(aDeleter)));
  mObjectsChart.insert(std::make_pair(aChild->getId(), aChild));
  mChildAliases.insert(aChild->getAlias());
}


} // namespace core
} // namespace swatch


#endif  /* __SWATCH_CORE_OBJECT_HPP__ */


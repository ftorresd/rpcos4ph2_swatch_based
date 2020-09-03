/**
 * @file    AbstractFactory.hpp
 * @author  Alessandro Thea
 * @brief   Template class to provide support for abstract classes
 * @date    July 2014
 *
 */

#ifndef __SWATCH_CORE_ABSTRACTFACTORY_HPP__
#define __SWATCH_CORE_ABSTRACTFACTORY_HPP__

// C++ headers
#include <string>

// boost headers
#include <boost/noncopyable.hpp>
#include <boost/unordered_map.hpp>
#include <boost/shared_ptr.hpp>

// SWATCH headers
#include "swatch/core/AbstractStub.hpp"
#include "swatch/core/exception.hpp"


// Standard factory registration macros
#define _SWATCH_ABSTRACT_REGISTER_CLASS( productname, classname ) \
template<> bool swatch::core::ClassRegistrationHelper< productname, classname >::sInitialised= \
  swatch::core::ClassRegistrationHelper< productname, classname >::init(#classname);

#define _SWATCH_ABSTRACT_REGISTER_CREATOR( productname, creatorname ) \
template<> bool swatch::core::CreatorRegistrationHelper< productname, creatorname >::sInitialised= \
  swatch::core::CreatorRegistrationHelper< productname, creatorname >::init(#creatorname);


namespace swatch {
namespace core {

///! Template class to provide support for factory-style creation of abstract classes
template<typename T>
class AbstractFactory : public boost::noncopyable {
private:
  AbstractFactory() {}

public:
  template< typename A, typename D > friend struct ClassRegistrationHelper;
  template< typename A, typename K > friend struct CreatorRegistrationHelper;

  typedef T Product;

  virtual ~AbstractFactory() {}

  static AbstractFactory* get();

  /*
  T* make( const std::string& aCreatorId, const std::string& aId, const swatch::core::XParameterSet& params );

  T* make( const swatch::core::XParameterSet& params );
   */

  /*
  template<typename P = T>
  P* make( const std::string& aCreatorId, const std::string& aId, const swatch::core::XParameterSet& params );

  template<typename P = T>
  P* make( const swatch::core::XParameterSet& params );
  */

  template<typename P>
  P* make( const std::string& aCreatorId, const AbstractStub& aStub );

  /*
  class CreatorInterface {
  public:
      virtual T* operator()( const std::string& aId, const swatch::core::XParameterSet& params ) = 0;
  };
  */


  class CreatorInterface {
  public:
    virtual T* operator()( const AbstractStub& aStub ) = 0;
  };

private:
  /**
   * Basic creator class
   *
   * @tparam D Product type, must be derived from T
   */
  /*
  template<typename D>
  class BasicCreator : public CreatorInterface {
  public:
      virtual T* operator() ( const std::string& aId, const swatch::core::XParameterSet& params );
  };
   */
  template<typename D>
  class BasicCreator : public CreatorInterface {
  public:
    virtual T* operator() ( const AbstractStub& aStub );
  };

  template<typename K>
  bool add(const std::string& aCreatorName);

private:
  static AbstractFactory* sInstance;
  boost::unordered_map<std::string, boost::shared_ptr<CreatorInterface> > mCreators;
};


/**
 * Factory helper class
 *
 * @tparam A Base product type
 * @tparam D Derived product type
 */
template< typename A, typename D >
struct ClassRegistrationHelper {
  //! Dummy variable required as initialisation target
  static bool sInitialised;

  static bool init(const std::string& aClassName)
  {
    AbstractFactory<A>::get()->template add< typename AbstractFactory<A>::template BasicCreator<D> > ( aClassName );
    return true;
  }
};


/**
 * Factory helper class
 *
 * @tparam A Base product type
 * @tparam K Creator type
 */
template< typename A, typename K >
struct CreatorRegistrationHelper {
  //! Dummy variable required as initialisation target
  static bool sInitialised;

  static bool init(const std::string& aCreatorName)
  {
    AbstractFactory<A>::get()->template add< K > ( aCreatorName );
    return true;
  }
};


} // namespace core
} // namespace swatch

SWATCH_DEFINE_EXCEPTION(CreatorNotFound)
SWATCH_DEFINE_EXCEPTION(FailedFactoryCast)

#include "swatch/core/AbstractFactory.hxx"


#endif  /* __SWATCH_CORE_ABSTRACTFACTORY_HPP__ */

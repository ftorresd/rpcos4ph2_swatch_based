/**
 * @file    exception.hpp
 * @author  Alessandro Thea
 * @date    July 2014
 */

#ifndef __SWATCH_CORE_EXCEPTION_HPP__
#define __SWATCH_CORE_EXCEPTION_HPP__


// Standard headers
#include <exception>
#include <string>

// boost headers
#include <boost/thread/thread.hpp>

#include "xcept/Exception.h"


//! MACRO to create definitions for the derived exceptions
#define DEFINE_SWATCH_EXCEPTION(T) 				\
	class T : public swatch::core::exception 	\
	{											\
		public:									\
		T() : swatch::core::exception()			\
		{}										\
		T(const std::string& aWhat) :			\
		swatch::core::exception(aWhat)			\
		{}										\
	};


namespace swatch {
namespace core {

class exception : public std::exception {
public:

  /**
   * Constructor
   */
  exception() throw ();

  /**
   * Overloaded constructor
   */
  exception(const std::string& aWhat);

  /**
   * Copy constructor
   * @param aOther The to copy to build this exception object
   */
  exception(const exception& aOther) throw ();

  /**
   * Assignment operator
   * @param aOther The exception object to assign to this one
   * @return Reference to a new exception object
   */
  exception& operator=(const exception& aOther) throw ();

  /**
   * Destructor. Destruction delegated to subclasses
   */
  virtual ~exception() throw ();

  /**
   * Retrieve the reason for throwing the exception
   * @return const char* containing the reason the exception was thrown
   */
  virtual const char* what() const throw ();


protected:

  //! Thread ID in which the exception was thrown
  boost::thread::id mThreadId;

  //! Description of the exception
  std::string mWhat;
};


#define SWATCH_DEFINE_EXCEPTION(EXCEPTION_NAME) \
class EXCEPTION_NAME: public swatch::core::Exception \
{\
    public: \
    EXCEPTION_NAME( std::string name, std::string message, std::string module, int line, std::string function ): \
        swatch::core::Exception(name, message, module, line, function) \
    {} \
    EXCEPTION_NAME( std::string name, std::string message, std::string module, int line, std::string function, swatch::core::Exception & e ): \
        swatch::core::Exception(name, message, module, line, function,e) \
    {} \
};

/*
#define SWATCH_DEFINE_EXCEPTION_WITH_PARENT(EXCEPTION_NAME, PARENT_NAME) \
class EXCEPTION_NAME: public NAMESPACE1::exception::PARENT_NAME \
{\
    public: \
    EXCEPTION_NAME( std::string name, std::string message, std::string module, int line, std::string function ): \
    PARENT_NAME(name, message, module, line, function) \
    {}; \
    EXCEPTION_NAME( std::string name, std::string message, std::string module, int line, std::string function, xcept::Exception& previous ): \
    PARENT_NAME(name, message, module, line, function, previous) \
    {}; \
};
*/

class Exception : public xcept::Exception {


public:

  Exception () throw ();

  Exception (const std::string &aName, const std::string &aMessage, const std::string &aModule, int aLine, const std::string &aFunction);
 
  Exception (const std::string &aName, const std::string &aMessage, const std::string &aModule, int aLine, const std::string &aFunction, Exception &aPrevious);
 
  virtual ~Exception () throw ();

protected:

  //! Thread ID in which the exception was thrown
  boost::thread::id mThreadId;
};

SWATCH_DEFINE_EXCEPTION(RuntimeError)
SWATCH_DEFINE_EXCEPTION(InvalidArgument)

} // namespace core
} // namespace swatch

#endif /* __SWATCH_CORE_EXCEPTION_HPP__ */

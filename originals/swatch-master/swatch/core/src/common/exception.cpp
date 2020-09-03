#include "swatch/core/exception.hpp"

namespace swatch {
namespace core {
  



exception::exception() throw() :
  std::exception(),
  mThreadId(boost::this_thread::get_id())
{
}



exception::exception(const std::string& aWhat) :
  std::exception(),
  mWhat(aWhat)
{
}



exception::exception(const exception& aOther) throw() :
  std::exception(),
  mThreadId(aOther.mThreadId),
  mWhat(aOther.mWhat)
{
}



exception::~exception() throw()
{
}



exception&
exception::operator=(const exception& aOther) throw()
{
  mThreadId = aOther.mThreadId;
  mWhat = aOther.mWhat;

  return *this;
}



const char*
exception::what() const throw()
{
  return mWhat.c_str();
}


// ----------------------------------------------------------------------------
Exception::Exception () throw () :
  mThreadId(boost::this_thread::get_id())
{

}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
Exception::Exception (const std::string &aName, const std::string &aMessage, const std::string &aModule, int aLine, const std::string &aFunction) :
  xcept::Exception(aName, aMessage, aModule, aLine, aFunction),
  mThreadId(boost::this_thread::get_id())
{

}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
Exception::Exception (const std::string &aName, const std::string &aMessage, const std::string &aModule, int aLine, const std::string &aFunction, swatch::core::Exception &aPrevious) :
  xcept::Exception(aName, aMessage, aModule, aLine, aFunction),
  mThreadId(aPrevious.mThreadId)
{

}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
Exception::~Exception () throw () {
}
// ----------------------------------------------------------------------------

} // namespace core  
} // namespace swatch
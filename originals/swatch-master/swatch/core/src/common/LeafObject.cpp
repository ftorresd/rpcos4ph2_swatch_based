
#include "swatch/core/LeafObject.hpp"


#include <cctype>
#include <sstream>
#include <stdexcept>                    // for runtime_error

#include "boost/lexical_cast.hpp"


namespace swatch {
namespace core {


LeafObject::LeafObject(const std::string& aId) :
  mId(aId),
  mAlias(""),
  mParent(0x0)
{
  if (mId.empty())
    XCEPT_RAISE(InvalidObjectId, "Cannot create object with empty ID string");
  else if (mId.find('.') != std::string::npos)
    XCEPT_RAISE(InvalidObjectId, "Object ID string '" + mId + "' contains a dot at position " + boost::lexical_cast<std::string>(mId.find('.')));

  for (size_t i = 0; i < mId.size(); i++) {
    if ( (isalnum(mId[i]) == 0) and (mId[i] != '_') )
      XCEPT_RAISE(InvalidObjectId, std::string("Found invalid character '") + mId[i] + "' at position " + boost::lexical_cast<std::string>(i) + " in ID '" + mId + "'; ID strings must only contain alphanumeric characters." );
  }
}


LeafObject::LeafObject(const std::string& aId, const std::string& aAlias) :
  mId(aId),
  mAlias(aAlias),
  mParent(0x0)
{
  if (mId.empty())
    XCEPT_RAISE(InvalidObjectId, "Cannot create object with empty ID string");
  else if (mId.find('.') != std::string::npos)
    XCEPT_RAISE(InvalidObjectId, "Object ID string '" + mId + "' contains a dot at position " + boost::lexical_cast<std::string>(mId.find('.')));

  for (size_t i = 0; i < mId.size(); i++) {
    if ( (isalnum(mId[i]) == 0) and (mId[i] != '_') )
      XCEPT_RAISE(InvalidObjectId, std::string("Found invalid character '") + mId[i] + "' at position " + boost::lexical_cast<std::string>(i) + " in ID '" + mId + "'; ID strings must only contain alphanumeric characters." );
  }
}


LeafObject::~LeafObject()
{
}


const std::string& LeafObject::getId() const
{
  return mId;
}


std::string LeafObject::getPath() const
{
  std::deque< const LeafObject* > lGenealogy;
  getAncestors(lGenealogy);
  std::string lRet;

  for (std::deque< const LeafObject* >::iterator lIt(lGenealogy.begin()); lIt != lGenealogy.end(); ++lIt) {
    if ((**lIt).getId().size()) {
      lRet += (**lIt).getId();
      lRet += ".";
    }
  }

  if (lRet.size()) {
    lRet.resize(lRet.size() - 1);
  }

  return lRet;
}


const std::string& LeafObject::getAlias() const
{
  return mAlias;
}


void LeafObject::setParent(const LeafObject* aParent)
{
  if (mParent) {
    std::ostringstream lMsgStream;
    lMsgStream << "Cannot set ancestor of " << this->getPath() << ". Already set to " << mParent->getPath();
    XCEPT_RAISE(FailedToSetParent,lMsgStream.str());
  }

  mParent = aParent;
}


void LeafObject::getAncestors(std::deque<const LeafObject*>& aGenealogy) const
{
  aGenealogy.push_front(this);

  if (mParent) {
    mParent->getAncestors(aGenealogy);
  }
}



} // end ns: core
} // end ns: swatch

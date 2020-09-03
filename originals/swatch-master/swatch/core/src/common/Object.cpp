
#include "swatch/core/Object.hpp"


#include <iostream>
#include <stdexcept>                    // for runtime_error

#include "swatch/core/utilities.hpp"


using namespace std;


namespace swatch {
namespace core {


Object::Object(const std::string& aId) :
  LeafObject(aId)
{
}


Object::Object(const std::string& aId, const std::string& aAlias) :
  LeafObject(aId, aAlias)
{
}


Object::~Object()
{
  mObjectsChart.clear();

  for (std::deque< std::pair<LeafObject*,Deleter*> >::iterator lIt(mChildren.begin()); lIt != mChildren.end(); ++lIt) {
    if ( lIt->second != NULL ) {
      (*lIt->second)(lIt->first);
      delete lIt->second;
    }
    else
      delete lIt->first;
  }
}


std::string Object::getTypeName() const
{
  return demangleName(typeid(*this).name());
}


void Object::addObj(LeafObject* aChild)
{
  aChild->setParent(this);

  // Insure the child does not have a twin
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

  mChildren.push_back(std::make_pair(aChild, (Deleter*)NULL));
  mObjectsChart.insert(std::make_pair(aChild->getId(), aChild));
  mChildAliases.insert(aChild->getAlias());
}


//const Object* Object::getAncestor(const uint32_t& aDepth) const {
//  if (!aDepth) {
//    return this;
//  }
//
//  if (!mParent) {
//    return NULL;
//  }
//
//  return mParent-> getAncestor(aDepth - 1);
//}
//
//
//Object* Object::getAncestor(const uint32_t& aDepth) {
//  if (!aDepth) {
//    return this;
//  }
//
//  if (!mParent) {
//    return NULL;
//  }
//
//  return mParent-> getAncestor(aDepth - 1);
//}


void Object::print(std::ostream& aStr, const uint32_t& aIndent) const
{
  aStr << '\n' << std::string(aIndent, ' ') << "- [" << getTypeName() << "] " << getId();

  for (std::deque< std::pair<LeafObject*,Deleter*> >::const_iterator lIt = mChildren.begin(); lIt != mChildren.end(); ++lIt) {
    if (const Object* lChildObj = dynamic_cast<const Object*>(lIt->first))
      lChildObj->print(aStr, aIndent + 1);
    else
      aStr << '\n' << std::string(aIndent+1, ' ') << "- [" << demangleName(typeid(*lIt->first).name()) << "] " << lIt->first->getId();
  }
}


void Object::getCharts(const std::string& aBasePath, boost::unordered_map<std::string, LeafObject*>& aChart) const
{
  std::string lPrefix(aBasePath.empty() ? "" : aBasePath + '.');
  // 1) Add children
  boost::unordered_map<std::string, LeafObject*>::const_iterator lItMap;

  for (lItMap = mObjectsChart.begin(); lItMap != mObjectsChart.end(); ++lItMap) {
    aChart.insert(std::make_pair(lPrefix + lItMap->first, lItMap->second));
  }

  // 2) Add children's children (ad infinitum) by recursive calls
  std::deque< std::pair<LeafObject*, Deleter*> >::const_iterator lIt;

  for (lIt = mChildren.begin(); lIt != mChildren.end(); ++lIt) {
    if (const Object* lChildObj = dynamic_cast<const Object*>(lIt->first))
      lChildObj->getCharts(lPrefix + lIt->first->getId(), aChart);
  }
}


std::vector<std::string> Object::getChildren() const
{
  std::vector<std::string> lNames;
  lNames.reserve(mObjectsChart.size());

  for (boost::unordered_map<std::string, LeafObject*>::const_iterator lIt = mObjectsChart.begin(); lIt != mObjectsChart.end(); lIt++) {
    lNames.push_back(lIt->first);
  }

  return lNames;
}


std::vector<std::string> Object::getDescendants() const
{
  boost::unordered_map<std::string, LeafObject*> lChart;
  this->getCharts("", lChart);
  std::vector<std::string> lNames;
  lNames.reserve(lChart.size());
  boost::unordered_map<std::string, LeafObject*>::const_iterator lIt;

  for (lIt = lChart.begin(); lIt != lChart.end(); ++lIt) {
    lNames.push_back(lIt->first);
  }

  return lNames;
}


LeafObject& Object::getObj(const std::string& aId)
{
  std::size_t lPos;
  string lFather, lChild;

  if ((lPos = aId.find('.')) != std::string::npos) {
    lFather = aId.substr(0, lPos);
    lChild = aId.substr(lPos + 1);
  }
  else {
    lFather = aId;
  }

  //    cout << "father = '" << father << "'   child = '" << child << "'" << endl;
  boost::unordered_map<std::string, LeafObject*>::const_iterator lIt;

  if ((lIt = mObjectsChart.find(lFather)) == mObjectsChart.end()) {
    stringstream lSS;
    lSS << "Object '" << lFather << "' not found in '" << getPath() << "'";
    XCEPT_RAISE(ObjectDoesNotExist,lSS.str());
  }

  if (lChild.empty()) {
    return *(lIt->second);
  }
  else if (Object* lChildObj = dynamic_cast<Object*>(lIt->second))
    return lChildObj->getObj(lChild);
  else {
    ostringstream lMsgStream;
    lMsgStream << "Object '" << lChild << "' not found in '" << lIt->second->getPath() << "'";
    XCEPT_RAISE(ObjectDoesNotExist,lMsgStream.str());
  }
}


const LeafObject& Object::getObj(const std::string& aId) const
{
  std::size_t lPos;
  string lFather, lChild;

  if ((lPos = aId.find('.')) != std::string::npos) {
    lFather = aId.substr(0, lPos);
    lChild = aId.substr(lPos + 1);
  }
  else {
    lFather = aId;
  }

  //    cout << "father = '" << father << "'   child = '" << child << "'" << endl;
  boost::unordered_map<std::string, LeafObject*>::const_iterator lIt;

  if ((lIt = mObjectsChart.find(lFather)) == mObjectsChart.end()) {
    stringstream lSS;
    lSS << "Object " << lFather << " not found in " << getId();
    XCEPT_RAISE(ObjectDoesNotExist,lSS.str());
  }

  if (lChild.empty()) {
    return *(lIt->second);
  }

  else if (Object* lChildObj = dynamic_cast<Object*>(lIt->second))
    return lChildObj->getObj(lChild);
  else {
    ostringstream lMsgStream;
    lMsgStream << "Object '" << lChild << "' not found in '" << lIt->second->getPath() << "'";
    XCEPT_RAISE(ObjectDoesNotExist,lMsgStream.str());
  }
}


bool Object::isAncestorOf(const LeafObject& aPotentialDescendant) const
{
  const LeafObject* lDescendantsAncestor = aPotentialDescendant.mParent;

  while (lDescendantsAncestor != NULL) {
    if (lDescendantsAncestor == this)
      return true;
    lDescendantsAncestor = lDescendantsAncestor->mParent;
  }
  return false;
}

size_t Object::getNumberOfGenerationsTo(const LeafObject& aDescendant) const
{
  const LeafObject* lDescendantsAncestor = aDescendant.mParent;
  size_t lNrGenerations = 1;

  while (lDescendantsAncestor != NULL) {
    if (lDescendantsAncestor == this)
      return lNrGenerations;

    lNrGenerations++;
    lDescendantsAncestor = lDescendantsAncestor->mParent;
  }

  XCEPT_RAISE(ObjectIsNotDescendant,"Object '" + aDescendant.getPath() + "' is not a descendant of '" + getPath() + "'");
}


// TODO: include self

std::vector<std::string> Object::getPaths() const
{
  std::vector<std::string> lNames;
  lNames.reserve(mObjectsChart.size());
  boost::unordered_map<std::string, LeafObject*>::const_iterator lIt;

  for (lIt = mObjectsChart.begin(); lIt != mObjectsChart.end(); ++lIt) {
    lNames.push_back(lIt->second->getPath());
  }

  return lNames;
}


Object::iterator Object::begin()
{
  Object::iterator lIt(*this);
  return lIt;
}


Object::iterator Object::end()
{
  Object::iterator lIt;
  return lIt;
}

// Object View Methods Implementation

ObjectView::ObjectView(const std::string& aId, const std::string& aAlias) :
  Object(aId, aAlias)
{
}

/*
ObjectView::ObjectView(const std::string& aId, const XParameterSet& params) :
  Object(aId, params) {
}
*/

ObjectView::~ObjectView()
{
  // clear the maps before ~Object, which would destroy them
  //    mChildren.clear();
  //    mObjectsChart.clear();
}


void ObjectView::addObj(LeafObject* aChild, const std::string& aAlias)
{
  // Insure the child does not have a twin
  if (mObjectsChart.find(aChild->getId()) != mObjectsChart.end()) {
    XCEPT_RAISE(ObjectOfSameIdAlreadyExists, "Object with ID '" + aChild->getId() + "' already exists as child of '" + getPath() + "'");
  }

  if ( not aChild->getAlias().empty() and (mChildAliases.find(aChild->getAlias()) != mChildAliases.end()) ) {
    XCEPT_RAISE(ObjectOfSameAliasAlreadyExists, "Object with alias '" + aChild->getAlias() + "' already exists as child of '" + getPath() + "'");
  }  

  mObjectsChart.insert(std::make_pair(aAlias, aChild));
  mChildAliases.insert(aChild->getAlias());
}


void ObjectView::addObj(LeafObject* aChild)
{
  this->addObj(aChild, aChild->getId());
}


// Object::iterator Methods Implementation

Object::iterator::iterator() : mBegin()
{
}


Object::iterator::iterator(Object& aBegin) : mBegin(&aBegin)
{
}


Object::iterator::iterator(const iterator& aOrig) : mBegin(aOrig.mBegin), mItStack(aOrig.mItStack)
{
}


Object::iterator::~iterator()
{
}


LeafObject& Object::iterator::value() const
{
  return ( mItStack.size()) ? ( * mItStack[0]->first) : (*mBegin);
}


LeafObject& Object::iterator::operator*() const
{
  return value();
}


LeafObject* Object::iterator::operator->() const
{
  return & (value());
}


bool Object::iterator::next()
{
  // Null iterator can't be incremented...
  if (!mBegin) {
    return false;
  }

  if (!mItStack.size()) {
    //We have just started and have no stack...
    if (mBegin->mChildren.size()) {
      //We have children so recurse down to them
      mItStack.push_front(mBegin->mChildren.begin());
      return true;
    }

    //We have no children so we are at the end of the iteration. Make Buffer NULL to stop infinite loop
    mBegin = NULL;
    return false;
  }

  //We are already in the tree...
  if (Object* lObj = dynamic_cast<Object*>(mItStack[0]->first)) {
    if (lObj->mChildren.size()) {
      // Entry has children, recurse...
      mItStack.push_front(lObj->mChildren.begin());
      return true;
    }
  }

  // No children so go to the next entry on this level
  while (mItStack.size()) {
    if (++(mItStack[0]) != ((mItStack.size() == 1) ? (mBegin) : ((Object*) mItStack[1]->first))->mChildren.end()) {
      // Next entry on this level is valid - return
      return true;
    }

    // No more valid entries in this level, go back up tree
    mItStack.pop_front();
  }

  //We have no more children so we are at the end of the iteration. Make Buffer NULL to stop infinite loop
  mBegin = NULL;
  return false;
}


Object::iterator& Object::iterator::operator++()
{
  next();
  return *this;
}


Object::iterator Object::iterator::operator++(int)
{
  Object::iterator lTemp(*this);
  next();
  return lTemp;
}


bool Object::iterator::operator!=(const Object::iterator& aIt) const
{
  return !(*this == aIt);
}


bool Object::iterator::operator==(const Object::iterator& aIt) const
{
  return ( aIt.mBegin == mBegin) && (aIt.mItStack == mItStack);
}


std::ostream& operator<<(std::ostream& aStr, const swatch::core::Object& aObject)
{
  aObject.print(aStr);
  return aStr;
}

}
}


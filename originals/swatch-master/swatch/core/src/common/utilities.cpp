#include "swatch/core/utilities.hpp"


// Standard headers
#include <cstdarg>
#include <cxxabi.h>
#include <iterator>
#include <set>
#include <sstream>
#include <stdlib.h>
#include <stddef.h>
#include <stdexcept>
#include <stdio.h>
#include <time.h>
#include <wordexp.h>

// boost headers
#include <boost/range/algorithm/copy.hpp>
#include <boost/range/adaptor/map.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lexical_cast.hpp>

// SWATCH headers
#include "swatch/core/Object.hpp"


namespace swatch {
namespace core {


//-----------------------------------------------------------------------------
std::string demangleName(const char* aMangledName)
{
  int lStatus = 0;
  char* lDemangled = abi::__cxa_demangle(aMangledName, 0, 0, &lStatus);
  const std::string lName(lStatus == 0 ? lDemangled : "[UNKNOWN]");

  if ((lStatus == 0) and ( NULL != lDemangled)) {
    free(lDemangled);
  }

  return lName;
}

//-----------------------------------------------------------------------------
std::string strPrintf(const char* aFmt, ...)
{
  char* lRet;
  va_list lAP;
  va_start(lAP, aFmt);
  vasprintf(&lRet, aFmt, lAP);
  va_end(lAP);
  std::string lStr(lRet);
  free(lRet);
  return lStr;
}


//-----------------------------------------------------------------------------
void millisleep(const double& aMillisec)
{
  //  using namespace uhal;
  //  logging();
  double lSecs(aMillisec / 1e3);
  int lIntPart((int) lSecs);
  double lFracPart(lSecs - (double) lIntPart);
  struct timespec lSleepTime, lReturnTime;
  lSleepTime.tv_sec = lIntPart;
  lSleepTime.tv_nsec = (long) (lFracPart * 1e9);
  //  log ( Notice() , "Sleeping " , Integer ( uint32_t ( sleepTime.tv_sec ) ) , "s " , Integer ( uint32_t ( sleepTime.tv_nsec ) ) , "ns" );
  nanosleep(&lSleepTime, &lReturnTime);
}

//-----------------------------------------------------------------------------
std::vector<std::string> shellExpandPaths(const std::string& aPath)
{
  if ( aPath.empty() )
    return std::vector<std::string>();

  std::vector<std::string> lPaths;
  wordexp_t lSubstitutedPath;
  int lCode = wordexp(aPath.c_str(), &lSubstitutedPath, WRDE_NOCMD);

  if (lCode) {
    XCEPT_RAISE(RuntimeError,"Failed expanding path: " + aPath);
  }

  for (std::size_t i = 0; i != lSubstitutedPath.we_wordc; i++) {
    lPaths.push_back(lSubstitutedPath.we_wordv[i]);
  }

  wordfree(&lSubstitutedPath);
  return lPaths;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::string shellExpandPath(const std::string& aPath)
{
  std::vector<std::string> lPaths = shellExpandPaths(aPath);

  if (lPaths.size() > 1) {
    XCEPT_RAISE(RuntimeError,"Failed to expand: multiple matches found");
  }
  else if ( lPaths.empty() ) {
    return "";
  }

  return lPaths[0];
}
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
std::string join(const std::vector<std::string>& aStrings, const std::string& aDelimiter)
{
  if ( aStrings.empty() ) return "";

  std::ostringstream lString;

  lString << *aStrings.begin();

  for(auto iStr = std::next(aStrings.begin()); iStr != aStrings.end(); ++iStr) {
    lString << aDelimiter; 
    lString << *iStr;
  }

  return lString.str();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template <typename T>
std::string vecFmt(const std::vector<T>& aVec)
{
  std::ostringstream lOss;
  lOss << "[";

  for (typename std::vector<T>::const_iterator it=aVec.begin(); it != aVec.end(); it++)
    lOss << *it << ",";
  lOss.seekp(lOss.tellp()-1l);
  lOss << "]";

  return lOss.str();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template <typename T>
std::string shortVecFmt(const std::vector<T>& aVec)
{
  if (aVec.size() == 0)
    return "[]";
  else if (aVec.size() == 1)
    return "[" + boost::lexical_cast<std::string>(aVec.at(0)) + "]";

  std::ostringstream lOss;
  lOss << "[";

  T lBegin = aVec.at(0);
  T lEnd   = lBegin;
  for (typename std::vector<T>::const_iterator it=aVec.begin()+1; it != aVec.end(); it++) {
    if ((*it) == (lEnd + 1)) {
      lEnd = *it;
      continue;
    }

    if (lBegin == lEnd)
      lOss << lBegin << ",";
    else
      lOss << lBegin << "-" << lEnd << ",";

    lBegin = *it;
    lEnd = *it;
  }

  if (lBegin == lEnd)
    lOss << lBegin << ",";
  else
    lOss << lBegin << "-" << lEnd << ",";

  // Replace final "," with a "]"
  lOss.seekp(lOss.tellp()-1l);
  lOss << "]";

  return lOss.str();
}
//-----------------------------------------------------------------------------

template std::string vecFmt<uint32_t>(const std::vector<uint32_t>& aVec);
template std::string shortVecFmt<uint32_t>(const std::vector<uint32_t>& aVec);

} // namespace core
} // namespace swatch

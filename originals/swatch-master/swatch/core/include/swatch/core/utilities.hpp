/**
   @file    Utilities.hpp
   @author  Alessandro Thea
   @date    22/07/14
 */

#ifndef __SWATCH_CORE_UTILITIES_HPP__
#define __SWATCH_CORE_UTILITIES_HPP__


// Standard headers
#include <set>                          // for set
#include <string>
#include <vector>

// SWATCH headers
#include "swatch/core/exception.hpp"


namespace swatch {
namespace core {


class LeafObject;
class Object;


/**
  Back-convert GCC-mangled names to class names
  @param aMangledName a GCC-mangled name
  @return the original class names
 */
std::string demangleName(const char* aMangledName);

/**
  Formats a std::string in printf fashion
  @param aFmt Format string
  @param ... List of arguments
  @return A formatted string
 */
std::string strPrintf(const char* aFmt, ...);

/**
  Sleeps for time in millisecs
  @param aMillisec a time in millisecs for which to sleep
 */
void millisleep(const double& aMillisec);

/**
  Expand a shell expression (stars, environment variables, etc) to a list of file paths
  @param aPath a shell expression to expand
  @return a list of all matching file paths, throw if no results found
 */
std::vector<std::string> shellExpandPaths(const std::string& aPath);


/**
  Expand a shell expression (stars, environment variables, etc) to a file path
  @param aPath a shell expression to expand
  @return a file path, throw if no results or more than one result found
 */
std::string shellExpandPath(const std::string& aPath);


/**
 * @brief      Converts a vector of strings in a delimiter-separated string.
 *
 * @param[in]  aStrings    Collection of strings
 * @param[in]  aDelimiter  A delimiter
 *
 * @return     { description_of_the_return_value }
 */
// See: boost::algorithm::join(fileNames, ",  ");
std::string join(const std::vector<std::string>& aStrings, const std::string& aDelimiter = " ");


/**
 * @brief      Converts an iterable collection is a delimiter-separated string
 *
 * @param[in]  aCollection  The collection
 * @param[in]  aDelimiter   A delimiter!
 *
 * @tparam     Collection   Type of the collection
 *
 * @return     the string
 */
template<typename Collection>
std::string joinAny(const Collection& aCollection, const std::string& aDelimiter = " ") {
  if ( aCollection.empty() ) return "";
  std::ostringstream lString;

  lString << *aCollection.begin();

  for(auto iStr = std::next(aCollection.begin()); iStr != aCollection.end(); ++iStr) {
    lString << aDelimiter; 
    lString << *iStr;
  }

  return lString.str();
}


/**
 * @brief      Converts a standard vector into a string representation
 *
 * @param[in]  aVec  The vector
 *
 * @tparam     T     The vector parameter type
 *
 * @return     The string
 */
template<typename T>
extern std::string vecFmt(const std::vector<T>& aVec);

/**
 * @brief      Converts a standard vector into a compact string representation
 *
 * @param[in]  aVec  The vector
 *
 * @tparam     T     The vector parameter type
 *
 * @return     The string
 */
template<typename T>
extern std::string shortVecFmt(const std::vector<T>& aVec);


} // namespace core
} // namespace swatch


#endif /* __SWATCH_CORE_UTILITIES_HPP__ */


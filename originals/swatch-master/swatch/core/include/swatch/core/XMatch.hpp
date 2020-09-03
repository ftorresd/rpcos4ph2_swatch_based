#ifndef __SWATCH_CORE_XMATCH_HPP__
#define __SWATCH_CORE_XMATCH_HPP__

#include <string>
#include <ostream>

namespace swatch {
namespace core {

/**
 * @brief      Class for x match.
 */
class XMatch {
public:


  /**
   * @brief      The constructor
   *
   * @param[in]  ok       Matching value
   * @param[in]  details  Details, optionals
   */
  XMatch(bool ok, std::string details="");

  bool ok;

  std::string details;
  
  /**
   * @brief      Not equal operator
   *
   * @param[in]  right  Object to be compared to
   *
   * @return     Result of the comparison
   */
  bool operator!=(const XMatch& right) const;

  /**
   * @brief      Equal operator
   *
   * @param[in]  right  Object to be compared to
   *
   * @return     Result of the comparison
   */
  bool operator==(const XMatch& right) const;

  /**
   * @brief      Streamer
   *
   * @param      os    The output streramer
   * @param[in]  obj   The object
   *
   * @return     { description_of_the_return_value }
   */
  friend std::ostream& operator<<(std::ostream& os, const XMatch& obj);

};

} // namespace core
} // namespace swatch

#endif /* __SWATCH_CORE_XMATCH_HPP__ */
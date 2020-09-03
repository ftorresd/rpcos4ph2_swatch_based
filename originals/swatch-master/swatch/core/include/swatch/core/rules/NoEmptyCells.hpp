#ifndef __SWATCH_CORE_RULES_NOEMPTYCELLS_HPP__
#define __SWATCH_CORE_RULES_NOEMPTYCELLS_HPP__


// XDAQ headers
#include "xdata/Table.h"

// SWATCH headers
#include "swatch/core/XRule.hpp"


namespace swatch {
namespace core {
namespace rules {


// ----------------------------------------------------------------------------

//! Class for no empty table cell rule.
class NoEmptyCells : public XRule<xdata::Table> {
public:
  
  NoEmptyCells() {};
  virtual ~NoEmptyCells() {};

    /**
   * @brief      Checks if there are no empty cells in user-supplied table
   *
   * @param[in]  aValue  The table to check
   *
   * @return     True if there are no empty cells in the table; false otherwise
   */
  virtual XMatch verify (const xdata::Table& aValue ) const;

private:

  virtual void describe(std::ostream& aStream) const;

  static const size_t kMaxColShown;

};
// ----------------------------------------------------------------------------


} // namespace rules
} // namespace core
} // namespace swatch

#endif /* __SWATCH_CORE_RULES_NOEMPTYCELLS_HPP__ */
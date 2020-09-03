
#ifndef __SWATCHCELL_FRAMEWORK_TOOLS_PANELFUNCTIONS_H__
#define __SWATCHCELL_FRAMEWORK_TOOLS_PANELFUNCTIONS_H__


#include <string>

#include "swatch/action/ActionableObject.hpp"
#include "swatch/core/StatusFlag.hpp"
#include "swatch/action/CommandSequence.hpp"
#include "swatch/action/Command.hpp"


namespace ajax{
  class Container;
  class Table;
  class Div;
}

namespace swatch{

namespace core{
  class AbstractMetric;
  class MonitorableObject;
  class MetricView;
}

namespace system {
  class Crate;
  class System;
}

namespace dtm {
class DaqTTCManager;
}

namespace processor {
  class Processor;
}

}


namespace swatchcellframework
{

   void addTextToCell( ajax::Table& aTable, const std::string& aColumn, uint32_t aRow, const std::string& aText, const std::string& style="");

  void addActionProgressToCell(ajax::Table& aTable, const std::string& aCol, uint32_t aRow, const swatch::action::ActionSnapshot& aStatus, const std::string& aStyle="");

  void addCmdSeqBoxToCell(ajax::Table& table, const std::string& column, uint32_t row, swatch::action::CommandSequence::State aStatue, const std::string& style="");

  void addCmdBoxToCell(ajax::Table& table, const std::string& column, uint32_t row, swatch::action::Command::State aStatue, const std::string& style="");

  //! Generates add a table displaying the metrics within a (regex-based) metric view
  void addMetricViewTable(swatch::core::MetricView& aView, ajax::Container& container);

  //! Adds an ajax::PlainHtml widget to the container, and returns the PlainHtml widget's input stream (widget is owned by the container)
  std::ostringstream& addPlainHtml(ajax::Container& container);

  //! Adds a div element to container that serves as a button to hide/show the element identified by hideId
  void addToggleSummary(ajax::Container& container, const std::string& hideId, const std::string& text, swatch::core::StatusFlag status);

  //! Adds a div element displaying a crate with its contents
  void addCrateDisplay(ajax::Container& container, const std::string& crateId, swatch::system::Crate& crate);

  //! Adds a compact div element displaying the status of a processor
  void addProcessorDisplay(ajax::Div& container, swatch::processor::Processor& proc, const std::string& aOverId, int32_t aSlotId, uint32_t aSlotHeight);

  //! Adds a more sophisticated div element displaying the status of a processor
  void addProcessorDetailDisplay(ajax::Div& container, const swatch::processor::Processor& proc);
  
  //! Add a compact div element displaying the status of an daqttcmanager
  void addDaqTTCManagerDisplay(ajax::Div& container, swatch::dtm::DaqTTCManager& proc, const std::string& aOverId, int32_t aSlotId, uint32_t aSlotHeight);

  void fillTableCell(ajax::Table& table, const std::string& column, uint32_t row, const std::string& text, bool center=true);

  //! Fills table cell with text and adds an id to the cell
  void fillTableCell(ajax::Table& table, const std::string& column, uint32_t row, const std::string& text, const std::string& id, bool center=true);

  void fillTableCell(ajax::Table& table, const std::string& column, uint32_t row, swatch::core::StatusFlag status);

  void fillTableCell(ajax::Table& table, const std::string& column, uint32_t row, swatch::core::AbstractMetric& metric);
  //! add background-colour to style; colour corresponding to status
  void addBackgroundColor(std::ostringstream& style, const swatch::core::StatusFlag& status);

  //! add text-colour to style; colour corresponding to status
  void addForegroundColor(std::ostringstream& style, const swatch::core::StatusFlag& status);

  //! add a mouse-over: mouseOverMessage appears in mouseOverId element, when hovering over element. OnMouseOut resets to defaultMessage
  void addMouseOver(ajax::Div& element, const std::string& mouseOverId, const std::string& mouseOverMessage, const std::string& defaultMessage="");

  //! adds click event to element: browser scrolls to the element with id idToMoveTo
  void addMoveToElement(ajax::Div& element, const std::string& idToMoveTo);

} // end ns swatchcellframework


#endif /* __SWATCHCELL_FRAMEWORK_TOOLS_PANELFUNCTIONS_H__ */


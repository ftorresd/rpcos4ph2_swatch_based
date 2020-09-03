#include "swatchcell/framework/tools/panelFunctions.h"

// SWATCH headers
#include "swatch/core/MonitorableObject.hpp"
#include "swatch/core/MetricView.hpp"
#include "swatch/system/Crate.hpp"
#include "swatch/processor/Processor.hpp"
#include "swatch/processor/Port.hpp"
#include "swatch/processor/PortCollection.hpp"
#include "swatch/processor/TTCInterface.hpp"
#include "swatch/dtm/DaqTTCManager.hpp"
#include "swatch/system/System.hpp"

// ajaxell headers
#include "ajax/Container.h"
#include "ajax/Table.h"
#include "ajax/PlainHtml.h"
#include "ajax/Div.h"

#include <sstream>

namespace swatchcellframework {

// --------------------------------------------------------
void addTextToCell(ajax::Table& aTable, const std::string& aColumn, uint32_t aRow, const std::string& aText, const std::string& aStyle) {
  ajax::Div* lDiv = new ajax::Div();

  if (aStyle.size())
    lDiv->set("style", aStyle);

  ajax::PlainHtml* lText = new ajax::PlainHtml();
  lText->getStream() << aText;

  lDiv->add(lText);

  aTable.setWidgetAt(aColumn, aRow, lDiv);
}

// --------------------------------------------------------
void addActionProgressToCell(ajax::Table& aTable, const std::string& aCol, uint32_t aRow, const swatch::action::ActionSnapshot& aStatus, const std::string& aStyle) {

  std::string lColor;
  swatch::action::ActionSnapshot::State lState = aStatus.getState();
  if (lState == swatch::action::Functionoid::kError)
    lColor = "#FF0000";
  else if (lState == swatch::action::Functionoid::kWarning)
    lColor = "#FFCC00";
  else if (lState == swatch::action::Functionoid::kDone)
    lColor = "#00CC00";
  else if (lState == swatch::action::Functionoid::kRunning)
    lColor = "#3399FF";
  // else
  //   lColor << "#000000";

  std::string lStyle = (!lColor.empty() ? "background-color: " + lColor + "; " + aStyle : aStyle);
  std::ostringstream oss;
  oss << lState;
  if ((lState == swatch::action::Functionoid::kRunning) || (lState == swatch::action::Functionoid::kError))
    oss << " (" << std::setprecision(4) << aStatus.getProgress()*100.0 << "%)";

  addTextToCell(aTable, aCol, aRow, oss.str(), lStyle);
}


// --------------------------------------------------------
void addCmdSeqBoxToCell(ajax::Table& aTable, const std::string& aCol, uint32_t aRow, swatch::action::CommandSequence::State aState, const std::string& aStyle) {

  std::string lColor;

  if (aState == swatch::action::Functionoid::kError)
    lColor = "#FF0000";
  else if (aState == swatch::action::Functionoid::kWarning)
    lColor = "#FFCC00";
  else if (aState == swatch::action::Functionoid::kDone)
    lColor = "#00CC00";
  else if (aState == swatch::action::Functionoid::kRunning)
    lColor = "#3399FF";
  // else
  //   lColor << "#000000";

  std::string lStyle = (!lColor.empty() ? "background-color: " + lColor + "; " + aStyle : aStyle);
  addTextToCell(aTable, aCol, aRow, boost::lexical_cast<std::string>(aState), lStyle);

}


// --------------------------------------------------------
void addCmdBoxToCell(ajax::Table& aTable, const std::string& aCol, uint32_t aRow, swatch::action::ActionSnapshot::State aState, const std::string& aStyle) {

  std::string lColor;

  if (aState == swatch::action::Functionoid::kError)
    lColor = "#FF0000";
  else if (aState == swatch::action::Functionoid::kWarning)
    lColor = "#FFCC00";
  else if (aState == swatch::action::Functionoid::kDone)
    lColor = "#00CC00";
  else if (aState == swatch::action::Functionoid::kRunning)
    lColor = "#3399FF";
  // else
  //   lColor << "#000000";

  std::string lStyle = (!lColor.empty() ? "background-color: " + lColor + "; " + aStyle : aStyle);
  addTextToCell(aTable, aCol, aRow, boost::lexical_cast<std::string>(aState), lStyle);

}



void addToggleSummary(ajax::Container& container, const std::string& hideId, const std::string& text, swatch::core::StatusFlag status) {
  ajax::Div* div = new ajax::Div();

  std::stringstream onclick;

  onclick << "if(getElementById('" << hideId << "').style.display=='block')";
  onclick << "{ getElementById('" << hideId << "').style.display='none' }";
  onclick << "else { getElementById('" << hideId << "').style.display='block'}";
  div->set("onclick", onclick.str());

  std::ostringstream style;
  style << "cursor:pointer;border:1px solid black;margin:2px 0px;padding:3px;";
  addBackgroundColor(style, status);

  div->set("style", style.str());

  addPlainHtml(*div) << "<b>" << text << "</b> - overall status: " << status;

  container.add(div);
}


void addMetricViewTable(swatch::core::MetricView& aView, ajax::Container& aContainer) {
  std::string colNameContainerId("Object ID");
  std::string colNameMetricId("Metric ID");
  std::string colNameMetricStatus("Status");
  std::string colNameMetricValue("Value");
  std::string colNameMetricErrorCondition("Error condition");
  std::string colNameMetricWarnCondition("Warning condition");
  std::string colNameUpdateTime("Update time (UTC)");

  ajax::Table* table = new ajax::Table();
  table->setShowHeader(true);
  aContainer.add(table);

  table->addColumn(colNameContainerId, ajax::Table::String);
  table->addColumn(colNameMetricId, ajax::Table::String);
  table->addColumn(colNameMetricValue, ajax::Table::String);
  table->addColumn(colNameMetricStatus, ajax::Table::String);
  table->addColumn(colNameMetricErrorCondition, ajax::Table::String);
  table->addColumn(colNameMetricWarnCondition, ajax::Table::String);
  table->addColumn(colNameUpdateTime, ajax::Table::String);

  size_t rowIdx = 0;

  std::string previousContainerId;
  for (swatch::core::MetricView::iterator it = aView.begin(); it != aView.end(); it++, rowIdx++) {
    swatch::core::AbstractMetric& metric = *(it->second);

    size_t pathSplitIdx = it->first.rfind(".");
    std::string containerId = it->first.substr(0, pathSplitIdx);
    std::string metricId = it->first.substr(pathSplitIdx + 1);

    if (previousContainerId != containerId)
      fillTableCell(*table, colNameContainerId, rowIdx, containerId, false);
    fillTableCell(*table, colNameMetricId, rowIdx, metricId, false);

    swatch::core::MetricSnapshot mSnapshot = metric.getSnapshot();

    // 1) Value & status
    fillTableCell(*table, colNameMetricValue, rowIdx, (mSnapshot.isValueKnown() ? mSnapshot.getValueAsString() : "Unknown"));
    fillTableCell(*table, colNameMetricStatus, rowIdx, mSnapshot.getStatusFlag());

    // 2) Error & warning conditions
    fillTableCell(*table, colNameMetricErrorCondition, rowIdx, mSnapshot.getErrorCondition() ? boost::lexical_cast<std::string>(*mSnapshot.getErrorCondition()) : "N/A");
    fillTableCell(*table, colNameMetricWarnCondition, rowIdx, mSnapshot.getWarningCondition() ? boost::lexical_cast<std::string>(*mSnapshot.getWarningCondition()) : "N/A");

    // 3) Timestamp
    //FIXME: Encapsulate this formatting in function and/or move from timeval to boost::posix_time::ptime
    time_t updateTime = boost::chrono::system_clock::to_time_t(mSnapshot.getUpdateTimestamp().system);
    char timeText[21];
    strftime(timeText, sizeof timeText, "%Y-%m-%d  %H:%M:%S", gmtime(&updateTime));
    std::ostringstream oss;
    oss << timeText;
    oss << "." << (boost::chrono::duration_cast<boost::chrono::microseconds>(mSnapshot.getUpdateTimestamp().system.time_since_epoch())
                   - boost::chrono::duration_cast<boost::chrono::seconds>(mSnapshot.getUpdateTimestamp().system.time_since_epoch())).count();
    //oss << "." << std::setw(6) << std::fill('0') << mSnapshot.getUpdateTimestamp().tv_usec;
    //oss << "." << std::setw(6) << std::fill('0') << mSnapshot.getUpdateTimestamp().tv_usec / 1000;
    fillTableCell(*table, colNameUpdateTime, rowIdx, oss.str(), false);

    previousContainerId = containerId;
  }
}


// --------------------------------------------------------
std::ostringstream& addPlainHtml(ajax::Container& container) {
  ajax::PlainHtml* html = new ajax::PlainHtml();
  container.add(html);
  return html->getStream();
}


// --------------------------------------------------------
void addBackgroundColor(std::ostringstream& style, const swatch::core::StatusFlag& status) {
  style << "background-color:";
  if (status == swatch::core::kError) {
    style << "#fc9f9f;";
  } else if (status == swatch::core::kWarning) {
    style << "#fcf99f;";
  } else if (status == swatch::core::kGood) {
    style << "#c8fc9f;";
  } else if (status == swatch::core::kUnknown) {
    style << "#ddd;";
  } else {
    style << "#fff";
  }
}


// --------------------------------------------------------
void addForegroundColor(std::ostringstream& style, const swatch::core::StatusFlag& status) {
  style << "color:";
  if (status == swatch::core::kError) {
    style << "#f00;";
  } else if (status == swatch::core::kWarning) {
    style << "#f60;";
  } else if (status == swatch::core::kGood) {
    style << "#0f0;";
  } else if (status == swatch::core::kUnknown) {
    style << "#888;";
  } else {
    style << "#000;";
  }
}


// --------------------------------------------------------
void addMouseOver(ajax::Div& element, const std::string& mouseOverId, const std::string& mouseOverMessage, const std::string& defaultMessage) {
  std::ostringstream mOver;
  std::ostringstream mOut;
  mOver << "getElementById('" << mouseOverId << "').innerHTML='" << mouseOverMessage << "';";
  mOut << "getElementById('" << mouseOverId << "').innerHTML='" << defaultMessage << "';";
  element.set("onmouseover", mOver.str());
  element.set("onmouseout", mOut.str());
}


// --------------------------------------------------------
void addMoveToElement(ajax::Div& element, const std::string& idToMoveTo) {
  std::ostringstream onclick;
  onclick << "getElementById('" << idToMoveTo << "').scrollIntoView();";
  element.set("onclick", onclick.str());

}


// --------------------------------------------------------
void fillTableCell(ajax::Table& table, const std::string& column, uint32_t row, const std::string& text, bool center) {
  std::string tag(center ? "center" : "div");
  ajax::PlainHtml* html = new ajax::PlainHtml();
  table.setWidgetAt(column, row, html);

  html->getStream() << "<" << tag << " style=\"color:#000000; font-size: 14px;\">" << text << "</" << tag << ">";
}


// --------------------------------------------------------
void fillTableCell(ajax::Table& table, const std::string& column, uint32_t row, swatch::core::StatusFlag status) {
  ajax::PlainHtml* html = new ajax::PlainHtml();
  table.setWidgetAt(column, row, html);

  // 1) center open tag, with text colour set based on status flag
  html->getStream() << "<center style=\"color:";
  if (status == swatch::core::kError)
    html->getStream() << "#FF0000";
  else if (status == swatch::core::kWarning)
    html->getStream() << "#FF6600";
  else if (status == swatch::core::kGood)
    html->getStream() << "#00FF00";
  else
    html->getStream() << "#000000";
  html->getStream() << "; font-size: 14px;\"><b>";

  // 2) add contents and center close tag
  html->getStream() << status << "</b></center>";
}


// --------------------------------------------------------
void fillTableCell(ajax::Table& table, const std::string& column, uint32_t row, swatch::core::AbstractMetric& metric) {
  ajax::PlainHtml* html = new ajax::PlainHtml();
  table.setWidgetAt(column, row, html);

  swatch::core::MetricSnapshot mSnapshot = metric.getSnapshot();

  // 1) center open tag, with text colour set based on status flag
  html->getStream() << "<center style=\"color:";
  if (mSnapshot.getStatusFlag() == swatch::core::kError)
    html->getStream() << "#FF0000";
  else if (mSnapshot.getStatusFlag() == swatch::core::kGood)
    html->getStream() << "#00FF00";
  else
    html->getStream() << "#000000";
  html->getStream() << "; font-size: 14px;\">";

  // 2) add contents and center close tag
  if (!mSnapshot.isValueKnown())
    html->getStream() << "UNKNOWN";
  else
    html->getStream() << mSnapshot.getValueAsString();

  html->getStream() << "</center>";

}


// --------------------------------------------------------
void fillTableCell(ajax::Table& table, const std::string& column, uint32_t row, const std::string& text, const std::string& id, bool center) {
  std::string tag(center ? "center" : "div");
  ajax::PlainHtml* html = new ajax::PlainHtml();
  table.setWidgetAt(column, row, html);

  html->getStream() << "<" << tag << " style=\"color:#000000; font-size: 14px;\" id=\"" << id << "\">" << text << "</" << tag << ">";
}


// --------------------------------------------------------
void addCrateDisplay(ajax::Container& container, const std::string& crateId, swatch::system::Crate& crate)
{
  const std::vector<std::string>& procSubIds(swatch::processor::Processor::kDefaultMonitorableObjects);
  const std::vector<std::string>& dtmSubIds(swatch::dtm::DaqTTCManager::kDefaultMonitorableObjects);
  int32_t nSubIds = std::max(procSubIds.size(), dtmSubIds.size());
  int32_t slotHeight = 50 + 16 * nSubIds + 20; // overallStatus-height + (subId-eight+1)*nSubIds + slotIndicator-height
  int32_t dummyHeight = 50 + 16 * nSubIds;
  swatch::dtm::DaqTTCManager* amc13 = crate.amc13();

  std::ostringstream fedId;
  if (amc13) {
    fedId << boost::lexical_cast<uint32_t>(amc13->getFedId());
  } else {
    fedId << "None";
  }

  ajax::Div* div = new ajax::Div();
  // Header
  addPlainHtml(*div) << "Crate: " << crateId << " (FED-ID: " << fedId.str() << ")<br>";
  // Define empty slot style
  std::ostringstream emptyCardStyle;
  emptyCardStyle << "width:15px;height:" << boost::lexical_cast<int32_t>(dummyHeight) << "px;border:1px solid gray;word-wrap:break-word;color:black;font-size:12px;";
  // Slot style
  std::ostringstream slot;
  slot << "width:15px;height:" << slotHeight << "px;word-wrap:break-word;float:left;margin:5px 2px 1px;font-size:9px;color:gray;";
  // What is this?
  emptyCardStyle << "width:15px;height:" << boost::lexical_cast<int32_t>(dummyHeight) << "px;border:1px solid gray;word-wrap:break-word;color:black;font-size:12px;";
  ajax::Div* slotsDiv = new ajax::Div();

  for (uint32_t i = 1; i <= 12; ++i) {
    if (crate.isAMCSlotTaken(i)) {
      // Insert the processor details in the slot box
      addProcessorDisplay(*slotsDiv, *(crate.amc(i)), crateId, i, slotHeight);
    } else {
      // or make it empty
      ajax::Div* pDiv = new ajax::Div();
      ajax::Div* slotDiv = new ajax::Div();
      addMouseOver(*pDiv, crateId, "Empty");
      pDiv->set("style", emptyCardStyle.str());
      slotDiv->set("style", slot.str());
      slotDiv->add(pDiv);
      addPlainHtml(*slotDiv) << boost::lexical_cast<uint32_t>(i);
      slotsDiv->add(slotDiv);
    }
  }

  if ( crate.amc13() ) {
    addDaqTTCManagerDisplay(*slotsDiv, *(crate.amc13()), crateId, 13, slotHeight);
  } else {
    // or make it empty
    ajax::Div* pDiv = new ajax::Div();
    ajax::Div* slotDiv = new ajax::Div();
    addMouseOver(*pDiv, crateId, "Empty");
    pDiv->set("style", emptyCardStyle.str());
    slotDiv->set("style", slot.str());
    slotDiv->add(pDiv);
    addPlainHtml(*slotDiv) << boost::lexical_cast<uint32_t>(13);
    slotsDiv->add(slotDiv);
  }
  
  div->add(slotsDiv);
  div->set("style", "margin:10px 15px;border:1px solid #bbb;padding:5px;display:inline-block;vertical-align:top;");
  ajax::Div* descDiv = new ajax::Div();
  descDiv->setId(crateId);
  descDiv->set("style", "height:20px;width:200px;clear:both;");
  div->add(descDiv);
  container.add(div);
}


// --------------------------------------------------------
void addProcessorDisplay(ajax::Div& container, swatch::processor::Processor& proc, const std::string& mOverId, int32_t slotId, uint32_t aSlotHeight) {
  
  const std::vector<std::string>& procSubIds(swatch::processor::Processor::kDefaultMonitorableObjects);
//  int32_t aSlotHeight = 50 + 16 * nSubIds + 20; // overallStatus-height + (subId-eight+1)*nSubIds + slotIndicator-height

  // Compute the overall status block height
  int32_t lOverallHeight = aSlotHeight - (16 * procSubIds.size() + 20);

  std::string overallStatusStyle("width:15px;height:"+boost::lexical_cast<std::string>(lOverallHeight)+"px;border:1px solid gray;word-wrap:break-word;color:black;font-size:12px;");
  std::string subIdBaseStyle("width:15px;height:15px;border:1px solid gray;border-top:0;word-wrap:break-word;");
  std::ostringstream slot;
  slot << "width:15px;height:" << boost::lexical_cast<int32_t>(aSlotHeight) << "px;word-wrap:break-word;float:left;margin:5px 2px 1px;font-size:9px;color:gray;";

  ajax::Div* slotDiv = new ajax::Div();
  ajax::Div* pDiv = new ajax::Div();

  swatch::core::StatusFlag status = proc.getStatusFlag();
  std::ostringstream style;
  slotDiv->set("style", slot.str());
  style << overallStatusStyle;
  addBackgroundColor(style, status);
  pDiv->set("style", style.str());
  addMouseOver(*pDiv, mOverId, proc.getId());

  slotDiv->add(pDiv);

  for (std::vector<std::string>::const_iterator it = procSubIds.begin(); it != procSubIds.end(); it++) {
    swatch::core::StatusFlag subStat = proc.getObjPtr<swatch::core::MonitorableObject>(*it)->getStatusFlag();
    ajax::Div* subDiv = new ajax::Div();
    std::ostringstream subStyle;
    subStyle << subIdBaseStyle;
    addBackgroundColor(subStyle, subStat);
    std::ostringstream mOver;
    mOver << proc.getId() << " " << (*it);
    addMouseOver(*subDiv, mOverId, mOver.str());
    subDiv->set("style", subStyle.str());
    slotDiv->add(subDiv);
  }
  if (slotId >= 0) {
    addPlainHtml(*slotDiv) << boost::lexical_cast<uint32_t>(slotId);
  }
  container.add(slotDiv);
}


// --------------------------------------------------------
void addProcessorDetailDisplay(ajax::Div& container, const swatch::processor::Processor& proc)
{
  std::vector<std::string> procSubIds(swatch::processor::Processor::kDefaultMonitorableObjects);
  ajax::Div* inDiv = new ajax::Div();
  
  using namespace swatch::processor;
  const std::deque<const InputPort *>& inports = proc.getInputPorts().getPorts();

  std::string linkBaseStyle = "margin:2px;width:200px;border:1px solid gray;display:inline-block;clear:both;";
  std::string indLinkBaseStyle = "margin:1px;float:left;width:7px;height:7px;border:1px solid gray;";
  addPlainHtml(*inDiv) << "input ports<br>";
  for (InputPortCollection::const_iterator itIn = inports.begin(); itIn != inports.end(); ++itIn) {
    ajax::Div* iDiv = new ajax::Div();

    std::ostringstream iStyle;
    iStyle << indLinkBaseStyle;
    addBackgroundColor(iStyle, (*itIn)->getStatusFlag());
    iDiv->set("style", iStyle.str());
    addMouseOver(*iDiv, proc.getId(), (*itIn)->getId(), proc.getId());
    addMoveToElement(*iDiv, (*itIn)->getId());
    inDiv->add(iDiv);
  }
  std::ostringstream inStyle;
  inStyle << linkBaseStyle;
  addBackgroundColor(inStyle, proc.getInputPorts().getStatusFlag());
  inDiv->set("style", inStyle.str());

  ajax::Div* outDiv = new ajax::Div();
  const std::deque<const OutputPort *> outports = proc.getOutputPorts().getPorts();
  OutputPortCollection::const_iterator itOut;
  addPlainHtml(*outDiv) << "output ports<br>";
  for (itOut = outports.begin(); itOut != outports.end(); ++itOut) {
    ajax::Div* iDiv = new ajax::Div();
    std::ostringstream iStyle;
    iStyle << indLinkBaseStyle;
    addBackgroundColor(iStyle, (*itOut)->getStatusFlag());
    iDiv->set("style", iStyle.str());
    addMouseOver(*iDiv, proc.getId(), (*itOut)->getId(), proc.getId());
    addMoveToElement(*iDiv, (*itOut)->getId());
    outDiv->add(iDiv);
  }

  std::ostringstream outStyle;
  outStyle << linkBaseStyle;
  addBackgroundColor(outStyle, proc.getOutputPorts().getStatusFlag());
  outDiv->set("style", outStyle.str());

  std::string subBaseStyle = "margin:2px;padding:2px;border: 1px solid gray;float:left;padding:0px 5px;";
  std::ostringstream internalBaseStyle;
  internalBaseStyle << "margin:2px;border: 1px solid gray;width:200px;display:inline-block;clear:both;";
  ajax::Div* internalDiv = new ajax::Div();
  internalDiv->set("style", internalBaseStyle.str());

  for (std::vector<std::string>::const_iterator it = procSubIds.begin(); it != procSubIds.end(); it++) {
    swatch::core::StatusFlag subStat = proc.getObjPtr<swatch::core::MonitorableObject>(*it)->getStatusFlag();
    ajax::Div* subDiv = new ajax::Div();
    addPlainHtml(*subDiv) << *it;
    std::ostringstream subStyle;
    subStyle << subBaseStyle;
    addBackgroundColor(subStyle, subStat);
    subDiv->set("style", subStyle.str());
    internalDiv->add(subDiv);
  }
  std::ostringstream procStyle;
  procStyle << "float:left;margin:10px 10px;border:1px solid gray;padding:5px;width:210px;text-align:center;";
  swatch::core::StatusFlag procStat = proc.getStatusFlag();
  addBackgroundColor(procStyle, procStat);

  ajax::Div* processorDiv = new ajax::Div();
  ajax::Div* processorDescription = new ajax::Div();
  processorDescription->setId(proc.getId());
  addPlainHtml(*processorDescription) << proc.getId();
  processorDiv->set("style", procStyle.str());
  processorDiv->add(processorDescription);
  processorDiv->add(inDiv);
  processorDiv->add(internalDiv);
  processorDiv->add(outDiv);
  container.add(processorDiv);
}


// --------------------------------------------------------
void addDaqTTCManagerDisplay(ajax::Div& container, swatch::dtm::DaqTTCManager& dtm, const std::string& mOverId, int32_t slotId, uint32_t aSlotHeight) {
  
  std::vector<std::string> dtmSubIds(swatch::dtm::DaqTTCManager::kDefaultMonitorableObjects);

  // Compute the overall status block height
  int32_t lOverallHeight = aSlotHeight - (16 * dtmSubIds.size() + 20);
  
  std::string overallStatusStyle("width:15px;height:"+boost::lexical_cast<std::string>(lOverallHeight)+"px;border:1px solid gray;word-wrap:break-word;color:black;font-size:12px;");
  std::string subIdBaseStyle("width:15px;height:15px;border:1px solid gray;border-top:0;word-wrap:break-word;");
  std::ostringstream slot;
  slot << "width:15px;height:" << boost::lexical_cast<int32_t>(aSlotHeight) << "px;word-wrap:break-word;float:left;margin:5px 2px 1px;font-size:9px;color:gray;";

  ajax::Div* slotDiv = new ajax::Div();
  ajax::Div* pDiv = new ajax::Div();

  swatch::core::StatusFlag status = dtm.getStatusFlag();
  std::ostringstream style;
  slotDiv->set("style", slot.str());
  style << overallStatusStyle;
  addBackgroundColor(style, status);
  pDiv->set("style", style.str());
  addMouseOver(*pDiv, mOverId, dtm.getId());

  slotDiv->add(pDiv);

  for (std::vector<std::string>::const_iterator it = dtmSubIds.begin(); it != dtmSubIds.end(); it++) {
    swatch::core::StatusFlag subStat = dtm.getObjPtr<swatch::core::MonitorableObject>(*it)->getStatusFlag();
    ajax::Div* subDiv = new ajax::Div();
    std::ostringstream subStyle;
    subStyle << subIdBaseStyle;
    addBackgroundColor(subStyle, subStat);
    std::ostringstream mOver;
    mOver << dtm.getId() << " " << (*it);
    addMouseOver(*subDiv, mOverId, mOver.str());
    subDiv->set("style", subStyle.str());
    slotDiv->add(subDiv);
  }
  if (slotId >= 0) {
    addPlainHtml(*slotDiv) << boost::lexical_cast<uint32_t>(slotId);
  }
  container.add(slotDiv);
}

} // end ns swatchcellframework

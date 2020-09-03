#include "swatchcell/framework/ChartsPanel.h"
#include "swatchcell/framework/tools/polymerFunctions.h"

#include "boost/foreach.hpp"
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include "boost/regex.hpp"

#include "ajax/PolymerElement.h"
#include "ajax/toolbox.h"

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"

#include "swatch/action/Command.hpp"
#include "swatch/core/ReadWriteXParameterSet.hpp"
#include "swatch/system/Crate.hpp"
#include "swatch/dtm/DaqTTCManager.hpp"
#include "swatch/system/System.hpp"
#include "swatch/processor/Processor.hpp"
#include "swatch/action/ActionableSystem.hpp"
#include "swatch/action/GateKeeper.hpp"

#include "swatch/core/MetricView.hpp"

#include <sstream>

#include "jsoncpp/json/json.h"

namespace swatchcellframework
{

  ChartsPanel::ChartsPanel(tsframework::CellAbstractContext* context, log4cplus::Logger& logger)
  :
    CellPanel(context, logger),
    logger_(logger),
    cellContext_(dynamic_cast<swatchcellframework::CellContext&>(*context)),
    resultBox_(new ajax::ResultBox())
  {
    resultBox_->setIsOwned(false);

  }


  ChartsPanel::~ChartsPanel()
  {
    delete resultBox_;
  }

  /*
  ██       █████  ██    ██  ██████  ██    ██ ████████
  ██      ██   ██  ██  ██  ██    ██ ██    ██    ██
  ██      ███████   ████   ██    ██ ██    ██    ██
  ██      ██   ██    ██    ██    ██ ██    ██    ██
  ███████ ██   ██    ██     ██████   ██████     ██
  */

  void ChartsPanel::layout(cgicc::Cgicc& cgi)
  {
    this -> remove();

    setEvent("ChartsPanel::getMetricsList", ajax::Eventable::OnClick, this, &swatchcellframework::ChartsPanel::getMetricsList);
    setEvent("ChartsPanel::getMetricsValues", ajax::Eventable::OnTime, this, &swatchcellframework::ChartsPanel::getMetricsValues);

    ajax::PolymerElement* lChartsPanel = new ajax::PolymerElement("charts-panel");
    resultBox_->add(lChartsPanel);
    this->add(resultBox_);

  }

  /*
  ███    ███     ██    ██  █████  ██      ██    ██ ███████ ███████
  ████  ████     ██    ██ ██   ██ ██      ██    ██ ██      ██
  ██ ████ ██     ██    ██ ███████ ██      ██    ██ █████   ███████
  ██  ██  ██      ██  ██  ██   ██ ██      ██    ██ ██           ██
  ██      ██       ████   ██   ██ ███████  ██████  ███████ ███████
  */
  //for debug
  //static int x = 5;

  void ChartsPanel::getMetricsValues(cgicc::Cgicc& cgi, std::ostream& out)
  {

    std::map<std::string, std::string> lValues(ajax::toolbox::getSubmittedValues(cgi));

    std::stringstream lMetricsListSS;
    lMetricsListSS << lValues["metricsList"];

    if(lMetricsListSS.str() == "undefined") return;

    Json::Value lMetricsListJSON;
    lMetricsListSS >> lMetricsListJSON;

    CellContext::SharedGuard_t lGuard(cellContext_);
    swatch::system::System& lSystem = cellContext_.getSystem(lGuard);

    // Create metric view from regex, whilst protecting against exceptions from invalid regexes
    try {
      Json::Value lJsonOutput(Json::arrayValue);

      for (Json::ValueIterator jsonit = lMetricsListJSON.begin(); jsonit != lMetricsListJSON.end(); jsonit++){


        Json::Value lMetricJson(Json::objectValue);

        const std::string lQuery = jsonit -> asString();
        //I escape the string thanks to the power of love <3 so basically do not ask me how this stuff works :)
        const boost::regex lEsc("[.^$|()\\[\\]{}*+?\\\\]");
        const std::string lRep("\\\\&");
        std::string lResult;
        lResult = regex_replace(lQuery, lEsc, lRep, boost::match_default | boost::format_sed);

        swatch::core::MetricView lView(lSystem, lResult);
        serializeMetricView(lJsonOutput, lView);
      }
      out << lJsonOutput;
    }
    catch (const std::exception& lExc) {
      std::ostringstream error;
      error << "> ERROR encountered when creating metric view in charts panel.<br>Details below ...<br>" << lExc.what();
      LOG4CPLUS_INFO(this->getLogger(), error.str());
    }



  }

  /*
  ███    ███     ██      ██ ███████ ████████
  ████  ████     ██      ██ ██         ██
  ██ ████ ██     ██      ██ ███████    ██
  ██  ██  ██     ██      ██      ██    ██
  ██      ██     ███████ ██ ███████    ██
  */

  void ChartsPanel::getMetricsList(cgicc::Cgicc& cgi, std::ostream& out)
  {
    std::map<std::string, std::string> lValues(ajax::toolbox::getSubmittedValues(cgi));

    const std::string lQuery = lValues["query"];

    CellContext::SharedGuard_t lGuard(cellContext_);
    swatch::system::System& lSystem = cellContext_.getSystem(lGuard);

    // Create metric view from regex, whilst protecting against exceptions from invalid regexes
    try {

      swatch::core::MetricView lView(lSystem, lQuery);

      Json::Value lJsonOutput(Json::arrayValue);

      //I create here the metrics list

      auto it = lView.begin();
      auto itend = lView.end();

      for (; it != itend; ++it) {
        if(it -> second -> getStatus().second != swatch::core::monitoring::kDisabled) {
          bool plottable = false;

          if (dynamic_cast< swatch::core::Metric<bool>* >((it -> second))) plottable = true;
          else if (dynamic_cast< swatch::core::Metric<int>* >((it -> second))) plottable = true;
          else if (dynamic_cast< swatch::core::Metric<float>* >((it -> second))) plottable = true;
          else if (dynamic_cast< swatch::core::Metric<double>* >((it -> second))) plottable = true;
          else if (dynamic_cast< swatch::core::Metric<unsigned int>* >((it -> second))) plottable = true;
          else if (dynamic_cast< swatch::core::Metric<uint16_t>* >((it -> second))) plottable = true;
          else if (dynamic_cast< swatch::core::Metric<uint32_t>* >((it -> second))) plottable = true;
          else if (dynamic_cast< swatch::core::Metric<uint64_t>* >((it -> second))) plottable = true;

          Json::Value metric(Json::objectValue);
          metric["id"] = it -> first;
          metric["plottable"] = plottable;

          lJsonOutput.append(metric);
        }
      }

      out << lJsonOutput;
    }
    catch (const std::exception& lExc) {
      std::ostringstream error;
      error << "> ERROR encountered when creating metric view in charts panel from regex: " << lQuery << "<br>Details below ...<br>" << lExc.what();
      LOG4CPLUS_INFO(this->getLogger(), error.str());
    }

  }

}

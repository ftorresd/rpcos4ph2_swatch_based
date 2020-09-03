#include "swatchcell/framework/ExplorePanel.h"

#include "boost/foreach.hpp"
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

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

#include "jsoncpp/json/json.h"

namespace swatchcellframework
{

  ExplorePanel::ExplorePanel(tsframework::CellAbstractContext* context, log4cplus::Logger& logger)
  :
    CellPanel(context, logger),
    mLogger(logger),
    mCellContext(dynamic_cast<swatchcellframework::CellContext&>(*context))
  {


  }

  ExplorePanel::~ExplorePanel()
  {

  }

  void ExplorePanel::layout(cgicc::Cgicc& cgi)
  {
    this->remove();

    setEvent("ExplorePanel::getExploreTree", ajax::Eventable::OnClick, this, &ExplorePanel::getExploreTree);

    ajax::PolymerElement* explorePanel = new ajax::PolymerElement("explore-panel");

    this -> add(explorePanel);

    return;

  }
  
  void ExplorePanel::getExploreTree( cgicc::Cgicc& cgi, std::ostream& out ) {
    
    //Json::Value lJsonOutput(Json::arrayValue);
    Json::Value lSystemNode(Json::objectValue);
    
    { // Lock system in this scope
      CellContext::SharedGuard_t lGuard(mCellContext);
      swatch::system::System& lSystem = mCellContext.getSystem(lGuard);
      lSystemNode["name"] = lSystem.getId();
      this -> serializeTreeNode(lSystem, lSystemNode);
    }
    
    //lJsonOutput.append(lSystemNode);
    
    //out << lJsonOutput;
    out << lSystemNode;
    
    return;
  }
  
  void ExplorePanel::serializeTreeNode(const swatch::core::Object& aObj, Json::Value& aJsonNode) {
    
    Json::Value lChildrenJson(Json::arrayValue);
    
    const std::vector<std::string>& lChildren = aObj.getChildren();

    BOOST_FOREACH( const  std::string& lChildId, lChildren) {
      Json::Value lChildJson(Json::objectValue);
      const swatch::core::LeafObject& lChild = aObj.getObj(lChildId);
      lChildJson["name"] = lChild.getId();
      if ( dynamic_cast<const swatch::core::ObjectView*>(&lChild) ) {
        lChildrenJson.append(lChildJson);
        continue;
      } else if (const swatch::core::Object* lObjPtr = dynamic_cast<const swatch::core::Object*>(&lChild) ) {
        this -> serializeTreeNode(*lObjPtr, lChildJson);
        lChildrenJson.append(lChildJson);
      } else {
        lChildrenJson.append(lChildJson);
      }
    }
    if (lChildrenJson.size() > 0) aJsonNode["tree"] = lChildrenJson;
    return;
  }

} // end ms swatchframework

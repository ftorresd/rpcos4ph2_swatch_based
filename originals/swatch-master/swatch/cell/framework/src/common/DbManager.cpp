/* 
 * File:   DbManager.cc
 * Author: tom
 * 
 * Created on 15 March 2016, 12:17
 */

#include "swatchcell/framework/DbManager.h"


#include "boost/algorithm/string/case_conv.hpp"

#include "pugixml.hpp"

#include "xcept/Exception.h"

#include "ts/candela/QueryResult.h"
#include "ts/candela/QueryResultIterator.h"
#include "ts/candela/Row.h"
#include "ts/candela/Table.h"

#include "swatch/system/SystemStub.hpp"
#include "swatch/xml/XmlGateKeeper.hpp"
#include "swatch/xml/XmlReader.hpp"
#include "swatch/xml/XmlSystem.hpp"

#include "swatchcell/framework/CellContext.h"


namespace swatchcellframework {


const std::string DbManager::kSchemaName = "CMS_TRG_L1_CONF";
const std::string DbManager::kGlobalRSTableFullName = DbManager::kSchemaName + ".L1_TRG_RS_KEYS";
const std::string DbManager::kTopTableAlgoCol = "ALGO";
const std::string DbManager::kTopTableHwCol = "HW";
const std::string DbManager::kTopTableInfraCol = "INFRA";
const std::string DbManager::kClobColName = "CONF";

DbManager::DbManager(CellContext& aContext, const std::string& aSubsystemId) : 
  mContext(&aContext),
  mSubsystemId(getUppercaseCopy(aSubsystemId)),
  mLogger(log4cplus::Logger::getInstance("DbManager."+mSubsystemId)),
  mTopTableName(kSchemaName + "." + mSubsystemId + "_KEYS"),
  mRunSettingsColName(mSubsystemId + "_RS_KEY"),
  mDbConnection(mLogger, mContext)
{
  LOG4CPLUS_INFO(mLogger, "Constructing DbManager for subsystem '" << mSubsystemId << "': config table is '" << mTopTableName << "'"); 
}


//DbManager::DbManager(const DbManager& orig)
//{
//}


DbManager::~DbManager()
{
}


swatch::system::SystemStub DbManager::getHwStub(const std::string& aConfigKey)
{
  // 1) Extract key for HW table from top-level table
  tscandela::Table lTopTable = mDbConnection.getTable(mTopTableName);
  tscandela::QueryResult lTopTableQueryResult = lTopTable.selectRowByPK(aConfigKey); 

  if (lTopTableQueryResult.size() != 1) {
    std::ostringstream lOss;
    lOss << "Config table query (table '" << mTopTableName << "', key '" << aConfigKey << "') returned incorrect number of rows (" << lTopTableQueryResult.size() << " rows)";
    XCEPT_RAISE(tsexception::CellException, lOss.str());
  }

  const std::string lHwKey = lTopTableQueryResult.begin()->get<std::string>(kTopTableHwCol);

  // 2) Extract clob from HW table
  tscandela::Table lHwTable = mDbConnection.getReferencedTable(lTopTable.getFullName(), kTopTableHwCol);
  tscandela::QueryResult lHwTableQueryResult = lHwTable.selectRowByPK(lHwKey);

  if (lHwTableQueryResult.size() != 1) {
    std::ostringstream lOss;
    lOss << "HW table query (table '" << lHwTable.getFullName() << "', key '" << lHwKey << "') returned incorrect number of rows (" << lHwTableQueryResult.size() << " rows)";
    XCEPT_RAISE(tsexception::CellException, lOss.str());
  }
  
  // 3) Parse the CLOB as XML, and create the system stub
  const std::string lHwClob = lHwTableQueryResult.begin()->get<std::string>(kClobColName);
  pugi::xml_document lXmlDoc;
  pugi::xml_parse_result lLoadResult = lXmlDoc.load(lHwClob.c_str());
  if (!lLoadResult) {
    std::ostringstream lOSStream;
    lOSStream << "Error parsing 'system stub' XML CLOB from table '" << lHwTable.getFullName() << "', column '" << kClobColName << "', key '" << lHwKey << "'. Details: " << lLoadResult.description();
    XCEPT_RAISE(tsexception::CellException, lOSStream.str());
  }

  return swatch::xml::system::xmlToSystemStub(lXmlDoc);
}


//swatch::xml::XmlGateKeeper DbManager::buildGateKeeper(const std::string& aConfigKey, const std::string& aRunSettingsKey)
//{
//  return buildGateKeeper(aConfigKey, aRunSettingsKey, "", NULL);
//}

swatch::xml::XmlGateKeeper DbManager::buildGateKeeper(const std::string& aConfigKey, const std::string& aRunSettingsKey, const std::string& aCustomClobColumn, const ClobCallback_t& aCustomClobCallback)
{
  LOG4CPLUS_INFO(mLogger, "Building gatekeeper from database keys '" << aConfigKey << "', '" << aRunSettingsKey << "'");

  // 1) Extract row from top-level config table
  tscandela::Table lConfigTable = mDbConnection.getTable(mTopTableName);
  std::map<std::string, std::string> lConfigRow = selectSingleRowByPK(lConfigTable, aConfigKey, mLogger);

  // 2) Extract algo clobs
  LOG4CPLUS_INFO(mLogger, "Extracting ALGO clobs");
  typedef std::vector<boost::shared_ptr<pugi::xml_document> > XmlClobVec_t;
  tscandela::Table lAlgoTable = mDbConnection.getReferencedTable(lConfigTable.getFullName(), kTopTableAlgoCol);
  XmlClobVec_t lAlgoClobs = getReferencedXmlConfigClobs(lAlgoTable, lConfigRow.find(kTopTableAlgoCol)->second, swatch::xml::XmlReader::kModuleNameAlgo);

  // 3) Extract infra clobs
  LOG4CPLUS_INFO(mLogger, "Extracting INFRA clobs");
  tscandela::Table lInfraTable = mDbConnection.getReferencedTable(lConfigTable.getFullName(), kTopTableInfraCol);
  XmlClobVec_t lInfraClobs = getReferencedXmlConfigClobs(lInfraTable, lConfigRow.find(kTopTableInfraCol)->second, swatch::xml::XmlReader::kModuleNameInfra);

  // 4) Extract + translate custom CLOB (if translation callback registered)
  XmlClobVec_t lDerivedCustomClobs;
  if ((!aCustomClobColumn.empty()) && (aCustomClobCallback != NULL)) {
    LOG4CPLUS_INFO(mLogger, "Extracting custom (non-SWATCH) config CLOB referenced from column '" + aCustomClobColumn + "' in table '" << mTopTableName << "'");

    std::map<std::string, std::string>::const_iterator lCustomClobKey = lConfigRow.find(aCustomClobColumn);
    if (lCustomClobKey == lConfigRow.end()) {
      std::ostringstream lOss;
      lOss << "Could not find column '" << aCustomClobColumn << "' (foreign key for custom CLOB) in table '" << mTopTableName << "'";
      XCEPT_RAISE(tsexception::CellException, lOss.str());
    }

    tscandela::Table lCustomClobTable = mDbConnection.getReferencedTable(lConfigTable.getFullName(), aCustomClobColumn);

    const std::map<std::string, std::string> lCustomClobRow = selectSingleRowByPK(lCustomClobTable, lCustomClobKey->second, mLogger);

    std::map<std::string, std::string>::const_iterator lCustomClobCell = lCustomClobRow.find(kClobColName);
    if (lCustomClobCell == lConfigRow.end()) {
      std::ostringstream lOss;
      lOss << "Could not find CLOB column '" << kClobColName << " (for custom non-SWATCH CLOB) in table '" << mTopTableName << "'";
      XCEPT_RAISE(tsexception::CellException, lOss.str());
    }

    LOG4CPLUS_INFO(mLogger, "Translating custom (non-SWATCH) CLOB into SWATCH format CLOB, using registered callback");
    boost::shared_ptr<pugi::xml_document> lDerivedXmlDoc(aCustomClobCallback(lCustomClobCell->second));
    if (lDerivedXmlDoc == NULL) {
      std::ostringstream lOss;
      lOss << "Callback for custom CLOB (table '" << lCustomClobTable.getFullName() << "', primary key '" << lCustomClobKey->second << "', col '" << kClobColName << "')  returned a NULL pointer!";
      XCEPT_RAISE(tsexception::CellException, lOss.str());
    }

    std::string lErrorMsg;
    if (!swatch::xml::XmlReader::checkSubConfig(*lDerivedXmlDoc, lErrorMsg)) {
      std::ostringstream lOSStream;
      lOSStream << "XML produced from custom config CLOB (table '" << lCustomClobTable.getFullName() << "', key '" << lCustomClobKey->second;
      lOSStream << "', column '" << kClobColName << "' is NOT a valid SWATCH XML config module. Details: " << std::endl << lErrorMsg;

      std::ostringstream lLogStream; 
      lLogStream << lOSStream.str() << std::endl << std::endl << " --> The offending XML is:" << std::endl;
      lDerivedXmlDoc->save(lLogStream, "  ");
      LOG4CPLUS_INFO(mLogger, lLogStream.str());

      XCEPT_RAISE(tsexception::CellException, lOSStream.str());
    }

    lDerivedCustomClobs.push_back(lDerivedXmlDoc);
  }

  // 4) Extract clobs from run settings table
  LOG4CPLUS_INFO(mLogger, "Extracting run settings clobs referenced from global RS table '" << kGlobalRSTableFullName << "', column '" << mRunSettingsColName << "'");
  tscandela::Table lRunSettingsTable = mDbConnection.getReferencedTable(kGlobalRSTableFullName, mRunSettingsColName);
  XmlClobVec_t lRunSettingsClobs = getReferencedXmlConfigClobs(lRunSettingsTable, aRunSettingsKey, swatch::xml::XmlReader::kModuleNameRunSettings);

  // 5) Merge together the CLOBs using the XML reader
  // (Context tag of same ID from different CLOBs must be merged before constructing gatekeeper)
  LOG4CPLUS_INFO(mLogger, "Merging XML DB clobs for gatekeeper (" << lAlgoClobs.size() << " algo, " << lInfraClobs.size() << " infra, " << lDerivedCustomClobs.size() << " custom config, " << lRunSettingsClobs.size() << " run settings)");
  pugi::xml_document lMultiSubConfigDoc;

  std::vector<XmlClobVec_t*> lClobVectors = {&lAlgoClobs, &lInfraClobs, &lDerivedCustomClobs, &lRunSettingsClobs};
  for (auto lIt1=lClobVectors.begin(); lIt1!=lClobVectors.end(); lIt1++) {
    for (XmlClobVec_t::const_iterator lIt2=(*lIt1)->begin(); lIt2 != (*lIt1)->end(); lIt2++) {
      const pugi::xml_document& lSubConfig = **lIt2;
      swatch::xml::XmlReader::appendSubConfig(lSubConfig, lMultiSubConfigDoc);
//      std:: cout << " ------------------------------------------------------- " << std::endl;
//      for (pugi::xml_node lChild = lSubConfig.first_child().first_child(); lChild; lChild = lChild.next_sibling())
//        std::cout << lChild.name() << "; ";
//      std::cout << std::endl;
//      lSubConfig.print(std::cout, "  ");
    }
  }
//  std:: cout << " ------------------------------------------------------- " << std::endl;
//  std::cout << "AFTER APPENDING (children: ";
//  for (pugi::xml_node lChild = lMultiSubConfigDoc.first_child(); lChild; lChild = lChild.next_sibling()) 
//    std::cout << lChild.name() << "; ";
//  std::cout << ") ..." << std::endl;
//  lMultiSubConfigDoc.print(std::cout, "  ");

  pugi::xml_document lFullConfig;
  pugi::xml_node lKeyNode = lFullConfig.append_child("db").append_child("key");
  std::string lMergedXmlKey = aConfigKey + ";" + aRunSettingsKey;
  lKeyNode.append_attribute("id") = lMergedXmlKey.c_str();
  swatch::xml::XmlReader::mergeContexts(lMultiSubConfigDoc, lKeyNode);
  
  std::ostringstream lOStream; 
  lFullConfig.print(lOStream, "  ");
  LOG4CPLUS_DEBUG(mLogger, "Merged config XML from DB, config key '" << aConfigKey << "', run settings key '" << aRunSettingsKey << "'" << lOStream.str());

  // 4) Finally create the gatekeeper
  return swatch::xml::XmlGateKeeper(lFullConfig, lMergedXmlKey);
}


std::string DbManager::getUppercaseCopy(const std::string& aString)
{
  std::string lInputString(aString);
  boost::to_upper(lInputString);
  return lInputString;
}


DbManager::TableCell::TableCell (const tscandela::Table& aTable, const std::map<std::string,std::string>& aRow, const std::string& aColName) : 
  mTableName(aTable.getFullName()),
  mColName(aColName),
  mPKValue(aRow.at(aTable.getPK())), //(lCurrentRow.count(lCurrentTable.getPK()) ? lCurrentRow.find(lCurrentTable.getPK())->second : "(__unknown__)")
  mData(new std::string(aRow.at(aColName)))
{
}

DbManager::TableCell::~TableCell()
{
}

const std::string& DbManager::TableCell::getTableName() const
{
  return mTableName;
}

const std::string& DbManager::TableCell::getColName() const
{
  return mColName;
}

const std::string& DbManager::TableCell::getPKValue() const
{
  return mPKValue;
}

const std::string& DbManager::TableCell::getData() const
{
  return *mData;
}


std::map<std::string, std::string> DbManager::selectSingleRowByPK(tscandela::Table& aTable, const std::string& aKey, log4cplus::Logger& aLogger)
{
  LOG4CPLUS_DEBUG(aLogger, "DbManager::selectSingleRowByPK - table '" << aTable.getFullName() << "', key '" << aKey << "'");
  tscandela::QueryResult lQueryResult = aTable.selectRowByPK(aKey);

  if (lQueryResult.size() != 1) {
    std::ostringstream lOss;
    lOss << "Select row by PK value '" << aKey << "' in table '" << aTable.getFullName() << "' returned incorrect number of rows (" << lQueryResult.size() << " rows)";
    XCEPT_RAISE(tsexception::CellException, lOss.str());
  }

  const std::vector<std::string> lColumns = lQueryResult.getColumns();
  std::map<std::string, std::string> lResult;
  for (auto lIt=lColumns.begin(); lIt!=lColumns.end(); lIt++) {
    lResult[*lIt] = lQueryResult.begin()->get<std::string>(*lIt);
  }
  return lResult;
}



std::vector<DbManager::TableCell> DbManager::getReferencedClobs(tscandela::Table& aSourceTable, const std::string& aKey)
{
  std::map<std::string,std::string> lSourceTableRow = selectSingleRowByPK(aSourceTable, aKey, mLogger);

  typedef std::map<std::string,std::string> Row_t;

  std::vector<TableCell > lClobVec;
  std::deque< std::pair<tscandela::Table, Row_t> > lReferencedRows; // N.B. Deque rather than vector so that doesn't get re-allocated
  lReferencedRows.push_back( std::make_pair(aSourceTable, lSourceTableRow) );

  for(size_t i=0; i<lReferencedRows.size(); i++) {
    const tscandela::Table& lCurrentTable = lReferencedRows.at(i).first;
    const Row_t& lCurrentRow = lReferencedRows.at(i).second;
    LOG4CPLUS_DEBUG(mLogger, "DbManager::getReferencedClobs('" << aSourceTable.getFullName() << "', key='" << aKey
            << "') -- referenced row " << i << ", table '" << lCurrentTable.getFullName() << "'");

    for(Row_t::const_iterator lColIt=lCurrentRow.begin(); lColIt != lCurrentRow.end(); lColIt++) {
      const std::string& lColName = lColIt->first;
      const std::string& lColValue = lColIt->second;

      // Check if foreign key; if not, and clob, then try to interpret as XML
      try {
        tscandela::Table lReferencedTable = mDbConnection.getReferencedTable(lCurrentTable.getFullName(), lColName);
        LOG4CPLUS_DEBUG(mLogger, "   Column '" << lColName << "': FK detected, to table '"  << lReferencedTable.getFullName() << "'");
        Row_t lReferencedRow = selectSingleRowByPK(lReferencedTable, lColValue, mLogger);
        lReferencedRows.push_back( std::make_pair(lReferencedTable, lReferencedRow) );
      } catch ( xcept::Exception ) {
        // this exception was raised by candela when we tried to get a hold on non-existing table
//        LOG4CPLUS_DEBUG(mLogger, "No intermediate key-table exists, going straight to the configuration table ...");

        // assuming only column that has the payload Id in its name will point to the relevant XML in the next-level table
        if (lColName == kClobColName) {
          LOG4CPLUS_DEBUG(mLogger, "   Column '" << lColName << "': CLOB detected");
          lClobVec.push_back( TableCell(lCurrentTable, lCurrentRow, lColName) );
        }
        else
          LOG4CPLUS_DEBUG(mLogger, "   Column '" << lColName << "': Neither foreign key, nor CLOB");

        
        continue;
      }

//      if (lReferencedTable) {
//        std::cout << "    FK detected - to table '" << lReferencedTable->getFullName() << "'!" << std::endl;
//        Row_t lReferencedRow = selectSingleRowByPK(*lReferencedTable, lColValue);
//        lReferencedRows.push_back( std::make_pair(*lReferencedTable, lReferencedRow) );
//      }
    }
  }

  return lClobVec;
}


std::vector<boost::shared_ptr<pugi::xml_document> > DbManager::getReferencedXmlConfigClobs(tscandela::Table& aSourceTable, const std::string& aKey, const std::string& aRootTagName)
{
  const std::vector<TableCell> lClobVec = getReferencedClobs(aSourceTable, aKey);
  std::vector<boost::shared_ptr<pugi::xml_document> > lResult;

  for (auto lIt=lClobVec.begin(); lIt!=lClobVec.end(); lIt++) {
    const std::string& lTableName = lIt->getTableName();
    const std::string& lPKeyValue = lIt->getPKValue();
    const std::string& lColName   = lIt->getColName();
    const std::string& lClob      = lIt->getData();

    boost::shared_ptr<pugi::xml_document> lXmlDoc(new pugi::xml_document());
    pugi::xml_parse_result lLoadResult = lXmlDoc->load(lClob.c_str());
    if (!lLoadResult) {
      std::ostringstream lOSStream;
      lOSStream << "Error parsing XML CLOB from table '" << lTableName << "', column '" << lColName << "', key '" << lPKeyValue << "'. Details: " << lLoadResult.description();
      XCEPT_RAISE(tsexception::CellException, lOSStream.str());
    }

    if (lXmlDoc->name() == aRootTagName) {
      std::ostringstream lOSStream;
      lOSStream << "Error in XML CLOB from table '" << lTableName << "', column '" << lColName << "', key '" << lPKeyValue << "'. Root tag is '" << lXmlDoc->name() << "', should be '" << aRootTagName << "'!";
      XCEPT_RAISE(tsexception::CellException, lOSStream.str());
    }

    std::string lErrorMsg;
    if (!swatch::xml::XmlReader::checkSubConfig(*lXmlDoc, lErrorMsg)) {
      std::ostringstream lOSStream;
      lOSStream << "XML CLOB from table '" << lTableName << "', column '" << lColName << "', key '" << lPKeyValue << "' is NOT a valid SWATCH XML config module. Details: " << std::endl << lErrorMsg;
      XCEPT_RAISE(tsexception::CellException, lOSStream.str());
    }

    lResult.push_back(lXmlDoc);
  }

  return lResult;
}

} // ns swatchcellframework

/* 
 * File:   DbManager.h
 * Author: tom
 *
 * Created on 15 March 2016, 12:17
 */

#ifndef __SWATCHCELL_FRAMEWORK_DBMANAGER_H__
#define __SWATCHCELL_FRAMEWORK_DBMANAGER_H__


#include <string>

#include "boost/function.hpp"
#include "boost/shared_ptr.hpp"

#include "log4cplus/logger.h"

#include "ts/candela/DatabaseConnection.h"


namespace candela {
class QueryResult;
class Row;
class Table;
}

namespace pugi {
class xml_document;
}

namespace swatch {
namespace system {
class SystemStub;
}
namespace xml {
class XmlGateKeeper;
}
}

namespace swatchcellframework {

class CellContext;

class DbManager {
public:

  typedef boost::function<boost::shared_ptr<pugi::xml_document>(const std::string&)> ClobCallback_t;


  DbManager(CellContext& aContext, const std::string& aSubsystemId);
//  DbManager(const DbManager& orig);
  ~DbManager();

  swatch::system::SystemStub getHwStub(const std::string& aConfigKey);

//  swatch::xml::XmlGateKeeper buildGateKeeper(const std::string& aConfigKey, const std::string& aRunSettingsKey);
  swatch::xml::XmlGateKeeper buildGateKeeper(const std::string& aConfigKey, const std::string& aRunSettingsKey, const std::string& aCustomClobColumn, const ClobCallback_t& aCustomClobCallback);

private:
  CellContext* mContext;
  std::string mSubsystemId;
  log4cplus::Logger mLogger;
  std::string mTopTableName;
  std::string mRunSettingsColName;
  tscandela::DatabaseConnection mDbConnection;

  static const std::string kSchemaName;
  static const std::string kGlobalRSTableFullName;
  static const std::string kTopTableAlgoCol;
  static const std::string kTopTableHwCol;
  static const std::string kTopTableInfraCol;
  static const std::string kClobColName;

  static std::string getUppercaseCopy(const std::string& aString);

  static std::map<std::string, std::string> selectSingleRowByPK(tscandela::Table& aTable, const std::string& aKey, log4cplus::Logger& aLogger);

  class TableCell {
  public:
    TableCell (const tscandela::Table& aTable, const std::map<std::string,std::string>& aRow, const std::string& aColName);
    ~TableCell();

    const std::string& getTableName() const;
    const std::string& getColName() const;
    const std::string& getPKValue() const;
    const std::string& getData() const;

  private:
    std::string mTableName;
    std::string mColName;
    std::string mPKValue;
    boost::shared_ptr<std::string> mData;
  };

  std::vector<TableCell> getReferencedClobs(tscandela::Table& aSourceTable, const std::string& aKey);

  std::vector<boost::shared_ptr<pugi::xml_document> > getReferencedXmlConfigClobs(tscandela::Table& aSourceTable, const std::string& aKey, const std::string& aRootTagName);
};

}

#endif	/* __SWATCHCELL_FRAMEWORK_DBMANAGER_H__ */


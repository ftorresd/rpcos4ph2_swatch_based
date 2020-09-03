/*************************************************************************
 *                                                                       *
 * Authors: Giuzz The Metal Slayer                                       *
 *                                                                       *
 *************************************************************************/

#ifndef __SWATCHCELL_FRAMEWORK_INFOSPACEBUILDER_H__
#define __SWATCHCELL_FRAMEWORK_INFOSPACEBUILDER_H__

#include "xdata/xdata.h"

#include <vector>
#include <map>
#include <string>
#include <typeinfo>

#include "log4cplus/logger.h"


namespace swatchcellframework
{

// Forward declarations
class CellAbstract;
class CellContext;


  class InfoSpaceBuilder : public xdata::ActionListener
  {
  public:

    InfoSpaceBuilder ( const std::string& infospaceName, CellContext & context, const log4cplus::Logger& aParentLogger);
    
    virtual ~InfoSpaceBuilder();

    //!refreshes the monitorable items: empty since we work in push mode
    void actionPerformed ( xdata::Event& received );
  
    //!adds a new Monitorable item that should be xdata::Serializable descendant
    void add ( const std::string& name, xdata::Serializable * data );

    //!remove a monitorable item
    void remove ( const std::string& name );

    //!fire item changed for one item only
    void push ( const std::string& name, const xdata::Serializable * data );

    //!fire item changed for all items
    void push( const std::map<std::string, xdata::Serializable*> & monitorables );

    //!build xml flashlist definition
    void getFlashlist( std::ostream & flash );

  private:

    tstoolbox::Mutex mMutex;

    std::string mInfospaceName;

    std::string mInfospaceUrn;

    std::set<std::string> mMoniNames;

    xdata::InfoSpace* mInfospace;

  protected:

    //!Passes a refernce to the InfoSpaceBuilder recursive mutex
    tstoolbox::Mutex& getMutex();

  private:
    log4cplus::Logger mLogger;
  };
}//ns swatchcellframework

#endif  /* __SWATCHCELL_FRAMEWORK_INFOSPACEBUILDER_H__ */

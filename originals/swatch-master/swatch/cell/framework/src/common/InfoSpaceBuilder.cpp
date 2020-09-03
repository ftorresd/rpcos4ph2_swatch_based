
 // SWATCH cell headers
#include "swatchcell/framework/CellAbstract.h"
#include "swatchcell/framework/tools/panelFunctions.h"
#include "swatchcell/framework/tools/utilities.h"
#include "swatchcell/framework/MonitoringThread.h"

///
#include "swatchcell/framework/InfoSpaceBuilder.h"

#include "xcept/Exception.h"
#include "xcept/tools.h"

#include "xdata/InfoSpaceFactory.h"
#include "xdata/ItemGroupEvent.h"

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"

#include "toolbox/net/URN.h"

#include "swatchcell/framework/CellAbstract.h"

namespace swatchcellframework
{

InfoSpaceBuilder::InfoSpaceBuilder ( const std::string& infospaceName, CellContext& context, const log4cplus::Logger& aParentLogger) :
  mMutex ( true ), 
  mInfospaceName( infospaceName ),
  mLogger(log4cplus::Logger::getInstance(aParentLogger.getName() + "InfoSpaceBuilder"))
{
    toolbox::net::URN urn = context.getCell()->createQualifiedInfoSpace ( infospaceName );
    mInfospaceUrn = urn.toString();
    mInfospace = xdata::InfoSpaceFactory::getInstance()->get ( mInfospaceUrn );
    mInfospace->addGroupRetrieveListener ( this );
    std::cout << "######################    " <<  mInfospaceName << std::endl;
    std::cout << "######################    " <<  mInfospaceUrn << std::endl;
}


InfoSpaceBuilder::~InfoSpaceBuilder()
{
}

void InfoSpaceBuilder::actionPerformed ( xdata::Event& received )
{
  LOG4CPLUS_DEBUG(mLogger, "InfoSpaceBuilder RECEIVED EVENT : '" <<  received.type()  << "'");
};


void
InfoSpaceBuilder::add ( const std::string& name, xdata::Serializable * data )
{
  tstoolbox::MutexHandler handler ( getMutex() );
  if ( mMoniNames.find ( name ) == mMoniNames.end() ) {
	mMoniNames.insert ( name );
  }
  mInfospace->fireItemAvailable ( name, data );
}


void 
InfoSpaceBuilder::remove ( const std::string& name )
{

  tstoolbox::MutexHandler handler ( getMutex() );


  auto lNameIterator = mMoniNames.find ( name );
  if ( lNameIterator !=  mMoniNames.end() ) {
    mInfospace->fireItemRevoked ( name, this );
    mMoniNames.erase( lNameIterator );
  } else {
    XCEPT_RAISE ( xcept::Exception,"Trying to erase a Monitorable item " + name + " that does not exist " );
  }
}



//!fire item changed for one item only
void
InfoSpaceBuilder::push ( const std::string& name, const xdata::Serializable * data )
{
  tstoolbox::MutexHandler handler ( getMutex() );

  if ( mMoniNames.find ( name ) == mMoniNames.end() )
    {
      std::string msg ( "Trying to push value in a Monitorable item " + name + " that does not exist " );
      XCEPT_RAISE ( xcept::Exception,msg );
    }

  std::list<std::string> lnames;
  lnames.push_back ( name );
  mInfospace->fireItemGroupChanged ( lnames, this );
}

//!fire item changed for all items
void
InfoSpaceBuilder::push( const std::map<std::string, xdata::Serializable*> & monitorables )
{
    tstoolbox::MutexHandler handler ( getMutex() );
    std::list<std::string> lnames;

    BOOST_FOREACH( const auto & moni, monitorables ) {
      if ( mMoniNames.find( moni.first ) == mMoniNames.end() ) {
	mInfospace->fireItemAvailable ( moni.first, moni.second );

	// std::cout << "$$$$$$$$$$$$$$$$$$$$$ " << moni.first << std::endl;
	// if ( moni.second->type() == "table" ) {
	//   xdata::Table* lTable = dynamic_cast<xdata::Table*>( moni.second );
	//   std::vector<std::string> columns = lTable->getColumns();
	//   BOOST_FOREACH( const auto & name, columns ) {
	//     std::cout << name << '\t' << lTable->getValueAt( 0, name )->type() << std::endl;
	//   }
	// } else {
	//   std::cout << moni.first << '\n' << moni.second->toString() << std::endl;
	// }
	mMoniNames.insert( moni.first );
      }
      lnames.push_back( moni.first );
    }

    if ( lnames.size() == 0 )
    {
        std::string msg ( "Trying to push an empty list of Monitorable items" );
        XCEPT_RAISE ( xcept::Exception,msg );
    }

    mInfospace->fireItemGroupChanged ( lnames, this );

}


//!fire item changed for all items
void
InfoSpaceBuilder::getFlashlist( std::ostream & flash )
{
    std::map<std::string, xdata::Serializable * > items = mInfospace->match( ".*" );

    flash << "<?xml version='1.0'?>\n"
	  << "<xmas:flash xmlns:xmas=\"http://xdaq.web.cern.ch/xdaq/xsd/2006/xmas-10\" id=\"urn:xdaq-flashlist:" << mInfospaceName << "\" version=\"1.0.0\" key=\"uniqueid\">\n"
	  << "  <xmas:item name=\"uniqueid\"  function=\"uuid()\"    type=\"string\" />\n"
	  << "  <xmas:item name=\"timestamp\" function=\"systime()\" type=\"time\" />\n"
	  << "  <xmas:item name=\"context\"   function=\"context()\" type=\"string\" />\n";
    
    BOOST_FOREACH( const auto & item, items ) {
      
      if ( item.second->type() == "table" ) {
	
	flash << "\n<xmas:item name=\"" << item.first << "\" infospace=\"" << mInfospaceName << "\" type=\"table\">\n";
	xdata::Table* lTable = dynamic_cast<xdata::Table*>( item.second );
	std::vector<std::string> columns = lTable->getColumns();
	BOOST_FOREACH( const auto & name, columns ) {
	  flash << "  <xmas:item name=\"" << name << "\"           type=\"" << lTable->getColumnType(name) << "\"/>\n";
	}
	flash << "</xmas:item>\n\n";
//      } else if ( item.second->type() == "properties" ) {
//	std::map<std::string, std::string>& props = dynamic_cast<xdata::Properties*>(item.second)->getProperties();
//	BOOST_FOREACH( const auto & prop, props ) {
//	  std::cerr << prop.first << '\t' << prop.second << std::endl;
//	}
      } else if ( item.second->type() != "properties" ) {
	flash << "  <xmas:item name=\"" << item.first << "\" infospace=\"" << mInfospaceName << "\" type=\"" << item.second->type() << "\"/>\n";
      }
    }

    flash << "</xmas:flash>\n";

}




tstoolbox::Mutex& 
InfoSpaceBuilder::getMutex()
{
    return mMutex;
}

} // end ns tsframework

/*************************************************************************
 * Trigger Supervisor SWATCH worker                                      *
 *                                                                       *
 * Authors: Carlos Ghabrous Larrea         				         *
 *************************************************************************/

#ifndef __SWATCHCELL_FRAMEWORK_CELLABSTRACT_H__
#define __SWATCHCELL_FRAMEWORK_CELLABSTRACT_H__


#include <string>

#include "boost/type_traits/is_base_of.hpp"

#include "uhal/log/log.hpp"

#include "xcept/tools.h"

#include "ts/framework/CellAbstract.h"
#include "ts/framework/CellOperationFactory.h"

#include "swatchcell/framework/CellContext.h"
#include "swatchcell/framework/RunControl.h"


namespace swatchcellframework {

class CellAbstract : public tsframework::CellAbstract
{
public:

    template< class Type > struct TypeCarrier{ typedef Type T; };
  
    CellAbstract(xdaq::ApplicationStub * s);

    /**!
     * Templated constructor, used to register custom "run control" operation class
     * @tparam RunControlOperationClass A custom "run control" TS operation class, that inherits from swatchcellframework::RunControl
     */ 
    template <class RunControlOperationClass>
    CellAbstract(xdaq::ApplicationStub * s, TypeCarrier<RunControlOperationClass>);

//    /**!
//     * Templated constructor, used to register classes for cell context and  "run control" operation 
//     * @tparam ContextClass A custom cell context class, that inherits from swatchcellframework::CellContext, and must have a constructor of signature ContextClass(log4cplus::Logger& log, tsframework::CellAbstract* cell)
//     * @tparam RunControlOperationClass A custom "run control" TS operation class, that inherits from swatchcellframework::RunControl
//     */ 
//    template <class ContextClass, class RunControlOperationClass>
//    CellAbstract(xdaq::ApplicationStub * s, TypeCarrier<ContextClass>, TypeCarrier<RunControlOperationClass>);

    virtual ~CellAbstract();

    //! Returns the CellContext pointer. CellContext contains the shared objects of the Cell
    CellContext* getContext() {
        CellContext* c( dynamic_cast<CellContext*>(cellContext_) );

        if (!c) {
            XCEPT_RAISE(tsexception::CellException,"The Cell Context is not of class CellContext");
        }

        return c;
    };

protected:
    //! Add the generic SWATCH monitoring and command panels to the subsystem's TS cell
    void addGenericSwatchComponents();

private:
    CellAbstract( const CellAbstract& );

    template <class ContextClass>
    void createContext();

    template <class RunControlOperationClass>
    void registerRunControlOperation();
};


template <class RunControlOperationClass>
CellAbstract::CellAbstract(xdaq::ApplicationStub * s, TypeCarrier<RunControlOperationClass>) :
  tsframework::CellAbstract(s)
{
  createContext<CellContext>();

  uhal::setLogLevelTo(uhal::Notice());

  registerRunControlOperation<RunControlOperationClass>();
}
 

//template <class ContextClass, class RunControlOperationClass>
//CellAbstract::CellAbstract(xdaq::ApplicationStub * s, TypeCarrier<ContextClass>, TypeCarrier<RunControlOperationClass>) :
//  tsframework::CellAbstract(s)
//{
//  createContext<ContextClass>();
//
//  uhal::setLogLevelTo(uhal::Notice());
//
//  registerRunControlOperation<RunControlOperationClass>();
//}
 

template <class ContextClass>
void CellAbstract::createContext()
{
  BOOST_STATIC_ASSERT_MSG( (boost::is_base_of<swatchcellframework::CellContext, ContextClass>::value) , "class ContextClass must be a descendant of swatchcellframework::CellContext" );

  try {
    cellContext_ = new ContextClass(getApplicationLogger(), this);
  } catch (xcept::Exception& e) {
    std::ostringstream msg;
    msg << "Error instantiating the CellContext. Find the bug and restart the executive. Exception found: ";
    msg << xcept::stdformat_exception_history(e);
    LOG4CPLUS_ERROR(getApplicationLogger(), msg.str());
  }
}


template <class RunControlOperationClass>
void CellAbstract::registerRunControlOperation()
{  
  BOOST_STATIC_ASSERT_MSG( (boost::is_base_of<swatchcellframework::RunControl, RunControlOperationClass>::value) , "class RunControlOperationType must be a descendant of swatchcellframework::RunControl" );

  // Register the run Control operation
  tsframework::CellOperationFactory* lOpFactory = getContext()->getOperationFactory();
  lOpFactory->add<RunControlOperationClass>(CellContext::kRunControlOperationName);
}


} // end ns swatchcellframework


#endif  /* __SWATCHCELL_FRAMEWORK_CELLABSTRACT_H__ */


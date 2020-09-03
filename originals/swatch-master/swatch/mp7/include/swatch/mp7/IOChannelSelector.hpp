/*
 * File:   IOChannelSelector.hpp
 * Author: ale
 *
 * Created on February 4, 2016, 11:57 AM
 */

#ifndef __SWATCH_MP7_IOCHANNELSELECTOR_HPP__
#define __SWATCH_MP7_IOCHANNELSELECTOR_HPP__


// Standard headers
#include <string>                       // for string
#include <vector>                       // for vector

// boost headers
#include "boost/function.hpp"           // for function

// SWATCH headers
#include "swatch/core/exception.hpp"    // for SWATCH_DEFINE_EXCEPTION
#include "swatch/mp7/ChannelDescriptorCollection.hpp"

// MP7 Headers
#include "mp7/ChannelManager.hpp"


namespace mp7 {
class MP7MiniController;
}


namespace swatch {

//forward declarations
namespace core {
class XParameterSet;
}
namespace action {
class Command;
}

namespace mp7 {

// Forward Declaration
class MP7AbstractProcessor;

/**
 * @class IOChannelSelector
 */
class IOChannelSelector {
public:
  IOChannelSelector( swatch::action::Command& );
  ~IOChannelSelector();

  /**
   * @brief      Add Channel Selector specific parameters to the current command
   */
  virtual void registerParameters();

  /**
   * @brief      Creates a mp7::ChannelManager object with channel selection
   *             applied based on current parameters
   *
   * @param[in]  aParams  Input Parameters.
   *
   * @return     mp7::ChannelManager object
   */
  ::mp7::ChannelManager manager( const swatch::core::XParameterSet& aParams ) const;

  /**
   * @brief      MP7Controller getter
   *
   * @return     reference to the MP7Controller
   */
  ::mp7::MP7MiniController& getDriver();

  
  /**
   * @brief      Extracts the IDs correspondig to all the channels known to the
   *             Channel Manager.
   *             
   * @param[in]  aManager  The channel manager object
   *
   * @return     Vector of MP7 channel ids
   */
  std::vector<std::string> extractIds(const ::mp7::ChannelManager& aManager) const;

  /**
   * @brief      Gets the collection of MP7 Channel Descriptors.
   *
   * @return     The descriptor collection.
   */
  virtual const ChannelDescriptorCollection& getDescriptors() const = 0;

  /**
   * @brief      Extract form a ParameterSet the value of the id selection parameter.
   *
   * @param      aParams  A parameters
   *
   * @return     The identifier selection.
   */
  virtual std::string getIdSelection( const swatch::core::XParameterSet& aParams ) const;

  /**
   *
   * @return
   */
  virtual const ChannelRule_t& getGroupFilter() const = 0;

  /**
   * @brief      Created a mask filter based on a set of parameters.
   *
   * @param      aParams  Input parameters.
   *
   * @return     The mask filter.
   */
  virtual const ChannelRule_t& getMaskFilter( const swatch::core::XParameterSet& aParams ) const;


  //! Name of the id Selection parameter
  static const std::string kIdSelection;

protected:
  //! Reference to command
  swatch::action::Command& mCommand;

  //! Reference to Processor, commodity
  MP7AbstractProcessor& mProcessor;

  static const ChannelRule_t kAlwaysTrue;
};


/**
 * @class RxChannelSelector
 */
class RxChannelSelector : public IOChannelSelector {
public:
  RxChannelSelector( swatch::action::Command& aCommand, const ChannelRule_t& aFilter );

  virtual ~RxChannelSelector() {}

  virtual void registerParameters();

  virtual const ChannelRule_t& getGroupFilter() const;

  virtual const ChannelDescriptorCollection& getDescriptors() const;

  virtual const ChannelRule_t& getMaskFilter(const swatch::core::XParameterSet& aParams) const;

  //!
  static const std::string kMaskSelection;

  //!
  static const std::string kApplyMasks;

  //!
  static const std::string kInvertMasks;

  //!
  static const std::string kIgnoreMasks;

private:
  const ChannelRule_t mRxGroupFilter;
  const ChannelRule_t mApplyMaskFilter;
  const ChannelRule_t mInvertMaskFilter;

};

SWATCH_DEFINE_EXCEPTION(ApplyMaskOptionInvalid)

/**
 * @class TxChannelSelector
 */
class TxChannelSelector : public IOChannelSelector {
public:
  TxChannelSelector(swatch::action::Command& aCommand, const ChannelRule_t& aFilter);

  virtual ~TxChannelSelector() {}

  virtual const ChannelRule_t& getGroupFilter() const;

  virtual const ChannelDescriptorCollection& getDescriptors() const;

private:
  const ChannelRule_t mTxGroupFilter;
};


/**
 * @class RxMGTSelector
 */
class RxMGTSelector : public RxChannelSelector {
public:

  RxMGTSelector( swatch::action::Command& aCommand );

  virtual ~RxMGTSelector() {}
};


/**
 * @class TxMGTSelector
 */
class TxMGTSelector : public TxChannelSelector {
public:
  TxMGTSelector(swatch::action::Command& aCommand);

  virtual ~TxMGTSelector() {}

};

/**
 * @class RxBufferSelector
 */
class RxBufferSelector : public RxChannelSelector {
public:

  RxBufferSelector( swatch::action::Command& aCommand );

  virtual ~RxBufferSelector() {}

};


/**
 * @class TxBufferSelector
 */
class TxBufferSelector : public TxChannelSelector {
public:
  TxBufferSelector(swatch::action::Command& aCommand);

  virtual ~TxBufferSelector() {}

};


} // namespace mp7
} // namespace swatch

#endif /* __SWATCH_MP7_CHANNELSELECTOR_HPP */



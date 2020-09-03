/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   MaskingConfigurator.hpp
 * Author: ale
 *
 * Created on February 15, 2016, 9:02 AM
 */

#ifndef __SWATCH_SYSTEM_MASKINGCONFIGURATOR_HPP__
#define __SWATCH_SYSTEM_MASKINGCONFIGURATOR_HPP__

// C++ Headers
#include <map>
#include <stdint.h>
#include <string>
#include <vector>

#include <boost/noncopyable.hpp>

namespace swatch {

// Forward declaration
namespace action {
class GateKeeper;
}

namespace system {
class System;

class MaskingConfigurator : boost::noncopyable {
public:

  typedef std::map<uint32_t, uint32_t> FEDEnableMap_t;

  MaskingConfigurator( swatch::system::System& aSystem );

  /**
   * Applies static masks from GateKeeper
   * @param aGateKeeper GateKeeper reference
   */
  void applyStatic(const swatch::action::GateKeeper& aGateKeeper);

  /**
   *
   * @param aFEDEnableMap
   */
  void disableCrates(const FEDEnableMap_t& aFEDEnableMap);

  /**
   * Applies FED-mask-based dynamic masks
   * @param aFEDEnableMap FEDEnableMap, listing enable and disabled FEDs
   */
  void applyDynamicMasks( const FEDEnableMap_t& aFEDEnableMap );

  /**
   * Clears masks and disable flags
   */
  void clear();

  /**
   * Returns the list of automatically disabled DTMs
   * @return list of automatically disabled DTMs
   */
  const std::vector<std::string>& getAutoDisabledDTMs() const;

  /**
   * Returns the list of automatically disabled AMCs
   * @return list of automatically disabled AMCs
   */
  const std::vector<std::string>& getAutoDisabledAMCs() const;

  const std::vector<std::string>& getAutoDisabledAMCPorts() const;
  const std::vector<std::string>& getAutoMaskedLinks() const;
  const std::vector<std::string>& getAutoMaskedInputs() const;

private:

  void clearLists();

  void maskAMCPorts();

  void maskInternalLinks();

  void maskExternalInputs( const FEDEnableMap_t& aFEDEnableMap );

  swatch::system::System& mSystem;

  //! List of automatically disabled DTMs
  std::vector<std::string> mAutoDisabledDtms;

  //! List of automatically disabled AMCs
  std::vector<std::string> mAutoDisabledAMCs;

  //! List of automatically disabled AMC ports
  std::vector<std::string> mAutoDisabledAMCPorts;

  //! List of automatically masked internal links
  std::vector<std::string> mAutoMaskedLinks;

  //! List of automatically disable inputs
  std::vector<std::string> mAutoMaskedInputs;

};

} // namespace system
} //namespace swatch

#endif /* __SWATCH_SYSTEM_MASKINGCONFIGURATOR_HPP__ */


#ifndef __SWATCH_MP7_CMDS_SAVEBUFFERSTOFILECOMMAND_HPP__
#define __SWATCH_MP7_CMDS_SAVEBUFFERSTOFILECOMMAND_HPP__

// MP7 headers
#include "mp7/definitions.hpp"
#include "mp7/PathConfigurator.hpp"

// SWATCH headers
#include "swatch/action/Command.hpp"      // for Command

// SWATCH MP7 headers
#include "swatch/mp7/IOChannelSelector.hpp"


namespace swatch {
namespace mp7 {
namespace cmds {

/**
 * @class      SaveBuffersToFileCommand
 *
 * @brief      Class for save buffers to file command.
 *
 * @tparam     Selector  { description }
 */
template<class Selector>
class SaveBuffersToFile : public swatch::action::Command {
public:

  SaveBuffersToFile(const std::string& aId, swatch::action::ActionableObject& aActionable);

  virtual ~SaveBuffersToFile() {}

  virtual swatch::action::Command::State code(const ::swatch::core::XParameterSet& aParams);

protected:

  Selector mBufferSelector;
};

typedef SaveBuffersToFile<RxBufferSelector> SaveRxBuffersToFile;
typedef SaveBuffersToFile<TxBufferSelector> SaveTxBuffersToFile;

} // namespace cmds
} // namespace mp7
} // namespace swatch



#endif /* __SWATCH_MP7_CMDS_SAVEBUFFERSTOFILECOMMAND_HPP__ */
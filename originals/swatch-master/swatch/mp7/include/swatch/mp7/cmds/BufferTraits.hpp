#ifndef __SWATCH_MP7_CMDS_BUFFERTRAITS_HPP__
#define __SWATCH_MP7_CMDS_BUFFERTRAITS_HPP__

#include "mp7/definitions.hpp"


namespace swatch {
namespace mp7 {
namespace cmds {

/**
 * Templated translator from BufferCore to BufferKind
 */
template<class Selector>
struct BufferTraits {
  const static ::mp7::RxTxSelector kRxTxSelector;
};

} // namespace cmds
} // namespace mp7
} // namespace swatch

#endif /* __SWATCH_MP7_CMDS_BUFFERTRAITS_HPP__ */
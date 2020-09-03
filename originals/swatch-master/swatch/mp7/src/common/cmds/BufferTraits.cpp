#include "swatch/mp7/cmds/BufferTraits.hpp"

// SWATCH MP7 headers
#include "swatch/mp7/IOChannelSelector.hpp"


namespace swatch {
namespace mp7 {
namespace cmds {

// Template specialisation
template<>
const ::mp7::RxTxSelector BufferTraits<RxBufferSelector>::kRxTxSelector = ::mp7::kRx;

template<>
const ::mp7::RxTxSelector BufferTraits<TxBufferSelector>::kRxTxSelector = ::mp7::kTx;


} // namespace cmds
} // namespace mp7
} // namespace swatch


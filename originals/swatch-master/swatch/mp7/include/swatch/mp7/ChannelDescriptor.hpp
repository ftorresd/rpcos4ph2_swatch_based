#ifndef __SWATCH_MP7_CHANNELDESCRIPTOR_HPP__
#define __SWATCH_MP7_CHANNELDESCRIPTOR_HPP__


#include <stdint.h>                     // for uint32_t
#include <string>                       // for string

// MP7 Headers
#include "mp7/definitions.hpp"


namespace swatch {

namespace action {
class MaskableObject;
} // namespace action

namespace mp7 {

// @class Helper class to describe for MP7 channel properties
class ChannelDescriptor {
public:

  /**
   * Default constructor
   */
  ChannelDescriptor();

  /**
   * Constructor. Doesn't rely on the channel to be mapped on a port
   * In case of future extensions
   * @param aId
   * @param aChannelId
   * @param aHasMGTs
   * @param aHasBuffer
   * @param aFmtKind
   */
  ChannelDescriptor(std::string aId, uint32_t aChannelId, bool aHasMGTs, bool aHasBuffer, ::mp7::FormatterKind aFmtKind, const action::MaskableObject* aMaskable = 0);

  const std::string& getId() const;

  uint32_t getChannelId() const;

  bool hasMGT() const;

  bool hasBuffer() const;

  ::mp7::FormatterKind getFormatterKind() const;

  bool isMasked() const;

private:
  std::string mId;
  uint32_t mChannelId;
  bool mHasMGT;
  bool mHasBuffer;
  ::mp7::FormatterKind mFormatterKind;
  const action::MaskableObject* mMaskable;

  friend class ChannelDescriptorCollection;
};


//typedef std::map<std::string, ChannelDescriptor> ChannelDescriptorMap_t;


} // namespace mp7
} // namespace swatch

#endif /* __SWATCH_MP7_CHANNEL_DESCRIPTOR_HPP__ */

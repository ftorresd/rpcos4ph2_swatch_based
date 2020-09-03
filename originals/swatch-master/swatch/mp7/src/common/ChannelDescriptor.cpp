#include "swatch/mp7/ChannelDescriptor.hpp"


// SWATCH Headers
#include "swatch/action/MaskableObject.hpp"


namespace swatch {
namespace mp7 {


ChannelDescriptor::ChannelDescriptor()
{

}

ChannelDescriptor::ChannelDescriptor(std::string aId, uint32_t aChannelId, bool aHasMGTs, bool aHasBuffer, ::mp7::FormatterKind aFmtKind, const action::MaskableObject* aMaskable) :
  mId(aId),
  mChannelId(aChannelId),
  mHasMGT(aHasMGTs),
  mHasBuffer(aHasBuffer),
  mFormatterKind(aFmtKind),
  mMaskable(aMaskable)
{

}

// Getters

const std::string&
ChannelDescriptor::getId() const
{
  return mId;
}

uint32_t
ChannelDescriptor::getChannelId() const
{
  return mChannelId;
}

bool ChannelDescriptor::hasMGT() const
{
  return mHasMGT;
}

bool ChannelDescriptor::hasBuffer() const
{
  return mHasBuffer;
}


::mp7::FormatterKind ChannelDescriptor::getFormatterKind() const
{
  return mFormatterKind;
}

bool ChannelDescriptor::isMasked() const
{
  if ( !mMaskable ) return false;

  return mMaskable->isMasked();
}


} // namespace mp7
} // namespace swatch

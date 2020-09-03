
#include "swatch/action/MaskableObject.hpp"


namespace swatch {
namespace action {


MaskableObject::MaskableObject(const std::string& aId, const std::string& aAlias) :
  MonitorableObject(aId, aAlias),
  mMasked(false)
{
}


bool MaskableObject::isMasked() const
{
  return mMasked;
}


void MaskableObject::setMasked(bool aMask)
{
  mMasked = aMask;
}


} // namespace action
} // namespace swatch

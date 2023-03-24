#include "gRes.h"
#include "xdBase/exce.h"
#include <memory>

namespace XD::Render
{
    GRes::GRes() : _uuid(UUID::gen()) { }
    uuids::uuid GRes::getUUID() const { return _uuid; }
} // namespace XD::Render

#include "gRes.h"

namespace XD::Render
{
    GRes::GRes() : _uuid(UUID::gen()) { }
    uuids::uuid GRes::getUUID() const { return _uuid; }
} // namespace XD::Render

#include <memory>

#include "bufferResHolder.hpp"
#include "render/gRes/buffer/buffer.h"
#include "render/gRes/gRes.h"
#include "render/impl/vk/vkMgr.h"

namespace XD::Render
{
    Buffer::Buffer(const uuids::uuid& devId)
    {
        _devId = devId;
        _holder = std::make_unique<GResHolder<gResType>>();
    }

    const GResHolder<Buffer::gResType>& Buffer::getImplInfo() const
    {
        return getResHolder<gResType>(_holder);
    }
}
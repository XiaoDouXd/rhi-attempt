#include <memory>

#include "bufferResHolder.hpp"
#include "render/public/gRes/buffer/buffer.h"
#include "render/public/gRes/gRes.h"
#include "vk/vkMgr.h"

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
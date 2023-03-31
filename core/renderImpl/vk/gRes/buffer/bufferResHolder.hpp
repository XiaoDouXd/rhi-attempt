
#pragma once

#include <vulkan/vulkan.hpp>

#include "render/public/gRes/buffer/buffer.h"
#include "render/public/gRes/gRes.h"

namespace XD::Render
{
    template<>
    class GResHolder<GResType::Buffer> : public GResHolderBase
    {
    public:
        vk::DeviceMemory    devMem;
        vk::Buffer          buf;
    };
}
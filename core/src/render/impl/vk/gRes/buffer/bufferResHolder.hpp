
#pragma once

#include <vulkan/vulkan.hpp>

#include "render/gRes/buffer/buffer.h"
#include "render/gRes/gRes.h"

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
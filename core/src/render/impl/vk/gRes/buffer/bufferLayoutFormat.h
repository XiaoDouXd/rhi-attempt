#pragma once

#include <array>
#include <vulkan/vulkan.hpp>

#include "render/gRes/gRes.h"

namespace XD::Render
{
    extern const std::array<vk::Format, (size_t)BufferLayoutType::Num> VkFormatMap;
}
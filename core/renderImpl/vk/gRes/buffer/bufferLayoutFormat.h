#pragma once

#include <array>
#include <vulkan/vulkan.hpp>

#include "render/public/gRes/gRes.h"

namespace XD::Render
{
    extern const std::array<vk::Format, (size_t)Format::Num> VkFormatMap;
}
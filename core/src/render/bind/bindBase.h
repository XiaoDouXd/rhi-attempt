#pragma once

#include <vulkan/vulkan.hpp>

#include "render/gRes.h"

namespace XD::Render
{
    class BindBase : public GRes
    {
    public:
        virtual void bind(vk::Pipeline& target) noexcept = 0;
    protected:
        BindBase() = default;
        virtual ~BindBase() = default;
    };
}
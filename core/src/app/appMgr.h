#pragma once

#include <cstdint>
#include <glm.hpp>
#include <vulkan/vulkan.hpp>

namespace XD::App::AppMgr
{
    int width();
    int height();
    glm::i32vec2 size();

    /// @brief 从上一次调用该函数至今窗口大小是否发生了改变
    /// @return 从上一次调用该函数至今窗口大小是否发生了改变
    bool isSizeChange();

    const char** init(uint32_t& extensionsCount);
    vk::SurfaceKHR createSurf(vk::Instance& vkInst);
    bool inited() noexcept;
}
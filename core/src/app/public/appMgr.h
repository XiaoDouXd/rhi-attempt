#pragma once

#include "SDL_video.h"
#include <cstdint>
#include <glm.hpp>

namespace XD::AppMgr
{
    /// @brief 窗口
    /// @return 窗口句柄
    SDL_Window& wnd();

    /// @brief 窗口宽
    /// @return 窗口宽
    int width();

    /// @brief 窗口高
    /// @return 窗口高
    int height();

    /// @brief 窗口大小
    /// @return 窗口大小
    glm::i32vec2 size();

    /// @brief 从上一次调用该函数至今窗口大小是否发生了改变
    /// @return 从上一次调用该函数至今窗口大小是否发生了改变
    bool isSizeChange();

    /// @brief 初始化
    /// @param extensionsCount vk 扩展信息
    /// @return vk 扩展数量
    void init();

    /// @brief 是否已经初始化
    /// @return 是否已经初始化
    bool inited() noexcept;

    /// @brief 刷新帧
    /// @param quit 是否退出
    /// @param checkPlatformSpecialEvent 是否检查平台特殊事件
    void update(bool& quit, [[maybe_unused]] bool checkPlatformSpecialEvent);

    /// @brief 销毁
    void destroy();
}
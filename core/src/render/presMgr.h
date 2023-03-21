#pragma once

#include <vulkan/vulkan.hpp>

namespace XD::Render { class Semaphore; }

namespace XD::Render::PresMgr
{
    /// @brief 初始化呈示管理器
    /// @param isClear 颜色缓冲是否清空
    void init(bool isClear);

    /// @brief 交换帧缓冲
    /// @param isRebuild 是否重建
    void swap(bool isRebuild = false);

    /// @brief 是否已初始化
    /// @return 是否已初始化
    bool inited() noexcept;

    /// @brief 获得信号量
    /// @return 信号量
    Semaphore requestSemaphore();

    /// @brief 删除信号量
    /// @param 信号量
    void deleteSemaphore(Semaphore&);

    /// @brief 销毁
    void destroy();
} // namespace XD

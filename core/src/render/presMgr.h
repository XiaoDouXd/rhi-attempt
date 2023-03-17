#pragma once

namespace XD::Render::PresMgr
{
    // prresent manager

    /// @brief 初始化呈示管理器
    /// @param isClear 颜色缓冲是否清空
    void init(bool isClear);

    /// @brief 交换帧缓冲
    /// @param isRebuild 是否重建
    void swapBuf(bool isRebuild = false);

    /// @brief 是否已初始化
    /// @return 是否已初始化
    bool inited();
} // namespace XD

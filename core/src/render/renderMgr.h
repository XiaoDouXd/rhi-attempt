#pragma once

namespace XD::Render::Mgr
{
    /// @brief 初始化
    void init();

    /// @brief 开始刷新
    void begUpdate();

    /// @brief 结束刷新
    void endUpdate();

    /// @brief 销毁
    void destroy();
}
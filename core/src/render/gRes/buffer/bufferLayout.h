#pragma once

#include <array>
#include <initializer_list>
#include <memory>
#include <tuple>
#include <vector>

#include "render/gRes/gRes.h"
#include "xdBase/exce.h"

namespace XD::Render
{
    class BufferLayout : public GRes
    {
    public:
        static constexpr GResType gResType = GResType::BufferLayout;

    public:
        /// @brief 创建缓冲布局
        /// @param types 单元内部的类型 (location 为 0~types.size() 顺序排列)
        /// @param rate 设置缓冲单元按实例输入还是按顶点输入
        /// @param order 排序方式
        BufferLayout(
            std::initializer_list<BufferLayoutType> types,
            BufferLayoutRate rate = BufferLayoutRate::Vertex);

        /// @brief 获得实现信息
        /// @return 实现信息
        const GResHolder<gResType>& getImplInfo();

        /// @brief 总大小
        size_t stride() const;

    private:
        size_t                          _stride;
        std::unique_ptr<GResHolderBase> _holder = nullptr;
    };
}
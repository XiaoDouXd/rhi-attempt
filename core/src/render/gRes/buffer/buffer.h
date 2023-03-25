#pragma once

#include <memory>
#include <stdint.h>

#include "render/gRes/gRes.h"

namespace XD::Render
{
    class Buffer : public GRes
    {
    public:
        static constexpr GResType gResType = GResType::Buffer;

    public:
        /// @brief 总大小
        virtual size_t size() const = 0;

        /// @brief 获得实现信息
        /// @return 实现信息
        const GResHolder<gResType>& getImplInfo() const;

    protected:
        Buffer();
        virtual ~Buffer() = default;

        std::unique_ptr<GResHolderBase> _holder = nullptr;
        size_t                          _size;
    };
} // namespace XD::Render

#pragma once

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

#include <memory>
#include <cstdint>

#include "render/public/gRes/gRes.h"

namespace XD::Render
{
    class Buffer : public GRes
    {
    public:
        static constexpr GResType gResType = GResType::Buffer;

    public:
        /// @brief 总大小
        [[nodiscard]] virtual size_t size() const = 0;

        /// @brief 设备 id
        [[nodiscard]] uuids::uuid getDevId() const { return _devId; }

        /// @brief 获得实现信息
        /// @return 实现信息
        [[nodiscard]] const GResHolder<gResType>& getImplInfo() const;

    protected:
        explicit Buffer(const uuids::uuid& devId);
        ~Buffer() override = default;

        std::unique_ptr<GResHolderBase> _holder = nullptr;
        size_t                          _size{};
        uuids::uuid                     _devId;
    };
} // namespace XD::Render

#pragma clang diagnostic pop
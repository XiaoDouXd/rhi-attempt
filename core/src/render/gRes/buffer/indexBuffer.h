#pragma once

#include <vulkan/vulkan.hpp>
#include "render/gRes/buffer/buffer.h"
#include "render/gRes/gRes.h"

namespace XD::Render
{
    class IndexBuffer : public Buffer
    {
    public:
        static constexpr GResType gResType = GResType::IndexBuffer;

    public:
        IndexBuffer(const std::vector<uint32_t>& indices, const uuids::uuid& devId = uuids::uuid());
        void bind(const uuids::uuid& target) noexcept;
        size_t size() const noexcept;

    private:
        size_t              _size;
    };
} // namespace XD::Render

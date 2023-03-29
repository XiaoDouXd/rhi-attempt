#pragma once

#include <span>
#include <stdint.h>

#include "render/gRes/buffer/buffer.h"

namespace XD::Render
{
    class IndexBuffer : public Buffer
    {
    public:
        IndexBuffer(const std::vector<uint32_t>& indices, bool isDynamic = false, const uuids::uuid& devId = uuids::uuid());
        IndexBuffer(const std::span<uint32_t>& indices, bool isDynamic = false, const uuids::uuid& devId = uuids::uuid());
        IndexBuffer(const uint32_t* data, size_t size, bool isDynamic = false, const uuids::uuid& devId = uuids::uuid());
        ~IndexBuffer();

        size_t size() const noexcept override { return _size; }

    private:
        bool _isDynamic;
    };
} // namespace XD::Render

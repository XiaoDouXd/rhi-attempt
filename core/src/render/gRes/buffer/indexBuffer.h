#pragma once

#include "render/gRes/buffer/buffer.h"
#include <stdint.h>

namespace XD::Render
{
    class IndexBuffer : public Buffer
    {
    public:
        IndexBuffer(const std::vector<uint32_t>& indices, const uuids::uuid& devId = uuids::uuid());
        size_t size() const noexcept override;
        ~IndexBuffer();
    };
} // namespace XD::Render

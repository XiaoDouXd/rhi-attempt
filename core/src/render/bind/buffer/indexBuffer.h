#pragma once

#include <vulkan/vulkan.hpp>
#include "render/bind/buffer/bufferBase.h"

namespace XD::Render
{
    class IndexBuffer : public BufferBase
    {
    public:
        IndexBuffer(const std::vector<uint32_t>& indices, const uuids::uuid& devId = uuids::uuid());
        void bind(vk::Pipeline& target) noexcept override;
        size_t size() const noexcept;

    private:
        size_t              _size;
        vk::DeviceMemory    _devMem;
        vk::Buffer          _buf;
    };
} // namespace XD::Render

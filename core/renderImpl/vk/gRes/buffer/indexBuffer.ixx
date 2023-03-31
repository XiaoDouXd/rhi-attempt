#include <memory>
#include <stdint.h>

#include "bufferResHolder.hpp"
#include "render/public/gRes/buffer/indexBuffer.h"
#include "render/public/gRes/gRes.h"
#include "vk/vkMgr.h"
#include "util/public/exce.h"

namespace XD::Render
{
    IndexBuffer::IndexBuffer(const std::vector<uint32_t>& indices, bool isDynamic, const uuids::uuid& devId)
    : Buffer(devId), _isDynamic(isDynamic)
    {
        auto& devPtr = Vk::VkMgr::getDev(devId);
        if (!devPtr) throw Exce(__LINE__, __FILE__, "XD::IndexBuffer Error: dev not found.");
        auto& dev = devPtr.dev;

        auto& holder = getResHolder<gResType>(_holder);

        _size = indices.size() / 3;
        vk::BufferCreateInfo bufInfo;
        bufInfo .setSize(_size * sizeof(uint32_t) * 3)
                .setUsage(vk::BufferUsageFlagBits::eIndexBuffer)
                .setSharingMode(vk::SharingMode::eExclusive);

        holder.buf = dev.createBuffer(bufInfo);
        auto memReq = dev.getBufferMemoryRequirements(holder.buf);

        vk::MemoryAllocateInfo allocInfo;
        allocInfo   .setAllocationSize(memReq.size)
                    .setMemoryTypeIndex(
                        Vk::VkMgr::findMemoryType(memReq.memoryTypeBits,
                        vk::MemoryPropertyFlagBits::eHostVisible |
                        vk::MemoryPropertyFlagBits::eHostCoherent)
                    );
        vk::Result err = dev.allocateMemory(&allocInfo, nullptr, &holder.devMem);
        Vk::VkMgr::checkVkResult(err);
        dev.bindBufferMemory(holder.buf, holder.devMem, 0);

        void* data;
        err = dev.mapMemory(holder.devMem, 0, bufInfo.size, (vk::MemoryMapFlags)0, &data);
        Vk::VkMgr::checkVkResult(err);
        memcpy(data, indices.data(), (size_t)bufInfo.size);
        dev.unmapMemory(holder.devMem);
    }

    IndexBuffer::~IndexBuffer()
    {
        auto& devPtr = Vk::VkMgr::getDev(_devId);
        if (!devPtr) return;
        auto& dev = devPtr.dev;
        auto& holder = getResHolder<gResType>(_holder);

        dev.destroyBuffer(holder.buf);
        dev.freeMemory(holder.devMem);
    }
} // namespace XD::Render

#include "bufferResHolder.hpp"
#include "render/public/gRes/buffer/vertexBuffer.hpp"
#include "vk/vkMgr.h"

namespace XD::Render
{
    VertexBufferBase::VertexBufferBase(const std::span<uint8_t>& data, bool isDynamic, const uuids::uuid& devId)
    : VertexBufferBase(data.data(), data.size(), isDynamic, devId) {}

    VertexBufferBase::VertexBufferBase(const std::vector<uint8_t>& data, bool isDynamic, const uuids::uuid& devId)
    : VertexBufferBase(data.data(), data.size(), isDynamic, devId) {}

    VertexBufferBase::VertexBufferBase(const uint8_t* data, size_t size, bool isDynamic, const uuids::uuid& devId)
    : _isDynamic(isDynamic), Buffer(devId)
    {
        auto& devPtr = Vk::VkMgr::getDev(devId);
        if (!devPtr) throw Exce(__LINE__, __FILE__, "XD::VertexBufferBase Error: dev not found.");
        auto& dev = devPtr.dev;

        _size = size;
        auto& holder = getResHolder<gResType>(_holder);

        vk::BufferCreateInfo bufInfo;
        bufInfo .setSize(_size)
                .setUsage(vk::BufferUsageFlagBits::eVertexBuffer)
                .setSharingMode(vk::SharingMode::eExclusive);
        holder.buf = dev.createBuffer(bufInfo);
        auto memReq = dev.getBufferMemoryRequirements(holder.buf);

        vk::MemoryAllocateInfo allocInfo;
        allocInfo   .setAllocationSize(memReq.size)
                    .setMemoryTypeIndex(
                        Vk::VkMgr::findMemoryType(memReq.memoryTypeBits,
                        vk::MemoryPropertyFlagBits::eHostVisible |
                        vk::MemoryPropertyFlagBits::eHostCoherent));
        auto err = dev.allocateMemory(&allocInfo, nullptr, &holder.devMem);
        Vk::VkMgr::checkVkResult(err);

        dev.bindBufferMemory(holder.buf, holder.devMem, 0);
        void* d;
        err = dev.mapMemory(holder.devMem, 0, _size, (vk::MemoryMapFlags)0, &d);
        Vk::VkMgr::checkVkResult(err);
        memcpy(d, data, _size);
        dev.unmapMemory(holder.devMem);
    }

    bool VertexBufferBase::update(const std::vector<uint8_t>& data)
    { return update(data.data(), data.size()); }

    bool VertexBufferBase::update(const std::span<uint8_t>& data)
    { return update(data.data(), data.size()); }

    bool VertexBufferBase::update(const uint8_t* data, size_t size)
    {
        auto& devPtr = Vk::VkMgr::getDev(_devId);
        if (!devPtr) throw Exce(__LINE__, __FILE__, "XD::VertexBufferBase Error: dev not found.");
        auto& dev = devPtr.dev;
        auto& holder = getResHolder<gResType>(_holder);

        if (!_isDynamic) return false;
        if (size != _size)
        {
            _size = size;
            dev.destroyBuffer(holder.buf);
            dev.freeMemory(holder.devMem);

            vk::BufferCreateInfo bufInfo;
            bufInfo .setSize(_size)
                    .setUsage(vk::BufferUsageFlagBits::eVertexBuffer)
                    .setSharingMode(vk::SharingMode::eExclusive);
            holder.buf = dev.createBuffer(bufInfo);
            auto memReq = dev.getBufferMemoryRequirements(holder.buf);

            vk::MemoryAllocateInfo allocInfo;
            allocInfo   .setAllocationSize(memReq.size)
                        .setMemoryTypeIndex(
                            Vk::VkMgr::findMemoryType(memReq.memoryTypeBits,
                            vk::MemoryPropertyFlagBits::eHostVisible |
                            vk::MemoryPropertyFlagBits::eHostCoherent));
            auto err = dev.allocateMemory(&allocInfo, nullptr, &holder.devMem);
            Vk::VkMgr::checkVkResult(err);
        }

        dev.bindBufferMemory(holder.buf, holder.devMem, 0);
        void* d;
        auto err = dev.mapMemory(holder.devMem, 0, _size, (vk::MemoryMapFlags)0, &d);
        Vk::VkMgr::checkVkResult(err);
        memcpy(d, data, _size);
        dev.unmapMemory(holder.devMem);
        return true;
    }

    VertexBufferBase::~VertexBufferBase()
    {
        auto& devPtr = Vk::VkMgr::getDev(_devId);
        if (!devPtr) return;
        auto& dev = devPtr.dev;
        auto& holder = getResHolder<gResType>(_holder);

        dev.destroyBuffer(holder.buf);
        dev.freeMemory(holder.devMem);
    }
}
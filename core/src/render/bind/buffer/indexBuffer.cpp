
#include "indexBuffer.h"
#include "render/vkMgr.h"
#include "xdBase/xdExce.h"

namespace XD::Render
{
    IndexBuffer::IndexBuffer(const std::vector<uint32_t>& indices, const uuids::uuid& devId) : _devMem(), _buf(), _size(0)
    {
        auto dev = VkMgr::getDev(devId).value().dev;

        _size = indices.size() / 3;
        vk::BufferCreateInfo bufInfo;
        bufInfo .setSize(indices.size() * sizeof(uint32_t))
                .setUsage(vk::BufferUsageFlagBits::eIndexBuffer)
                .setSharingMode(vk::SharingMode::eExclusive);

        _buf = dev.createBuffer(bufInfo, nullptr, indices.data());
        vk::MemoryRequirements memReq;
        dev.getBufferMemoryRequirements(_buf, &memReq);

        vk::MemoryAllocateInfo allocInfo;
        allocInfo   .setAllocationSize(memReq.size)
                    .setMemoryTypeIndex(
                        findMemoryType(memReq.memoryTypeBits,
                        vk::MemoryPropertyFlagBits::eHostVisible |
                        vk::MemoryPropertyFlagBits::eHostCoherent)
                    );
        dev.allocateMemory(&allocInfo, nullptr, &_devMem);
        dev.bindBufferMemory(_buf, _devMem, 0);

        void* data;
        dev.mapMemory(_devMem, 0, bufInfo.size, (vk::MemoryMapFlags)0, &data);
        memcpy(data, indices.data(), (size_t)bufInfo.size);
        dev.unmapMemory(_devMem);
    }

    void IndexBuffer::bind(vk::Pipeline& target)
    {
        
    }
} // namespace XD::Render

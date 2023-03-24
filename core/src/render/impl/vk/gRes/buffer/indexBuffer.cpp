#include "render/gRes/buffer/indexBuffer.h"
#include "render/gRes/gRes.h"
#include "render/impl/vk/vkMgr.h"
#include "xdBase/exce.h"

namespace XD::Render
{
    template<>
    class GResMemHolder<GResType::IndexBuffer> : public GResMemHolderBase
    {
    public:
        vk::DeviceMemory    devMem;
        vk::Buffer          buf;
    };

    IndexBuffer::IndexBuffer(const std::vector<uint32_t>& indices, const uuids::uuid& devId) : _size(indices.size() / 3)
    {
        auto& devPtr = Vk::VkMgr::getDev(devId);
        if (!devPtr) throw Exce(__LINE__, __FILE__, "XD::IndexBuffer Error: dev not found.");
        auto& dev = devPtr.dev;

        vk::BufferCreateInfo bufInfo;
        bufInfo .setSize(_size * sizeof(uint32_t) * 3)
                .setUsage(vk::BufferUsageFlagBits::eIndexBuffer)
                .setSharingMode(vk::SharingMode::eExclusive);

        auto& MemData = GetMemHolder<gResType>(_memHolder);
        MemData.buf = dev.createBuffer(bufInfo);
        vk::MemoryRequirements memReq;
        dev.getBufferMemoryRequirements(MemData.buf, &memReq);

        vk::MemoryAllocateInfo allocInfo;
        allocInfo   .setAllocationSize(memReq.size)
                    .setMemoryTypeIndex(
                        Vk::VkMgr::findMemoryType(memReq.memoryTypeBits,
                        vk::MemoryPropertyFlagBits::eHostVisible |
                        vk::MemoryPropertyFlagBits::eHostCoherent)
                    );
        vk::Result err = dev.allocateMemory(&allocInfo, nullptr, &MemData.devMem);
        Vk::VkMgr::checkVkResult(err);
        dev.bindBufferMemory(MemData.buf, MemData.devMem, 0);

        void* data;
        err = dev.mapMemory(MemData.devMem, 0, bufInfo.size, (vk::MemoryMapFlags)0, &data);
        Vk::VkMgr::checkVkResult(err);
        memcpy(data, indices.data(), (size_t)bufInfo.size);
        dev.unmapMemory(MemData.devMem);
    }

    void IndexBuffer::bind(const uuids::uuid& target) noexcept
    {
        
    }

    size_t IndexBuffer::size() const noexcept { return _size; }
} // namespace XD::Render

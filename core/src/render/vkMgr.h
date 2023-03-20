#pragma once

#include <optional>
#include <vulkan/vulkan.hpp>

#include "uuid.h"

#ifndef NDEBUG
#define XD_VK_DEBUG_REPORT
#endif

namespace XD::Render::VkMgr
{
    struct Dev
    {
    public:
        Dev() = default;
        Dev(const Dev&) = delete;
        Dev(Dev&&) = default;
        ~Dev() = default;

        vk::Device          dev;
        vk::Queue           frontQueue;
        vk::DescriptorPool  descPool;
        std::list<uint32_t> queueFamily;
        operator bool() const { return (bool)dev; }
        Dev& operator=(const Dev&) = delete;
        Dev& operator=(Dev&&) = default;
    };

    bool inited() noexcept;
    void checkVkResultCtype(VkResult err);
    void checkVkResult(vk::Result err);

    void init(const char** extensions, uint32_t extensionsCount);
    void destroy();

    vk::Instance& getInst();
    vk::PhysicalDevice& getPhyDev();
    uint32_t getMinImageCount();
    Dev& getDev(const uuids::uuid& devId = uuids::uuid());

    std::optional<size_t> selectQueueFamily(std::function<bool(const vk::QueueFamilyProperties&, const size_t& idx)>);
    std::optional<uuids::uuid> createDevice(vk::DeviceCreateInfo& devInfo);
    bool createDescPool(const uuids::uuid& devId, vk::DescriptorPoolCreateInfo poolInfo);

    inline uint32_t findMemoryType(uint32_t type_filter, vk::MemoryPropertyFlags properties)
    {
        vk::PhysicalDeviceMemoryProperties mem_prop = VkMgr::getPhyDev().getMemoryProperties();

        for (uint32_t i = 0; i < mem_prop.memoryTypeCount; i++)
            if ((type_filter & (1 << i)) && (mem_prop.memoryTypes[i].propertyFlags & properties) == properties)
                return i;

        return 0xFFFFFFFF;
    }
}
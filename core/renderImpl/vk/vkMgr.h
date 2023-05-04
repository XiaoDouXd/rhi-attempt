#pragma once

#include <optional>
#include <vulkan/vulkan.hpp>
#include <list>

#include "uuid.h"

#ifndef NDEBUG
#define XD_VK_DEBUG_REPORT
#endif

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

namespace XD::Render::Vk::VkMgr
{
    /// @brief 虚拟设备信息
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
        operator bool() const { return (bool)dev; } // NOLINT(google-explicit-constructor)
        Dev& operator=(const Dev&) = delete;
        Dev& operator=(Dev&&) = default;
    };

    /// @brief 是否已初始化
    /// @return 是否已初始化
    bool inited() noexcept;

    /// @brief c 风格的 VkResult 检查
    /// @param err result
    void checkVkResultCType(VkResult err);

    /// @brief c++ 风格的 vk::Result 检查
    /// @param err result
    void checkVkResult(vk::Result err);

    /// @brief 初始化
    /// @param extensions 扩展信息
    /// @param extensionsCount 扩展数量
    void init(const char** extensions, uint32_t extensionsCount);

    /// @brief 销毁
    void destroy();

    /// @brief 获得 vk 实例
    /// @return vk 实例
    vk::Instance& getInst();

    /// @brief 获得物理设备
    /// @return 物理设备
    vk::PhysicalDevice& getPhyDev();

    /// @brief 获得最小帧缓冲数
    /// @return 帧缓冲数
    uint32_t getMinImageCount();

    /// @brief 获得虚拟设备信息
    /// @param devId 虚拟设备 id (不填则为主虚拟设备, 为多设备程序铺路)
    /// @return 虚拟设备信息
    Dev& getDev(const uuids::uuid& devId = uuids::uuid());

    /// @brief 遍历并遴选队列簇
    /// @param 队列簇遴选函数 (返回 true 则认为选中该队列簇并结束遍历)
    /// @return 选中的队列簇
    std::optional<size_t> selectQueueFamily(std::function<bool(const vk::QueueFamilyProperties&, const size_t& idx)>);

    /// @brief 创建虚拟设备
    /// @param devInfo 设备描述符
    /// @return 创建好的虚拟设备的 uuid
    std::optional<uuids::uuid> createDevice(vk::DeviceCreateInfo& devInfo);

    /// @brief 创建描述池
    /// @param devId 虚拟设备 id
    /// @param poolInfo 池描述符
    /// @return 是否创建成功
    bool createDescPool(const uuids::uuid& devId, vk::DescriptorPoolCreateInfo poolInfo);

    /// @brief 遴选内存类型
    /// @param type_filter 目标类型过滤
    /// @param properties 属性
    /// @return 内存类型
    inline uint32_t findMemoryType(uint32_t type_filter, vk::MemoryPropertyFlags properties)
    {
        vk::PhysicalDeviceMemoryProperties mem_prop = VkMgr::getPhyDev().getMemoryProperties();

        for (uint32_t i = 0; i < mem_prop.memoryTypeCount; i++)
            if ((type_filter & (1 << i)) && (mem_prop.memoryTypes[i].propertyFlags & properties) == properties)
                return i;

        return 0xFFFFFFFF;
    }
}
#pragma clang diagnostic pop
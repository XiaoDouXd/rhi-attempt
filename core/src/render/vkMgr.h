#pragma once

#include <memory>
#include <optional>
#include <vulkan/vulkan.hpp>

#include "uuid.h"

#ifndef NDEBUG
#define XD_VK_DEBUG_REPORT
#endif

namespace XD::Render
{
    class VkMgr
    {
    public:
        struct Dev
        {
        public:
            Dev() = default;
            Dev(const Dev&) = delete;
            ~Dev() = default;

            vk::Device          dev;
            vk::Queue           frontQueue;
            vk::DescriptorPool  descPool;
            std::list<uint32_t> queueFamily;
            operator bool() const { return (bool)dev; }
            Dev& operator=(const Dev&) = delete;
        };

    private:
        struct Data
        {
        public:
            struct InitRec
            {
            public:
                bool dev;
                bool phyDev;
                bool queueFamily;
                void clear() { dev = phyDev = queueFamily = false; }
            };

            vk::Instance        ins;
            vk::PhysicalDevice  phyDev;
            Dev                 mainDev;
            std::unordered_map<uuids::uuid, Dev> cDevs;

            uint32_t            minImageCount = 2;
            InitRec             initRec;

            // vk::Device                  dev;
            // vk::Queue                   queue;
            // vk::DescriptorPool          descPool;
            // uint32_t                    queueFamily;


#ifdef XD_VK_DEBUG_REPORT
            vk::DebugReportCallbackEXT  debugReportCB;
#endif
        };
        static std::unique_ptr<Data> _inst;
        static Dev _emptyDev;

    public:
        static bool inited() { return (bool)_inst; }
        static void checkVkResultCtype(VkResult err);
        static void checkVkResult(vk::Result err);

    public:
        static void init(const char** extensions, uint32_t extensionsCount);
        static void destroy();

    public:
        static vk::Instance& getInst() { return _inst->ins; }
        static vk::PhysicalDevice& getPhyDev() { return _inst->phyDev; }
        static vk::Device& getDev() { return _inst->mainDev.dev; }
        static uint32_t getMinImageCount() { return _inst->minImageCount; }
        static Dev& getDev(const uuids::uuid& devId = uuids::uuid())
        {
            if (devId.is_nil()) return _inst->mainDev;
            std::unordered_map<uuids::uuid, Dev>::iterator o;
            if ((o = _inst->cDevs.find(devId)) == _inst->cDevs.end()) return _emptyDev;
            return o->second;
        }

    public:
        static std::optional<size_t> selectQueueFamily(std::function<bool(const vk::QueueFamilyProperties&, const size_t& idx)>);
        static std::optional<uuids::uuid> createDevice(vk::DeviceCreateInfo& devInfo);
        static bool createDescPool(const uuids::uuid& devId, vk::DescriptorPoolCreateInfo poolInfo);

    private:
        static void createInst(const char** extensions, uint32_t extensionsCount);
        static void selectGPU();

        static void selectMainQueueFamily();
        static void createMainDevice();
        static void createMainDescPool();
    };

    inline uint32_t findMemoryType(uint32_t type_filter, vk::MemoryPropertyFlags properties)
    {
        vk::PhysicalDeviceMemoryProperties mem_prop = VkMgr::getPhyDev().getMemoryProperties();

        for (uint32_t i = 0; i < mem_prop.memoryTypeCount; i++)
            if ((type_filter & (1 << i)) && (mem_prop.memoryTypes[i].propertyFlags & properties) == properties)
                return i;

        return 0xFFFFFFFF;
    }
}
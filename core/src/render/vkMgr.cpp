
#include "render/vkMgr.h"
#include "xdBase/exce.h"
#include "xdBase/uuidGen.h"

#include <cassert>
#ifdef XD_VK_DEBUG_REPORT
#include <cstdio>         // printf, fprintf
#include <cstdlib>        // abort
#endif
#include <cstring>        // memcpy
#include <memory>

namespace XD::Render::VkMgr
{
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

#ifdef XD_VK_DEBUG_REPORT
        vk::DebugReportCallbackEXT  debugReportCB;
#endif
    };

    static std::unique_ptr<XD::Render::VkMgr::Data> _inst = nullptr;
    static Dev _emptyDev = Dev();

#ifdef XD_VK_DEBUG_REPORT
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugReport(
        VkDebugReportFlagsEXT flags,
        VkDebugReportObjectTypeEXT objectType,
        uint64_t object, size_t location,
        int32_t messageCode,
        const char* pLayerPrefix,
        const char* pMessage,
        void* pUserData)
    {
        (void)flags; (void)object; (void)location; (void)messageCode; (void)pUserData; (void)pLayerPrefix; // Unused arguments
        fprintf(stderr, "XD::VkMgr 来自对象类型为 %i 的 Debug 报告: \n\nMessage: %s\n\n", objectType, pMessage);
        return VK_FALSE;
    }
#endif

    // -----------------------------------------------------

    bool inited() { return (bool)_inst; }
    vk::Instance& getInst() { return _inst->ins; }
    vk::PhysicalDevice& getPhyDev() { return _inst->phyDev; }
    vk::Device& getDev() { return _inst->mainDev.dev; }
    uint32_t getMinImageCount() { return _inst->minImageCount; }
    Dev& getDev(const uuids::uuid& devId)
    {
        if (devId.is_nil()) return _inst->mainDev;
        std::unordered_map<uuids::uuid, Dev>::iterator o;
        if ((o = _inst->cDevs.find(devId)) == _inst->cDevs.end()) return _emptyDev;
        return o->second;
    }

    // -----------------------------------------------------

    std::optional<size_t> selectQueueFamily(std::function<bool(const vk::QueueFamilyProperties&, const size_t& idx)> func)
    {
        if (!_inst) throw Exce(__LINE__, __FILE__, "XD::VulkanMgr Exce: Instance Empty");
        if (!(_inst->initRec.phyDev)) throw Exce(__LINE__, __FILE__, "XD::VulkanMgr Exce: 没有初始化 GPU");
        auto found = false;
        auto queueProp = _inst->phyDev.getQueueFamilyProperties();
        for(auto i = 0; i < queueProp.size(); i++)
        {
            if (func(queueProp[i], i)) return i;
        }
        return std::nullopt;
    }

    std::optional<uuids::uuid> createDevice(vk::DeviceCreateInfo& devInfo)
    {
        auto uuid = UUID::gen();
        auto& dev = (_inst->cDevs[uuid] = std::move(Dev()));

        auto queueCount = devInfo.queueCreateInfoCount;
        for (size_t i = 0; i < queueCount; i++)
            dev.queueFamily.push_back(devInfo.pQueueCreateInfos[i].queueFamilyIndex);
        dev.dev = _inst->phyDev.createDevice(devInfo);
        dev.frontQueue = dev.dev.getQueue(dev.queueFamily.front(), 0);
        return uuid;
    }

    bool createDescPool(const uuids::uuid& devId, vk::DescriptorPoolCreateInfo poolInfo)
    {
        auto& dev = getDev(devId);
        if (!dev) return false;

        auto err = dev.dev.createDescriptorPool(&poolInfo, nullptr, &(dev.descPool));
        checkVkResult(err);
        return true;
    }

    void checkVkResultCtype(VkResult r)
    {
        if (r == 0) return;
        throw Exce(__LINE__, __FILE__, ("XD::VkMgr Exce: Result = " + std::to_string((int)r)).c_str());
        if (r < 0) abort();
    }

    void checkVkResult(vk::Result r)
    {
        if (r == vk::Result::eSuccess) return;
        throw Exce(__LINE__, __FILE__, ("XD::VkMgr Exce: Result = " + std::to_string((int)r)).c_str());
    }

    void destroy()
    {
        vkDestroyDescriptorPool(_inst->mainDev.dev, _inst->mainDev.descPool, nullptr);
#ifdef XD_VK_DEBUG_REPORT
        auto destroyFunc = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr((_inst->ins), "vkDestroyDebugReportCallbackEXT");
        destroyFunc((_inst->ins), (_inst->debugReportCB), nullptr);
#endif
        vkDestroyDevice(_inst->mainDev.dev, nullptr);
        vkDestroyInstance(_inst->ins, nullptr);
        _inst->initRec.clear();
        _inst.reset();
    }

    // -----------------------------------------------------

    void createInst(const char** extensions, uint32_t extensionsCount)
    {
        if (_inst) return;
        _inst = std::make_unique<XD::Render::VkMgr::Data>();

        // 创建 vk 实例并应用校验层
        vk::InstanceCreateInfo insInfo;
        insInfo.setEnabledExtensionCount(extensionsCount);
        insInfo.setPpEnabledExtensionNames(extensions);

#ifdef XD_VK_DEBUG_REPORT
        // 校验层
        const char* layers[] = { "VK_LAYER_KHRONOS_validation" };
        insInfo.setEnabledLayerCount(1);
        insInfo.setPpEnabledLayerNames(layers);

        // 扩展报告
        uint32_t extensionsExtCount = 1;
        const char** extensionsExt = (const char**)malloc(sizeof(const char*) * (extensionsCount + extensionsExtCount));
        memcpy(extensionsExt, extensions, extensionsCount * sizeof(const char*));
        extensionsExt[extensionsCount] = "VK_EXT_debug_report";
        insInfo.setEnabledExtensionCount(extensionsCount + extensionsExtCount);
        insInfo.setPpEnabledExtensionNames(extensionsExt);

        // 创建 vk 实例
        _inst->ins = vk::createInstance(insInfo);
        free(extensionsExt);

        // 创建校验报告回调函数
        auto vkCreateDebugReportCallbackEXT = reinterpret_cast<PFN_vkDebugReportCallbackEXT>(debugReport);
        auto debugMessengerInfo = vk::DebugReportCallbackCreateInfoEXT()
            .setFlags(vk::DebugReportFlagBitsEXT::eError | vk::DebugReportFlagBitsEXT::eWarning | vk::DebugReportFlagBitsEXT::ePerformanceWarning)
            .setPfnCallback(vkCreateDebugReportCallbackEXT)
            .setPUserData(nullptr);

        vk::DynamicLoader dl;
        auto GetInstanceProcAddr = dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
        vk::DispatchLoaderDynamic dispatch((_inst->ins), GetInstanceProcAddr);
        if (_inst->ins.createDebugReportCallbackEXT(&debugMessengerInfo, 0, &(_inst->debugReportCB), dispatch) != vk::Result::eSuccess)
        { throw Exce(__LINE__, __FILE__, "XD::VulkanMgr Exce: 初始化调试信息失败"); }
#else
        // 创建 vk 实例
        _inst->ins = vk::createInstance(insInfo);
#endif
    }

    void selectGPU()
    {
        if (!_inst) throw Exce(__LINE__, __FILE__, "XD::VkMgr Exce: Instance Empty");
        if (_inst->initRec.phyDev) throw Exce(__LINE__, __FILE__, "XD::VkMgr Exce: 重复选择 GPU");
        auto allGPU = _inst->ins.enumeratePhysicalDevices();
        if (allGPU.empty()) throw Exce(__LINE__, __FILE__, "XD::VkMgr Exce: 找不到可用 GPU");

        uint32_t selectedGpu = 0;
        for (auto i = std::size_t(0); i < selectedGpu; i++)
        {
            if (allGPU[i].getProperties().deviceType ==
                vk::PhysicalDeviceType::eDiscreteGpu)
            {
                selectedGpu = i;
                break;
            }
        }
        _inst->phyDev = allGPU[selectedGpu];
        _inst->initRec.phyDev = true;
    }

    void selectMainQueueFamily()
    {
        if (!_inst) throw Exce(__LINE__, __FILE__, "XD::VulkanMgr Exce: Instance Empty");
        if (!(_inst->initRec.phyDev)) throw Exce(__LINE__, __FILE__, "XD::VulkanMgr Exce: 没有初始化 GPU");
        auto found = false;
        auto queueProp = _inst->phyDev.getQueueFamilyProperties();
        for(auto i = 0; i < queueProp.size(); i++)
        {
            if (queueProp[i].queueFlags & vk::QueueFlagBits::eGraphics)
            {
                _inst->mainDev.queueFamily.push_back(i);
                found = true;
                _inst->initRec.queueFamily = true;
                return;
            }
        }
    }

    void createMainDevice()
    {
        if (!_inst) throw Exce(__LINE__, __FILE__, "XD::VkMgr Exce: Instance Empty");
        if (!(_inst->initRec.queueFamily)) throw Exce(__LINE__, __FILE__, "XD::VkMgr Exce: 没有初始化队列簇");

        int devExtCount = 1;
        const char* devExt[] = { "VK_KHR_swapchain" };
        const float queuePriority[] = { 1.0f };
        vk::DeviceQueueCreateInfo queueInfo[1] = {};
        queueInfo[0].setQueueFamilyIndex(_inst->mainDev.queueFamily.front());
        queueInfo[0].setQueueCount(1);
        queueInfo[0].setPQueuePriorities(queuePriority);
        vk::DeviceCreateInfo devInfo;
        devInfo.setQueueCreateInfoCount(sizeof(queueInfo) / sizeof(queueInfo[0]));
        devInfo.setPQueueCreateInfos(queueInfo);
        devInfo.setEnabledExtensionCount(devExtCount);
        devInfo.setPpEnabledExtensionNames(devExt);
        _inst->mainDev.dev = _inst->phyDev.createDevice(devInfo);
        _inst->mainDev.frontQueue = _inst->mainDev.dev.getQueue(_inst->mainDev.queueFamily.front(), 0);
        _inst->initRec.dev = true;
    }

    void createMainDescPool()
    {
        if (!_inst) throw Exce(__LINE__, __FILE__, "XD::VulkanMgr Exce: Instance Empty");
        if (!(_inst->initRec.dev)) throw Exce(__LINE__, __FILE__, "XD::VulkanMgr Exce: 没有初始化 Dev");

#define ARRAYSIZE(_ARR)          ((int)(sizeof(_ARR) / sizeof(*(_ARR))))
        vk::DescriptorPoolSize poolSizes[] =
        {
            {vk::DescriptorType::eSampler, 1000},
            {vk::DescriptorType::eCombinedImageSampler, 1000},
            {vk::DescriptorType::eSampledImage, 1000},
            {vk::DescriptorType::eStorageImage, 1000},
            {vk::DescriptorType::eUniformTexelBuffer, 1000},
            {vk::DescriptorType::eStorageTexelBuffer, 1000},
            {vk::DescriptorType::eUniformBuffer, 1000},
            {vk::DescriptorType::eStorageBuffer, 1000},
            {vk::DescriptorType::eUniformBufferDynamic, 1000},
            {vk::DescriptorType::eStorageBufferDynamic, 1000},
            {vk::DescriptorType::eInputAttachment, 1000}
        };
        uint32_t poolSizesSize = ARRAYSIZE(poolSizes);
        vk::DescriptorPoolCreateInfo poolInfo = {};
        poolInfo.setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet);
        poolInfo.setMaxSets(1000 * poolSizesSize);
        poolInfo.setPoolSizeCount(poolSizesSize);
        poolInfo.setPPoolSizes(poolSizes);
        auto err = _inst->mainDev.dev.createDescriptorPool(&poolInfo, nullptr, &(_inst->mainDev.descPool));
        checkVkResult(err);
#undef ARRAYSIZE
    }

    void init(const char** extensions, uint32_t extensionsCount)
    {
        if (_inst) return;
        createInst(extensions, extensionsCount);
        selectGPU();
        selectMainQueueFamily();
        createMainDevice();
        createMainDescPool();
    }
} // namespace XD

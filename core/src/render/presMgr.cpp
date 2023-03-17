#include <memory>
#include <SDL3/SDL_vulkan.h>
#include <SDL3/SDL.h>

#include "xdBase/entrance.h"
#include "xdBase/exce.h"
#include "vkMgr.h"

namespace XD::Render::PresMgr
{
    struct FormatData
    {
    public:
        vk::SurfaceFormatKHR        surfFormat;
        vk::PresentModeKHR          presMode;
        uint32_t                    minImgCountFromPresMode;
        vk::SurfaceCapabilitiesKHR  surfCapabilities;
        uint32_t                    minImgCount;

        vk::ImageSubresourceRange   imgSubresRange;
        vk::ImageViewCreateInfo     imgViewCreateInfo;

        bool                        isClearEnable;
    };

    struct FrameData
    {
    public:
        vk::Fence                   fence;
        vk::Framebuffer             frameBuf;
        vk::CommandBuffer           cmdBuf;
        vk::CommandPool             cmdPool;
        vk::Image                   img;
        vk::ImageView               imgView;

        vk::Semaphore               imgAcquired;
        vk::Semaphore               renderComplete;
    };

    struct Data
    {
    public: // --- sdl
        SDL_Window*                 window;

    public: // --- vulkan
        vk::SurfaceKHR              surf;
        vk::SwapchainKHR            swapchain;

        std::vector<FrameData>      frames;
        vk::RenderPass              mainRenderPass;
        vk::Pipeline                mainPipeline;
        FormatData                  swapchainFormat;
    };

    static std::unique_ptr<XD::Render::PresMgr::Data> _inst = nullptr;

    // -----------------------------------------------------

    void createWindow()
    {
        const char* windowName = "wndName";

        // 绑定到 vulkan 实例
        SDL_WindowFlags wndFlags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
        _inst->window = SDL_CreateWindow(xdWndInitConf_wndName, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, xdWndInitConf_defWndWidth - 1, xdWndInitConf_defWndHeight, wndFlags);
        SDL_SetWindowMinimumSize(_inst->window, xdWndInitConf_lodingWidth, xdWndInitConf_lodingHeight);

        // 遴选支持的表面格式 和 呈现格式
        auto surfFormats = VkMgr::getPhyDev().getSurfaceFormatsKHR();
        auto presentModes = VkMgr::getPhyDev().getSurfacePresentModesKHR();

        size_t targetFormat = SIZE_MAX;
        std::array targetVkFormat =
        {
            std::pair(vk::Format::eR8G8B8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear),
            std::pair(vk::Format::eR8G8B8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear),
            std::pair(vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear),
            std::pair(vk::Format::eB8G8R8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear),
            std::pair(vk::Format::eR8G8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear),
            std::pair(vk::Format::eR8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear),
        };
        for (auto& vkFormat : targetVkFormat)
        {
            for (size_t i = 0; i < surfFormats.size(); i++)
            {
                if (surfFormats[i].format == vkFormat.first && surfFormats[i].colorSpace == vkFormat.second)
                {
                    targetFormat = i;
                    break;
                }
            }
            if (targetFormat != SIZE_MAX) break;
        }
        targetFormat = targetFormat == SIZE_MAX ? 0 : targetFormat;

        size_t targetPresentMode = SIZE_MAX;
        std::array targetVkPresentMode =
        {
            vk::PresentModeKHR::eFifoRelaxed,
            vk::PresentModeKHR::eFifo,
            vk::PresentModeKHR::eMailbox,
            vk::PresentModeKHR::eImmediate,
        };
        for (auto vkPrescntMode : targetVkPresentMode)
        {
            for (size_t i = 0; i < presentModes.size(); i++)
            {
                if (presentModes[i] == vkPrescntMode)
                {
                    targetPresentMode = i;
                    break;
                }
            }
            if (targetPresentMode != SIZE_MAX) break;
        }
        targetPresentMode = targetPresentMode == SIZE_MAX ? 0 : targetPresentMode;

        uint32_t minImgCount = 2;
        switch (presentModes[targetPresentMode]) {
        case vk::PresentModeKHR::eImmediate: minImgCount = 1; break;
        case vk::PresentModeKHR::eFifo:
        case vk::PresentModeKHR::eFifoRelaxed: minImgCount = 2; break;
        case vk::PresentModeKHR::eMailbox: minImgCount = 3; break;
        default: break;}
        _inst->swapchainFormat.minImgCountFromPresMode = minImgCount;
        _inst->swapchainFormat.presMode = presentModes[targetPresentMode];
        _inst->swapchainFormat.surfFormat = surfFormats[targetFormat];

        vk::ImageSubresourceRange imgSubresRange;
        imgSubresRange  .setAspectMask(
                            vk::ImageAspectFlagBits::eColor |
                            vk::ImageAspectFlagBits::eDepth |
                            vk::ImageAspectFlagBits::eStencil)
                        .setBaseArrayLayer(0)
                        .setLayerCount(0)
                        .setBaseMipLevel(0)
                        .setLevelCount(1);
        vk::ImageViewCreateInfo imgViewCreateInfo;
        imgViewCreateInfo   .setViewType(vk::ImageViewType::e2D)
                            .setFormat(_inst->swapchainFormat.surfFormat.format)
                            .setComponents(vk::ComponentMapping())
                            .setSubresourceRange(imgSubresRange);
        _inst->swapchainFormat.imgSubresRange = imgSubresRange;
        _inst->swapchainFormat.imgViewCreateInfo = imgViewCreateInfo;
    }

    void createSurf()
    {
        VkSurfaceKHR surf = {};
        if (SDL_Vulkan_CreateSurface(_inst->window, VkMgr::getInst(), &surf) == 0)
            throw XD::Exce(__LINE__, __FILE__, "XD::Render::PresMgr: SDL Vulkan Surface 创建失败");
        _inst->surf = surf;

        auto surfCapabilities = VkMgr::getPhyDev().getSurfaceCapabilitiesKHR(_inst->surf);
        uint32_t minImgCount = _inst->swapchainFormat.minImgCountFromPresMode;
        minImgCount = std::max(surfCapabilities.minImageCount, minImgCount);
        minImgCount = surfCapabilities.maxImageCount ?
            std::min(surfCapabilities.maxImageCount, minImgCount) : minImgCount;
        _inst->swapchainFormat.surfCapabilities = surfCapabilities;
        _inst->swapchainFormat.minImgCount = minImgCount;
    }

    void createRenderPass_whileResetSwapchain(vk::Device& dev)
    {
        vk::AttachmentDescription attachment;
        attachment  .setFormat(_inst->swapchainFormat.surfFormat.format)
                    .setSamples(vk::SampleCountFlagBits::e1)
                    .setLoadOp(_inst->swapchainFormat.isClearEnable ? vk::AttachmentLoadOp::eClear : vk::AttachmentLoadOp::eDontCare)
                    .setStoreOp(vk::AttachmentStoreOp::eStore)
                    .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
                    .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
                    .setInitialLayout(vk::ImageLayout::eUndefined)
                    .setFinalLayout(vk::ImageLayout::ePresentSrcKHR);
        vk::AttachmentReference attachmentRef;
        attachmentRef.setAttachment(0).setLayout(vk::ImageLayout::eColorAttachmentOptimal);
        vk::SubpassDescription subpass;
        subpass .setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
                .setColorAttachmentCount(1)
                .setPColorAttachments(&attachmentRef);
        vk::SubpassDependency subpassDependency;
        subpassDependency   .setSrcSubpass(VK_SUBPASS_EXTERNAL)
                            .setDstSubpass(0)
                            .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
                            .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
                            .setSrcAccessMask(vk::AccessFlagBits::eNone)
                            .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite);
        vk::RenderPassCreateInfo createInfo;
        createInfo  .setAttachmentCount(1)
                    .setPAttachments(&attachment)
                    .setSubpassCount(1)
                    .setPSubpasses(&subpass)
                    .setDependencyCount(1)
                    .setPDependencies(&subpassDependency);
        _inst->mainRenderPass = dev.createRenderPass(createInfo);
    }

    void resetSwapchain()
    {
        static int w, h;
        auto& dev = VkMgr::getDev().dev;
        SDL_GetWindowSize(_inst->window, &w, &h);

        if (_inst->frames.size())
        {
            // img 字段是由交换链给出的 其生命周期受到交换链控制
            for (auto& frame : _inst->frames)
            {
                dev.destroyFence(frame.fence);
                dev.freeCommandBuffers(frame.cmdPool, {frame.cmdBuf});
                dev.destroyCommandPool(frame.cmdPool);
                dev.destroyImageView(frame.imgView);
                dev.destroyFramebuffer(frame.frameBuf);
                dev.destroySemaphore(frame.imgAcquired);
                dev.destroySemaphore(frame.renderComplete);
            }
            _inst->frames.clear();
        }
        if (_inst->mainRenderPass) dev.destroyRenderPass(_inst->mainRenderPass);
        if (_inst->mainPipeline) dev.destroyPipeline(_inst->mainPipeline);

        vk::SwapchainKHR oldSwapchain = _inst->swapchain;
        vk::SwapchainCreateInfoKHR createInfo;
        createInfo  .setSurface(_inst->surf)
                    .setMinImageCount(_inst->swapchainFormat.minImgCount)
                    .setImageFormat(_inst->swapchainFormat.surfFormat.format)
                    .setImageColorSpace(_inst->swapchainFormat.surfFormat.colorSpace)
                    .setPresentMode(_inst->swapchainFormat.presMode)
                    .setImageArrayLayers(1)
                    .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
                    .setPreTransform(_inst->swapchainFormat.surfCapabilities.currentTransform)
                    .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::ePreMultiplied)
                    .setClipped(true)
                    .setOldSwapchain(oldSwapchain);
        if (_inst->swapchainFormat.surfCapabilities.currentExtent.width == 0xffffffff)
        {
            createInfo.imageExtent.width = w;
            createInfo.imageExtent.height = h;
        }
        else
        {
            createInfo.imageExtent.width = _inst->swapchainFormat.surfCapabilities.currentExtent.width;
            createInfo.imageExtent.height = _inst->swapchainFormat.surfCapabilities.currentExtent.height;
        }

        _inst->swapchain = dev.createSwapchainKHR(createInfo);
        if (oldSwapchain) dev.destroySwapchainKHR(oldSwapchain);
        auto images = dev.getSwapchainImagesKHR(_inst->swapchain);
        _inst->frames.resize(images.size());

        vk::ImageSubresourceRange subresRange = {};
        subresRange .setAspectMask(
                        vk::ImageAspectFlagBits::eColor |
                        vk::ImageAspectFlagBits::eDepth |
                        vk::ImageAspectFlagBits::eStencil)
                    .setBaseMipLevel(0)
                    .setLevelCount(1)
                    .setBaseArrayLayer(0)
                    .setLayerCount(0);

        // -------------------------------------- 创建渲染通道
        createRenderPass_whileResetSwapchain(dev);
        // --------------------------------------

        vk::FramebufferCreateInfo frameBufCreateInfo;
        frameBufCreateInfo  .setAttachmentCount(1)
                            .setWidth(w)
                            .setHeight(h)
                            .setLayers(1)
                            .setRenderPass(_inst->mainRenderPass);

        for (size_t i = 0; i < images.size(); i++)
        {
            auto& img = images[i];
            auto& frame = _inst->frames[i];

            frame.img = img;
            frame.imgView = dev.createImageView(_inst->swapchainFormat.imgViewCreateInfo.setImage(img));
            frame.frameBuf = dev.createFramebuffer(frameBufCreateInfo.setPAttachments(&frame.imgView));

            // 创建 cmdbuf
            vk::CommandPoolCreateInfo cmdPoolCreateInfo;
            cmdPoolCreateInfo   .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
                                .setQueueFamilyIndex(VkMgr::getDev().queueFamily.front());
            frame.cmdPool = dev.createCommandPool(cmdPoolCreateInfo);
            vk::CommandBufferAllocateInfo cmdBufAllocInfo;
            cmdBufAllocInfo .setCommandPool(frame.cmdPool)
                            .setLevel(vk::CommandBufferLevel::ePrimary)
                            .setCommandBufferCount(1);
            frame.cmdBuf = dev.allocateCommandBuffers(cmdBufAllocInfo).back();

            // 创建围挡和状态量
            vk::FenceCreateInfo fenceCreateInfo;
            fenceCreateInfo .setFlags(vk::FenceCreateFlagBits::eSignaled);
            frame.fence = dev.createFence(fenceCreateInfo);
            vk::SemaphoreCreateInfo semaphoreCreateInfo;
            frame.imgAcquired    = dev.createSemaphore(semaphoreCreateInfo);
            frame.renderComplete = dev.createSemaphore(semaphoreCreateInfo);
        }
    }

    // -----------------------------------------------------

    bool inited() { return (bool)_inst; }

    /// @brief 初始化呈示管理器
    /// @param isClear 颜色缓冲是否清空
    void init(bool isClear)
    {
        if (inited()) throw Exce(__LINE__, __FILE__, "XD::Render::PresMgr: 重复初始化");
        _inst->swapchainFormat.isClearEnable = isClear;
        _inst = std::make_unique<XD::Render::PresMgr::Data>();

        createWindow();
        createSurf();
        resetSwapchain();
    }
}
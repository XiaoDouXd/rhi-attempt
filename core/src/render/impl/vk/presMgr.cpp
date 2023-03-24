#include <memory>
#include <SDL3/SDL_vulkan.h>
#include <SDL3/SDL.h>

#include "app/appMgr.h"
#include "render/impl/vk/presMgr.h"
#include "render/impl/vk/vkMgr.h"
#include "vkMgr.h"
#include "xdBase/entrance.h"
#include "xdBase/exce.h"

namespace XD::Render::Vk::PresMgr
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
        vk::Image                   img;
        vk::ImageView               imgView;
        vk::Semaphore               imgAcquired;
    };

    struct Data
    {
    public:
        vk::SurfaceKHR              surf;
        vk::SwapchainKHR            swapchain;

        std::vector<FrameData>      frames;
        uint32_t                    frameIdx;
        vk::RenderPass              mainRenderPass;
        vk::Pipeline                mainPipeline;
        FormatData                  swapchainFormat;
        bool                        rebuildSwapchain;

        std::list<vk::Semaphore>    renderSemaphores;
        std::vector<vk::Semaphore>  renderSemaphoresCache;
        bool                        renderSemaphoreDirty;
    };

    static std::unique_ptr<XD::Render::Vk::PresMgr::Data> _inst = nullptr;

    // -----------------------------------------------------

    void createWindow()
    {
        if (!_inst) throw Exce(__LINE__, __FILE__, "XD::App::AppMgr: 未初始化 SDL");

        VkSurfaceKHR surf = {};
        if (SDL_Vulkan_CreateSurface(&AppMgr::wnd(), VkMgr::getInst(), &surf) == 0)
            throw XD::Exce(__LINE__, __FILE__, "XD::Render::PresMgr: SDL Vulkan Surface 创建失败");
        _inst->surf = surf;

        // 遴选支持的表面格式 和 呈现格式
        auto surfFormats = VkMgr::getPhyDev().getSurfaceFormatsKHR(_inst->surf);
        auto presentModes = VkMgr::getPhyDev().getSurfacePresentModesKHR(_inst->surf);

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
        imgSubresRange  .setAspectMask(vk::ImageAspectFlagBits::eColor)
                        .setLayerCount(VK_REMAINING_ARRAY_LAYERS)
                        .setBaseArrayLayer(0)
                        .setLevelCount(1)
                        .setBaseMipLevel(0);
        vk::ImageViewCreateInfo imgViewCreateInfo;
        imgViewCreateInfo   .setViewType(vk::ImageViewType::e2D)
                            .setFormat(_inst->swapchainFormat.surfFormat.format)
                            .setComponents(vk::ComponentMapping())
                            .setSubresourceRange(imgSubresRange);
        _inst->swapchainFormat.imgSubresRange = imgSubresRange;
        _inst->swapchainFormat.imgViewCreateInfo = imgViewCreateInfo;

        auto surfCapabilities = VkMgr::getPhyDev().getSurfaceCapabilitiesKHR(_inst->surf);
        minImgCount = _inst->swapchainFormat.minImgCountFromPresMode;
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
        if (AppMgr::isSizeChange() && _inst->swapchain) return;
        auto size = AppMgr::size();
        auto& dev = VkMgr::getDev().dev;

        if (_inst->frames.size())
        {
            // img 字段是由交换链给出的 其生命周期受到交换链控制
            for (auto& frame : _inst->frames)
            {
                dev.destroyFence(frame.fence);
                dev.destroyImageView(frame.imgView);
                dev.destroyFramebuffer(frame.frameBuf);
                dev.destroySemaphore(frame.imgAcquired);
            }
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
                    // .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::ePreMultiplied)
                    .setClipped(true);
        if (oldSwapchain) createInfo.setOldSwapchain(oldSwapchain);
        if (_inst->swapchainFormat.surfCapabilities.currentExtent.width == 0xffffffff)
        {
            createInfo.imageExtent.width = size.x;
            createInfo.imageExtent.height = size.y;
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

        // -------------------------------------- 创建渲染通道
        createRenderPass_whileResetSwapchain(dev);
        // --------------------------------------

        vk::FramebufferCreateInfo frameBufCreateInfo;
        frameBufCreateInfo  .setAttachmentCount(1)
                            .setWidth(size.x)
                            .setHeight(size.y)
                            .setLayers(1)
                            .setRenderPass(_inst->mainRenderPass);

        for (size_t i = 0; i < images.size(); i++)
        {
            auto& img = images[i];
            auto& frame = _inst->frames[i];

            frame.img = img;
            frame.imgView = dev.createImageView(_inst->swapchainFormat.imgViewCreateInfo.setImage(img));
            frame.frameBuf = dev.createFramebuffer(frameBufCreateInfo.setPAttachments(&frame.imgView));

            // 创建围挡和状态量
            vk::FenceCreateInfo fenceCreateInfo;
            fenceCreateInfo .setFlags(vk::FenceCreateFlagBits::eSignaled);
            frame.fence = dev.createFence(fenceCreateInfo);

            vk::SemaphoreCreateInfo semaphoreCreateInfo;
            frame.imgAcquired = dev.createSemaphore(semaphoreCreateInfo);
        }
    }

    // -----------------------------------------------------

    bool inited() noexcept { return (bool)_inst; }

    void init(bool isClear)
    {
        if (inited()) throw Exce(__LINE__, __FILE__, "XD::Render::PresMgr: 重复初始化");
        _inst = std::make_unique<XD::Render::Vk::PresMgr::Data>();
        _inst->swapchainFormat.isClearEnable = isClear;
        _inst->renderSemaphoreDirty = true;
        _inst->rebuildSwapchain = true;

        createWindow();
        resetSwapchain();
    }

    void begRender()
    {
        if (_inst->rebuildSwapchain) resetSwapchain();
        auto& dev = VkMgr::getDev().dev;
        auto& lastFrame = _inst->frames[_inst->frameIdx];
        auto resVal = dev.acquireNextImageKHR(_inst->swapchain, UINT64_MAX, lastFrame.imgAcquired);
        if (resVal.result == vk::Result::eErrorOutOfDateKHR || resVal.result == vk::Result::eSuboptimalKHR)
        { _inst->rebuildSwapchain = true; return; }
        else VkMgr::checkVkResult(resVal.result);
        _inst->frameIdx = resVal.value;
    }

    void endRender(bool isRebuild)
    {
        auto& dev = VkMgr::getDev().dev;
        auto& queue = VkMgr::getDev().frontQueue;
        auto idx = _inst->frameIdx;
        auto& frame = _inst->frames[_inst->frameIdx];

        auto err = dev.waitForFences({frame.fence}, true, UINT64_MAX);
        VkMgr::checkVkResult(err);
        dev.resetFences({frame.fence});

        if (_inst->renderSemaphoreDirty)
        {
            size_t i = 0;
            _inst->renderSemaphoresCache.resize(_inst->renderSemaphores.size() + 1);
            for (const auto& semaphore : _inst->renderSemaphores)
                _inst->renderSemaphoresCache[i++] = semaphore;
            _inst->renderSemaphoreDirty = false;
        }

        vk::PresentInfoKHR presentInfo;
        presentInfo.setWaitSemaphores(_inst->renderSemaphoresCache)
            .setSwapchainCount(1)
            .setPSwapchains(&_inst->swapchain)
            .setPImageIndices(&idx);
        auto result = queue.presentKHR(presentInfo);
        if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR)
            _inst->rebuildSwapchain = true;
        else VkMgr::checkVkResult(result);
    }

    void destroy()
    {
        if (!inited()) return;
        auto& dev = VkMgr::getDev().dev;
        if (_inst->frames.size())
        {
            // img 字段是由交换链给出的 其生命周期受到交换链控制
            for (auto& frame : _inst->frames)
            {
                dev.destroyFence(frame.fence);
                dev.destroyImageView(frame.imgView);
                dev.destroyFramebuffer(frame.frameBuf);
                dev.destroySemaphore(frame.imgAcquired);
            }
            _inst->frames.clear();
        }
        if (_inst->mainRenderPass) dev.destroyRenderPass(_inst->mainRenderPass);
        if (_inst->mainPipeline) dev.destroyPipeline(_inst->mainPipeline);
        if (_inst->swapchain) dev.destroySwapchainKHR(_inst->swapchain);
        _inst.reset();
    }

    // ----------------------------------------------------- 信号量

    Semaphore requestSemaphore()
    {
        auto& dev = VkMgr::getDev().dev;
        XD::Render::Vk::Semaphore o;
        vk::SemaphoreCreateInfo info;
        auto inst = dev.createSemaphore(info);

        _inst->renderSemaphores.emplace_front(std::move(inst));
        o._itr = _inst->renderSemaphores.begin();

        _inst->renderSemaphoreDirty = true;
        return o;
    }
}


namespace XD::Render::Vk
{
    void Semaphore::del()
    {
        if (_itr == PresMgr::_inst->renderSemaphores.cend()) return;
        auto& dev = VkMgr::getDev().dev;
        dev.destroySemaphore(*this);
        PresMgr::_inst->renderSemaphores.erase(_itr);
        _itr = PresMgr::_inst->renderSemaphores.cend();
        PresMgr::_inst->renderSemaphoreDirty = true;
    }

    void Semaphore::set(const uint64_t& value)
    {
        auto& dev = VkMgr::getDev().dev;
        vk::SemaphoreSignalInfo info;
        info.setSemaphore(*this)
            .setValue(value);
        dev.signalSemaphore(info);
    }

    constexpr Semaphore::operator bool() const
    {
        return _itr != PresMgr::_inst->renderSemaphores.cend();
    }
}
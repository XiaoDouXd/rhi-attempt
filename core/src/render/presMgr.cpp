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
    };

    struct FrameData
    {
    public:
        vk::CommandBuffer           cmdBuf;
        vk::CommandPool             cmdPool;
        vk::Fence                   Fence;
        vk::Image                   Backbuffer;
        vk::ImageView               BackbufferView;
        vk::Framebuffer             Framebuffer;
    };

    struct Data
    {
    public: // --- sdl
        SDL_Window*                 window;

    public: // --- vulkan
        vk::SurfaceKHR              surf;
        vk::SwapchainKHR            swapchain;
        std::vector<vk::Image>      scImages;
        std::vector<vk::ImageView>  scImageViews;

        FormatData                  format;
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
        _inst->format.minImgCountFromPresMode = minImgCount;
        _inst->format.presMode = presentModes[targetPresentMode];
        _inst->format.surfFormat = surfFormats[targetFormat];
    }

    void createSurf()
    {
        VkSurfaceKHR surf = {};
        if (SDL_Vulkan_CreateSurface(_inst->window, VkMgr::getInst(), &surf) == 0)
            throw XD::Exce(__LINE__, __FILE__, "XD::PresMgr: SDL Vulkan Surface 创建失败");
        _inst->surf = surf;

        auto surfCapabilities = VkMgr::getPhyDev().getSurfaceCapabilitiesKHR(_inst->surf);
        uint32_t minImgCount = _inst->format.minImgCountFromPresMode;
        minImgCount = std::max(surfCapabilities.minImageCount, minImgCount);
        minImgCount = surfCapabilities.maxImageCount ?
            std::min(surfCapabilities.maxImageCount, minImgCount) : minImgCount;
        _inst->format.surfCapabilities = surfCapabilities;
        _inst->format.minImgCount = minImgCount;
    }

    void createFrameData()
    {

    }

    void resetSwapchain()
    {
        vk::SwapchainKHR oldSwapchain = _inst->swapchain;
        vk::SwapchainCreateInfoKHR createInfo;
        createInfo  .setSurface(_inst->surf)
                    .setMinImageCount(_inst->format.minImgCount)
                    .setImageFormat(_inst->format.surfFormat.format)
                    .setImageColorSpace(_inst->format.surfFormat.colorSpace)
                    .setPresentMode(_inst->format.presMode)
                    .setImageArrayLayers(1)
                    .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
                    .setPreTransform(_inst->format.surfCapabilities.currentTransform)
                    .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::ePreMultiplied)
                    .setClipped(true)
                    .setOldSwapchain(oldSwapchain);
        if (_inst->format.surfCapabilities.currentExtent.width == 0xffffffff)
        {
            static int w, h;
            SDL_GetWindowSize(_inst->window, &w, &h);
            createInfo.imageExtent.width = w;
            createInfo.imageExtent.height = h;
        }
        else
        {
            createInfo.imageExtent.width = _inst->format.surfCapabilities.currentExtent.width;
            createInfo.imageExtent.height = _inst->format.surfCapabilities.currentExtent.height;
        }

        auto& dev = VkMgr::getDev().dev;
        _inst->swapchain = dev.createSwapchainKHR(createInfo);
        _inst->scImages = dev.getSwapchainImagesKHR(_inst->swapchain);
        _inst->scImageViews.resize(_inst->scImages.size());

        vk::ImageSubresourceRange subresRange = {};
        subresRange .setAspectMask(
                        vk::ImageAspectFlagBits::eColor |
                        vk::ImageAspectFlagBits::eDepth |
                        vk::ImageAspectFlagBits::eStencil)
                    .setBaseMipLevel(0)
                    .setLevelCount(1)
                    .setBaseArrayLayer(0)
                    .setLayerCount(0);

        for (size_t i = 0; i < _inst->scImages.size(); i++)
        {
            auto& scImage = _inst->scImages[i];
            auto& scImageView = _inst->scImageViews[i];

            vk::ImageViewCreateInfo imgViewInfo = {};
            imgViewInfo .setImage(scImage)
                        .setViewType(vk::ImageViewType::e2D)
                        .setFormat(_inst->format.surfFormat.format)
                        .setComponents(vk::ComponentMapping())
                        .setSubresourceRange(subresRange);
            scImageView = dev.createImageView(imgViewInfo);
        }
        if (oldSwapchain) dev.destroySwapchainKHR(oldSwapchain);
    }

    // -----------------------------------------------------

    bool inited() { return (bool)_inst; }
    void init()
    {
        if (inited()) return;
        _inst = std::make_unique<XD::Render::PresMgr::Data>();
    }
}
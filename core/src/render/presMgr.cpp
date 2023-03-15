#include <memory>
#include <SDL3/SDL_vulkan.h>
#include <SDL3/SDL.h>

#include "xdBase/entrance.h"
#include "xdBase/exce.h"
#include "vkMgr.h"

namespace XD::Render::PresMgr
{
    struct Data
    {
    public: // --- sdl
        SDL_Window* window;

    public: // --- vulkan
        vk::SurfaceKHR surf;
        vk::SwapchainKHR swapchain;

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
    }

    void createSurf()
    {
        auto surfFormats = VkMgr::getPhyDev().getSurfaceFormatsKHR();
        auto presentModes = VkMgr::getPhyDev().getSurfacePresentModesKHR();

        size_t targetFormat = SIZE_MAX;
        std::array targetVkFormat =
        {
            std::pair(vk::Format::eR8G8B8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear),
            std::pair(vk::Format::eR8G8B8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear),
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
            vk::PresentModeKHR::eMailbox,
            vk::PresentModeKHR::eImmediate,
            vk::PresentModeKHR::eFifoRelaxed,
            vk::PresentModeKHR::eFifo,
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

        VkSurfaceKHR surf = {};
        if (SDL_Vulkan_CreateSurface(_inst->window, VkMgr::getInst(), &surf) == 0)
            throw XD::Exce(__LINE__, __FILE__, "XD::PresMgr: SDL Vulkan Surface 创建失败");
        _inst->surf = surf;
        auto surfCapabilities = VkMgr::getPhyDev().getSurfaceCapabilitiesKHR(_inst->surf);

        vk::SwapchainCreateInfoKHR createInfo;
        createInfo  .setSurface(_inst->surf)
                    .setMinImageCount(surfCapabilities.minImageCount + 1)
                    .setImageFormat(surfFormats[targetFormat].format)
                    .setImageColorSpace(surfFormats[targetFormat].colorSpace)
                    .setPresentMode(presentModes[targetPresentMode])
                    .setImageArrayLayers(1)
                    .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
                    .setPreTransform(surfCapabilities.currentTransform)
                    .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::ePreMultiplied)
                    .setClipped(true);
    }

    void createSwapChain()
    {
        auto& dev = VkMgr::getDev().dev;
        dev.getSwapchainImagesKHR();
    }

    // -----------------------------------------------------

    bool inited() { return (bool)_inst; }
    void init()
    {
        if (inited()) return;
        _inst = std::make_unique<XD::Render::PresMgr::Data>();
    }
}
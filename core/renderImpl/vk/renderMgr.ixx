#include <SDL_vulkan.h>

#include "vk/vkMgr.h"
#include "vk/presMgr.h"
#include "render/public/renderMgr.h"

namespace XD::Render::Mgr
{
    void init()
    {
        uint32_t extensionsCount = 0;
        SDL_Vulkan_GetInstanceExtensions(&extensionsCount, NULL);
        const char** extensions = new const char*[extensionsCount];
        SDL_Vulkan_GetInstanceExtensions(&extensionsCount, extensions);

        XD::Render::Vk::VkMgr::init(extensions, extensionsCount);
        XD::Render::Vk::PresMgr::init(true);
    }

    void begUpdate()
    {
        XD::Render::Vk::PresMgr::begRender();
    }

    void endUpdate()
    {
        XD::Render::Vk::PresMgr::endRender();
    }

    void destroy()
    {
        XD::Render::Vk::PresMgr::destroy();
        XD::Render::Vk::VkMgr::destroy();
    }
}
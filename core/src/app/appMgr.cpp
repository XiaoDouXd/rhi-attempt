#include <memory>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#include "appMgr.h"
#include "xdBase/entrance.h"
#include "xdBase/exce.h"

namespace XD::App::AppMgr
{
    struct Data
    {
    public:
        SDL_Window*         hWnd;

    public:
        int                 w;
        int                 h;
    };
    static std::unique_ptr<Data> _inst = nullptr;

    // --------------------------------------

    void RefreshSize() { SDL_GetWindowSize(_inst->hWnd, &_inst->w, &_inst->h); }
    int width() { return _inst->w; }
    int height() { return _inst->h; }
    glm::i32vec2 size() { return {_inst->w, _inst->h}; }
    bool isSizeChange()
    {
        static int oldW = -1, oldH = -1;
        SDL_GetWindowSize(_inst->hWnd, &_inst->w, &_inst->h);
        if (_inst->w == oldW && _inst->h == oldH) return false;
        oldW = _inst->w; oldH = _inst->h;
        return true;
    }

    // --------------------------------------

    bool inited() noexcept { return (bool)_inst; }

    const char** init(uint32_t& extensionsCount)
    {
        if (_inst) return nullptr;
        _inst = std::make_unique<Data>();

        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMEPAD) != 0)
            throw Exce(__LINE__, __FILE__, "XD::App::AppMgr: SDL 初始化失败");
        if (SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1") != SDL_TRUE)
            throw Exce(__LINE__, __FILE__, "XD::App::AppMgr: SDL 输入法设置失败");

        // 绑定到 vulkan 实例
        SDL_WindowFlags wndFlags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
        _inst->hWnd = SDL_CreateWindow(xdWndInitConf_wndName, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, xdWndInitConf_defWndWidth - 1, xdWndInitConf_defWndHeight, wndFlags);
        SDL_SetWindowMinimumSize(_inst->hWnd, xdWndInitConf_lodingWidth, xdWndInitConf_lodingHeight);

        extensionsCount = 0;
        SDL_Vulkan_GetInstanceExtensions(&extensionsCount, NULL);
        const char** extensions = new const char*[extensionsCount];
        SDL_Vulkan_GetInstanceExtensions(&extensionsCount, extensions);
        return extensions;
    }

    vk::SurfaceKHR createSurf(vk::Instance& vkInst)
    {
        if (!_inst) throw Exce(__LINE__, __FILE__, "XD::App::AppMgr: 未初始化 SDL");

        VkSurfaceKHR surf = {};
        if (SDL_Vulkan_CreateSurface(_inst->hWnd, vkInst, &surf) == 0)
            throw XD::Exce(__LINE__, __FILE__, "XD::Render::PresMgr: SDL Vulkan Surface 创建失败");
        return surf;
    }
}
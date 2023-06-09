#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-narrowing-conversions"

#include <memory>
#include <SDL3/SDL.h>
#include <SDL3/SDL_image.h>

#include "SDL_video.h"
#include "app/public/appMgr.h"
#include "entrance.h"
#include "util/public/exce.h"

namespace XD::AppMgr
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

    static inline bool isWindowEvent(const SDL_Event& event)
    {
        return event.type >= SDL_EventType::SDL_EVENT_WINDOW_FIRST && event.type <= SDL_EventType::SDL_EVENT_WINDOW_LAST;
    }

    // --------------------------------------

    void refreshSize() { SDL_GetWindowSize(_inst->hWnd, &_inst->w, &_inst->h); }
    int width() { return _inst->w; }
    int height() { return _inst->h; }
    SDL_Window& wnd() { return *_inst->hWnd; }
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

    void init()
    {
        if (_inst) return;
        _inst = std::make_unique<Data>();

        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMEPAD) != 0)
            throw Exce(__LINE__, __FILE__, "XD::App::AppMgr: SDL 初始化失败");
        if (SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1") != SDL_TRUE)
            throw Exce(__LINE__, __FILE__, "XD::App::AppMgr: SDL 输入法设置失败");

        // 绑定到 vulkan 实例
        auto wndFlags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
        _inst->hWnd = SDL_CreateWindow(xdWndInitConf_wndName, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, xdWndInitConf_defWndWidth - 1, xdWndInitConf_defWndHeight, wndFlags);
        SDL_SetWindowMinimumSize(_inst->hWnd, xdWndInitConf_loadingWidth, xdWndInitConf_loadingHeight);

        if (xdWndInitConf_iconPngData && xdWndInitConf_iconPngSize)
        {
            auto iconSurf = SDL_RWFromConstMem((void*)xdWndInitConf_iconPngData, xdWndInitConf_iconPngSize);
            auto icon = IMG_LoadPNG_RW(iconSurf);
            SDL_SetWindowIcon(_inst->hWnd, icon);
        }
    }

    void destroy()
    {
        SDL_DestroyWindow(_inst->hWnd);
        _inst->hWnd = nullptr;
        _inst.reset();
    }

    void update(bool& quit, [[maybe_unused]] bool checkPlatformSpecialEvent)
    {
        // 在此处处理事件处理事件
        static SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            // 平台特化的事件处理
            // todo

            if (event.type == SDL_EVENT_QUIT)
                quit = true;
            if (isWindowEvent(event) && event.window.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(_inst->hWnd))
                quit = true;
        }
        refreshSize();
    }
}
#pragma clang diagnostic pop
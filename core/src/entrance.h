#pragma once
#include <cstdint>

namespace XD
{
    extern const uint8_t* xdWndInitConf_iconPngData;
    extern size_t xdWndInitConf_iconPngSize;
    extern const char* xdWndInitConf_wndName;
    extern int xdWndInitConf_defWndWidth;
    extern int xdWndInitConf_defWndHeight;
    extern int xdWndInitConf_loadingWidth;
    extern int xdWndInitConf_loadingHeight;
}

namespace XD
{
    bool inited() noexcept;
    void init();
    void update(bool& quit);
    void destroy();
}
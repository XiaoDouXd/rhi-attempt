#pragma once
#include <cstdint>
#include <locale>

namespace XD
{
    extern const uint8_t* xdWndInitConf_iconPngData;
    extern size_t xdWndInitConf_iconPngSize;

    extern const char* xdWndInitConf_wndName;
    extern int xdWndInitConf_defWndWidth;
    extern int xdWndInitConf_defWndHeight;
    extern int xdWndInitConf_loadingWidth;
    extern int xdWndInitConf_loadingHeight;

    extern std::u8string xdAssetInitConf_rootResMapPath;

    extern const std::locale defaultLocale;
}

namespace XD
{
    bool inited() noexcept;
    void init();
    void update(bool& quit);
    void destroy();
}
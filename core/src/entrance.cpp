
#include <iostream>

#include "render/presMgr.h"
#include "render/vkMgr.h"
#include "app/appMgr.h"
#include "xdBase/uuidGen.h"

namespace XD
{
    const uint8_t* xdWndInitConf_iconPngData = nullptr;
    size_t xdWndInitConf_iconPngSize = 0;
    const char* xdWndInitConf_wndName = "xdWnd";
    const char* xdWndInitConf_confFileName = "xdWnd.ini";
    double xdWndInitConf_waitTime = -1.;
    int xdWndInitConf_defWndWidth = 640;
    int xdWndInitConf_defWndHeight = 360;
    int xdWndInitConf_lodingWidth = 512;
    int xdWndInitConf_lodingHeight = 288;
}

namespace XD::UUID
{
    uuids::uuid gen()
    {
        static std::unique_ptr<uuids::uuid_random_generator> generator = nullptr;
        if (!generator)
        {
            std::random_device rd;
            auto seed_data = std::array<int, std::mt19937::state_size> {};
            std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
            std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
            std::mt19937 engine(seq);
            generator = std::make_unique<uuids::uuid_random_generator>(&engine);
        }
        return generator->operator()();
    }
} // namespace XD

void initRender()
{
    uint32_t externCount = 0;
    XD::Render::VkMgr::init(XD::App::AppMgr::init(externCount), externCount);
    XD::Render::PresMgr::init(true);
}

// 临时的 main 函数
int main(int argc, char** argv)
{
    initRender();
    std::cout << "program end";
    return 0;
}
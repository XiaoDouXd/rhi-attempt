
#include <iostream>
#include <memory>
#include <ostream>
#include <stdint.h>
#include <vector>

#include "uuid.h"
#include "xdBase/exce.h"
#include "xdBase/uuidGen.h"

#include "app/appMgr.h"
#include "app/event/staticEventMgr.hpp"
#include "app/util/timeMgr.h"
#include "render/renderMgr.h"

namespace XD
{
    const uint8_t* xdWndInitConf_iconPngData = nullptr;
    size_t xdWndInitConf_iconPngSize = 0;
    const char* xdWndInitConf_wndName = "xdWnd";
    int xdWndInitConf_defWndWidth = 640;
    int xdWndInitConf_defWndHeight = 360;
    int xdWndInitConf_loadingWidth = 512;
    int xdWndInitConf_loadingHeight = 288;
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

namespace XD
{
    static bool _inited = false;
    bool inited() { return _inited; }
    void init()
    {
        XD::AppMgr::init();
        XD::Render::Mgr::init();

        // 初始化事件
        XD::Event::StaticEventMgr::init();

        // 初始化工具
        XD::Util::TimeMgr::init();
        _inited = true;
    }

    void update(bool& quit)
    {
        if (!_inited) throw Exce(__LINE__, __FILE__, "XD::entrance: 未初始化实例");

        // 更新各个管理器
        XD::Util::TimeMgr::update();
        XD::Event::StaticEventMgr::update();

        // 切换交换链
        XD::Render::Mgr::begUpdate();
        XD::AppMgr::update(quit, true);
        XD::Render::Mgr::endUpdate();
    }

    void destroy()
    {
        if (!_inited) throw Exce(__LINE__, __FILE__, "XD::entrance: 未初始化实例");

        XD::Render::Mgr::destroy();
        XD::AppMgr::destroy();

        XD::Event::StaticEventMgr::destroy();
        XD::Util::TimeMgr::destroy();

        _inited = false;
    }
} // namespace XD

// 临时的 main 函数
int main(int argc, char** argv)
{
    try
    {
        std::cout<< "测试汉字 utf-8" << std::endl;
        XD::init();
        bool done = false;
        while (!done) XD::update(done);
        XD::destroy();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        system("pause");
    }
    return 0;
}
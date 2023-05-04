// #define XD_VK_IMPL

#include <iostream>
#include <ostream>
#include <cstdint>
#include <vector>

#include "app/public/appMgr.h"
#include "app/public/event/staticEventMgr.hpp"
#include "app/public/util/timeMgr.h"
#include "render/public/renderMgr.h"
#include "util/public/exce.h"

namespace XD
{
    const uint8_t* xdWndInitConf_iconPngData = nullptr;
    size_t xdWndInitConf_iconPngSize = 0;

    const char* xdWndInitConf_wndName = "xdWnd";
    int xdWndInitConf_defWndWidth = 640;
    int xdWndInitConf_defWndHeight = 360;
    int xdWndInitConf_loadingWidth = 512;
    int xdWndInitConf_loadingHeight = 288;

    [[maybe_unused]] std::u8string xdAssetInitConf_rootResMapPath = u8"./appRes/res-map-default.json";
    [[maybe_unused]] const std::locale defaultLocale = std::locale();
}

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

#include "render/public/gRes/buffer/vertexBuffer.hpp"

using namespace XD::Render;

// 临时的 main 函数
int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
    try
    {
        std::cout<< "测试汉字 utf-8" << std::endl;
        XD::init();

        {   // 测试顶点缓冲用的块
            Vertex<UvBufferLayout> vert;
            Vertex<UvBufferLayout> vertRef(nullptr);
            vert.set<0>({1, 2, 3});
            vert.set<1>({1, 2});
            vert.set<2>({152, 21, 55, 88});

            auto pos = vert.get<0>();
            auto uv = vert.get<1>();
            auto col = vert.get<2>();

            vertRef.bind(vert.data());
            auto pos2 = vertRef.get<0>();
            vertRef.set<1>({5, 6});
            auto uv2 = vert.get<1>();

            std::list<Vertex<UvBufferLayout>> vertList(10);
            auto& vertBack = vertList.back();
            vertBack = vert;
            VertexBuffer<UvBufferLayout> vertBuf(vertList);
        }

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
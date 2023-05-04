#pragma once

#include <cstdint>
#include <typeindex>

#include "asset/public/asset.h"

namespace XD::Asset::Mgr
{
    void init();
    // todo void update();
    void destroy();

    void add();
    const Asset& get(const uuids::uuid& assetId);
}
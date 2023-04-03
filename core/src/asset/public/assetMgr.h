#pragma once

#include <cstdint>
#include <typeindex>

#include "asset/public/asset.h"

namespace XD::Asset::Mgr
{
    void init();
    void add();
    std::weak_ptr<Asset> get(const uuids::uuid& assetId);
}
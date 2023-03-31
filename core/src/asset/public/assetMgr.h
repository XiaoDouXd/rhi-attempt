#pragma once

#include "asset/public/dynamic/asssetStream.h"

namespace XD::Asset::Mgr
{
    void init();
    std::weak_ptr<Asset> get(const uuids::uuid& assetId);
}
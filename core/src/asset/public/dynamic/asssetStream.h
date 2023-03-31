#pragma once

#include <filesystem>
#include <future>
#include <lz4.h>
#include <mio/mio.hpp>
#include <span>

#include "util/public/uuidGen.h"
#include "util/public/blob.h"

namespace XD::Asset
{
    class Asset
    {
    public:
        Asset(const char* path);
        Asset(uuids::uuid assetId);
        ~Asset();

        [[nodiscard]] std::future<Blob> load();
        [[nodiscard]] Blob loadSync();

    private:
        uuids::uuid             _assetId;
        mio::ummap_source       _source;
        std::filesystem::path   _path;

        operator bool() const;
    };
}
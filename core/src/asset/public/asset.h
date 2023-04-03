#pragma once

#include <filesystem>
#include <mio/mio.hpp>
#include <span>

#include "util/public/uuidGen.h"
#include "util/public/blob.h"

namespace XD::Asset::Mgr { class AssetInfo; }
namespace XD::Asset
{
    enum class LoadState : uint8_t
    {
        Lose = 0,   // 丢失

        UnLoaded,   // 未加载
        Loading,    // 加载中
        Loaded,     // 已载入
    };

    class Asset
    {
        friend class XD::Asset::Mgr::AssetInfo;
    public:
        Asset(Asset&&) = default;
        Asset(const Asset&) = delete;

        void load();
        void load(std::function<void(std::weak_ptr<Blob>&)>);
        [[nodiscard]] std::weak_ptr<Blob> loadSync();

        [[nodiscard]] std::weak_ptr<Blob> getData() const;
        [[nodiscard]] uuids::uuid getId() const;
        [[nodiscard]] LoadState getState() const;

        operator LoadState() const;

    private:
        Asset(uuids::uuid assetId);
        ~Asset();

        mutable std::shared_ptr<Blob>   _data;
        uuids::uuid             _assetId;
        mio::ummap_source       _source;
        LoadState               _state;
        bool                    _loaded;
    };
}
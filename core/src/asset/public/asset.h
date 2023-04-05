#pragma once

#include <filesystem>
#include <mio/mio.hpp>
#include <set>
#include <span>

#include "util/public/uuidGen.h"
#include "util/public/blob.h"

namespace XD::Asset::Mgr
{
    class AssetInfo;
    void update();
}

namespace XD::Asset
{
    enum class LoadState : uint8_t
    {
        UnLoaded = 0,   // 未加载
        Loading,        // 加载中
        Loaded,         // 已载入
    };

    enum class LoadFlagBits : uint8_t
    {
        Self        = 0b0001,
        Dependency  = 0b0010,
    };

    class Asset
    {
        friend void XD::Asset::Mgr::update();
        friend class XD::Asset::Mgr::AssetInfo;
    public:
        Asset() = default;
        ~Asset() = default;
        Asset(const Asset&) = delete;

        void load() const noexcept;
        void load(std::function<void(std::weak_ptr<Blob>)>) const noexcept;
        [[nodiscard]] std::weak_ptr<Blob> loadSync() const;
        void unload() const noexcept;

        [[nodiscard]] std::weak_ptr<Blob> getData() const noexcept;
        [[nodiscard]] uuids::uuid getId() const noexcept;
        [[nodiscard]] LoadState getState() const noexcept;

        operator LoadState() const noexcept;
        operator bool() const noexcept;

    private:
        Asset(Asset&&) = default;
        Asset(const uuids::uuid& assetId);

        uuids::uuid             _assetId = uuids::uuid();
        LoadState               _state = LoadState::UnLoaded;
        mutable bool            _selfLoaded = false;
        mutable std::shared_ptr<Blob> _data = nullptr;

        mutable std::list<std::function<void(std::weak_ptr<Blob>)>> _loadFinishCB;
        mutable std::list<std::function<void()>> _loadAsDependencySuccessCB;
        mutable std::list<std::function<void()>> _loadAsDependencyFailureCB;
        mutable std::chrono::time_point<std::chrono::steady_clock> _startLoadingTime;

        void onLoadFinish();
        void onLoadAsDependencyFailure() const;
        void onLoadAsDependencySuccess() const;
        void loadAsDependency(std::set<uuids::uuid>& load, size_t& loadingCount) const;
        void unloadAsDependency(std::set<uuids::uuid>& unload) const;
    };

    extern const Asset emptyAsset;
}
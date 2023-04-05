#include <chrono>
#include <thread>

#include "asset/public/asset.h"
#include "asset/public/assetMgr.h"
#include "assetMgr.h"

#include "util/public/exce.h"
#include "util/public/log.h"

namespace XD::Asset
{
    static constexpr size_t waitDependencyTimeMillisecond = 4;
    const Asset emptyAsset = Asset();

    Asset::Asset(const uuids::uuid& uid)
    : _assetId(uid) { _state = LoadState::UnLoaded; }

    std::weak_ptr<Blob> Asset::getData() const noexcept
    { return _data ? _data : std::weak_ptr<Blob>(); }
    uuids::uuid Asset::getId() const noexcept { return _assetId; }
    LoadState Asset::getState() const noexcept { return _state; }
    Asset::operator XD::Asset::LoadState() const noexcept { return getState(); }
    Asset::operator bool() const noexcept { return !_assetId.is_nil(); }

    void Asset::load() const noexcept
    {
        if (_state == LoadState::Loaded) { _selfLoaded = true; return; }

        auto info = Mgr::getInfo(_assetId);
        if (!info) return;
        if (_state == LoadState::Loading && info->asyncHandle)
        {
            info->asyncHandle->flag |= (uint8_t)LoadFlagBits::Self;
            return;
        }

        auto fut = std::async([info](std::shared_ptr<Blob>& data) -> std::shared_ptr<Blob>&
        {
            data.reset();
            if (std::filesystem::exists(info->pack->path))
            {
                try
                {
                    uint8_t* dst = new uint8_t[info->size];
                    mio::ummap_source map{info->pack->path.string()};
                    const uint8_t* beginPtr = map.begin() + info->offset;
                    memcpy(dst, beginPtr, info->size);
                    data = std::make_shared<Blob>(dst, info->size);
                }
                catch (std::exception& e)
                {
                    data.reset();
                }
            }

            size_t dependencyCount = 0;
            std::set<uuids::uuid> load;
            load.insert(info->asset._assetId);
            for (auto& did : info->dependency)
            {
                if (load.find(did) != load.end()) continue;
                auto& ass = Mgr::get(did);
                if (!ass) continue;
                ass.loadAsDependency(load, dependencyCount);
            }

            while (dependencyCount)
            {
                std::this_thread::sleep_for(
                    std::chrono::milliseconds(waitDependencyTimeMillisecond));
            }
            return data;
        }, std::ref(_data));
        info->asyncHandle = std::make_unique<Mgr::AssetAsyncInfo>(std::move(fut), LoadFlagBits::Self);
        _startLoadingTime = std::chrono::high_resolution_clock::now();
    }

    void Asset::load(std::function<void(std::weak_ptr<Blob>)> cb) const noexcept
    {
        if (_state == LoadState::Loaded)
        {
            _selfLoaded = true;
            cb(_data);
        }
        else
        {
            _loadFinishCB.emplace_back(cb); load();
        }
    }

    std::weak_ptr<Blob> Asset::loadSync() const
    {
        if (_state == LoadState::Loaded)
        {
            _selfLoaded = true;
            return getData();
        }

        if (_state == LoadState::Loading)
            throw Exce(__LINE__, __FILE__, "XD::Asset loadSync: 当前申请同步加载的资源正在进行异步加载");

        auto info = Mgr::getInfo(_assetId);
        if (!info)
            throw Exce(__LINE__, __FILE__, "XD::Asset loadSync: 尝试加载未注册资源");

        if (std::filesystem::exists(info->pack->path))
        {
            uint8_t* dst = new uint8_t[info->size];
            mio::ummap_source map{info->pack->path.string()};
            const uint8_t* beginPtr = map.begin() + info->offset;
            memcpy(dst, beginPtr, info->size);
            _data = std::make_shared<Blob>(dst, info->size);
        }
        else
            throw Exce(__LINE__, __FILE__, "XD::Asset loadSync: 资源不存在");
        return _data;
    }

    void Asset::unload() const noexcept
    {
        if (_state != LoadState::UnLoaded) return;
        if (!_selfLoaded) return;

        _selfLoaded = false;
        auto info = Mgr::getInfo(_assetId);
        if (!info) return;

        std::set<uuids::uuid> unload;
        unload.insert(_assetId);
        for (auto& did : info->dependency)
        {
            if (unload.find(did) != unload.end()) continue;
            auto& ass = Mgr::get(did);
            if (!ass) continue;
            ass.unloadAsDependency(unload);
        }
        if (!info->refCount && !_selfLoaded) { _data.reset(); }
    }

    void Asset::loadAsDependency(std::set<uuids::uuid>& load, size_t& loadingCount) const
    {
        auto info = Mgr::getInfo(_assetId);
        if (!info) return;

        loadingCount++;
        load.insert(_assetId);
        for (auto& did : info->dependency)
        {
            if (load.find(did) != load.end()) continue;
            auto& ass = Mgr::get(did);
            if (!ass) continue;
            ass.loadAsDependency(load, loadingCount);
        }

        if (_state == LoadState::Loaded)
        {
            info->refCount++;
            loadingCount--;
            return;
        }

        _loadAsDependencySuccessCB.emplace_back([info, &loadingCount]()
        {
            info->refCount++;
            loadingCount--;
        });
        _loadAsDependencyFailureCB.emplace_back([info, &loadingCount]()
        {
            Log::logError(std::string("Asset::Asset::loadAsDependency : 加载依赖失败 - ") +
                uuids::to_string(info->asset._assetId));

            info->refCount++;
            loadingCount--;
        });

        if (_state == LoadState::Loading && info->asyncHandle)
        {
            info->asyncHandle->flag |= (uint8_t)LoadFlagBits::Dependency;
            return;
        }

        auto fut = std::async([info](std::shared_ptr<Blob>& data) -> std::shared_ptr<Blob>&
        {
            data.reset();
            if (std::filesystem::exists(info->pack->path))
            {
                try
                {
                    uint8_t* dst = new uint8_t[info->size];
                    mio::ummap_source map{info->pack->path.string()};
                    const uint8_t* beginPtr = map.begin() + info->offset;
                    memcpy(dst, beginPtr, info->size);
                    data = std::make_shared<Blob>(dst, info->size);
                }
                catch (std::exception& e)
                {
                    data.reset();
                }
            }
            return data;
        }, std::ref(_data));
        info->asyncHandle = std::make_unique<Mgr::AssetAsyncInfo>(std::move(fut), LoadFlagBits::Dependency);
        _startLoadingTime = std::chrono::high_resolution_clock::now();
    }

    void Asset::onLoadFinish()
    {
        if (_state == LoadState::Loaded)
        {
            _selfLoaded = true;
            for (auto& finCB : _loadFinishCB)
                finCB(_data);
            _loadFinishCB.clear();
        }
        else
        {
            for (auto& finCB : _loadFinishCB)
                finCB(std::weak_ptr<Blob>());
            _loadFinishCB.clear();
        }
    }

    void Asset::onLoadAsDependencySuccess() const
    {
        for (auto& finCB : _loadAsDependencySuccessCB)
            finCB();
        _loadAsDependencySuccessCB.clear();
    }

    void Asset::onLoadAsDependencyFailure() const
    {
        for (auto& finCB : _loadAsDependencyFailureCB)
            finCB();
        _loadAsDependencyFailureCB.clear();
    }

    void Asset::unloadAsDependency(std::set<uuids::uuid>& unload) const
    {
        auto info = Mgr::getInfo(_assetId);
        if (!info) return;

        if (info->refCount) info->refCount--;
        unload.insert(_assetId);
        for (auto& did : info->dependency)
        {
            if (unload.find(did) != unload.end()) continue;
            auto& ass = Mgr::get(did);
            if (!ass) continue;
            ass.unloadAsDependency(unload);
        }
        if (!info->refCount && !_selfLoaded) { _data.reset(); }
    }
}
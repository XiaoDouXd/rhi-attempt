#include <chrono>
#include <filesystem>
#include <fstream>
#include <future>
#include <list>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "assetMgr.h"
#include "entrance.h"
#include "util/public/exce.h"

namespace XD::Asset::Mgr
{

    struct Data
    {
    public:
        std::unordered_map<uuids::uuid, AssetPackInfo>  packMap;
        std::unordered_map<uuids::uuid, AssetInfo>      assetMap;
        std::list<uuids::uuid>                          loadingAsset;
        std::unordered_map<std::string, std::list<AssetPackInfo*>> packNameMap;
    };

    static std::unique_ptr<Data> _inst = nullptr;
    static constexpr size_t waitNanoseconds = 64;
    static constexpr size_t updateClipMiliseconds = 4;

    // ------------------------------------- 初始化和数据获取

    void init()
    {
        if (_inst) return;
        _inst = std::make_unique<Data>();

        std::ifstream ifs(std::filesystem::path(xdAssetInitConf_rootResMapPath), std::ios::in);
        std::string content((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));

        rapidjson::Document doc;
        doc.Parse(content.c_str());
        if (!doc.IsObject()) throw Exce(__LINE__, __FILE__, "XD::Asset Mgr: 资源根节点读取失败");

        auto& packs = doc["packs"];
        if (!packs.IsArray()) throw Exce(__LINE__, __FILE__, "XD::Asset Mgr: 资源根节点读取失败");
        auto packsArray = packs.GetArray();
        for (auto& p : packsArray)
        {
            auto& id = p["id"];
            if (!id.IsString() || uuids::uuid::is_valid_uuid(id.GetString())) continue;
            auto uid = uuids::uuid::from_string(id.GetString());
            if (!uid || _inst->packMap.find(uid.value()) != _inst->packMap.end()) continue;

            try
            {
                _inst->packMap.insert({uid.value(), AssetPackInfo(p, uid.value())});
            }
            catch (std::string& e)
            {

            }
        }
    }

    const Asset& get(const uuids::uuid& assetId)
    {
        auto itr = _inst->assetMap.find(assetId);
        return itr != _inst->assetMap.end() ? itr->second.asset : emptyAsset;
    }

    AssetInfo* getInfo(const uuids::uuid& id)
    {
        auto res = _inst->assetMap.find(id);
        if (res == _inst->assetMap.end()) return nullptr;
        return &(res->second);
    }

    // ------------------------------------- 更新

    AssetPackAlgorithm toPackAlgorithm(const std::string_view& str)
    {
        if (str == "lz4") return AssetPackAlgorithm::LZ4;
        return AssetPackAlgorithm::Undefined;
    }

    void update()
    {
        if (!_inst) return;
        static std::unique_ptr<std::list<uuids::uuid>::iterator> itrStatic = nullptr;
        if (!itrStatic || _inst->loadingAsset.end() == *itrStatic)
            itrStatic = std::make_unique<std::list<uuids::uuid>::iterator>(_inst->loadingAsset.begin());
        auto start = std::chrono::high_resolution_clock::now();
        auto end = std::chrono::high_resolution_clock::now();

        for (auto& itr = *itrStatic; itr != _inst->loadingAsset.end();)
        {
            auto assItr = _inst->assetMap.find(*itr);
            while (assItr == _inst->assetMap.end() || !assItr->second.asyncHandle)
            {
                auto itrDel = itr;
                itr++;
                _inst->loadingAsset.erase(itrDel);
                if (itr == _inst->loadingAsset.end()) return;
                assItr = _inst->assetMap.find(*itr);
            }

            end = std::chrono::high_resolution_clock::now();
            if (std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count() > updateClipMiliseconds)
                return;

            auto& assetInfo = assItr->second;
            if (assetInfo.asyncHandle->handle.wait_for(std::chrono::nanoseconds(waitNanoseconds))
                != std::future_status::ready)
            { itr++; continue; }

            auto& asset = assetInfo.asset;
            auto& assetDataPtr = assetInfo.asyncHandle->handle.get();
            if (assetDataPtr)
            {
                asset._state = LoadState::Loaded;
                if (assetInfo.asyncHandle->flag & (uint8_t)LoadFlagBits::Dependency) asset.onLoadAsDependencySuccess();
                if (assetInfo.asyncHandle->flag & (uint8_t)LoadFlagBits::Self) asset.onLoadFinish();
            }
            else
            {
                asset._state = LoadState::UnLoaded;
                if (assetInfo.asyncHandle->flag & (uint8_t)LoadFlagBits::Dependency) asset.onLoadAsDependencyFailure();
                if (assetInfo.asyncHandle->flag & (uint8_t)LoadFlagBits::Self) asset.onLoadFinish();
            }
            assetInfo.asyncHandle.reset();
            auto itrDel = itr;
            itr++;
            _inst->loadingAsset.erase(itrDel);
        }
    }

    // ------------------------------------- 一些构造函数

#pragma region Inited
    AssetAsyncInfo::AssetAsyncInfo(std::future<std::shared_ptr<Blob>&>&& h, const LoadFlagBits& d)
    : flag((uint8_t)d) { handle = std::move(h); }

    AssetPackInfo::AssetPackInfo(const rapidjson::GenericValue<rapidjson::UTF8<>>& data, const uuids::uuid& uid)
    {
        id = uid;
        auto& nameObj = data["name"];
        auto& algorithmObj = data["algorithm"];
        auto& pathObj = data["path"];
        auto& tagObj = data["tag"];
        auto& versionObj = data["version"];
        auto& contentObj = data["content"];

        name = nameObj.IsString() ? nameObj.GetString() : "";
        algorithm = algorithmObj.IsString() ? toPackAlgorithm(algorithmObj.GetString()) : AssetPackAlgorithm::Undefined;
        path = pathObj.IsString() ? std::filesystem::path(pathObj.GetString()) : "";
        version = versionObj.IsInt64() ? versionObj.GetInt64() : INT64_MIN;
        if (tagObj.IsArray())
        {
            for (auto& t : tagObj.GetArray())
            {
                if (!t.IsString()) continue;
                tag.emplace_back(t.GetString());
            }
            tag.shrink_to_fit();
        }

        if (!contentObj.IsArray()) return;
        for (auto& c : contentObj.GetArray())
        {
            auto& cid = c["id"];
            if (!cid.IsString() || uuids::uuid::is_valid_uuid(cid.GetString())) continue;
            auto cuid = uuids::uuid::from_string(cid.GetString());
            if (!cuid || _inst->assetMap.find(cuid.value()) != _inst->assetMap.end()) continue;

            try
            {
                _inst->assetMap.insert({cuid.value(), AssetInfo(c, cuid.value(), this)});
                auto asset = _inst->assetMap.find(cuid.value());
                if (asset == _inst->assetMap.end())
                    throw std::string("Failure to load AssetPackInfo: ") + uuids::to_string(cuid.value());

                if (assets.find(asset->second.name) == assets.end())
                    assets.insert({asset->second.name, {}});
                assets.find(asset->second.name)->
                    second.emplace_back(asset->second.asset.getId());
            }
            catch (std::string& str) { throw str; }
        }
        if (assets.empty()) throw std::string("Failure to load AssetPackInfo: ") + uuids::to_string(uid);
    }

    AssetInfo::AssetInfo(const rapidjson::GenericValue<rapidjson::UTF8<>>& data, const uuids::uuid& uid, AssetPackInfo* packInfo)
    : asset(uid), pack(packInfo), asyncHandle(nullptr)
    {
        auto& versionObj = data["version"];
        auto& nameObj = data["name"];
        auto& typeObj = data["type"];
        auto& tagObj = data["tag"];
        auto& dependencyObj = data["dependency"];
        auto& sizeObj = data["size"];
        auto& offsetObj = data["offset"];

        version = versionObj.IsInt64() ? versionObj.GetInt64() : INT64_MIN;
        name = nameObj.IsString() ? nameObj.GetString() : "";
        type = typeObj.IsString() ? typeObj.GetString() : "";
        size = sizeObj.IsUint64() ? typeObj.GetUint64() : 0;
        offset = offsetObj.IsUint64() ? typeObj.GetUint64() : 0;

        if (!size) throw std::string("Failure to load AssetInfo: ") + uuids::to_string(uid);
        if (tagObj.IsArray())
        {
            for (auto& t : tagObj.GetArray())
            {
                if (!t.IsString()) continue;
                tag.emplace_back(t.GetString());
            }
            tag.shrink_to_fit();
        }

        if (dependencyObj.IsString() && uuids::uuid::is_valid_uuid(dependencyObj.GetString()))
        {
            dependency.emplace_back(uuids::uuid::from_string(dependencyObj.GetString()).value());
        }
        else if (dependencyObj.IsArray())
        {
            for (auto& d : dependencyObj.GetArray())
            {
                if (!d.IsString() || uuids::uuid::is_valid_uuid(d.GetString())) continue;
                dependency.emplace_back(uuids::uuid::from_string(d.GetString()).value());
            }
        }
    }
#pragma endregion
}
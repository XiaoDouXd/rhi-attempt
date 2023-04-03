#include <fstream>
#include <list>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include <rapidjson/document.h>

#include "asset/public/assetMgr.h"
#include "entrance.h"
#include "util/public/exce.h"
#include "util/public/uuidGen.h"

namespace XD::Asset::Mgr
{
    enum class AssetPackAlgorithm : uint8_t
    {
        Undefined,
        // ---------

        LZ4,

        // ---------
        Num,
    };
    AssetPackAlgorithm toPackAlgorithm(const std::string_view& str);

    class AssetPackInfo;

    struct AssetInfo
    {
    public:
        std::shared_ptr<Asset>      asset;
        size_t                      size;
        size_t                      refCount;
        int64_t                     version;
        std::string                 name;
        std::string                 type;
        AssetPackInfo*              pack;
        std::vector<std::string>    tag;
        std::list<uuids::uuid>      dependency;
        AssetInfo(const rapidjson::GenericValue<rapidjson::UTF8<>>& data, const uuids::uuid& uid, AssetPackInfo* packInfo);
    };

    struct AssetPackInfo
    {
    public:
        uuids::uuid                 id;
        std::string                 name;
        std::int64_t                version;
        std::filesystem::path       path;
        AssetPackAlgorithm          algorithm;
        std::vector<std::string>    tag;
        std::unordered_map<std::string, std::list<uuids::uuid>> assets;
        AssetPackInfo(const rapidjson::GenericValue<rapidjson::UTF8<>>& data, const uuids::uuid& uid);
    };

    struct Data
    {
    public:
        std::unordered_map<uuids::uuid, AssetPackInfo>  packMap;
        std::unordered_map<uuids::uuid, AssetInfo>      assetMap;
        std::unordered_map<std::string, std::list<AssetPackInfo*>> packNameMap;
    };

    static std::unique_ptr<Data> _inst = nullptr;

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
                auto& asset = _inst->assetMap[cuid.value()];

                if (assets.find(asset.name) == assets.end())
                    assets.insert({asset.name, {}});
                assets.find(asset.name)->second.emplace_back(asset.asset->getId());
            }
            catch (std::string& str)
            {
                throw str;
            }
        }
        if (assets.empty()) throw std::string("Failure to load AssetPackInfo: ") + uuids::to_string(uid);
    }

    AssetInfo::AssetInfo(const rapidjson::GenericValue<rapidjson::UTF8<>>& data, const uuids::uuid& uid, AssetPackInfo* packInfo)
    : asset(std::make_shared<Asset>(uid)), pack(packInfo)
    {
        auto& versionObj = data["version"];
        auto& nameObj = data["name"];
        auto& typeObj = data["type"];
        auto& tagObj = data["tag"];
        auto& dependencyObj = data["dependency"];
        auto& sizeObj = data["size"];

        version = versionObj.IsInt64() ? versionObj.GetInt64() : INT64_MIN;
        name = nameObj.IsString() ? nameObj.GetString() : "";
        type = typeObj.IsString() ? typeObj.GetString() : "";
        size = sizeObj.IsUint64() ? typeObj.GetUint64() : 0;

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

    AssetPackAlgorithm toPackAlgorithm(const std::string_view& str)
    {
        if (str == "lz4") return AssetPackAlgorithm::LZ4;
        return AssetPackAlgorithm::Undefined;
    }
}
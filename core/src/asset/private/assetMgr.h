#pragma once

#include <filesystem>
#include <fstream>
#include <future>
#include <list>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include <rapidjson/document.h>

#include "asset/public/assetMgr.h"
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

    struct AssetAsyncInfo
    {
    public:
        std::future<std::shared_ptr<Blob>&> handle;
        uint8_t flag;
        AssetAsyncInfo(std::future<std::shared_ptr<Blob>&>&& h, const LoadFlagBits& d);
    };

    struct AssetInfo
    {
    public:
        Asset                       asset;
        size_t                      offset;
        size_t                      size;
        size_t                      refCount;
        int64_t                     version;
        std::string                 name;
        std::string                 type;
        AssetPackInfo*              pack;
        std::vector<std::string>    tag;
        std::list<uuids::uuid>      dependency;
        std::unique_ptr<AssetAsyncInfo> asyncHandle;
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

    AssetInfo* getInfo(const uuids::uuid& id);
}
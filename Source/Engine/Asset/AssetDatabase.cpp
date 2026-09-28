#include "AssetDatabase.h"

#include "Engine/Object/Class/Class.h"

namespace URay
{

AssetDatabase::AssetDatabase() = default;

AssetDatabase::~AssetDatabase() = default;

Asset* AssetDatabase::Find(const UUID& uuid, Class* assetClass) const
{
    const auto it = assets.find(uuid);
    if (it == assets.end())
        return nullptr;

    Asset* asset = it->second.get();
    if (asset->IsA(assetClass))
        return asset;

    return nullptr;
}

std::vector<Asset*> AssetDatabase::GetAssets(Class* assetClass) const
{
    std::vector<Asset*> ret;

    for (const auto& [uuid, asset] : assets)
    {
        if (asset->IsA(assetClass))
            ret.push_back(asset.get());
    }

    return ret;
}

void AssetDatabase::Add(std::unique_ptr<Asset> asset)
{
    const UUID& uuid = asset->GetUUID();
    if (!uuid.IsValid())
        return;

    const auto it = assets.find(uuid);
    if (it != assets.end())
    {
        // Duplicate uuid found!
        // TODO: Print error msg.
        return;
    }

    assets.insert({ uuid, std::move(asset) });
}

} // namespace URay

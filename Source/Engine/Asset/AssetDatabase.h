#pragma once

#include "Engine/Asset/Asset.h"
#include "Engine/Asset/Material/Material.h"
#include "Engine/Asset/Mesh/Mesh.h"
#include "Engine/Asset/Texture/Texture.h"

#include "Core/UUID.h"

#include <memory>
#include <unordered_map>
#include <vector>

namespace URay
{

class Class;

class AssetDatabase
{
public:
    AssetDatabase();
    ~AssetDatabase();

public:
    template <typename T>
    T* Find(const UUID& uuid) const
    {
        const auto it = assets.find(uuid);
        if (it == assets.end())
            return nullptr;

        Asset* asset = it->second.get();
        if (!asset->IsA<T>())
            return nullptr;

        return static_cast<T*>(asset);
    }
    Asset* Find(const UUID& uuid, Class* assetClass) const;

    template <typename T>
    std::vector<T*> GetAssets() const
    {
        std::vector<T*> ret;

        for (const auto& [uuid, asset] : assets)
        {
            if (asset->IsA<T>())
                ret.push_back(static_cast<T*>(asset.get()));
        }

        return ret;
    }
    std::vector<Asset*> GetAssets(Class* assetClass) const;

    void Add(std::unique_ptr<Asset> asset);

private:
    std::unordered_map<UUID, std::unique_ptr<Asset>, UUIDHash> assets;
};

} // namespace URay

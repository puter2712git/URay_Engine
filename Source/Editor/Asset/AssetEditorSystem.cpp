#include "AssetEditorSystem.h"

#include <Core/Log/LogSystem.h>
#include <Core/Type/Type.h>
#include <Engine/Asset/Asset.h>
#include <Engine/Asset/AssetDatabase.h>

#include <algorithm>
#include <utility>

namespace URay
{

AssetEditorSystem::AssetEditorSystem(AssetDatabase& assetDatabase) : assetDatabase(assetDatabase) {}

bool AssetEditorSystem::Register(const Type& assetType, OpenHandler handler)
{
    if (!handler || !IsTypeOf(assetType, Asset::StaticType()))
        return false;

    return handlers.emplace(&assetType, std::move(handler)).second;
}

void AssetEditorSystem::RequestOpen(const AssetHandle& handle)
{
    if (!handle.IsValid())
        return;

    if (std::find(pendingRequests.begin(), pendingRequests.end(), handle) != pendingRequests.end())
        return;

    pendingRequests.push_back(handle);
}

void AssetEditorSystem::ProcessPendingRequests()
{
    std::vector<AssetHandle> requests;
    requests.swap(pendingRequests);

    for (const AssetHandle& handle : requests)
    {
        Asset* asset = assetDatabase.Find<Asset>(handle);
        if (!asset)
        {
            URAY_LOG("[AssetEditorSystem] Requested asset was not found.");
            continue;
        }

        const auto it = handlers.find(&asset->GetType());
        if (it == handlers.end())
        {
            URAY_LOG("[AssetEditorSystem] No editor registered for asset type: %s", asset->GetType().name.c_str());
            continue;
        }

        OpenHandler handler = it->second;
        handler(handle);
    }
}

} // namespace URay

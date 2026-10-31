#pragma once

#include <Engine/Asset/AssetHandle.h>

#include <functional>
#include <unordered_map>
#include <vector>

namespace URay
{

class AssetDatabase;
struct Type;

class AssetEditorSystem
{
public:
    using OpenHandler = std::function<void(const AssetHandle&)>;

public:
    explicit AssetEditorSystem(AssetDatabase& assetDatabase);

public:
    bool Register(const Type& assetType, OpenHandler handler);
    void RequestOpen(const AssetHandle& handle);
    void ProcessPendingRequests();

private:
    AssetDatabase& assetDatabase;

    std::unordered_map<const Type*, OpenHandler> handlers;
    std::vector<AssetHandle> pendingRequests;
};

} // namespace URay

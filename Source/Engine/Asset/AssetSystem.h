#pragma once

#include "Engine/Asset/DefaultAssets.h"

#include "Core/UUID.h"

#include <filesystem>
#include <memory>
#include <string>
#include <type_traits>
#include <unordered_map>

namespace URay
{

class VirtualFilesystem;
class VirtualPath;
class Asset;
class AssetFactory;
class AssetPipeline;
class Engine;

namespace Render
{
class RenderDevice;
class GPUResourceManager;
class ShaderManager;
} // namespace Render

class AssetSystem
{
public:
    AssetSystem(Engine& engine);
    ~AssetSystem();

public:
    bool Initialize(const std::string& enginePath, const std::string& projectPath);
    bool CreateDefaultAssets();
    bool LoadAssets(const VirtualPath& sourceDir);

    void Finalize();

    UUID Import(const VirtualPath& path);

    template <typename T>
    T* Find(const UUID& uuid) const
    {
        const auto it = assets.find(uuid);

        if (it == assets.end())
            return nullptr;

        return Cast<T>(it->second);
    }

    template <typename T>
    std::vector<T*> FindAssets() const
    {
        std::vector<T*> ret;

        for (auto& [uuid, asset] : assets)
        {
            if (T* obj = Cast<T>(asset))
            {
                ret.push_back(obj);
            }
        }

        return ret;
    }

    VirtualFilesystem& GetFilesystem() const { return *filesystem; }

    AssetFactory& GetAssetFactory() const { return *factory; } // TODO: Remove this getter.

    const DefaultAssets& GetDefaultAssets() const { return defaultAssets; }

private:
    Engine& engine;

    std::unique_ptr<VirtualFilesystem> filesystem = nullptr;

    std::unique_ptr<AssetFactory> factory = nullptr;
    std::unique_ptr<AssetPipeline> pipeline = nullptr;

    std::unordered_map<UUID, Asset*, UUIDHash> assets;
    std::unordered_map<std::string, UUID> sourceAssets;
    DefaultAssets defaultAssets = {};
};

} // namespace URay

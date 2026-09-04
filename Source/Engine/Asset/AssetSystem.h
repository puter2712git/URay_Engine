#pragma once

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
class MaterialManager;
class MeshManager;
class TextureManager;
class FontManager;
class Importer;
class Mesh;
class Material;
class Texture;
class Font;
class Object;
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
    bool LoadAssets(const VirtualPath& path);

    bool InitializeRuntimeAssets(
        Render::RenderDevice& renderDevice,
        Render::GPUResourceManager& resourceManager,
        Render::ShaderManager& shaderManager);

    void Finalize();

    void Load(const VirtualPath& path);

    template <typename T>
    T* Find(const UUID& uuid) const
    {
        const auto it = assets.find(uuid);

        if (it == assets.end())
            return nullptr;

        return Cast<T>(it->second);
    }

    Mesh* FindMesh(const std::string& key) const;
    Material* FindMaterial(const std::string& key) const;
    Texture* FindTexture(const std::string& key) const;
    Font* FindFont(const std::string& key) const;

    VirtualFilesystem& GetFilesystem() const { return *filesystem; }

    const std::unordered_map<std::string, Mesh*>& GetMeshes() const;
    const std::unordered_map<std::string, Material*>& GetMaterials() const;
    std::vector<Texture*> GetTextures() const;

private:
    Engine& engine;

    std::unique_ptr<VirtualFilesystem> filesystem = nullptr;

    std::unordered_map<UUID, Object*, UUIDHash> assets;

    std::unique_ptr<MaterialManager> materialManager = nullptr;
    std::unique_ptr<MeshManager> meshManager = nullptr;
    std::unique_ptr<TextureManager> textureManager = nullptr;
    std::unique_ptr<FontManager> fontManager = nullptr;

    std::vector<std::unique_ptr<Importer>> importers;
};

} // namespace URay

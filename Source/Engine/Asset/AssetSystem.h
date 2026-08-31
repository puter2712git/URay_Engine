#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>

namespace URay
{

class VirtualFilesystem;
class MaterialManager;
class MeshManager;
class TextureManager;
class FontManager;
class ObjImporter;
class Mesh;
class Material;
class Texture;
class Font;

namespace Render
{
class RenderDevice;
class GPUResourceManager;
class ShaderManager;
} // namespace Render

class AssetSystem
{
public:
    AssetSystem();
    ~AssetSystem();

public:
    bool Initialize(const std::string& enginePath, const std::string& projectPath);
    bool InitializeRuntimeAssets(
        Render::RenderDevice& renderDevice,
        Render::GPUResourceManager& resourceManager,
        Render::ShaderManager& shaderManager);

    void Finalize();

    Mesh* FindMesh(const std::string& key) const;
    Material* FindMaterial(const std::string& key) const;
    Texture* FindTexture(const std::string& key) const;
    Font* FindFont(const std::string& key) const;

    VirtualFilesystem& GetFilesystem() const { return *filesystem; }

    const std::unordered_map<std::string, Mesh*>& GetMeshes() const;
    const std::unordered_map<std::string, Material*>& GetMaterials() const;
    const std::unordered_map<std::string, Texture*>& GetTextures() const;

private:
    std::unique_ptr<VirtualFilesystem> filesystem = nullptr;

    std::unique_ptr<MaterialManager> materialManager = nullptr;
    std::unique_ptr<MeshManager> meshManager = nullptr;
    std::unique_ptr<TextureManager> textureManager = nullptr;
    std::unique_ptr<FontManager> fontManager = nullptr;
    std::unique_ptr<ObjImporter> objImporter = nullptr;
};

} // namespace URay

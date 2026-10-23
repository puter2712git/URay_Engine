#pragma once

#include <Engine/Asset/Metadata/AssetMetadata.h>

#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace URay
{

class VirtualFileSystem;
class VirtualPath;
class Asset;
class AssetDatabase;
class AssetPipeline;
class AssetFactory;
class AssetMetadataSerializer;
class Importer;
class TextureSerializer;
class MaterialSerializer;
class MeshSerializer;
class FontSerializer;
class Texture;
class Material;
struct MeshInfo;

class AssetSystem
{
public:
    AssetSystem();
    ~AssetSystem();

public:
    bool Initialize(const std::string& enginePath, const std::string& projectPath);
    void Finalize();

    bool Import(const VirtualPath& sourcePath);

    void LoadAssetsRecursive(const VirtualPath& path);
    bool RegisterAsset(const AssetMetadata& metadata);

    void CreateDefaultAssets();

    VirtualPath GetMetadataPath(const VirtualPath& sourcePath) const;

    VirtualFileSystem& GetFileSystem() const { return *fileSystem; }
    AssetDatabase& GetDatabase() const { return *database; }
    AssetFactory& GetFactory() const { return *factory; }
    AssetMetadataSerializer& GetMetadataSerializer() const { return *metadataSerializer; }
    TextureSerializer& GetTextureSerializer() const { return *textureSerializer; }
    MaterialSerializer& GetMaterialSerializer() const { return *materialSerializer; }
    FontSerializer& GetFontSerializer() const { return *fontSerializer; }
    MeshSerializer& GetMeshSerializer() const { return *meshSerializer; }

private:
    void CreateDefaultMesh(const AssetHandle& handle, const std::string& name, const MeshInfo& meshInfo, const AssetHandle& materialHandle);

private:
    std::unique_ptr<VirtualFileSystem> fileSystem = nullptr;

    std::unique_ptr<AssetDatabase> database = nullptr;
    std::unique_ptr<AssetPipeline> pipeline = nullptr;
    std::unique_ptr<AssetFactory> factory = nullptr;
    std::unique_ptr<AssetMetadataSerializer> metadataSerializer = nullptr;

    std::unique_ptr<TextureSerializer> textureSerializer = nullptr;
    std::unique_ptr<MaterialSerializer> materialSerializer = nullptr;
    std::unique_ptr<FontSerializer> fontSerializer = nullptr;
    std::unique_ptr<MeshSerializer> meshSerializer = nullptr;
};

} // namespace URay

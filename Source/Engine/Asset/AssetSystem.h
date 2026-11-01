#pragma once
#include <Core/Type/TypeAlias.h>

#include <Engine/Asset/Metadata/AssetMetadata.h>

#include <filesystem>
#include <memory>
#include <optional>
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
struct ImportRequest;

class AssetSystem
{
public:
    AssetSystem();
    ~AssetSystem();

public:
    bool Initialize(const String& enginePath, const String& projectPath);
    void Finalize();

    bool Import(const ImportRequest& request);
    bool CanImport(const VirtualPath& sourcePath) const;

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
    void CreateDefaultMesh(const AssetHandle& handle, const String& name, const MeshInfo& meshInfo, const AssetHandle& materialHandle);

private:
    UniquePtr<VirtualFileSystem> fileSystem = nullptr;

    UniquePtr<AssetDatabase> database = nullptr;
    UniquePtr<AssetPipeline> pipeline = nullptr;
    UniquePtr<AssetFactory> factory = nullptr;
    UniquePtr<AssetMetadataSerializer> metadataSerializer = nullptr;

    UniquePtr<TextureSerializer> textureSerializer = nullptr;
    UniquePtr<MaterialSerializer> materialSerializer = nullptr;
    UniquePtr<FontSerializer> fontSerializer = nullptr;
    UniquePtr<MeshSerializer> meshSerializer = nullptr;
};

} // namespace URay

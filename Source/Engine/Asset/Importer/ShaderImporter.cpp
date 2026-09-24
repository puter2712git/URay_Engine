#include "ShaderImporter.h"

#include "Engine/Asset/AssetFactory.h"
#include "Engine/Asset/AssetSystem.h"
#include "Engine/Asset/Shader/Shader.h"
#include "Engine/Engine.h"

#include "Core/File/VirtualFilesystem.h"
#include "Core/File/VirtualPath.h"

namespace URay
{

ShaderImporter::ShaderImporter() = default;

ShaderImporter::~ShaderImporter() = default;

Asset* ShaderImporter::Import(const VirtualPath& sourcePath)
{
    Asset* ret = nullptr;

    AssetSystem& assetSystem = gEngine->GetAssetSystem();
    AssetFactory& assetFactory = assetSystem.GetAssetFactory();
    VirtualFilesystem& filesystem = assetSystem.GetFilesystem();

    VirtualPath importPath = assetSystem.GetImportAssetPath(sourcePath);
    VirtualPath metadataPath = VirtualPath(importPath.ToString() + ".meta");

    AssetMetadata metadata = {};
    Shader* shader = nullptr;

    if (!filesystem.Exists(metadataPath))
    {
        metadata = CreateMetadata(sourcePath);

        YAML::Node metadataNode = metadata.Serialize();
        filesystem.WriteText(metadataPath, YAML::Dump(metadataNode));
    }
    else
    {
        std::string metadataNodeString = filesystem.ReadText(metadataPath);
        YAML::Node metadataNode = YAML::Load(metadataNodeString);
        metadata.Deserialize(metadataNode);
    }

    ret = assetFactory.CreateShader(metadata);

    return ret;
}

AssetMetadata ShaderImporter::CreateMetadata(const VirtualPath& sourcePath) const
{
    AssetMetadata ret = {};

    AssetSystem& assetSystem = gEngine->GetAssetSystem();

    ret.uuid = UUID::Generate();
    ret.type = AssetType::Shader;
    ret.sourcePath = sourcePath;

    const VirtualPath importPath = assetSystem.GetImportAssetPath(sourcePath);
    ret.metadataPath = VirtualPath(importPath.ToString() + ".meta");
    ret.assetPath = VirtualPath(importPath.ToString() + ".asset");

    return ret;
}

std::vector<UUID> ShaderImporter::CollectDependencies(
    const VirtualPath& sourcePath) const
{
    return {};
}

bool ShaderImporter::CanImport(const std::string& extension) const
{
    return extension == ".hlsl";
}

} // namespace URay

#include "ShaderImporter.h"

#include "Engine/Asset/AssetDatabase.h"
#include "Engine/Asset/AssetSystem.h"
#include "Engine/Asset/Shader/Shader.h"
#include "Engine/Engine.h"

#include "Core/File/VirtualFileSystem.h"
#include "Core/File/VirtualPath.h"

namespace URay
{

ShaderImporter::ShaderImporter() = default;

ShaderImporter::~ShaderImporter() = default;

void ShaderImporter::Import(const VirtualPath& sourcePath)
{
    AssetSystem& assetSystem = gEngine->GetAssetSystem();
    AssetDatabase& assetDatabase = assetSystem.GetDatabase();
    VirtualFileSystem& fileSystem = assetSystem.GetFileSystem();

    AssetMetadata metadata = {};

    const VirtualPath importPath = assetSystem.GetImportAssetPath(sourcePath);
    const VirtualPath metadataPath = VirtualPath(importPath.ToString() + ".meta");

    if (fileSystem.Exists(metadataPath))
    {
        const std::string fileText = fileSystem.ReadText(metadataPath);
        const YAML::Node node = YAML::Load(fileText);
        metadata.Deserialize(node);
    }
    else
    {
        metadata = CreateMetadata(sourcePath);
        const YAML::Node node = metadata.Serialize();
        fileSystem.WriteText(metadataPath, YAML::Dump(node));
    }

    std::unique_ptr<Shader> shader = std::make_unique<Shader>(sourcePath);
    shader->SetName(metadata.sourcePath.GetStem());
    shader->SetUUID(metadata.uuid);

    assetDatabase.Add(std::move(shader));
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

bool ShaderImporter::CanImport(const std::string& extension) const
{
    return extension == ".hlsl";
}

} // namespace URay

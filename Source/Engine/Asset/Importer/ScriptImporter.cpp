#include "ScriptImporter.h"

#include <Core/File/VirtualFileSystem.h>
#include <Core/File/VirtualPath.h>
#include <Engine/Asset/AssetDatabase.h>
#include <Engine/Asset/AssetSystem.h>
#include <Engine/Asset/Script/ScriptAsset.h>
#include <Engine/Engine.h>

namespace URay
{

void ScriptImporter::Import(const VirtualPath& sourcePath)
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

    assetSystem.RegisterMetadata(metadata);

    auto script = std::make_unique<ScriptAsset>();
    script->SetName(metadata.sourcePath.GetStem());
    script->SetHandle(metadata.handle);

    assetDatabase.Add(std::move(script));
}

AssetMetadata ScriptImporter::CreateMetadata(const VirtualPath& sourcePath) const
{
    AssetSystem& assetSystem = gEngine->GetAssetSystem();

    AssetMetadata metadata = {};
    metadata.handle = AssetHandle::Generate();
    metadata.type = AssetType::Script;
    metadata.sourcePath = sourcePath;

    const VirtualPath importPath = assetSystem.GetImportAssetPath(sourcePath);
    metadata.metadataPath = VirtualPath(importPath.ToString() + ".meta");
    metadata.assetPath = VirtualPath(importPath.ToString() + ".asset");

    return metadata;
}

bool ScriptImporter::CanImport(const std::string& extension) const
{
    return extension == ".cs";
}

} // namespace URay

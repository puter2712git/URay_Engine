#include "ShaderImporter.h"

#include <Engine/Asset/AssetDatabase.h>
#include <Engine/Asset/AssetSystem.h>
#include <Engine/Asset/Shader/Shader.h>
#include <Engine/Engine.h>

#include <Core/File/VirtualFileSystem.h>
#include <Core/File/VirtualPath.h>

namespace URay
{

ShaderImporter::ShaderImporter(AssetSystem& assetSystem, VirtualFileSystem& fileSystem)
    : Importer(assetSystem, fileSystem) {}

ShaderImporter::~ShaderImporter() = default;

void ShaderImporter::Import(const VirtualPath& sourcePath)
{
    AssetSystem& assetSystem = gEngine->GetAssetSystem();
    AssetDatabase& assetDatabase = assetSystem.GetDatabase();
    VirtualFileSystem& fileSystem = assetSystem.GetFileSystem();

    const VirtualPath metadataPath = assetSystem.GetMetadataPath(sourcePath);
    const AssetMetadata metadata = LoadOrCreateMetadata(sourcePath, metadataPath, AssetType::Shader);

    assetSystem.RegisterMetadata(metadata);

    std::unique_ptr<Shader> shader = std::make_unique<Shader>(sourcePath);
    shader->SetName(metadata.sourcePath.GetStem());
    shader->SetHandle(metadata.handle);

    assetDatabase.Add(std::move(shader));
}

bool ShaderImporter::CanImport(const std::string& extension) const
{
    return extension == ".hlsl";
}

} // namespace URay

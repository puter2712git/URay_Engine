#include "AssetPipeline.h"

#include <Core/File/VirtualFileSystem.h>
#include <Core/Log/LogSystem.h>
#include <Engine/Asset/AssetDatabase.h>
#include <Engine/Asset/AssetFactory.h>
#include <Engine/Asset/AssetSystem.h>
#include <Engine/Asset/Importer/FBXImporter.h>
#include <Engine/Asset/Importer/FontImporter.h>
#include <Engine/Asset/Importer/MaterialImporter.h>
#include <Engine/Asset/Importer/OBJImporter.h>
#include <Engine/Asset/Importer/ShaderImporter.h>
#include <Engine/Asset/Importer/TextureImporter.h>
#include <Engine/Asset/Metadata/AssetMetadataSerializer.h>

namespace URay
{

AssetPipeline::AssetPipeline(AssetSystem& assetSystem, VirtualFileSystem& fileSystem)
    : assetSystem(assetSystem), fileSystem(fileSystem)
{
    importers.push_back(std::make_unique<TextureImporter>(assetSystem, fileSystem));
    importers.push_back(std::make_unique<ShaderImporter>(assetSystem, fileSystem));
    importers.push_back(std::make_unique<MaterialImporter>(assetSystem, fileSystem));
    importers.push_back(std::make_unique<FontImporter>(assetSystem, fileSystem));
    importers.push_back(std::make_unique<OBJImporter>(assetSystem, fileSystem));
    importers.push_back(std::make_unique<FBXImporter>(assetSystem, fileSystem));
}

AssetPipeline::~AssetPipeline() = default;

bool AssetPipeline::Execute(const VirtualPath& path) const
{
    const std::string extension = path.GetExtension();

    Importer* importer = FindImporter(extension);
    if (!importer)
        return false;

    DependencyResult dependencies = importer->DiscoverDependencies(path);
    if (!dependencies)
        return false;

    for (const VirtualPath& dependency : dependencies.sourcePaths)
    {
        if (!Execute(dependency))
            return false;
    }

    ImportResult result = importer->Import(path);
    if (!result)
    {
        URAY_LOG("[AssetPipeline] Failed to import asset: %s. Error: %s", path.ToString().c_str(), result.errorMessage.c_str());
        return false;
    }

    for (const AssetMetadata& metadata : result.metadatas)
    {
        const VirtualPath metadataPath = assetSystem.GetMetadataPath(metadata.assetPath);

        if (!assetSystem.GetMetadataSerializer().Save(metadata, metadataPath))
        {
            URAY_LOG("[AssetPipeline] Failed to save metadata: %s", metadataPath.ToString().c_str());
            return false;
        }

        std::unique_ptr<Asset> asset = assetSystem.GetFactory().Create(metadata);

        if (!asset)
        {
            URAY_LOG("[AssetPipeline] Failed to create asset: %s", metadata.assetPath.ToString().c_str());
            return false;
        }

        assetSystem.GetDatabase().InsertOrReplace(std::move(asset));
    }

    return true;
}

Importer* AssetPipeline::FindImporter(const std::string& extension) const
{
    for (const auto& importer : importers)
    {
        if (importer->CanImport(extension))
            return importer.get();
    }

    return nullptr;
}

} // namespace URay

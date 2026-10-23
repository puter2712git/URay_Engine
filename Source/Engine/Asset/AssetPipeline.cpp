#include "AssetPipeline.h"

#include <Engine/Asset/AssetSystem.h>
#include <Engine/Asset/Importer/FBXImporter.h>
#include <Engine/Asset/Importer/FontImporter.h>
#include <Engine/Asset/Importer/MaterialImporter.h>
#include <Engine/Asset/Importer/OBJImporter.h>
#include <Engine/Asset/Importer/ShaderImporter.h>
#include <Engine/Asset/Importer/TextureImporter.h>
#include <Engine/Engine.h>

#include <Core/File/VirtualFileSystem.h>

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

void AssetPipeline::Execute(const VirtualPath& path) const
{
    AssetSystem& assetSystem = gEngine->GetAssetSystem();
    VirtualFileSystem& fileSystem = assetSystem.GetFileSystem();

    const std::string extension = path.GetExtension();

    Importer* importer = FindImporter(extension);
    if (!importer)
        return;

    importer->Import(path);
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

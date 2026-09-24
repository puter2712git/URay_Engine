#include "ObjImporter.h"

#include "Engine/Asset/AssetFactory.h"
#include "Engine/Asset/AssetSystem.h"
#include "Engine/Asset/DefaultAssets.h"
#include "Engine/Asset/Material/Material.h"
#include "Engine/Asset/Mesh/Mesh.h"
#include "Engine/Asset/Texture/Texture.h"
#include "Engine/Engine.h"

#include "Core/File/VirtualFilesystem.h"
#include "Core/Type/Types.h"

#include <iostream>
#include <sstream>
#include <unordered_map>

namespace URay
{

OBJImporter::OBJImporter() = default;

OBJImporter::~OBJImporter() = default;

Asset* OBJImporter::Import(const VirtualPath& sourcePath)
{
    Asset* ret = nullptr;

    AssetSystem& assetSystem = gEngine->GetAssetSystem();
    AssetFactory& assetFactory = assetSystem.GetAssetFactory();
    VirtualFilesystem& filesystem = assetSystem.GetFilesystem();

    const VirtualPath importPath = assetSystem.GetImportAssetPath(sourcePath);
    const VirtualPath metadataPath = VirtualPath(importPath.ToString() + ".meta");
    const VirtualPath assetPath = VirtualPath(importPath.ToString() + ".asset");

    AssetMetadata metadata = {};

    if (!filesystem.Exists(metadataPath))
    {
        metadata = CreateMetadata(sourcePath);
        metadata.dependencies = CollectDependencies(sourcePath);

        const YAML::Node metadataNode = metadata.Serialize();
        filesystem.WriteText(metadataPath, YAML::Dump(metadataNode));
    }
    else
    {
        const std::string metadataNodeString = filesystem.ReadText(metadataPath);
        const YAML::Node metadataNode = YAML::Load(metadataNodeString);

        metadata.Deserialize(metadataNode);
    }

    // Load mesh
    std::vector<Material*> defaultMaterials;

    for (const UUID& uuid : metadata.dependencies)
    {
        std::optional<AssetMetadata> dependencyMetadata = assetSystem.FindAssetMetadataByUUID(uuid);
        if (dependencyMetadata.has_value())
            continue;

        switch (dependencyMetadata->type)
        {
        case AssetType::Material:
        {
            Material* material = assetSystem.Find<Material>(dependencyMetadata->uuid);

            if (material)
            {
                defaultMaterials.push_back(material);
            }
        }
        default:
            break;
        }
    }

    return ret;
}

AssetMetadata OBJImporter::CreateMetadata(const VirtualPath& sourcePath) const
{
    AssetMetadata ret = {};

    AssetSystem& assetSystem = gEngine->GetAssetSystem();

    ret.uuid = UUID::Generate();
    ret.type = AssetType::Material;
    ret.sourcePath = sourcePath;

    const VirtualPath importPath = assetSystem.GetImportAssetPath(sourcePath);
    ret.metadataPath = VirtualPath(importPath.ToString() + ".meta");
    ret.assetPath = VirtualPath(importPath.ToString() + ".asset");

    return ret;
}

std::vector<UUID> OBJImporter::CollectDependencies(const VirtualPath& sourcePath) const
{
    return {};
}

bool OBJImporter::CanImport(const std::string& extension) const
{
    return extension == ".obj";
}

} // namespace URay

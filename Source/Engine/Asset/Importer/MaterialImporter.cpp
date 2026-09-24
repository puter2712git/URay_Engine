#include "MaterialImporter.h"

#include "Engine/Asset/AssetFactory.h"
#include "Engine/Asset/AssetSystem.h"
#include "Engine/Asset/Material/Material.h"
#include "Engine/Asset/Shader/Shader.h"
#include "Engine/Asset/Texture/Texture.h"
#include "Engine/Engine.h"

#include "Core/File/VirtualFilesystem.h"
#include "Core/File/VirtualPath.h"

#include <yaml-cpp/yaml.h>

namespace URay
{

MaterialImporter::MaterialImporter() = default;

MaterialImporter::~MaterialImporter() = default;

Asset* MaterialImporter::Import(const VirtualPath& sourcePath)
{
    Asset* ret = nullptr;

    AssetSystem& assetSystem = gEngine->GetAssetSystem();
    AssetFactory& factory = assetSystem.GetAssetFactory();
    VirtualFilesystem& filesystem = assetSystem.GetFilesystem();

    const VirtualPath importPath = assetSystem.GetImportAssetPath(sourcePath);
    const VirtualPath metadataPath = VirtualPath(importPath.ToString() + ".meta");

    AssetMetadata metadata = {};
    if (!filesystem.Exists(metadataPath))
    {
        metadata = CreateMetadata(sourcePath);

        const YAML::Node node = metadata.Serialize();
        filesystem.WriteText(metadataPath, YAML::Dump(node));
    }
    else
    {
        const std::string fileText = filesystem.ReadText(metadataPath);
        const YAML::Node node = YAML::Load(fileText);
        metadata.Deserialize(node);
    }

    ret = factory.CreateMaterial(metadata);

    return ret;
}

AssetMetadata MaterialImporter::CreateMetadata(const VirtualPath& sourcePath) const
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

std::vector<UUID> MaterialImporter::CollectDependencies(
    const VirtualPath& sourcePath) const
{
    std::vector<UUID> ret;

    AssetSystem& assetSystem = gEngine->GetAssetSystem();
    VirtualFilesystem& filesystem = assetSystem.GetFilesystem();

    const std::string fileText = filesystem.ReadText(sourcePath);
    const YAML::Node node = YAML::Load(fileText);

    const UUID shaderUUID = UUID::FromString(node["Shader"].as<std::string>());
    ret.push_back(shaderUUID);

    const YAML::Node parameters = node["Parameters"];
    for (const auto& entry : parameters)
    {
        const std::string type = entry.second["Type"].as<std::string>();
        const YAML::Node value = entry.second["Value"];

        if (type == "Texture2D")
        {
            const std::string textureSourcePath = value.as<std::string>();
            std::optional<UUID> uuid = assetSystem.FindUUIDBySourcePath(textureSourcePath);

            ret.push_back(uuid.value());
        }
    }

    return ret;
}

bool MaterialImporter::CanImport(const std::string& extension) const
{
    return extension == ".urmat";
}

} // namespace URay

#include "MaterialImporter.h"

#include "Engine/Asset/AssetDatabase.h"
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

void MaterialImporter::Import(const VirtualPath& sourcePath)
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

    const std::string materialText = fileSystem.ReadText(metadata.sourcePath);
    const YAML::Node materialNode = YAML::Load(materialText);

    const UUID shaderUUID = UUID::FromString(materialNode["Shader"].as<std::string>());

    std::unique_ptr<Material> material = std::make_unique<Material>(shaderUUID);
    material->SetName(metadata.sourcePath.GetStem());
    material->SetUUID(metadata.uuid);

    const YAML::Node parametersNode = materialNode["Parameters"];
    if (parametersNode)
    {
        for (const auto& parameterNode : parametersNode)
        {
            const std::string name = parameterNode.first.as<std::string>();
            const YAML::Node parameter = parameterNode.second;

            const std::string typeName = parameter["Type"].as<std::string>();

            if (typeName == "Texture2D")
            {
                const UUID textureUUID = UUID::FromString(parameter["Value"].as<std::string>());
                material->AddParameter(name, MaterialParameterType::Texture2D, textureUUID);
            }
        }
    }

    assetDatabase.Add(std::move(material));
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

bool MaterialImporter::CanImport(const std::string& extension) const
{
    return extension == ".urmat";
}

} // namespace URay

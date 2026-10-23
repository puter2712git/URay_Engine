#include "MaterialImporter.h"

#include <Core/File/VirtualFileSystem.h>
#include <Core/File/VirtualPath.h>
#include <Engine/Asset/AssetDatabase.h>
#include <Engine/Asset/AssetSystem.h>
#include <Engine/Asset/Material/Material.h>
#include <Engine/Asset/Shader/Shader.h>
#include <Engine/Asset/Texture/Texture.h>
#include <Engine/Engine.h>

#include <yaml-cpp/yaml.h>

namespace URay
{

MaterialImporter::MaterialImporter(AssetSystem& assetSystem, VirtualFileSystem& fileSystem)
    : Importer(assetSystem, fileSystem) {}

MaterialImporter::~MaterialImporter() = default;

void MaterialImporter::Import(const VirtualPath& sourcePath)
{
    AssetSystem& assetSystem = gEngine->GetAssetSystem();
    AssetDatabase& assetDatabase = assetSystem.GetDatabase();
    VirtualFileSystem& fileSystem = assetSystem.GetFileSystem();

    const VirtualPath metadataPath = assetSystem.GetMetadataPath(sourcePath);

    AssetMetadata metadata = LoadOrCreateMetadata(sourcePath, metadataPath, Material::StaticClass());

    assetSystem.RegisterMetadata(metadata);

    const std::string materialText = fileSystem.ReadText(metadata.sourcePath);
    const YAML::Node materialNode = YAML::Load(materialText);

    const AssetHandle shaderHandle = AssetHandle::FromString(materialNode["Shader"].as<std::string>());

    std::unique_ptr<Material> material = std::make_unique<Material>(shaderHandle);
    material->SetName(metadata.sourcePath.GetStem());
    material->SetHandle(metadata.handle);

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
                const AssetHandle textureHandle = AssetHandle::FromString(parameter["Value"].as<std::string>());
                material->AddParameter(name, MaterialParameterType::Texture2D, textureHandle);
            }
        }
    }

    assetDatabase.Add(std::move(material));
}

bool MaterialImporter::CanImport(const std::string& extension) const
{
    return extension == ".urmat";
}

} // namespace URay

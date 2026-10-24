#include "MaterialImporter.h"

#include <Core/File/VirtualFileSystem.h>
#include <Core/File/VirtualPath.h>
#include <Core/Serialization/YAMLConverters.h>
#include <Engine/Asset/AssetSystem.h>
#include <Engine/Asset/Material/Material.h>
#include <Engine/Asset/Material/MaterialData.h>
#include <Engine/Asset/Material/MaterialSerializer.h>
#include <Engine/Asset/Metadata/AssetMetadataSerializer.h>
#include <Engine/Asset/Shader/Shader.h>
#include <Engine/Asset/Texture/Texture.h>

#include <yaml-cpp/yaml.h>

namespace URay
{

MaterialImporter::MaterialImporter(AssetSystem& assetSystem, VirtualFileSystem& fileSystem)
    : Importer(assetSystem, fileSystem) {}

MaterialImporter::~MaterialImporter() = default;

ImportResult MaterialImporter::Import(const ImportRequest& request)
{
    const VirtualPath assetPath = sourcePath;
    const VirtualPath metadataPath = assetSystem.GetMetadataPath(assetPath);

    MaterialData materialData;
    if (!ParseMaterial(materialData, sourcePath))
        return ImportResult::Failure("Failed to parse material: " + sourcePath.ToString());

    // MaterialSerializer& serializer = assetSystem.GetMaterialSerializer();
    // serializer.Save(materialData, assetPath);

    AssetMetadata metadata = LoadOrCreateMetadata(sourcePath, metadataPath, Material::StaticClass());

    ImportResult result = ImportResult::Success();
    result.metadatas.push_back(metadata);
    return result;

    // assetSystem.GetMetadataSerializer().Save(metadata, metadataPath);
    // assetSystem.RegisterMetadata(metadata);

    // assetSystem.LoadMaterial(metadata);
}

bool MaterialImporter::CanImport(const std::string& extension) const
{
    return extension == ".urmat";
}

bool MaterialImporter::ParseMaterial(MaterialData& outMaterial, const VirtualPath& sourcePath) const
{
    const std::string fileText = fileSystem.ReadText(sourcePath);
    if (fileText.empty())
        return false;

    YAML::Node node = YAML::Load(fileText);
    outMaterial.Deserialize(node);

    return true;
}

} // namespace URay

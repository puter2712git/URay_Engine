#include "AssetMetadataSerializer.h"

#include <Core/File/VirtualFileSystem.h>
#include <Engine/Asset/Metadata/AssetMetadata.h>

#include <yaml-cpp/yaml.h>

namespace URay
{

AssetMetadataSerializer::AssetMetadataSerializer(VirtualFileSystem& fileSystem)
    : fileSystem(fileSystem) {}

bool AssetMetadataSerializer::Save(const AssetMetadata& metadata, const VirtualPath& metadataPath) const
{
    YAML::Node node = metadata.Serialize();

    const std::string yamlString = YAML::Dump(node);

    return fileSystem.WriteText(metadataPath, yamlString);
}

bool AssetMetadataSerializer::Load(AssetMetadata& outMetadata, const VirtualPath& metadataPath) const
{
    if (!fileSystem.Exists(metadataPath))
        return false;

    const std::string yamlString = fileSystem.ReadText(metadataPath);
    const YAML::Node node = YAML::Load(yamlString);
    outMetadata.Deserialize(node);

    return true;
}

} // namespace URay

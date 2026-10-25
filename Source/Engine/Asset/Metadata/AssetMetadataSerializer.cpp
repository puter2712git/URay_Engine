#include "AssetMetadataSerializer.h"
#include <Core/Type/Types.h>

#include <Core/File/VirtualFileSystem.h>
#include <Core/Log/LogSystem.h>
#include <Engine/Asset/Metadata/AssetMetadata.h>

#include <yaml-cpp/yaml.h>

namespace URay
{

AssetMetadataSerializer::AssetMetadataSerializer(VirtualFileSystem& fileSystem)
    : fileSystem(fileSystem) {}

bool AssetMetadataSerializer::Save(const AssetMetadata& metadata, const VirtualPath& metadataPath) const
{
    YAML::Node node = metadata.Serialize();

    const String yamlString = YAML::Dump(node);

    return fileSystem.WriteText(metadataPath, yamlString);
}

bool AssetMetadataSerializer::Load(AssetMetadata& outMetadata, const VirtualPath& metadataPath) const
{
    if (!fileSystem.Exists(metadataPath))
        return false;

    try
    {
        const String yamlString = fileSystem.ReadText(metadataPath);
        const YAML::Node node = YAML::Load(yamlString);
        outMetadata.Deserialize(node);
        return true;
    }
    catch (const YAML::ParserException& e)
    {
        URAY_LOG("[AssetMetadataSerializer] Failed to parse metadata file: %s. Error: %s", metadataPath.ToString().c_str(), e.what());
        return false;
    }
    catch (const YAML::BadFile& e)
    {
        URAY_LOG("[AssetMetadataSerializer] Failed to read metadata file: %s. Error: %s", metadataPath.ToString().c_str(), e.what());
        return false;
    }
}

} // namespace URay

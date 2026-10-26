#include "AssetMetadata.h"
#include <Core/Type/TypeAlias.h>

#include <Core/Serialization/YAMLConverters.h>

namespace URay
{

YAML::Node AssetMetadata::Serialize() const
{
    YAML::Node node;

    node["MetadataVersion"] = metadataVersion;

    node["Handle"] = handle;
    node["ClassName"] = className;

    node["AssetPath"] = assetPath.ToString();
    node["SourcePath"] = sourcePath.ToString();
    node["ImporterVersion"] = importerVersion;

    return node;
}

void AssetMetadata::Deserialize(const YAML::Node& node)
{
    metadataVersion = node["MetadataVersion"].as<uint32>();

    handle = node["Handle"].as<UUID>();
    className = node["ClassName"].as<String>();

    assetPath = VirtualPath(node["AssetPath"].as<String>());
    sourcePath = VirtualPath(node["SourcePath"].as<String>());
    importerVersion = node["ImporterVersion"].as<uint32>();
}

} // namespace URay

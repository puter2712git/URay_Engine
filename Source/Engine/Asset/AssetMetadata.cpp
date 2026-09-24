#include "AssetMetadata.h"

namespace URay
{

YAML::Node AssetMetadata::Serialize() const
{
    YAML::Node node;
    node["UUID"] = uuid.ToString();

    switch (type)
    {
    case AssetType::Texture:
        node["Type"] = "Texture";
        break;
    case AssetType::Mesh:
        node["Type"] = "Mesh";
        break;
    case AssetType::Material:
        node["Type"] = "Material";
        break;
    case AssetType::Unknown:
    default:
        node["Type"] = "Unknown";
        break;
    }

    node["SourcePath"] = sourcePath.ToString();
    node["MetadataPath"] = metadataPath.ToString();
    node["AssetPath"] = assetPath.ToString();

    return node;
}

void AssetMetadata::Deserialize(const YAML::Node& node)
{
    uuid = UUID::FromString(node["UUID"].as<std::string>());

    std::string typeString = node["Type"].as<std::string>();

    if (typeString == "Texture")
    {
        type = AssetType::Texture;
    }
    else if (typeString == "Mesh")
    {
        type = AssetType::Mesh;
    }
    else if (typeString == "Material")
    {
        type = AssetType::Material;
    }
    else
    {
        type = AssetType::Unknown;
    }

    sourcePath = VirtualPath(node["SourcePath"].as<std::string>());
    metadataPath = VirtualPath(node["MetadataPath"].as<std::string>());
    assetPath = VirtualPath(node["AssetPath"].as<std::string>());
}

} // namespace URay

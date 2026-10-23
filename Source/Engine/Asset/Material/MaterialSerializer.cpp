#include "MaterialSerializer.h"

#include <Core/File/VirtualFileSystem.h>
#include <Core/Type/Types.h>
#include <Engine/Asset/Material/MaterialData.h>

#include <yaml-cpp/yaml.h>

namespace URay
{

constexpr uint32 MaterialAssetVersion = 1;

MaterialSerializer::MaterialSerializer(VirtualFileSystem& fileSystem)
    : fileSystem(fileSystem) {}

bool MaterialSerializer::Save(const MaterialData& material, const VirtualPath& assetPath) const
{
    YAML::Node node;

    node["Version"] = MaterialAssetVersion;
    material.Serialize(node);

    return fileSystem.WriteText(assetPath, YAML::Dump(node));
}

bool MaterialSerializer::Load(MaterialData& outMaterial, const VirtualPath& assetPath) const
{
    const std::string fileText = fileSystem.ReadText(assetPath);
    if (fileText.empty())
        return false;

    YAML::Node node = YAML::Load(fileText);
    if (node["Version"].as<uint32>() != MaterialAssetVersion)
        return false;

    outMaterial.Deserialize(node);

    return true;
}

} // namespace URay
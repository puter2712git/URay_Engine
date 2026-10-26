#pragma once
#include <Core/Type/TypeAlias.h>

#include <Engine/Asset/AssetType.h>

#include <Core/File/VirtualPath.h>
#include <Engine/Asset/AssetHandle.h>

#include <yaml-cpp/yaml.h>

#include <vector>

namespace URay
{

constexpr static uint32 AssetMetadataVersion = 1;

struct AssetMetadata
{
    uint32 metadataVersion = AssetMetadataVersion;

    AssetHandle handle = {};
    String className;

    VirtualPath assetPath;
    VirtualPath sourcePath;
    uint32 importerVersion = 0;

    YAML::Node Serialize() const;
    void Deserialize(const YAML::Node& node);
};

} // namespace URay

#pragma once

#include "Engine/Asset/AssetType.h"

#include "Core/File/VirtualPath.h"
#include "Engine/Asset/Asset.h"

#include <yaml-cpp/yaml.h>

#include <vector>

namespace URay
{

struct AssetMetadata
{
    AssetHandle handle = {};
    AssetType type = AssetType::Unknown;
    VirtualPath sourcePath;
    VirtualPath metadataPath;
    VirtualPath assetPath;

    YAML::Node Serialize() const;
    void Deserialize(const YAML::Node& node);
};

} // namespace URay

#pragma once

#include "Engine/Asset/AssetType.h"

#include "Core/File/VirtualPath.h"
#include "Core/UUID.h"

namespace URay
{

struct AssetMetadata
{
    UUID uuid = {};
    AssetType type = AssetType::Unknown;
    VirtualPath sourcePath;
    VirtualPath importPath;
};

} // namespace URay

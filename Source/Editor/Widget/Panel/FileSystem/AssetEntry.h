#pragma once

#include <Core/File/VirtualPath.h>
#include <Engine/Asset/AssetHandle.h>
#include <Engine/Asset/AssetType.h>

#include <string>

namespace URay
{

enum class AssetEntryKind
{
    Directory,
    Asset,
    Scene
};

struct AssetEntry
{
    AssetEntryKind kind = AssetEntryKind::Directory;
    VirtualPath path; // Source Path
    std::string displayName;

    AssetHandle assetHandle = {};
    std::string className;
};

} // namespace URay

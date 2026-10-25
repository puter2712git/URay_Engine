#pragma once
#include <Core/Type/Types.h>

#include <Core/File/VirtualPath.h>
#include <Engine/Asset/AssetHandle.h>
#include <Engine/Asset/AssetType.h>


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
    String displayName;

    AssetHandle assetHandle = {};
    String className;
};

} // namespace URay

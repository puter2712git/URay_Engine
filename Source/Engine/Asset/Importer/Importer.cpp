#include "Importer.h"
#include <Core/Type/TypeAlias.h>

#include <Core/File/VirtualFileSystem.h>
#include <Engine/Asset/AssetSystem.h>
#include <Engine/Asset/Metadata/AssetMetadataSerializer.h>
#include <Engine/Object/Class/Class.h>

namespace URay
{

Importer::Importer(AssetSystem& assetSystem, VirtualFileSystem& fileSystem)
    : assetSystem(assetSystem), fileSystem(fileSystem) {}

AssetMetadata Importer::LoadOrCreateMetadata(const VirtualPath& sourcePath, const VirtualPath& metadataPath, Class* assetClass) const
{
    const String metadataText = fileSystem.ReadText(metadataPath);

    AssetMetadata metadata;

    if (assetSystem.GetMetadataSerializer().Load(metadata, metadataPath))
        return metadata;

    metadata.handle = AssetHandle::Generate();
    metadata.className = assetClass->GetType().name;
    metadata.assetPath = metadataPath.RemoveExtension();
    metadata.sourcePath = sourcePath;

    return metadata;
}

} // namespace URay

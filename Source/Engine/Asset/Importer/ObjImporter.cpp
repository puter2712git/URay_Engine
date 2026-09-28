#include "ObjImporter.h"

#include "Engine/Asset/AssetSystem.h"
#include "Engine/Asset/Material/Material.h"
#include "Engine/Asset/Mesh/Mesh.h"
#include "Engine/Asset/Texture/Texture.h"
#include "Engine/Engine.h"

#include "Core/File/VirtualFilesystem.h"
#include "Core/Type/Types.h"

#include <iostream>
#include <sstream>
#include <unordered_map>

namespace URay
{

OBJImporter::OBJImporter() = default;

OBJImporter::~OBJImporter() = default;

void OBJImporter::Import(const VirtualPath& sourcePath)
{
    AssetSystem& assetSystem = gEngine->GetAssetSystem();
    VirtualFileSystem& filesystem = assetSystem.GetFileSystem();
}

AssetMetadata OBJImporter::CreateMetadata(const VirtualPath& sourcePath) const
{
    AssetMetadata ret = {};

    AssetSystem& assetSystem = gEngine->GetAssetSystem();

    ret.handle = AssetHandle::Generate();
    ret.type = AssetType::Material;
    ret.sourcePath = sourcePath;

    const VirtualPath importPath = assetSystem.GetImportAssetPath(sourcePath);
    ret.metadataPath = VirtualPath(importPath.ToString() + ".meta");
    ret.assetPath = VirtualPath(importPath.ToString() + ".asset");

    return ret;
}

bool OBJImporter::CanImport(const std::string& extension) const
{
    return extension == ".obj";
}

} // namespace URay

#include "AssetSystem.h"

#include <Engine/Asset/AssetDatabase.h>
#include <Engine/Asset/AssetPipeline.h>
#include <Engine/Asset/EngineAsset.h>
#include <Engine/Asset/Importer/MaterialImporter.h>
#include <Engine/Asset/Importer/OBJImporter.h>
#include <Engine/Asset/Importer/ShaderImporter.h>
#include <Engine/Asset/Importer/TextureImporter.h>
#include <Engine/Asset/Material/Material.h>
#include <Engine/Asset/Mesh/MeshGenerator.h>
#include <Engine/Asset/Shader/Shader.h>
#include <Engine/Asset/Texture/Texture.h>
#include <Engine/Engine.h>
#include <Engine/Object/Object.h>

#include <Core/File/VirtualFileSystem.h>

#include <iostream>
#include <string>

namespace URay
{

AssetSystem::AssetSystem() = default;

AssetSystem::~AssetSystem() = default;

bool AssetSystem::Initialize(
    const std::string& enginePath,
    const std::string& projectPath)
{
    fileSystem = std::make_unique<VirtualFileSystem>();
    if (!fileSystem->Initialize(enginePath, projectPath))
        return false;

    database = std::make_unique<AssetDatabase>();
    pipeline = std::make_unique<AssetPipeline>();

    ImportRecursive("Engine://Asset/Source");
    ImportRecursive("Project://Asset/Source");

    CreateDefaultAssets();

    return true;
}

void AssetSystem::Finalize()
{
    pipeline.reset();
    database.reset();
    metadataBySourcePath.clear();

    fileSystem.reset();
}

void AssetSystem::ImportRecursive(const VirtualPath& path)
{
    const bool isDirectory = fileSystem->IsDirectory(path);

    if (!isDirectory)
    {
        pipeline->Execute(path);
        return;
    }

    const auto& fileEntries = fileSystem->ListDirectory(path);
    for (const auto& entry : fileEntries)
    {
        ImportRecursive(entry.path);
    }
}

void AssetSystem::CreateDefaultAssets()
{
    MeshGenerator meshGenerator;

    CreateDefaultMesh(EngineAsset::QuadMesh, "Quad", meshGenerator.CreateQuad(), EngineAsset::MeshMaterial);
    CreateDefaultMesh(EngineAsset::CubeMesh, "Cube", meshGenerator.CreateCube(), EngineAsset::MeshMaterial);
    CreateDefaultMesh(EngineAsset::CylinderMesh, "Cylinder", meshGenerator.CreateCylinder(), EngineAsset::MeshMaterial);
    CreateDefaultMesh(EngineAsset::ConeMesh, "Cone", meshGenerator.CreateCone(), EngineAsset::MeshMaterial);
    CreateDefaultMesh(EngineAsset::ArrowMesh, "Arrow", meshGenerator.CreateArrow(), EngineAsset::MeshMaterial);
    CreateDefaultMesh(EngineAsset::RotationGizmoMesh, "Rotation Gizmo", meshGenerator.CreateRotationGizmo(), EngineAsset::MeshMaterial);
    CreateDefaultMesh(EngineAsset::ScaleGizmoMesh, "Scale Gizmo", meshGenerator.CreateScaleGizmo(), EngineAsset::MeshMaterial);
}

VirtualPath AssetSystem::GetImportAssetPath(const VirtualPath& sourcePath) const
{
    if (sourcePath.GetMountName() == "RawAsset")
        return VirtualPath("Asset://" + sourcePath.GetRelativePath());

    constexpr const char* sourceDirectory = "/Source/";
    constexpr const char* importDirectory = "/Imported/";

    std::string importPath = sourcePath.ToString();
    const size_t sourceDirectoryPos = importPath.find(sourceDirectory);

    if (sourceDirectoryPos == std::string::npos)
        return sourcePath;

    importPath.replace(
        sourceDirectoryPos,
        std::char_traits<char>::length(sourceDirectory),
        importDirectory);

    return VirtualPath(importPath);
}

void AssetSystem::RegisterMetadata(const AssetMetadata& metadata)
{
    if (!metadata.handle.IsValid() || metadata.sourcePath.ToString().empty())
        return;

    metadataBySourcePath.insert_or_assign(metadata.sourcePath.ToString(), metadata);
}

const AssetMetadata* AssetSystem::FindMetadataBySourcePath(
    const VirtualPath& sourcePath) const
{
    const auto it = metadataBySourcePath.find(sourcePath.ToString());
    if (it == metadataBySourcePath.end())
        return nullptr;

    return &it->second;
}

void AssetSystem::CreateDefaultMesh(const AssetHandle& handle, const std::string& name, const MeshInfo& meshInfo, const AssetHandle& materialHandle)
{
    std::unique_ptr<Mesh> mesh = std::make_unique<Mesh>();
    mesh->SetHandle(handle);
    mesh->SetName(name);
    mesh->SetVertices(meshInfo.vertices);
    mesh->SetIndices(meshInfo.indices);
    mesh->SetSections(meshInfo.sections);
    mesh->SetDefaultMaterials({ materialHandle });

    database->Add(std::move(mesh));
}

} // namespace URay

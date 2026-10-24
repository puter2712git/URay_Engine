#include "AssetSystem.h"

#include <Core/File/VirtualFileSystem.h>
#include <Core/Log/LogSystem.h>
#include <Engine/Asset/AssetDatabase.h>
#include <Engine/Asset/AssetFactory.h>
#include <Engine/Asset/AssetPipeline.h>
#include <Engine/Asset/EngineAsset.h>
#include <Engine/Asset/Font/Font.h>
#include <Engine/Asset/Font/FontSerializer.h>
#include <Engine/Asset/Importer/ImportRequest.h>
#include <Engine/Asset/Importer/OBJImporter.h>
#include <Engine/Asset/Importer/ShaderImporter.h>
#include <Engine/Asset/Importer/TextureImporter.h>
#include <Engine/Asset/Material/Material.h>
#include <Engine/Asset/Material/MaterialData.h>
#include <Engine/Asset/Material/MaterialSerializer.h>
#include <Engine/Asset/Mesh/MeshGenerator.h>
#include <Engine/Asset/Mesh/MeshSerializer.h>
#include <Engine/Asset/Metadata/AssetMetadataSerializer.h>
#include <Engine/Asset/Shader/Shader.h>
#include <Engine/Asset/Texture/Texture.h>
#include <Engine/Asset/Texture/TextureData.h>
#include <Engine/Asset/Texture/TextureSerializer.h>
#include <Engine/Engine.h>
#include <Engine/Object/Object.h>

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
    pipeline = std::make_unique<AssetPipeline>(*this, *fileSystem);
    factory = std::make_unique<AssetFactory>(*this);
    metadataSerializer = std::make_unique<AssetMetadataSerializer>(*fileSystem);
    textureSerializer = std::make_unique<TextureSerializer>(*fileSystem);
    materialSerializer = std::make_unique<MaterialSerializer>(*fileSystem);
    fontSerializer = std::make_unique<FontSerializer>(*fileSystem);
    meshSerializer = std::make_unique<MeshSerializer>(*fileSystem);

    LoadAssetsRecursive("Engine://Asset");
    LoadAssetsRecursive("Project://Asset");

    CreateDefaultAssets();

    return true;
}

void AssetSystem::Finalize()
{
    meshSerializer.reset();
    fontSerializer.reset();
    materialSerializer.reset();
    textureSerializer.reset();
    metadataSerializer.reset();
    factory.reset();
    pipeline.reset();
    database.reset();

    fileSystem.reset();
}

bool AssetSystem::Import(const ImportRequest& request)
{
    return pipeline->Execute(request);
}

void AssetSystem::LoadAssetsRecursive(const VirtualPath& path)
{
    if (fileSystem->IsDirectory(path))
    {
        for (const auto& entry : fileSystem->ListDirectory(path))
        {
            LoadAssetsRecursive(entry.path);
        }
        return;
    }

    if (path.GetExtension() != ".meta")
        return;

    AssetMetadata metadata;
    if (!metadataSerializer->Load(metadata, path))
        return;

    const VirtualPath expectedPath = GetMetadataPath(metadata.assetPath);
    if (expectedPath.ToString() != path.ToString())
        return;

    if (!fileSystem->Exists(metadata.assetPath))
        return;

    if (!RegisterAsset(metadata))
    {
        URAY_LOG("[AssetSystem] Failed to register asset: %s", metadata.assetPath.ToString().c_str());
        return;
    }
}

bool AssetSystem::RegisterAsset(const AssetMetadata& metadata)
{
    std::unique_ptr<Asset> asset = factory->Create(metadata);
    if (!asset)
        return false;

    database->InsertOrReplace(std::move(asset));
    return true;
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

VirtualPath AssetSystem::GetMetadataPath(const VirtualPath& sourcePath) const
{
    return VirtualPath(sourcePath.ToString() + ".meta");
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

    database->InsertOrReplace(std::move(mesh));
}

} // namespace URay

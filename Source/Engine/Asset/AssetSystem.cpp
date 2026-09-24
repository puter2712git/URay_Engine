#include "AssetSystem.h"

#include "Engine/Asset/AssetFactory.h"
#include "Engine/Asset/Importer/MaterialImporter.h"
#include "Engine/Asset/Importer/OBJImporter.h"
#include "Engine/Asset/Importer/ShaderImporter.h"
#include "Engine/Asset/Importer/TextureImporter.h"
#include "Engine/Asset/Material/Material.h"
#include "Engine/Asset/Mesh/MeshGenerator.h"
#include "Engine/Asset/Shader/Shader.h"
#include "Engine/Asset/Texture/Texture.h"
#include "Engine/Engine.h"
#include "Engine/Object/Object.h"

#include "Core/File/VirtualFilesystem.h"
#include "Core/Log/Log.h"

#include <iostream>
#include <string>

namespace URay::DefaultMeshUUIDs
{
inline constexpr UUID Quad{ .high = 0, .low = 1 };
inline constexpr UUID Cube{ .high = 0, .low = 2 };
inline constexpr UUID Arrow{ .high = 0, .low = 3 };
inline constexpr UUID RotationGizmo{ .high = 0, .low = 4 };
inline constexpr UUID ScaleGizmo{ .high = 0, .low = 5 };
} // namespace URay::DefaultMeshUUIDs

namespace URay
{

AssetSystem::AssetSystem() = default;

AssetSystem::~AssetSystem() = default;

bool AssetSystem::Initialize(
    const std::string& enginePath,
    const std::string& projectPath)
{
    filesystem = std::make_unique<VirtualFilesystem>();
    filesystem->Mount("Engine", enginePath);
    filesystem->Mount("Project", projectPath);
    filesystem->Mount("RawAsset", fs::path(projectPath) / "Asset/Source");
    filesystem->Mount("Asset", fs::path(projectPath) / "Asset/Imported");

    factory = std::make_unique<AssetFactory>();

    importers.push_back(std::make_unique<TextureImporter>());
    importers.push_back(std::make_unique<MaterialImporter>());
    importers.push_back(std::make_unique<OBJImporter>());
    importers.push_back(std::make_unique<ShaderImporter>());

    /* === Asset Pipeline === */

    // 0. Collect metadatas.
    std::vector<AssetMetadata> collectedMetadatas = ScanAssets();

    // 1. Sort metadatas by dependency.
    std::vector<AssetMetadata> sortedMetadatas = SortByDependency(collectedMetadatas);

    // 2. Import assets in order.
    ImportAll(sortedMetadatas);

    return true;
}

bool AssetSystem::CreateDefaultAssets()
{
    MeshGenerator meshGenerator;
    MeshInfo quadMeshInfo = meshGenerator.CreateQuad();
    MeshInfo cubeMeshInfo = meshGenerator.CreateCube();
    MeshInfo arrowMeshInfo = meshGenerator.CreateArrow();
    MeshInfo rotationGizmoMeshInfo = meshGenerator.CreateRotationGizmo();
    MeshInfo scaleGizmoMeshInfo = meshGenerator.CreateScaleGizmo();

    Mesh* quadMesh = factory->CreateMesh(
        AssetMetadata{
            .uuid = DefaultMeshUUIDs::Quad,
            .type = AssetType::Mesh,
            .sourcePath = "Quad" },
        quadMeshInfo.vertices,
        quadMeshInfo.indices,
        quadMeshInfo.sections,
        { defaultAssets.meshMaterial });
    assets.insert({ quadMesh->GetUUID(), quadMesh });
    defaultAssets.quadMesh = quadMesh;

    Mesh* cubeMesh = factory->CreateMesh(
        AssetMetadata{
            .uuid = DefaultMeshUUIDs::Cube,
            .type = AssetType::Mesh,
            .sourcePath = "Cube" },
        cubeMeshInfo.vertices,
        cubeMeshInfo.indices,
        cubeMeshInfo.sections,
        { defaultAssets.meshMaterial });
    assets.insert({ cubeMesh->GetUUID(), cubeMesh });
    defaultAssets.cubeMesh = cubeMesh;

    Mesh* arrowMesh = factory->CreateMesh(
        AssetMetadata{
            .uuid = DefaultMeshUUIDs::Arrow,
            .type = AssetType::Mesh,
            .sourcePath = "Arrow" },
        arrowMeshInfo.vertices,
        arrowMeshInfo.indices,
        arrowMeshInfo.sections,
        { defaultAssets.meshMaterial });
    assets.insert({ arrowMesh->GetUUID(), arrowMesh });
    defaultAssets.arrowMesh = arrowMesh;

    Mesh* rotationGizmoMesh = factory->CreateMesh(
        AssetMetadata{
            .uuid = DefaultMeshUUIDs::RotationGizmo,
            .type = AssetType::Mesh,
            .sourcePath = "RotationGizmo" },
        rotationGizmoMeshInfo.vertices,
        rotationGizmoMeshInfo.indices,
        rotationGizmoMeshInfo.sections,
        { defaultAssets.meshMaterial });
    assets.insert({ rotationGizmoMesh->GetUUID(), rotationGizmoMesh });
    defaultAssets.rotationGizmoMesh = rotationGizmoMesh;

    Mesh* scaleGizmoMesh = factory->CreateMesh(
        AssetMetadata{
            .uuid = DefaultMeshUUIDs::ScaleGizmo,
            .type = AssetType::Mesh,
            .sourcePath = "ScaleGizmo" },
        scaleGizmoMeshInfo.vertices,
        scaleGizmoMeshInfo.indices,
        scaleGizmoMeshInfo.sections,
        { defaultAssets.meshMaterial });
    assets.insert({ scaleGizmoMesh->GetUUID(), scaleGizmoMesh });
    defaultAssets.scaleGizmoMesh = scaleGizmoMesh;

    return true;
}

bool AssetSystem::LoadAssets(const VirtualPath& sourceDir)
{
    std::vector<VirtualFileEntry> entries = filesystem->ListDirectory(sourceDir);

    for (const auto& entry : entries)
    {
        if (entry.isDirectory)
        {
            LoadAssets(entry.path);
            continue;
        }

        const VirtualPath& path = entry.path;
        Import(path);
    }

    return true;
}

void AssetSystem::Finalize()
{
    for (auto& [uuid, asset] : assets)
    {
        if (asset)
        {
            delete asset;
            asset = nullptr;
        }
    }
    assets.clear();
    sourceAssets.clear();

    filesystem.reset();
}

Asset* AssetSystem::Import(const VirtualPath& path)
{
    const std::string extension = path.GetExtension();
    Importer* importer = GetImporterByExtension(extension);
    if (!importer)
        return nullptr;

    Asset* asset = importer->Import(path);
    if (!asset)
        return nullptr;

    return asset;
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

std::optional<AssetMetadata> AssetSystem::FindAssetMetadataByUUID(const UUID& uuid) const
{
    std::optional<AssetMetadata> ret = std::nullopt;

    const auto it = assetMetadatas.find(uuid);
    if (it == assetMetadatas.end())
        return std::nullopt;

    ret = it->second;

    return ret;
}

std::optional<UUID> AssetSystem::FindUUIDBySourcePath(const std::string& sourcePath) const
{
    std::optional<UUID> ret = std::nullopt;

    const auto it = sourceAssets.find(sourcePath);
    if (it == sourceAssets.end())
        return std::nullopt;

    ret = it->second;

    return ret;
}

std::vector<AssetMetadata> AssetSystem::ScanAssets()
{
    std::vector<AssetMetadata> ret;

    std::vector<AssetMetadata> engineAssetMetadatas = ScanAssetsRecursive("Engine://Asset/Soruce");
    std::vector<AssetMetadata> projectAssetMetadatas = ScanAssetsRecursive("RawAsset://");

    ret.insert(ret.end(), engineAssetMetadatas.begin(), engineAssetMetadatas.end());
    ret.insert(ret.end(), projectAssetMetadatas.begin(), projectAssetMetadatas.end());

    return ret;
}

std::vector<AssetMetadata> AssetSystem::ScanAssetsRecursive(const VirtualPath& path)
{
    std::vector<AssetMetadata> ret;

    std::vector<VirtualFileEntry> entries = filesystem->ListDirectory(path);

    for (const auto& entry : entries)
    {
        if (entry.isDirectory)
        {
            std::vector<AssetMetadata> metadatas = ScanAssetsRecursive(entry.path);
            ret.insert(ret.end(), metadatas.begin(), metadatas.end());
            continue;
        }

        AssetMetadata metadata = LoadAssetMetadata(entry.path);
        ret.push_back(metadata);
    }

    return ret;
}

AssetMetadata AssetSystem::LoadAssetMetadata(const VirtualPath& path)
{
    AssetMetadata ret = {};

    const VirtualPath importPath = GetImportAssetPath(path);
    const VirtualPath metadataPath = VirtualPath(importPath.ToString() + ".meta");

    if (!filesystem->Exists(metadataPath))
    {
        const std::string fileText = filesystem->ReadText(metadataPath);
        const YAML::Node node = YAML::Load(fileText);
        ret.Deserialize(node);
    }
    else
    {
        const std::string extension = path.GetExtension();
        Importer* importer = GetImporterByExtension(extension);

        if (!importer)
            return {};

        ret = importer->CreateMetadata(path);
    }

    return ret;
}

std::vector<AssetMetadata> AssetSystem::SortByDependency(
    const std::vector<AssetMetadata>& metadatas) const
{
    return {};
}

} // namespace URay

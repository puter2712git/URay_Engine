#include "AssetSystem.h"

#include "Engine/Asset/AssetFactory.h"
#include "Engine/Asset/AssetPipeline.h"
#include "Engine/Asset/Material/Material.h"
#include "Engine/Asset/Mesh/MeshGenerator.h"
#include "Engine/Asset/Shader/Shader.h"
#include "Engine/Asset/Texture/Texture.h"
#include "Engine/Engine.h"
#include "Engine/Object/Object.h"

#include "Core/File/VirtualFilesystem.h"
#include "Core/Log/Log.h"

namespace URay
{

AssetSystem::AssetSystem(Engine& engine)
    : engine(engine)
{
}

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

    factory = std::make_unique<AssetFactory>(engine);

    pipeline = std::make_unique<AssetPipeline>(*this);
    if (!pipeline->Initialize())
        return false;

    Shader* spriteShader = factory->CreateShader("Engine://Asset/Source/Shader/Sprite.hlsl");
    Shader* meshShader = factory->CreateShader("Engine://Asset/Source/Shader/Mesh.hlsl");
    Shader* fontShader = factory->CreateShader("Engine://Asset/Source/Shader/Font.hlsl");
    Shader* decalShader = factory->CreateShader("Engine://Asset/Source/Shader/Decal.hlsl");
    Shader* lineShader = factory->CreateShader("Engine://Asset/Source/Shader/Line.hlsl");
    Shader* billboardShader = factory->CreateShader("Engine://Asset/Source/Shader/Billboard.hlsl");
    Shader* fogShader = factory->CreateShader("Engine://Asset/Source/Shader/PostProcess/Fog.hlsl");

    assets.insert({ spriteShader->GetUUID(), spriteShader });
    assets.insert({ meshShader->GetUUID(), meshShader });
    assets.insert({ fontShader->GetUUID(), fontShader });
    assets.insert({ decalShader->GetUUID(), decalShader });
    assets.insert({ lineShader->GetUUID(), lineShader });
    assets.insert({ billboardShader->GetUUID(), billboardShader });
    assets.insert({ fogShader->GetUUID(), fogShader });

    return true;
}

bool AssetSystem::CreateDefaultAssets()
{
    UUID whiteTextureUUID = Import("RawAsset://Texture/White.png");
    defaultAssets.whiteTexture = Find<Texture>(whiteTextureUUID);

    UUID fontTextureUUID = Import("RawAsset://Texture/DejaVu Sans Mono.png");
    defaultAssets.fontTexture = Find<Texture>(fontTextureUUID);

    UUID decalTextureUUID = Import("RawAsset://Texture/bullet_hole.png");
    defaultAssets.decalTexture = Find<Texture>(decalTextureUUID);

    UUID directionalLightTextureUUID = Import("Engine://Asset/Source/Texture/DirectionalLightIcon.png");
    defaultAssets.directionalLightBillboardTexture = Find<Texture>(directionalLightTextureUUID);

    std::vector<Shader*> shaders = FindAssets<Shader>();
    Shader* spriteShader = nullptr;
    Shader* billboardShader = nullptr;
    Shader* meshShader = nullptr;
    Shader* decalShader = nullptr;

    for (Shader* shader : shaders)
    {
        if (shader->GetName() == "Sprite")
        {
            spriteShader = shader;
        }
        if (shader->GetName() == "Billboard")
        {
            billboardShader = shader;
        }
        if (shader->GetName() == "Mesh")
        {
            meshShader = shader;
        }
        if (shader->GetName() == "Decal")
        {
            decalShader = shader;
        }
    }

    Material* spriteMaterial = factory->CreateMaterial(
        AssetMetadata{
            .uuid = UUID::Generate(),
            .type = AssetType::Material,
            .sourcePath = "Sprite Material" },
        spriteShader);
    Material* billboardMaterial = factory->CreateMaterial(
        AssetMetadata{
            .uuid = UUID::Generate(),
            .type = AssetType::Material,
            .sourcePath = "Billboard Material" },
        billboardShader);
    billboardMaterial->SetParameter("textureImage", { .type = MaterialParameterType::Texture2D, .value = defaultAssets.directionalLightBillboardTexture });
    Material* meshMaterial = factory->CreateMaterial(
        AssetMetadata{
            .uuid = UUID::Generate(),
            .type = AssetType::Material,
            .sourcePath = "Mesh Material" },
        meshShader);
    Material* decalMaterial = factory->CreateMaterial(
        AssetMetadata{
            .uuid = UUID::Generate(),
            .type = AssetType::Material,
            .sourcePath = "Decal Material" },
        decalShader);
    decalMaterial->SetParameter("textureImage", { .type = MaterialParameterType::Texture2D, .value = defaultAssets.decalTexture });

    assets.insert({ spriteMaterial->GetUUID(), spriteMaterial });
    assets.insert({ billboardMaterial->GetUUID(), billboardMaterial });
    assets.insert({ meshMaterial->GetUUID(), meshMaterial });
    assets.insert({ decalMaterial->GetUUID(), decalMaterial });

    defaultAssets.spriteMaterial = spriteMaterial;
    defaultAssets.billboardMaterial = billboardMaterial;
    defaultAssets.meshMaterial = meshMaterial;
    defaultAssets.decalMaterial = decalMaterial;

    MeshGenerator meshGenerator;
    MeshInfo quadMeshInfo = meshGenerator.CreateQuad();
    MeshInfo cubeMeshInfo = meshGenerator.CreateCube();
    MeshInfo arrowMeshInfo = meshGenerator.CreateArrow();
    MeshInfo rotationGizmoMeshInfo = meshGenerator.CreateRotationGizmo();
    MeshInfo scaleGizmoMeshInfo = meshGenerator.CreateScaleGizmo();

    Mesh* quadMesh = factory->CreateMesh(
        AssetMetadata{
            .uuid = UUID::Generate(),
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
            .uuid = UUID::Generate(),
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
            .uuid = UUID::Generate(),
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
            .uuid = UUID::Generate(),
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
            .uuid = UUID::Generate(),
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

    pipeline->Finalize();
    pipeline.reset();

    filesystem.reset();
}

UUID AssetSystem::Import(const VirtualPath& path)
{
    const std::string& sourcePath = path.ToString();
    if (const auto it = sourceAssets.find(sourcePath); it != sourceAssets.end())
        return it->second;

    ImportResult importResult = pipeline->Import(path);

    if (importResult.entries.empty())
        return UUID{};

    const UUID primaryUUID = importResult.entries.front().metadata.uuid;

    for (AssetEntry& entry : importResult.entries)
    {
        Asset* asset = entry.asset;
        AssetMetadata& metadata = entry.metadata;

        const auto it = assets.find(metadata.uuid);

        if (it != assets.end())
        {
            if (asset)
            {
                delete asset;
            }

            Logger::Log("Import Failed. UUID already exists: " + metadata.uuid.ToString());
            continue;
        }
        else
        {
            assets.insert({ asset->GetUUID(), asset });
            sourceAssets.insert({ metadata.sourcePath.ToString(), metadata.uuid });
        }
    }

    return primaryUUID;
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

} // namespace URay

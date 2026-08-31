#include "AssetSystem.h"

#include "Engine/Asset/Font/FontManager.h"
#include "Engine/Asset/Importer/ObjImporter.h"
#include "Engine/Asset/Material/Material.h"
#include "Engine/Asset/Material/MaterialManager.h"
#include "Engine/Asset/Mesh/MeshManager.h"
#include "Engine/Asset/Texture/TextureManager.h"

#include "Core/File/VirtualFilesystem.h"

#include "Render/Shader/ShaderManager.h"

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

    return true;
}

bool AssetSystem::InitializeRuntimeAssets(
    Render::RenderDevice& renderDevice,
    Render::GPUResourceManager& resourceManager,
    Render::ShaderManager& shaderManager)
{
    textureManager = std::make_unique<TextureManager>(*filesystem);
    Texture* defaultWhite = textureManager->LoadTexture(
        "DefaultWhite", "RawAsset://Texture/white.png");
    Texture* fontTexture = textureManager->LoadTexture(
        "FontTexture", "RawAsset://Texture/DejaVu Sans Mono.png");

    materialManager = std::make_unique<MaterialManager>(
        &renderDevice, &resourceManager, defaultWhite);
    Material* defaultMat = materialManager->GetOrCreate(
        "Mesh", shaderManager.GetOrCreate("Mesh"));
    materialManager->GetOrCreate(
        "Sprite", shaderManager.GetOrCreate("Sprite"));

    meshManager = std::make_unique<MeshManager>();
    meshManager->CreateDefaultMeshes(defaultMat);

    fontManager = std::make_unique<FontManager>();
    fontManager->LoadFont("Default", fontTexture);

    objImporter = std::make_unique<ObjImporter>(
        *filesystem, *meshManager, *textureManager,
        *materialManager, defaultMat->GetShader());
    objImporter->Import("RawAsset://Mesh/untitled.obj");
    objImporter->Import("RawAsset://Mesh/SilverWolf/SilverWolf.obj");
    objImporter->Import("RawAsset://Mesh/apple_mid.obj");
    objImporter->Import("RawAsset://Mesh/bitten_apple_mid.obj");

    return true;
}

void AssetSystem::Finalize()
{
    if (objImporter)
    {
        objImporter.reset();
    }
    if (fontManager)
    {
        fontManager.reset();
    }
    if (meshManager)
    {
        meshManager.reset();
    }
    if (materialManager)
    {
        materialManager.reset();
    }
    if (textureManager)
    {
        textureManager.reset();
    }

    if (filesystem)
    {
        filesystem.reset();
    }
}

Mesh* AssetSystem::FindMesh(const std::string& key) const
{
    Mesh* mesh = meshManager->GetMesh(key);
    return mesh;
}

Material* AssetSystem::FindMaterial(const std::string& key) const
{
    Material* material = materialManager->GetOrCreate(key);
    return material;
}

Texture* AssetSystem::FindTexture(const std::string& key) const
{
    Texture* texture = textureManager->GetTexture(key);
    return texture;
}

Font* AssetSystem::FindFont(const std::string& key) const
{
    Font* font = fontManager->GetFont(key);
    return font;
}

const std::unordered_map<std::string, Mesh*>& AssetSystem::GetMeshes() const
{
    return meshManager->GetMeshes();
}

const std::unordered_map<std::string, Material*>& AssetSystem::GetMaterials() const
{
    return materialManager->GetMaterials();
}

const std::unordered_map<std::string, Texture*>& AssetSystem::GetTextures() const
{
    return textureManager->GetTextures();
}

} // namespace URay

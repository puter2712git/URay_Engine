#include "AssetFactory.h"

#include "Engine/Asset/AssetMetadata.h"
#include "Engine/Asset/AssetSystem.h"
#include "Engine/Asset/Material/Material.h"
#include "Engine/Asset/Texture/Texture.h"
#include "Engine/Engine.h"

#include "Render/RHI/RenderDevice.h"
#include "Render/RenderSystem.h"

namespace URay
{

AssetFactory::AssetFactory(Engine& engine)
    : engine(engine) {}

AssetFactory::~AssetFactory() {}

Mesh* AssetFactory::CreateMesh(const AssetMetadata& metadata,
                               const std::vector<Render::VertexPNT>& vertices,
                               const std::vector<uint32>& indices,
                               const std::vector<MeshSection>& sections,
                               const std::vector<Material*>& materials)
{
    Mesh* newMesh = new Mesh();
    newMesh->SetVertices(vertices);
    newMesh->SetIndices(indices);
    newMesh->SetSections(sections);
    newMesh->SetDefaultMaterials(materials);

    newMesh->SetName(metadata.sourcePath.GetStem());
    newMesh->SetUUID(metadata.uuid);

    return newMesh;
}

Material* AssetFactory::CreateMaterial(const AssetMetadata& metadata,
                                       Render::Shader* shader)
{
    AssetSystem& assetSystem = engine.GetAssetSystem();
    Render::RenderSystem& renderSystem = engine.GetRenderSystem();
    const DefaultAssets& defaultAssets = assetSystem.GetDefaultAssets();

    Material* newMaterial = new Material(shader);

    if (!newMaterial->Initialize(
            &renderSystem.GetDevice(),
            &renderSystem.GetResourceManager(),
            defaultAssets.whiteTexture))
    {
        delete newMaterial;
        return nullptr;
    }

    newMaterial->SetName(metadata.sourcePath.GetStem());
    newMaterial->SetUUID(metadata.uuid);

    return newMaterial;
}

Texture* AssetFactory::CreateTexture(const AssetMetadata& metadata,
                                     int32 width, int32 height, int32 channels,
                                     const std::vector<uint8>& pixels)
{
    Texture* newTexture = new Texture(width, height, channels,
                                      pixels);
    newTexture->SetName(metadata.sourcePath.GetStem());
    newTexture->SetUUID(metadata.uuid);

    return newTexture;
}

} // namespace URay

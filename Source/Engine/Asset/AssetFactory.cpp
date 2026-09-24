#include "AssetFactory.h"

#include "Engine/Asset/AssetMetadata.h"
#include "Engine/Asset/AssetSystem.h"
#include "Engine/Asset/Material/Material.h"
#include "Engine/Asset/Shader/Shader.h"
#include "Engine/Asset/Texture/Texture.h"
#include "Engine/Engine.h"

#include "Render/RHI/RenderDevice.h"
#include "Render/RenderSystem.h"

namespace URay
{

AssetFactory::AssetFactory() = default;

AssetFactory::~AssetFactory() = default;

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

Material* AssetFactory::CreateMaterial(const AssetMetadata& metadata)
{
    Material* ret = nullptr;

    AssetSystem& assetSystem = gEngine->GetAssetSystem();
    Render::RenderSystem& renderSystem = gEngine->GetRenderSystem();

    Shader* shader = assetSystem.Find<Shader>(metadata.dependencies[0]);

    ret = new Material(shader);
    if (!ret->Initialize())
    {
        delete ret;
        return nullptr;
    }

    ret->SetName(metadata.sourcePath.GetStem());
    ret->SetUUID(metadata.uuid);

    return ret;
}

Texture* AssetFactory::CreateTexture(const AssetMetadata& metadata,
                                     int32 width, int32 height, int32 channels,
                                     const std::vector<uint8>& pixels)
{
    Texture* ret = new Texture(width, height, channels, pixels);

    ret->SetName(metadata.sourcePath.GetStem());
    ret->SetUUID(metadata.uuid);

    return ret;
}

Shader* AssetFactory::CreateShader(const AssetMetadata& metadata)
{
    Shader* ret = new Shader(metadata.sourcePath);

    ret->SetName(metadata.sourcePath.GetStem());
    ret->SetUUID(metadata.uuid);

    return ret;
}

} // namespace URay

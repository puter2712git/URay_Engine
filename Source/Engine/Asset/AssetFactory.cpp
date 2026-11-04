#include "AssetFactory.h"

#include <Core/File/VirtualFileSystem.h>
#include <Engine/Asset/AssetDatabase.h>
#include <Engine/Asset/AssetSystem.h>
#include <Engine/Asset/Font/Font.h>
#include <Engine/Asset/Font/FontData.h>
#include <Engine/Asset/Font/FontSerializer.h>
#include <Engine/Asset/Material/Material.h>
#include <Engine/Asset/Material/MaterialData.h>
#include <Engine/Asset/Material/MaterialInstance.h>
#include <Engine/Asset/Material/MaterialSerializer.h>
#include <Engine/Asset/Mesh/Mesh.h>
#include <Engine/Asset/Mesh/MeshData.h>
#include <Engine/Asset/Mesh/MeshSerializer.h>
#include <Engine/Asset/Metadata/AssetMetadata.h>
#include <Engine/Asset/Shader/Shader.h>
#include <Engine/Asset/Texture/Texture.h>
#include <Engine/Asset/Texture/TextureData.h>
#include <Engine/Asset/Texture/TextureSerializer.h>

namespace URay
{

AssetFactory::AssetFactory(AssetSystem& assetSystem)
    : assetSystem(assetSystem) {}

UniquePtr<Asset> AssetFactory::Create(const AssetMetadata& metadata) const
{
    if (metadata.className == Texture::StaticType().name)
    {
        return CreateTexture(metadata);
    }
    else if (metadata.className == Material::StaticType().name)
    {
        return CreateMaterial(metadata);
    }
    else if (metadata.className == Shader::StaticType().name)
    {
        return CreateShader(metadata);
    }
    else if (metadata.className == Font::StaticType().name)
    {
        return CreateFont(metadata);
    }
    else if (metadata.className == Mesh::StaticType().name)
    {
        return CreateMesh(metadata);
    }

    return nullptr;
}

UniquePtr<MaterialInstance> AssetFactory::CreateMaterialInstance(const AssetHandle& parentHandle) const
{
    AssetDatabase& assetDB = assetSystem.GetDatabase();

    Material* parent = assetDB.Find<Material>(parentHandle);
    if (!parent)
        return nullptr;

    auto instance = MakeUnique<MaterialInstance>(parent);
    instance->SetHandle(AssetHandle::Generate());

    return instance;
}

UniquePtr<Texture> AssetFactory::CreateTexture(const AssetMetadata& metadata) const
{
    TextureData data;
    if (!assetSystem.GetTextureSerializer().Load(data, metadata.assetPath))
        return nullptr;

    auto texture = MakeUnique<Texture>(
        data.width,
        data.height,
        4,
        data.pixels);

    texture->SetHandle(metadata.handle);
    texture->SetName(metadata.assetPath.GetStem());

    return texture;
}

UniquePtr<Material> AssetFactory::CreateMaterial(const AssetMetadata& metadata) const
{
    MaterialData data;
    if (!assetSystem.GetMaterialSerializer().Load(data, metadata.assetPath))
        return nullptr;

    auto material = MakeUnique<Material>(data.shaderHandle);
    material->SetHandle(metadata.handle);
    material->SetName(metadata.assetPath.GetStem());

    for (const MaterialParameterData& parameter : data.parameters)
    {
        if (!material->AddParameter(
                parameter.name,
                parameter.type,
                parameter.value))
        {
            return nullptr;
        }
    }

    return material;
}

UniquePtr<Shader> AssetFactory::CreateShader(const AssetMetadata& metadata) const
{
    auto shader = MakeUnique<Shader>(metadata.assetPath);

    shader->SetHandle(metadata.handle);
    shader->SetName(metadata.assetPath.GetStem());

    return shader;
}

UniquePtr<Font> AssetFactory::CreateFont(const AssetMetadata& metadata) const
{
    FontData data;
    if (!assetSystem.GetFontSerializer().Load(data, metadata.assetPath))
        return nullptr;

    auto font = MakeUnique<Font>(data.bytes);
    font->SetHandle(metadata.handle);
    font->SetName(metadata.assetPath.GetStem());

    return font;
}

UniquePtr<Mesh> AssetFactory::CreateMesh(const AssetMetadata& metadata) const
{
    MeshData data;
    if (!assetSystem.GetMeshSerializer().Load(data, metadata.assetPath))
        return nullptr;

    auto mesh = MakeUnique<Mesh>();
    mesh->SetHandle(metadata.handle);
    mesh->SetName(metadata.assetPath.GetStem());
    mesh->SetVertices(data.vertices);
    mesh->SetIndices(data.indices);
    mesh->SetSections(data.sections);
    mesh->SetDefaultMaterials(data.defaultMaterials);

    return mesh;
}

} // namespace URay

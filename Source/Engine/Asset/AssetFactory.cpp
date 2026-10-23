#include "AssetFactory.h"

#include <Core/File/VirtualFileSystem.h>
#include <Engine/Asset/AssetSystem.h>
#include <Engine/Asset/Font/Font.h>
#include <Engine/Asset/Material/Material.h>
#include <Engine/Asset/Material/MaterialData.h>
#include <Engine/Asset/Material/MaterialSerializer.h>
#include <Engine/Asset/Metadata/AssetMetadata.h>
#include <Engine/Asset/Shader/Shader.h>
#include <Engine/Asset/Texture/Texture.h>
#include <Engine/Asset/Texture/TextureData.h>
#include <Engine/Asset/Texture/TextureSerializer.h>

namespace URay
{

AssetFactory::AssetFactory(AssetSystem& assetSystem)
    : assetSystem(assetSystem) {}

std::unique_ptr<Asset> AssetFactory::Create(const AssetMetadata& metadata) const
{
    if (metadata.className == Texture::StaticRuntimeType().name)
    {
        return CreateTexture(metadata);
    }
    else if (metadata.className == Material::StaticRuntimeType().name)
    {
        return CreateMaterial(metadata);
    }
    else if (metadata.className == Shader::StaticRuntimeType().name)
    {
        return CreateShader(metadata);
    }
    else if (metadata.className == Font::StaticRuntimeType().name)
    {
        return CreateFont(metadata);
    }

    return nullptr;
}

std::unique_ptr<Texture> AssetFactory::CreateTexture(const AssetMetadata& metadata) const
{
    TextureData data;
    if (!assetSystem.GetTextureSerializer().Load(data, metadata.assetPath))
        return nullptr;

    auto texture = std::make_unique<Texture>(
        data.width,
        data.height,
        4,
        data.pixels);

    texture->SetHandle(metadata.handle);
    texture->SetName(metadata.assetPath.GetStem());

    return texture;
}

std::unique_ptr<Material> AssetFactory::CreateMaterial(const AssetMetadata& metadata) const
{
    MaterialData data;
    if (!assetSystem.GetMaterialSerializer().Load(data, metadata.assetPath))
        return nullptr;

    auto material = std::make_unique<Material>(data.shaderHandle);
    material->SetHandle(metadata.handle);
    material->SetName(metadata.assetPath.GetStem());

    for (const MaterialParameterData& parameter : data.parameters)
    {
        material->AddParameter(
            parameter.name,
            parameter.type,
            parameter.value);
    }

    return material;
}

std::unique_ptr<Shader> AssetFactory::CreateShader(const AssetMetadata& metadata) const
{
    auto shader = std::make_unique<Shader>(metadata.assetPath);

    shader->SetHandle(metadata.handle);
    shader->SetName(metadata.assetPath.GetStem());

    return shader;
}

std::unique_ptr<Font> AssetFactory::CreateFont(const AssetMetadata& metadata) const
{
    VirtualFileSystem& fileSystem = assetSystem.GetFileSystem();

    std::vector<uint8> data = fileSystem.ReadBinary(metadata.assetPath);
    if (data.empty())
        return nullptr;

    auto font = std::make_unique<Font>(data);
    font->SetHandle(metadata.handle);
    font->SetName(metadata.assetPath.GetStem());

    return font;
}

} // namespace URay

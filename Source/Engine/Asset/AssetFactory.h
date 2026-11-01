#pragma once

#include <Core/Type/TypeAlias.h>
#include <memory>

namespace URay
{

class AssetSystem;
class Asset;
class Texture;
class Material;
class Shader;
class Font;
class Mesh;
struct AssetMetadata;

class AssetFactory
{
public:
    explicit AssetFactory(AssetSystem& assetSystem);

public:
    UniquePtr<Asset> Create(const AssetMetadata& metadata) const;

private:
    UniquePtr<Texture> CreateTexture(const AssetMetadata& metadata) const;
    UniquePtr<Material> CreateMaterial(const AssetMetadata& metadata) const;
    UniquePtr<Shader> CreateShader(const AssetMetadata& metadata) const;
    UniquePtr<Font> CreateFont(const AssetMetadata& metadata) const;
    UniquePtr<Mesh> CreateMesh(const AssetMetadata& metadata) const;

private:
    AssetSystem& assetSystem;
};

} // namespace URay

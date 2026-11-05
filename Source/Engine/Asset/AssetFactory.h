#pragma once

#include <Core/Type/TypeAlias.h>
#include <Engine/Asset/AssetHandle.h>

namespace URay
{

class AssetSystem;
class Asset;
class Texture;
class Material;
class MaterialInstance;
class Shader;
class Font;
class Mesh;
class Script;
struct AssetMetadata;

class AssetFactory
{
public:
    explicit AssetFactory(AssetSystem& assetSystem);

public:
    UniquePtr<Asset> Create(const AssetMetadata& metadata) const;

    UniquePtr<MaterialInstance> CreateMaterialInstance(const AssetHandle& parentHandle) const; // Currently, material instance is runtime only.

private:
    UniquePtr<Texture> CreateTexture(const AssetMetadata& metadata) const;
    UniquePtr<Material> CreateMaterial(const AssetMetadata& metadata) const;
    UniquePtr<Shader> CreateShader(const AssetMetadata& metadata) const;
    UniquePtr<Font> CreateFont(const AssetMetadata& metadata) const;
    UniquePtr<Mesh> CreateMesh(const AssetMetadata& metadata) const;
    UniquePtr<Script> CreateScript(const AssetMetadata& metadata) const;

private:
    AssetSystem& assetSystem;
};

} // namespace URay

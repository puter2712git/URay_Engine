#pragma once

#include <memory>

namespace URay
{

class AssetSystem;
class Asset;
class Texture;
class Material;
class Shader;
class Font;
struct AssetMetadata;

class AssetFactory
{
public:
    explicit AssetFactory(AssetSystem& assetSystem);

public:
    std::unique_ptr<Asset> Create(const AssetMetadata& metadata) const;

private:
    std::unique_ptr<Texture> CreateTexture(const AssetMetadata& metadata) const;
    std::unique_ptr<Material> CreateMaterial(const AssetMetadata& metadata) const;
    std::unique_ptr<Shader> CreateShader(const AssetMetadata& metadata) const;
    std::unique_ptr<Font> CreateFont(const AssetMetadata& metadata) const;

private:
    AssetSystem& assetSystem;
};

} // namespace URay
